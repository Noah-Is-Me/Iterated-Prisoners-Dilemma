#ifndef STRATEGY_H
#define STRATEGY_H

#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <mutex>

#include "helper.h"

class Strategy
{
public:
    double probCopAfterCop;
    double probCopAfterDef;
    double probCopFirst;

    std::mutex mtx;
    int points;

    void setup(double probCopAfterCop, double probCopAfterDef, double probCopFirst);
    Move getNextMove(Move opponentMove) const;
    Move getFirstMove() const;
    void addPoints(int value);

    Strategy() = default;
    Strategy(Strategy &&other) noexcept;            // Move constructor
    Strategy &operator=(Strategy &&other) noexcept; // Move assignment operator
    Strategy(const Strategy &) = delete;
    Strategy &operator=(const Strategy &) = delete;
    // Delete copy constructor and copy assignment operator to prevent accidental copying
};

#endif