#include <stdio.h>
#include <iostream>
#include <array>
#include <memory>

#include "strategy.h"

void runIteration(Strategy &s1, Strategy &s2)
{
    Move s1Move = s1.nextMove;
    Move s2Move = s2.nextMove;

    s1.onMove(s2Move);
    s2.onMove(s1Move);

    s1.points += pointMatrix[s1Move][s2Move];
    s2.points += pointMatrix[s2Move][s1Move];

    // std::cout << s1.name << " Points: " << s1.points << ", "
    //           << s2.name << " Points: " << s2.points << std::endl;
}

int main()
{
    std::array<std::unique_ptr<Strategy>, 8> strategies = {
        std::make_unique<TitForTat>(),
        std::make_unique<ForgivingTitForTat>(),
        std::make_unique<AlwaysDefect>(),
        std::make_unique<TitForTwoTats>(),
        std::make_unique<GrimTrigger>(),
        std::make_unique<PavLov>(),
        std::make_unique<AlwaysCooperate>(),
        std::make_unique<Random>()};

    for (int i = 0; i < strategies.size(); i++)
    {
        for (int j = i + 1; j < strategies.size(); j++)
        {
            Strategy &s1 = *strategies[i];
            Strategy &s2 = *strategies[j];

            s1.reset();
            s2.reset();

            for (int u = 0; u < 100; u++)
            {
                runIteration(s1, s2);
            }

            std::cout << s1.name << " Points: " << s1.points << ", "
                      << s2.name << " Points: " << s2.points << "\n"
                      << std::endl;
        }
    }

    return 0;
}
