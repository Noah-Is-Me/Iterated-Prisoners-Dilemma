#include <stdio.h>
#include <iostream>
#include <array>
#include <unordered_map>
#include <chrono>
#include <thread>

#include "strategy.h"

Move getFail(Move move, double failRate)
{
    if (randomChance(failRate))
    {
        return randomChance(0.5) ? cooperate : defect;
    }
    return move;
}

void runIteration(Strategy &s1, Strategy &s2, double miscommunicationRate, double misexecutionRate)
{
    Move s1Move = getFail(s1.nextMove, misexecutionRate);
    Move s2Move = getFail(s2.nextMove, misexecutionRate);

    s1.onMove(getFail(s2Move, miscommunicationRate));
    s2.onMove(getFail(s1Move, miscommunicationRate));

    s1.points += pointMatrix[s1Move][s2Move];
    s2.points += pointMatrix[s2Move][s1Move];
}

int main()
{
    const int cores = std::thread::hardware_concurrency();
    const int maxThreads = cores;

    const bool useParallelProcessing = false;

    auto totalStart = std::chrono::high_resolution_clock::now();

    const double startingMiscommunicationRate = 0.00;
    const double startingMisexecutionRate = 0.00;

    const double miscommunicationRateIncrement = 0.01;
    const double misexecutionRateIncrement = 0.01;

    const int totalRounds = 100;
    const int iterationCount = 1000;

    std::array<double, totalRounds> miscommunicationRates;
    std::array<double, totalRounds> misexecutionRates;
    for (int i = 0; i < totalRounds; i++)
    {
        miscommunicationRates[i] = startingMiscommunicationRate + miscommunicationRateIncrement * i;
        misexecutionRates[i] = startingMisexecutionRate + misexecutionRateIncrement * i;
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

    std::array<StrategyData, strategyConstructors.size()> strategies;

    for (int i = 0; i < strategyConstructors.size(); i++)
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

        std::vector<std::thread> threads;

        for (int i = 0; i < strategies.size(); i++)
        {
            for (int j = i; j < strategies.size(); j++)
            {

                if (useParallelProcessing)
                {
                    if (threads.size() >= maxThreads)
                    {
                        for (auto &t : threads)
                        {
                            t.join();
                        }
                        threads.clear();
                    }

                    threads.push_back(std::thread(
                        [i, j, &strategies, miscommunicationRates, misexecutionRates, u, iterationCount]()
                        {
                            StrategyData &sd1 = strategies[i];
                            StrategyData &sd2 = strategies[j];

                            std::unique_ptr<Strategy> s1_ = sd1.constructor();
                            std::unique_ptr<Strategy> s2_ = sd2.constructor();

                            Strategy &s1 = *s1_;
                            Strategy &s2 = *s2_;

                            s1.reset();
                            s2.reset();

                            for (int k = 0; k < iterationCount; k++)
                            {
                                runIteration(s1, s2, miscommunicationRates[u], misexecutionRates[u]);
                            }

                            sd1.totalPoints[u] += s1.points;
                            sd2.totalPoints[u] += s2.points;
                        }));
                }
                else
                {
                    StrategyData &sd1 = strategies[i];
                    StrategyData &sd2 = strategies[j];

                    std::unique_ptr<Strategy> s1_ = sd1.constructor();
                    std::unique_ptr<Strategy> s2_ = sd2.constructor();

                    Strategy &s1 = *s1_;
                    Strategy &s2 = *s2_;

                    s1.reset();
                    s2.reset();

                    for (int k = 0; k < iterationCount; k++)
                    {
                        runIteration(s1, s2, miscommunicationRates[u], misexecutionRates[u]);
                    }

                    sd1.totalPoints[u] += s1.points;
                    sd2.totalPoints[u] += s2.points;
                }
            }
        }

        // for (auto &t : threads)
        // {
        //     t.join();
        // }

        auto roundEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> roundDuration = roundEnd - roundStart;
        double estimatedTime = roundDuration.count() * (totalRounds - (u + 1));
        std::cout << "[NOTICE] Round " << u << " complete, Time: " << roundDuration.count() << " seconds\n"
                  << "[NOTICE] Estimated time: " << estimatedTime << " seconds  (" << estimatedTime / 60 << " minutes)" << std::endl;
    }

    for (int i = 0; i < strategyConstructors.size(); i++)
    {
        auto &strategy = strategies[i];

        for (int j = 0; j < totalRounds; j++)
        {
            strategy.averagePoints[j] = 1.0 * strategy.totalPoints[j] / (strategyConstructors.size() + 1);
        }
    }

    std::string fullData = "";

    for (StrategyData s : strategies)
    {
        std::string data = s.name + ",";
        for (int i = 0; i < totalRounds; i++)
        {
            data += std::to_string(miscommunicationRates[i]) + "," + std::to_string(misexecutionRates[i]) + "," + std::to_string(s.averagePoints[i]) + ",";
        }

        fullData += data + "\n";
        std::cout << data << std::endl;
    }

    auto totalEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> totalDuration = totalEnd - totalStart;
    std::cout << "[NOTICE] Total time taken: " << totalDuration.count() << " seconds" << std::endl;

    return 0;
}