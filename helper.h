#ifndef HELPER_H
#define HELPER_H

#include <vector>

// double randomDouble(double min, double max);

// int randomInt(int min, int max);

bool randomChance(double probability);

double randomDouble();

int randomInt(int startIncluding, int endExcluding);

double normalDist(double mean, double stddev);

double clamp01(double n);

enum Move
{
    cooperate,
    defect
};

enum PointValues
{
    cop_cop = 3,
    cop_def = 0,
    def_cop = 5,
    def_def = 1,
};

extern const int pointMatrix[2][2];

Move getFail(Move move, double failRate);

struct GenerationData
{
    double averageProbCopAfterCop;
    double averageProbCopAfterDef;
};

double getStddev(const std::vector<double> &);

#endif