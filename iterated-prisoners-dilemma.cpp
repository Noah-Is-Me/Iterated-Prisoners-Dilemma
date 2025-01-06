#include <stdio.h>
#include <iostream>
#include <array>
#include <unordered_map>
#include <chrono>
#include <thread>

#include "strategy.h"

void runIteration(matchupData &data, double miscommunicationRate, double misexecutionRate)
{
    Move s1Move = getFail(data.s1NextMove, misexecutionRate);
    Move s2Move = getFail(data.s2NextMove, misexecutionRate);

    data.s1Points += pointMatrix[s1Move][s2Move];
    data.s2Points += pointMatrix[s2Move][s1Move];

    data.s1NextMove = data.s1.onMove(getFail(s2Move, miscommunicationRate));
    data.s2NextMove = data.s2.onMove(getFail(s1Move, miscommunicationRate));
}

template <std::size_t N1, std::size_t N2>
void runMatchup(int u, int i, int j, std::array<std::unique_ptr<Strategy>, N1> &strategies, const std::array<double, N2> &miscommunicationRates, const std::array<double, N2> &misexecutionRates, int iterationCount)
{
    std::unique_ptr<Strategy> &s1_ = strategies[i];
    std::unique_ptr<Strategy> &s2_ = strategies[j];

    Strategy &s1 = *s1_;
    Strategy &s2 = *s2_;

    matchupData data = {
        s1,
        s2,
        s1.firstMove,
        s2.firstMove,
        0,
        0,
    };

    for (int k = 0; k < iterationCount; k++)
    {
        runIteration(data, miscommunicationRates[u], misexecutionRates[u]);
    }

    s1.addPoints(u, data.s1Points);
    s2.addPoints(u, data.s2Points);
}

void joinThreads(std::vector<std::thread> &threads)
{
    for (std::thread &t : threads)
    {
        t.join();
    }
    threads.clear();
}

template <std::size_t N1, std::size_t N2>
void runRound(int u, std::array<std::unique_ptr<Strategy>, N1> &strategies, const std::array<double, N2> &miscommunicationRates, const std::array<double, N2> &misexecutionRates, int iterationCount, int parallelProcessMatchups, int maxMatchupThreads, int giveRoundUpdates)
{
    auto roundStart = std::chrono::high_resolution_clock::now();

    /* int totalMatchups = (strategies.size() * (strategies.size() + 1)) / 2;
    runThreads(totalMatchups, strategies.size(), strategies.size(), parallelProcessMatchups, maxMatchupThreads,
               runMatchup<N1, N2>,
               std::ref(strategies), iterationCount, std::cref(miscommunicationRates), std::cref(misexecutionRates));
    */

    std::vector<std::thread> threads;

    for (int i = 0; i < strategies.size(); i++)
    {
        for (int j = i; j < strategies.size(); j++)
        {
            if (parallelProcessMatchups)
            {
                if (threads.size() >= maxMatchupThreads)
                {
                    joinThreads(threads);
                }

                threads.push_back(std::thread(runMatchup<N1, N2>, u, i, j, std::ref(strategies), std::cref(miscommunicationRates), std::cref(misexecutionRates), iterationCount));
            }
            else
            {
                runMatchup(u, i, j, strategies, miscommunicationRates, misexecutionRates, iterationCount);
            }
        }
    }

    if (parallelProcessMatchups)
        joinThreads(threads);

    if (giveRoundUpdates && u % 5 == 0)
    {
        int totalRounds = miscommunicationRates.size();
        auto roundEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> roundDuration = roundEnd - roundStart;
        double estimatedTime = roundDuration.count() * (totalRounds - (u + 1));
        std::cout << "[NOTICE] Round " << u << " complete, Time: " << roundDuration.count() << " seconds\n"
                  << "[NOTICE] Estimated time: " << estimatedTime << " seconds  (" << estimatedTime / 60 << " minutes)" << std::endl;
    }
}

/* template <typename Function, typename... Args>
void runThreads(int iterations, int iterator1, int iterator2, bool useParallelProcessing, int maxThreads, Function &&func, Args &&...args)
{
    std::vector<std::thread> threads;

    int i1 = 0;
    int i2 = 0;

    for (int i = 0; i < iterations; i++)
    {
        if (useParallelProcessing)
        {
            if (threads.size() >= maxThreads)
            {
                joinThreads(threads);
            }

            threads.push_back(std::thread(
                std::forward<Function>(func), i, i1, i2++,
                std::forward<Args>(args)...));
        }
        else
        {
            std::forward<Function>(func)(i, i1, i2++, std::forward<Args>(args)...);
        }

        if (i1 % iterator1 == 0)
            i1 = 0;

        if (i2 % iterator2 == 0)
        {
            i1++;
            i2 = 0;
        }
    }

    if (useParallelProcessing)
        joinThreads(threads);
}
*/

template <std::size_t N1, std::size_t N2>
void outputData(const std::array<std::unique_ptr<Strategy>, N1> &strategies, const std::array<double, N2> &miscommunicationRates, const std::array<double, N2> &misexecutionRates)
{
    std::string fullData = "";

    for (const std::unique_ptr<Strategy> &strategy : strategies)
    {
        std::string data = strategy->name + ",";
        for (int i = 0; i < N2; i++)
        {
            data += std::to_string(miscommunicationRates[i]) + "," + std::to_string(misexecutionRates[i]) + "," + std::to_string(strategy->averagePoints[i]) + ",";
        }

        fullData += data + "\n";
        std::cout << data << std::endl;
    }
}

int main()
{
    auto totalStart = std::chrono::high_resolution_clock::now();

    const int cores = std::thread::hardware_concurrency();

    const int maxRoundThreads = cores;
    const int maxMatchupThreads = cores;

    const bool parallelProcessRounds = false;
    const bool parallelProcessMatchups = false;

    const bool giveRoundUpdates = false;

    const double startingMiscommunicationRate = 0.00;
    const double startingMisexecutionRate = 0.00;

    const double miscommunicationRateIncrement = 0.01;
    const double misexecutionRateIncrement = 0.01;

    const int totalRounds = 100 + 1;
    const int iterationCount = 1000;

    std::array<double, totalRounds> miscommunicationRates;
    std::array<double, totalRounds> misexecutionRates;
    for (int i = 0; i < totalRounds; i++)
    {
        miscommunicationRates[i] = startingMiscommunicationRate + miscommunicationRateIncrement * i;
        misexecutionRates[i] = startingMisexecutionRate + misexecutionRateIncrement * i;
    }

    std::array<std::unique_ptr<Strategy>, 15> strategies = {
        std::make_unique<TitForTat>(),
        std::make_unique<ForgivingTitForTat>(),
        std::make_unique<AlwaysDefect>(),
        std::make_unique<AlwaysCooperate>(),
        std::make_unique<Random>(),
        std::make_unique<ProbabilityCooperator>(),
        std::make_unique<ProbabilityDefector>(),
        std::make_unique<SuspiciousTitForTat>(),
        std::make_unique<GenerousTitForTat>(),
        std::make_unique<GradualTitForTat>(),
        std::make_unique<ImperfectTitForTat>(),
        std::make_unique<TitForTwoTats>(),
        std::make_unique<TwoTitsForTat>(),
        std::make_unique<GrimTrigger>(),
        std::make_unique<Pavlov>()};

    for (int i = 0; i < strategies.size(); i++)
    {
        strategies[i]->totalPoints.assign(totalRounds, 0);
        strategies[i]->averagePoints.assign(totalRounds, 0);
    }

    /* runThreads(totalRounds, parallelProcessRounds, maxRoundThreads, runRound<strategies.size(), totalRounds>,
               std::ref(strategies), iterationCount, std::cref(miscommunicationRates), std::cref(misexecutionRates), parallelProcessMatchups, giveRoundUpdates, totalRounds, maxMatchupThreads);
    */

    std::vector<std::thread> threads;

    for (int u = 0; u < totalRounds; u++)
    {
        if (parallelProcessRounds)
        {
            if (threads.size() >= maxRoundThreads)
            {
                joinThreads(threads);
            }

            threads.push_back(std::thread(
                runRound<strategies.size(), totalRounds>, u, std::ref(strategies), std::cref(miscommunicationRates), std::cref(misexecutionRates), iterationCount, parallelProcessMatchups, maxMatchupThreads, giveRoundUpdates));
        }
        else
        {
            runRound(u, strategies, miscommunicationRates, misexecutionRates, iterationCount, parallelProcessMatchups, maxMatchupThreads, giveRoundUpdates);
        }
    }

    if (parallelProcessRounds)
        joinThreads(threads);

    for (std::unique_ptr<Strategy> &strategy : strategies)
    {
        for (int i = 0; i < totalRounds; i++)
        {
            strategy->averagePoints[i] = 1.0 * strategy->totalPoints[i] / (strategies.size() + 1);
        }
    }

    outputData(strategies, miscommunicationRates, misexecutionRates);

    auto totalEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> totalDuration = totalEnd - totalStart;
    std::cout << "[NOTICE] Total time taken: " << totalDuration.count() << " seconds\n"
              << "[NOTICE] Average round time : " << totalDuration.count() / totalRounds << " seconds " << std::endl;

    return 0;
}