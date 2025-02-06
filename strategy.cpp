#include "strategy.h"
#include "helper.h"

void Strategy::setup(double probCopAfterCop, double probCopAfterDef, double probCopFirst, double frequency)
{
    this->probCopAfterCop = probCopAfterCop;
    this->probCopAfterDef = probCopAfterDef;
    this->probCopFirst = probCopFirst;
    this->frequency = frequency;
    this->points = 0;
    this->averagePayoff = 0;
}

void Strategy::setup(double probCopAfterCop, double probCopAfterDef, double probCopFirst)
{
    this->probCopAfterCop = probCopAfterCop;
    this->probCopAfterDef = probCopAfterDef;
    this->probCopFirst = probCopFirst;
    this->points = 0;
    this->averagePayoff = 0;
}

Move Strategy::getNextMove(Move opponentMove) const
{
    double probCop = opponentMove == cooperate ? this->probCopAfterCop : this->probCopAfterDef;
    return randomChance(probCop) ? cooperate : defect;
}

Move Strategy::getFirstMove() const
{
    return randomChance(this->probCopFirst) ? cooperate : defect;
}

void Strategy::addPoints(int value)
{

    std::lock_guard<std::mutex> lock(this->mtx);
    this->points += value;
}

// Move constructor
Strategy::Strategy(Strategy &&other) noexcept
    : probCopAfterCop(other.probCopAfterCop),
      probCopAfterDef(other.probCopAfterDef),
      probCopFirst(other.probCopFirst),
      points(other.points)
{
    other.points = 0;
}

// Move assignment operator
Strategy &Strategy::operator=(Strategy &&other) noexcept
{
    if (this != &other)
    {
        probCopAfterCop = other.probCopAfterCop;
        probCopAfterDef = other.probCopAfterDef;
        probCopFirst = other.probCopFirst;
        points = other.points;
        other.points = 0;
    }
    return *this;
}
