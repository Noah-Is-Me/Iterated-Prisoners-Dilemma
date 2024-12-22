#ifndef HELPER_H
#define HELPER_H

double randomDouble(double min, double max);

int randomInt(int min, int max);

bool randomChance(double probability);

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

#endif