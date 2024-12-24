#include "helper.h"
#include <random>
#include <algorithm>

double randomDouble(double min, double max)
{
    std::random_device rd;  // Obtain a random seed from the hardware
    std::mt19937 gen(rd()); // Mersenne Twister engine seeded with rd()
    std::uniform_real_distribution<> dis(min, max);

    return dis(gen);
}

int randomInt(int min, int max)
{
    std::random_device rd;  // Obtain a random seed from the hardware
    std::mt19937 gen(rd()); // Mersenne Twister engine seeded with rd()
    std::uniform_int_distribution<> dis(min, max);

    return dis(gen);
}

bool randomChance(double probability)
{
    return randomDouble(0.0, 1.0) < probability;
}

const int pointMatrix[2][2] = {
    {cop_cop, cop_def},
    {def_cop, def_def}};