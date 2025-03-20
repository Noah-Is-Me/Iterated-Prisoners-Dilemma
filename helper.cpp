#include "helper.h"
#include <random>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <cmath>

// static thread_local std::mt19937 gen(std::random_device{}());
// static thread_local std::uniform_real_distribution<> dis(0.0, 1.0);

static std::mt19937 gen(std::random_device{}());
static std::uniform_real_distribution<> dis(0.0, 1.0);

/* double randomDouble(double min, double max)
{
    static thread_local std::mt19937 gen(std::random_device{}());
    static thread_local std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen) * (max - min) + min;
}
*/

bool randomChance(double probability)
{
    return dis(gen) < probability;
}

double randomDouble()
{
    return dis(gen);
}

int randomInt(int startIncluding, int endExcluding)
{
    std::uniform_int_distribution<> dist(startIncluding, endExcluding - 1);
    return dist(gen);
}

double normalDist(double mean, double stddev)
{
    std::normal_distribution<double> dist(mean, stddev);
    return dist(gen);
}

double clamp01(double n)
{
    return std::clamp(n, 0.0, 1.0);
}

const int pointMatrix[2][2] = {
    {cop_cop, cop_def},
    {def_cop, def_def}};

Move getFail(Move move, double failRate)
{
    double probChange = failRate * 0.5;
    if (dis(gen) < probChange)
    {
        return (move == cooperate) ? defect : cooperate;
    }
    return move;
}

/* Move getFail(Move move, double failRate)
{
    if (randomChance(failRate))
    {
        return randomChance(0.5) ? cooperate : defect;
    }
    return move;
}
*/

double getStddev(const std::vector<double> &values)
{
    int size = values.size();

    double sum = 0.0;
    for (int i = 0; i < size; ++i)
    {
        sum += values[i];
    }
    double mean = sum / size;

    double standardDeviation = 0.0;
    for (int i = 0; i < size; ++i)
    {
        standardDeviation += pow(values[i] - mean, 2);
    }

    return sqrt(standardDeviation / size);
}
