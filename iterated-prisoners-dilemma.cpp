#include <stdio.h>
#include <iostream>
#include <array>
#include <memory>
#include <functional>

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
    double failRate = 0.05;

    std::array<std::function<std::unique_ptr<Strategy>()>, 8> strategies = {
        []()
        { return std::make_unique<TitForTat>(); },
        []()
        { return std::make_unique<ForgivingTitForTat>(); },
        []()
        { return std::make_unique<AlwaysDefect>(); },
        []()
        { return std::make_unique<TitForTwoTats>(); },
        []()
        { return std::make_unique<GrimTrigger>(); },
        []()
        { return std::make_unique<PavLov>(); },
        []()
        { return std::make_unique<AlwaysCooperate>(); },
        []()
        { return std::make_unique<Random>(); },
    };

    for (int i = 0; i < strategies.size(); i++)
    {
        for (int j = i + 1; j < strategies.size(); j++)
        {
            std::unique_ptr<Strategy> s1_ = strategies[i]();
            std::unique_ptr<Strategy> s2_ = strategies[j]();

            Strategy &s1 = *s1_;
            Strategy &s2 = *s2_;

            s1.reset();
            s2.reset();

            for (int u = 0; u < 100; u++)
            {
                runIteration(s1, s2, failRate);
            }

            std::cout << s1.name << " Points: " << s1.points << ", "
                      << s2.name << " Points: " << s2.points << "\n"
                      << std::endl;
        }
    }

    return 0;
}
