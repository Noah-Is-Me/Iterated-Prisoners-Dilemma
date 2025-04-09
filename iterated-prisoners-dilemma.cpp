
#include <stdio.h>
#include <iostream>
#include <array>
#include <unordered_map>
#include <chrono>
#include <thread>

#ifdef __linux__
#include <pthread.h>
#endif

#include "strategy.h"

void joinThreads(std::vector<std::thread> &threads)
{
    // std::cout << "Joining threads..." << std::endl;
    for (std::thread &t : threads)
    {
        t.join();
    }
    threads.clear();
}

void runMatchup(Strategy &s1, Strategy &s2, double miscommunicationRate, double misexecutionRate, int iterationCount)
{
    // auto matchupStart = std::chrono::high_resolution_clock::now();

    Move s1Move = s1.getFirstMove();
    Move s2Move = s2.getFirstMove();

    // std::cout << "start: " << s1.points << ", " << s2.points << std::endl;

    int s1Points = 0;
    int s2Points = 0;

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
        Move s1Move_ = getFail(s1Move, misexecutionRate);
        Move s2Move_ = getFail(s2Move, misexecutionRate);

        s1Points += pointMatrix[s1Move_][s2Move_];
        s2Points += pointMatrix[s2Move_][s1Move_];

        s1Move = s1.getNextMove(getFail(s2Move_, miscommunicationRate));
        s2Move = s2.getNextMove(getFail(s1Move_, miscommunicationRate));
    }

    s1.addPoints(s1Points);
    s2.addPoints(s2Points);

    // std::cout << "end: " << s1.points << ", " << s2.points << std::endl;

    // s1.points += s1Points;
    // s2.points += s2Points;

    // auto matchupEnd = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> matchDuration = matchupEnd - matchupStart;
    // std::cout << "[NOTICE] Matchup complete, Time: " << matchDuration.count() << " seconds" << std::endl;
}

template <std::size_t N1>
void runLineup(int i, std::array<Strategy, N1> &strategies, double miscommunicationRate, double misexecutionRate, int iterationCount)
{
    // auto lineupStart = std::chrono::high_resolution_clock::now();

    Strategy &s1 = strategies[i];

    for (int j = i + 1; j < strategies.size(); j++)
    {
        Strategy &s2 = strategies[j];
        runMatchup(s1, s2, miscommunicationRate, misexecutionRate, iterationCount);
    }

    // auto lineupEnd = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> lineupDuration = lineupEnd - lineupStart;
    // std::cout << "[NOTICE] Lineup complete, Time: " << lineupDuration.count() << " seconds" << std::endl;
}

template <std::size_t N1>
void runRoundRobin(int generation, std::array<Strategy, N1> &strategies, double miscommunicationRate, double misexecutionRate, int iterationCount, bool parallelProcessLineups, int maxThreads, bool giveGenerationUpdates)
{
    auto roundStart = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;

    for (int i = 0; i < strategies.size(); i++)
    {
        if (parallelProcessLineups)
        {

            if (threads.size() >= maxThreads / 2)
            {
                joinThreads(threads);
            }

            threads.push_back(std::thread(runLineup<N1>, i, std::ref(strategies), miscommunicationRate, misexecutionRate, iterationCount));
        }
        else
        {
            runLineup(i, strategies, miscommunicationRate, misexecutionRate, iterationCount);
        }
    }

    if (parallelProcessLineups)
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
GenerationData getGenerationData(const std::array<Strategy, N1> &strategies)
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

    for (const Strategy &strategy : strategies)
    {
        averageProbCopAfterCop += strategy.probCopAfterCop;
        averageProbCopAfterDef += strategy.probCopAfterDef;
    }

    averageProbCopAfterCop /= N1;
    averageProbCopAfterDef /= N1;

    // std::cout << averageProbCopAfterCop << ", " << averageProbCopAfterDef << ", " << totalFrequency << std::endl;

    return GenerationData{averageProbCopAfterCop, averageProbCopAfterDef};
}

template <std::size_t N1>
void outputSingleGenerationData(int generation, const std::array<Strategy, N1> &strategies)
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

    for (const Strategy &strategy : strategies)
    {
        fullData += std::to_string(strategy.probCopAfterCop) + ", " + std::to_string(strategy.probCopAfterDef) + ",";
    }

    std::cout << fullData << std::endl;
}

template <std::size_t N1>
void generateOffspring(std::array<Strategy, N1> &strategies, double mutationStddev)
{
    int totalPoints = 0;
    for (const Strategy &strategy : strategies)
    {
        totalPoints += strategy.points;
        // std::cout << "[NOTICE] " << strategy.points << std::endl;
    }

    std::array<Strategy, N1> offspring;
    int currentOffspring = 0;

    for (const Strategy &strategy : strategies)
    {
        double newFrequency = 1.0 * strategy.points / totalPoints;
        // std::cout << newFrequency << std::endl;

        // int offspringCount = std::round(newFrequency * N1);
        int offspringCount = newFrequency * N1;

        // std::cout << offspringCount << ", " << newFrequency << std::endl;

        for (int i = 0; i < offspringCount; i++)
        {
            if (currentOffspring < N1)
            {
                double newProbcopAfterCop = clamp01(normalDist(strategy.probCopAfterCop, mutationStddev));
                double newProbCopAfterDef = clamp01(normalDist(strategy.probCopAfterDef, mutationStddev));
                double newProbCopFirst = clamp01(normalDist(strategy.probCopFirst, mutationStddev));
                offspring[currentOffspring++].setup(newProbcopAfterCop, newProbCopAfterDef, newProbCopFirst);
            }
            else
            { // offspring array is full
                // std::cerr << "ERROR: too many offspring!" << std::endl;
                strategies = std::move(offspring);
                return;
            }
        }
    }

    // std::cout << "Count before extra: " << currentOffspring << std ::endl;

    // randomly create offspring if offspring array has empty spaces (due to roundoff error)
    while (currentOffspring < N1)
    {
        // std::cout << "EXTRA: " << currentOffspring << std ::endl;
        Strategy &randomStrategy = strategies[randomInt(0, N1)];
        double newProbcopAfterCop = clamp01(normalDist(randomStrategy.probCopAfterCop, mutationStddev));
        double newProbCopAfterDef = clamp01(normalDist(randomStrategy.probCopAfterDef, mutationStddev));
        double newProbCopFirst = clamp01(normalDist(randomStrategy.probCopFirst, mutationStddev));
        offspring[currentOffspring++].setup(newProbcopAfterCop, newProbCopAfterDef, newProbCopFirst);
    }

    strategies = std::move(offspring);

    // TODO: Implement mutations
}

void outputData(const std::vector<GenerationData> &generationData, const std::vector<double> &stabilityValues)
{
    std::string fullData = "[GRAPH 2]";

    for (int i = 0; i < generationData.size(); i++)
    {
        fullData += std::to_string(generationData[i].averageProbCopAfterCop) + ", " +
                    std::to_string(generationData[i].averageProbCopAfterDef) + "," +
                    std::to_string(stabilityValues[i]) + ",";
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

double calculateStability(const std::vector<GenerationData> &generationData, int slidingWindowSize)
{
    const int totalValues = static_cast<int>(generationData.size());
    if (totalValues < slidingWindowSize)
        return 1;

    std::vector<double> slidingWindowCAC;
    std::vector<double> slidingWindowCAD;
    slidingWindowCAC.reserve(slidingWindowSize);
    slidingWindowCAD.reserve(slidingWindowSize);

    int endingX = totalValues;
    int startingX = std::max(endingX - slidingWindowSize, 0);
    // std::vector<double> slidingWindow(yValues.begin() + startingX, yValues.begin() + endingX + 1);
    for (int i = startingX; i < endingX; i++)
    {
        slidingWindowCAC.push_back(generationData[i].averageProbCopAfterCop);
        slidingWindowCAD.push_back(generationData[i].averageProbCopAfterDef);
    }
    double stddevCAC = getStddev(slidingWindowCAC);
    double stddevCAD = getStddev(slidingWindowCAD);

    return std::max(stddevCAC, stddevCAD);
}

int main(int argc, char *argv[])
{
    auto totalStart = std::chrono::high_resolution_clock::now();

    const int cores = std::thread::hardware_concurrency();
    const int maxThreads = cores;
    std::cout << "[NOTICE] Cores: " << cores << std::endl;

    bool parallelProcessLineups = false;
    /* Parallel process slows down speed because the matchup
       lengths aren't long enough to justify thread creation */

    const bool giveGenerationUpdates = true;

    double miscommunicationRate = 0;
    double misexecutionRate = 0.0;
    double mutationStddev = 0.005;
    int totalGenerations = 10;
    int iterationCount = 100;
    const int strategyCount = 100;
    double stabilityThreshold = 0.005;
    int slidingWindowSize = 20;

    if (argc > 1)
    {
        miscommunicationRate = std::atof(argv[1]);
        misexecutionRate = std::atof(argv[2]);
        mutationStddev = std::atof(argv[3]);
        totalGenerations = std::atof(argv[4]);
        iterationCount = std::atof(argv[5]);
        // strategyCount = std::atof(argv[6]);
        stabilityThreshold = std::atof(argv[7]);
        slidingWindowSize = std::atof(argv[8]);
        parallelProcessLineups = std::atof(argv[9]);
    }

    std::cout << "[NOTICE]\n"
              << "miscommunicationRate: " << miscommunicationRate << "\n"
              << "misexecutionRate: " << misexecutionRate << "\n"
              << "mutationStddev: " << mutationStddev << "\n"
              << "totalGenerations: " << totalGenerations << "\n"
              << "iterationCount: " << iterationCount << "\n"
              << "strategyCount: " << strategyCount << "\n"
              << "parallelProcessLineups: " << parallelProcessLineups << std::endl;

    const int frameFrequency = 1;

    std::vector<GenerationData> generationData;
    std::vector<double> stabilityValues;
    std::array<Strategy, strategyCount> strategies;

    for (int i = 0; i < strategies.size(); i++)
    {
        double probCopAfterCop = randomDouble();
        double probCopAfterDef = randomDouble();
        double probCopFirst = randomDouble();
        strategies[i].setup(probCopAfterCop, probCopAfterDef, probCopFirst);
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

    generationData.push_back(getGenerationData(strategies));
    stabilityValues.push_back(calculateStability(generationData, slidingWindowSize));
    // outputSingleGenerationData(0, strategies);
    for (int generation = 1; generation <= totalGenerations; generation++)
    {
        runRoundRobin(generation, strategies, miscommunicationRate, misexecutionRate, iterationCount, parallelProcessLineups, maxThreads, giveGenerationUpdates);
        generateOffspring(strategies, mutationStddev);
        // if (generation==100)
        //     strategies[0].setup(1, 0, 1);

        // if (true || generation % 5 == 0)
        // {
        //     for (int i = 0; i < 3; i++)
        //     {
        //         strategiesSchematic[i].setup(1.0, 0.00, 1.00);
        //     }
        // }

        // if (generation % 5 == 0)
        // {
        //     for (int i = 0; i < 10; i++)
        //     {
        //         strategies[randomInt(0, strategyCount)] = &strategiesSchematic[0];
        //     }
        // }

        // for (int i = 0; 10 < 1; i++)
        // {
        //     strategies[randomInt(0, strategyCount)] = &strategiesSchematic[randomInt(0,strategyTypeCount)];
        // }

        if (generation % frameFrequency == 0)
        {
            outputSingleGenerationData(generation, strategies);
        }

        generationData.push_back(getGenerationData(strategies));

        double stability = calculateStability(generationData, slidingWindowSize);
        stabilityValues.push_back(stability);
        if (stability <= stabilityThreshold)
        {
            std::cout << "[NOTICE] STABLE AT: generation=" << generation << std::endl;
            break;
        }
    }

    outputSingleGenerationData(totalGenerations, strategies);
    outputData(generationData, stabilityValues);

    // TODO: find out why this stopwatch is cooked
    /*
    auto totalEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> totalDuration = totalEnd - totalStart;
    std::cout << "[NOTICE] Total time taken: " << totalDuration.count() << " seconds\n"
              << "[NOTICE] Average generation time : " << totalDuration.count() / totalGenerations << " seconds " << std::endl;
    */

    return 0;
}
