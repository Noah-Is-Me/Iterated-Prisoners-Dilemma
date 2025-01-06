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
    std::string name;
    Move firstMove;

    std::vector<int> totalPoints;
    std::vector<double> averagePoints;
    std::mutex mtx;

    Move onMove(Move move) const;

    virtual ~Strategy();

    void addPoints(int index, int value)
    {
        std::lock_guard<std::mutex> lock(mtx);
        totalPoints[index] += value;
    }

private:
    virtual Move onCooperate() const = 0;
    virtual Move onDefect() const = 0;
};

struct matchupData
{
    const Strategy &s1;
    const Strategy &s2;
    Move s1NextMove;
    Move s2NextMove;
    int s1Points;
    int s2Points;
    std::vector<int> s1Arguments;
    std::vector<int> s2Arguments;
};

class TitForTat : public Strategy
{
public:
    TitForTat();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class ForgivingTitForTat : public Strategy
{
public:
    ForgivingTitForTat();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class AlwaysDefect : public Strategy
{
public:
    AlwaysDefect();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class AlwaysCooperate : public Strategy
{
public:
    AlwaysCooperate();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class Random : public Strategy
{
public:
    Random();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class ProbabilityCooperator : public Strategy
{
public:
    ProbabilityCooperator();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class ProbabilityDefector : public Strategy
{
public:
    ProbabilityDefector();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class SuspiciousTitForTat : public Strategy
{
public:
    SuspiciousTitForTat();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class GenerousTitForTat : public Strategy
{
public:
    GenerousTitForTat();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class GradualTitForTat : public Strategy
{
public:
    GradualTitForTat();
    int continueDefecting;
    int highestDefectStreak;

    Move onCooperate() const override;

    Move onDefect() const override;
};

class ImperfectTitForTat : public Strategy
{
public:
    ImperfectTitForTat();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class TitForTwoTats : public Strategy
{
public:
    TitForTwoTats();
    int consecutiveDefectCount;

    Move onCooperate() const override;

    Move onDefect() const override;
};

class TwoTitsForTat : public Strategy
{
public:
    TwoTitsForTat();
    bool defectAgain;

    Move onCooperate() const override;

    Move onDefect() const override;
};

/*
class OmegaTitForTat : public Strategy
{
public:
    OmegaTitForTat();

    Move onCooperate() const override;

    Move onDefect() const override;
};
*/

class GrimTrigger : public Strategy
{
public:
    GrimTrigger();
    bool triggered;

    Move onCooperate() const override;

    Move onDefect() const override;
};

class Pavlov : public Strategy
{
public:
    Pavlov();
    Move previousMove;

    Move onCooperate() const override;

    Move onDefect() const override;
};

/*
class NPavlov : public Strategy
{
public:
    NPavlov();

    Move onCooperate() const override;

    Move onDefect() const override;
};
*/

/*
class AdaptivePavlov : public Strategy
{
public:
    AdaptivePavlov();

    Move onCooperate() const override;

    Move onDefect() const override;
};
*/

/*
class Reactive : public Strategy
{
public:
    Reactive();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class MemoryOne : public Strategy
{
public:
    MemoryOne();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class ZeroDeterminant : public Strategy
{
public:
    ZeroDeterminant();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class Equalizer : public Strategy
{
public:
    Equalizer();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class Extortionary : public Strategy
{
public:
    Extortionary();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class Generous : public Strategy
{
public:
    Generous();

    Move onCooperate() const override;

    Move onDefect() const override;
};

class Generous : public Strategy
{
public:
    Good();

    Move onCooperate() const override;

    Move onDefect() const override;
};
*/

#endif