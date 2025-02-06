
#include <stdio.h>
#include <iostream>
#include <array>
#include <unordered_map>
#include <chrono>
#include <thread>

#include "strategy.h"

void joinThreads(std::vector<std::thread> &threads)
{
    for (std::thread &t : threads)
    {
        t.join();
    }
    threads.clear();
}

void runIteration(Strategy &s1, Strategy &s2, Move &s1Move, Move &s2Move, double miscommunicationRate, double misexecutionRate)
{
    Move s1Move_ = s1Move;
    Move s2Move_ = s2Move;

    // Move s1Move_ = getFail(s1Move, misexecutionRate);
    // Move s2Move_ = getFail(s2Move, misexecutionRate);

    // s1.addPoints(pointMatrix[s1Move_][s2Move_]);
    // s2.addPoints(pointMatrix[s2Move_][s1Move_]);

    s1Move = cooperate;
    s2Move = cooperate;

    // s1Move = s1.getNextMove(s2Move_);
    // s2Move = s2.getNextMove(s1Move_);

    // s1Move = s1.getNextMove(getFail(s2Move_, miscommunicationRate));
    // s2Move = s2.getNextMove(getFail(s1Move_, miscommunicationRate));
}

void runMatchup(Strategy &s1, Strategy &s2, double miscommunicationRate, double misexecutionRate, int iterationCount)
{
    Move s1Move = s1.getFirstMove();
    Move s2Move = s2.getFirstMove();

    /*
    const double q1 = s1.probCopAfterCop;
    const double p1 = s1.probCopAfterDef;
    const double q2 = s2.probCopAfterCop;
    const double p2 = s2.probCopAfterDef;

    const double const1 = (q1 + (p1 - q1) * q2) / (1 - (p1 - q1) * (p2 - q2));
    const double const2 = (q2 + (p2 - q2) * q1) / (1 - (p2 - q2) * (p1 - q1));
    const double s1Payoff = 1 + (4 * const2) - const1 - (const1 * const2);
    const double s2Payoff = 1 + (4 * const1) - const2 - (const2 * const1);

    s1.averagePayoff += s1Payoff;
    s2.averagePayoff += s2Payoff;

    */

    for (int k = 0; k < iterationCount; k++)
    {
        // runIteration(s1, s2, s1Move, s2Move, miscommunicationRate, misexecutionRate);

        Move s1Move_ = getFail(s1Move, misexecutionRate);
        Move s2Move_ = getFail(s2Move, misexecutionRate);

        // s1.addPoints(pointMatrix[s1Move_][s2Move_]);
        // s2.addPoints(pointMatrix[s2Move_][s1Move_]);

        s1.points += pointMatrix[s1Move_][s2Move_];
        s2.points += pointMatrix[s2Move_][s1Move_];

        s1Move = s1.getNextMove(getFail(s2Move_, miscommunicationRate));
        s2Move = s2.getNextMove(getFail(s1Move_, miscommunicationRate));
    }
}

template <std::size_t N1>
void runRoundRobin(int generation, std::array<Strategy *, N1> &strategies, double miscommunicationRate, double misexecutionRate, int iterationCount, bool parallelProcessMatchups, int maxMatchupThreads, bool giveGenerationUpdates)
{
    auto roundStart = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;

    for (int i = 0; i < strategies.size(); i++)
    {
        for (int j = i + 1; j < strategies.size(); j++)
        {
            Strategy &s1 = *strategies[i];
            Strategy &s2 = *strategies[j];

            if (parallelProcessMatchups)
            {
                if (threads.size() >= maxMatchupThreads)
                {
                    joinThreads(threads);
                }

                threads.push_back(std::thread(runMatchup, std::ref(s1), std::ref(s2), miscommunicationRate, misexecutionRate, iterationCount));
            }
            else
            {
                runMatchup(s1, s2, miscommunicationRate, misexecutionRate, iterationCount);
            }
        }
    }

    if (parallelProcessMatchups)
        joinThreads(threads);

    if (giveGenerationUpdates && generation % 5 == 0)
    {
        // int totalRounds = miscommunicationRates.size();
        auto roundEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> roundDuration = roundEnd - roundStart;
        // double estimatedTime = roundDuration.count() * (totalRounds - (u + 1));
        std::cout << "[NOTICE] Generation " << generation << " complete, Time: " << roundDuration.count() << " seconds" << std::endl;
        //<< "[NOTICE] Estimated time: " << estimatedTime << " seconds  (" << estimatedTime / 60 << " minutes)" << std::endl;
    }
}

template <std::size_t N1>
GenerationData getGenerationData(/*const std::array<Strategy *, N1> &strategies*/ const std::array<Strategy, N1> &strategiesSchematic)
{
    /*
    double averageProbCopAfterCop = 0;
    double averageProbCopAfterDef = 0;

    // double averagePoints;
    // TODO: Will have to use hashtables or something to combine scores of the same strategies

    for (const Strategy &strategy : strategies)
    {
        averageProbCopAfterCop += strategy.probCopAfterCop;
        averageProbCopAfterDef += strategy.probCopAfterDef;
    }

    averageProbCopAfterCop /= N1;
    averageProbCopAfterDef /= N1;

    return GenerationData{averageProbCopAfterCop, averageProbCopAfterDef};
    */

    double averageProbCopAfterCop = 0;
    double averageProbCopAfterDef = 0;
    // double totalFrequency = 0;

    for (const Strategy &strategy : strategiesSchematic)
    {
        averageProbCopAfterCop += strategy.probCopAfterCop * strategy.frequency;
        averageProbCopAfterDef += strategy.probCopAfterDef * strategy.frequency;
        // totalFrequency += strategy.frequency;
    }

    // std::cout << averageProbCopAfterCop << ", " << averageProbCopAfterDef << ", " << totalFrequency << std::endl;

    return GenerationData{averageProbCopAfterCop, averageProbCopAfterDef};
}

template <std::size_t N1>
void outputPopulationData(int generation, /*const std::array<Strategy *, N1> &strategies*/ const std::array<Strategy, N1> &strategiesSchematic)
{
    /*
    std::string fullData = "[GRAPH 1]" + std::to_string(generation) + ",";

    for (const Strategy &strategy : strategies)
    {
        fullData += std::to_string(strategy.probCopAfterCop) + ", " + std::to_string(strategy.probCopAfterDef) + ",";
    }

    std::cout << fullData << std::endl;

    */

    std::string fullData = "[GRAPH 1]" + std::to_string(generation) + ",";

    for (const Strategy &strategy : strategiesSchematic)
    {
        fullData += std::to_string(strategy.probCopAfterCop) + ", " + std::to_string(strategy.probCopAfterDef) + "," + std::to_string(strategy.frequency) + ",";
    }

    std::cout << fullData << std::endl;
}

template <std::size_t N1, std::size_t N2>
void copyStrategiesFromSchematic(std::array<Strategy, N1> &strategiesSchematic, std::array<Strategy *, N2> &strategies)
{
    int currentOffspring = 0;

    for (Strategy &strategy : strategiesSchematic)
    {
        int offspringCount = strategy.frequency * N2;

        for (int i = 0; i < offspringCount; i++)
        {
            if (currentOffspring < N2)
                strategies[currentOffspring++] = &strategy;
            else // offspring array is full
                return;
        }
    }

    // randomly create offspring if offspring array has empty spaces (due to roundoff error)
    while (currentOffspring < N2)
    {
        // std::cout << currentOffspring << std ::endl;
        Strategy &randomStrategy = strategiesSchematic[randomInt(0, N1)];
        strategies[currentOffspring++] = &randomStrategy;
    }
}

template <std::size_t N1, std::size_t N2>
void generateOffspring(std::array<Strategy, N1> &strategiesSchematic, std::array<Strategy *, N2> &strategies)
{
    // double totalAveragePayoff = 0;
    int totalPoints = 0;
    for (const Strategy &strategy : strategiesSchematic)
    {
        totalPoints += strategy.points;
        // totalAveragePayoff += strategy.averagePayoff;
    }

    for (Strategy &strategy : strategiesSchematic)
    {
        // double newFrequency = strategy.frequency / (N1 - 1) * strategy.averagePayoff / totalAveragePayoff;
        // double newFrequency = strategy.averagePayoff / totalAveragePayoff;
        // strategy.frequency = newFrequency;
        double newFrequency = 1.0 * strategy.points / totalPoints;
        strategy.frequency = newFrequency;
        strategy.points = 0;

        // int offspringCount = newFrequency * N2;
        //  int offspringCount = (1.0 * strategy.points / totalPoints) * N2;

        // std::cout << offspringCount << ", " << newFrequency << std::endl;
        /*
        for (int i = 0; i < offspringCount; i++)
        {
            if (currentOffspring < N2)
                strategies[currentOffspring++] = &strategy;
            else // offspring array is full
                return;
        }
        */
    }

    copyStrategiesFromSchematic(strategiesSchematic, strategies);
}

template <std::size_t N1>
void outputData(const std::array<GenerationData, N1> &generationData)
{
    std::string fullData = "[GRAPH 2]";

    for (const GenerationData &data : generationData)
    {
        fullData += std::to_string(data.averageProbCopAfterCop) + ", " + std::to_string(data.averageProbCopAfterDef) + ",";
    }

    std::cout << fullData << std::endl;

    /*
    double averageProbCopAfterCop = 0;
    double averageProbCopAfterDef = 0;

    // double averagePoints;
    // TODO: Will have to use hashtables or something to combine scores of the same strategies

    for (const Strategy &strategy : strategies)
    {
        std::string data = std::to_string(strategy.probCopAfterCop) + "," + std::to_string(strategy.probCopAfterDef) + "," + std::to_string(strategy.points) + ",";
        // std::cout << data << std::endl;

        averageProbCopAfterCop += strategy.probCopAfterCop;
        averageProbCopAfterDef += strategy.probCopAfterDef;
    }

    averageProbCopAfterCop /= N1;
    averageProbCopAfterDef /= N1;

    std::cout << std::to_string(averageProbCopAfterCop) + ", " + std::to_string(averageProbCopAfterDef) << std::endl;

    // return fullData;
    */
}

int main()
{
    auto totalStart = std::chrono::high_resolution_clock::now();

    const int cores = std::thread::hardware_concurrency();

    const int maxMatchupThreads = cores;
    const bool parallelProcessMatchups = false;
    // TODO: parallel process is slowing down the program for some reason

    const bool giveGenerationUpdates = true;

    const double startingMiscommunicationRate = 0.00;
    const double startingMisexecutionRate = 0.00;

    const double miscommunicationRateIncrement = 0.01;
    const double misexecutionRateIncrement = 0.00;

    const int totalGenerations = 100;
    const int iterationCount = 30;

    const int strategyTypeCount = 100;
    const int strategyCount = 300;

    const int frameFrequency = 1;

    // TODO:
    // const double startingMutationRate = 0.00;
    // const mutationRateIncrement = 0.00;

    // -----------------

    // TODO: these rates should be constant over an entire simulation
    /*
    std::array<double, totalGenerations> miscommunicationRates;
    std::array<double, totalGenerations> misexecutionRates;
    for (int i = 0; i < totalGenerations; i++)
    {
        miscommunicationRates[i] = startingMiscommunicationRate + miscommunicationRateIncrement * i;
        misexecutionRates[i] = startingMisexecutionRate + misexecutionRateIncrement * i;
    }
    */
    double miscommunicationRate = 0.00;
    double misexecutionRate = 0.00;

    std::array<GenerationData, totalGenerations> generationData;

    std::array<Strategy, strategyTypeCount> strategiesSchematic;
    std::array<Strategy *, strategyCount> strategies;

    for (int i = 0; i < strategiesSchematic.size(); i++)
    {
        double probCopAfterCop = randomDouble();
        double probCopAfterDef = randomDouble();
        double probCopFirst = randomDouble();
        strategiesSchematic[i].setup(probCopAfterCop, probCopAfterDef, probCopFirst, 1.0 / strategyTypeCount);
    }

    // for (int i = 0; i < strategyTypeCount; i++)
    // {
    //     int n = i % (strategyCount / strategyTypeCount);
    //     for (int j = 0; j < n; j++)
    //     {
    //         strategies[i * n + j] = &strategiesSchematic[i];
    //     }
    // }

    // for (int i = 0; i < 1; i++)
    // {
    //     strategiesSchematic[i].setup(1.0, 0.0, 1.00, 1.0 / strategyCount);
    // }
    copyStrategiesFromSchematic(strategiesSchematic, strategies);

    generationData[0] = getGenerationData(strategiesSchematic);
    outputPopulationData(0, strategiesSchematic);
    for (int generation = 1; generation <= totalGenerations; generation++)
    {
        runRoundRobin(generation, strategies, miscommunicationRate, misexecutionRate, iterationCount, parallelProcessMatchups, maxMatchupThreads, giveGenerationUpdates);
        generateOffspring(strategiesSchematic, strategies);

        // if (true || generation % 5 == 0)
        // {
        //     for (int i = 0; i < 3; i++)
        //     {
        //         strategiesSchematic[i].setup(1.0, 0.00, 1.00);
        //     }
        // }

        if (generation % frameFrequency == 0)
        {
            outputPopulationData(generation, strategiesSchematic);
        }

        generationData[generation] = getGenerationData(strategiesSchematic);
    }

    outputPopulationData(totalGenerations, strategiesSchematic);
    outputData(generationData);

    // TODO: find out why this stopwatch is cooked
    /*
    auto totalEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> totalDuration = totalEnd - totalStart;
    std::cout << "[NOTICE] Total time taken: " << totalDuration.count() << " seconds\n"
              << "[NOTICE] Average generation time : " << totalDuration.count() / totalGenerations << " seconds " << std::endl;
    */

    return 0;
}
