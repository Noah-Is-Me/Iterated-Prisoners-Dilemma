#include <stdio.h>
#include <iostream>
#include <array>
#include <unordered_map>
#include <chrono>

#include "strategy.h"

Move getFail(Move move, double failRate)
{
    if (randomChance(failRate))
    {
        return randomChance(0.5) ? cooperate : defect;
    }
    return move;
}

void runIteration(Strategy &s1, Strategy &s2, double failRate)
{
    Move s1Move = getFail(s1.nextMove, failRate);
    Move s2Move = getFail(s2.nextMove, failRate);

    s1.onMove(s2Move);
    s2.onMove(s1Move);

    s1.points += pointMatrix[s1Move][s2Move];
    s2.points += pointMatrix[s2Move][s1Move];

    // std::cout << s1.name << " Points: " << s1.points << ", "
    //           << s2.name << " Points: " << s2.points << std::endl;
}

int main()
{
    auto totalStart = std::chrono::high_resolution_clock::now();

    double failRate = 0.00;
    const double failRateIncrement = 0.01;
    const int totalRounds = 100;
    const int iterationCount = 1000;

    std::array<double, totalRounds> failRates;
    for (int i = 0; i < totalRounds; i++)
    {
        failRates[i] = failRate + failRateIncrement * i;
    }

    std::array<std::function<std::unique_ptr<Strategy>()>, 15> strategyConstructors = {
        []()
        { return std::make_unique<TitForTat>(); },
        []()
        { return std::make_unique<ForgivingTitForTat>(); },
        []()
        { return std::make_unique<AlwaysDefect>(); },
        []()
        { return std::make_unique<AlwaysCooperate>(); },
        []()
        { return std::make_unique<Random>(); },
        []()
        { return std::make_unique<ProbabilityCooperator>(); },
        []()
        { return std::make_unique<ProbabilityDefector>(); },
        []()
        { return std::make_unique<SuspiciousTitForTat>(); },
        []()
        { return std::make_unique<GenerousTitForTat>(); },
        []()
        { return std::make_unique<GradualTitForTat>(); },
        []()
        { return std::make_unique<ImperfectTitForTat>(); },
        []()
        { return std::make_unique<TitForTwoTats>(); },
        []()
        { return std::make_unique<TwoTitsForTat>(); },
        []()
        { return std::make_unique<GrimTrigger>(); },
        []()
        { return std::make_unique<Pavlov>(); },
    };

    const int strategiesCount = strategyConstructors.size();

    std::array<StrategyData, strategiesCount> strategies;

    for (int i = 0; i < strategiesCount; i++)
    {
        std::unique_ptr<Strategy> s_ = strategyConstructors[i]();
        Strategy &s = *s_;

        strategies[i].name = s.name;
        strategies[i].totalPoints.assign(totalRounds, 0);
        strategies[i].averagePoints.assign(totalRounds, 0);
        strategies[i].constructor = strategyConstructors[i];
    }

    for (int u = 0; u < totalRounds; u++)
    {
        auto roundStart = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < strategiesCount; i++)
        {
            for (int j = i; j < strategiesCount; j++)
            {
                StrategyData &sd1 = strategies[i];
                StrategyData &sd2 = strategies[j];

                std::unique_ptr<Strategy> s1_ = sd1.constructor();
                std::unique_ptr<Strategy> s2_ = sd2.constructor();

                Strategy &s1 = *s1_;
                Strategy &s2 = *s2_;

                s1.reset();
                s2.reset();

                for (int u = 0; u < iterationCount; u++)
                {
                    runIteration(s1, s2, failRate);
                }

                sd1.totalPoints[u] += s1.points;
                sd2.totalPoints[u] += s2.points;

                // std::cout << s1.name << " Points: " << strategies[i].totalPoints[u] << ", "
                //           << s2.name << " Points: " << strategies[j].totalPoints[u]
                //           << std::endl;
            }
        }

        failRate += failRateIncrement;

        auto roundEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> roundDuration = roundEnd - roundStart;
        double estimatedTime = roundDuration.count() * (totalRounds - (u + 1));

        std::cout << "[NOTICE] Round " << u << " complete" << std::endl;
        std::cout << "[NOTICE] Time taken: " << roundDuration.count() << " seconds" << std::endl;
        std::cout << "[NOTICE] Estimated time: " << estimatedTime << " seconds  (" << estimatedTime / 60 << " minutes)" << std::endl;
    }

    for (int i = 0; i < strategiesCount; i++)
    {
        auto &strategy = strategies[i];

        for (int j = 0; j < totalRounds; j++)
        {
            strategy.averagePoints[j] = 1.0 * strategy.totalPoints[j] / (strategiesCount + 1);
        }
    }

    std::string fullData = "";

    for (StrategyData s : strategies)
    {
        std::string data = s.name + ",";
        for (int i = 0; i < totalRounds; i++)
        {
            data += std::to_string(failRates[i]) + "," + std::to_string(s.averagePoints[i]) + ",";
        }

        fullData += data + "\n";
        std::cout << data << std::endl;
    }

    // std::cout << fullData << std::endl;

    auto totalEnd = std::chrono::high_resolution_clock::now();
    // Calculate the duration in milliseconds, microseconds, or seconds
    // std::chrono::duration<double> duration = end - start;
    // std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;
}
