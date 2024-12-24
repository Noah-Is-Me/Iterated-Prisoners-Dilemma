#ifndef STRATEGY_H
#define STRATEGY_H

#include <functional>
#include <string>
#include <memory>

#include "helper.h"

class Strategy
{
public:
    std::string name;
    Move firstMove;
    Move nextMove;
    int points;

    void onMove(Move move);

    void reset();

    virtual ~Strategy();

private:
    virtual void onCooperate() = 0;
    virtual void onDefect() = 0;
};

class StrategyData
{
public:
    std::string name;
    std::vector<int> totalPoints;
    std::vector<double> averagePoints;
    std::function<std::unique_ptr<Strategy>()> constructor;
};

class TitForTat : public Strategy
{
public:
    TitForTat();

    void onCooperate() override;

    void onDefect() override;
};

class ForgivingTitForTat : public Strategy
{
public:
    ForgivingTitForTat();

    void onCooperate() override;

    void onDefect() override;
};

class AlwaysDefect : public Strategy
{
public:
    AlwaysDefect();

    void onCooperate() override;

    void onDefect() override;
};

class AlwaysCooperate : public Strategy
{
public:
    AlwaysCooperate();

    void onCooperate() override;

    void onDefect() override;
};

class Random : public Strategy
{
public:
    Random();

    void onCooperate() override;

    void onDefect() override;
};

class ProbabilityCooperator : public Strategy
{
public:
    ProbabilityCooperator();

    void onCooperate() override;

    void onDefect() override;
};

class ProbabilityDefector : public Strategy
{
public:
    ProbabilityDefector();

    void onCooperate() override;

    void onDefect() override;
};

class SuspiciousTitForTat : public Strategy
{
public:
    SuspiciousTitForTat();

    void onCooperate() override;

    void onDefect() override;
};

class GenerousTitForTat : public Strategy
{
public:
    GenerousTitForTat();

    void onCooperate() override;

    void onDefect() override;
};

class GradualTitForTat : public Strategy
{
public:
    GradualTitForTat();
    int continueDefecting;
    int highestDefectStreak;

    void onCooperate() override;

    void onDefect() override;
};

class ImperfectTitForTat : public Strategy
{
public:
    ImperfectTitForTat();

    void onCooperate() override;

    void onDefect() override;
};

class TitForTwoTats : public Strategy
{
public:
    TitForTwoTats();
    int consecutiveDefectCount;

    void onCooperate() override;

    void onDefect() override;
};

class TwoTitsForTat : public Strategy
{
public:
    TwoTitsForTat();
    bool defectAgain;

    void onCooperate() override;

    void onDefect() override;
};

/*
class OmegaTitForTat : public Strategy
{
public:
    OmegaTitForTat();

    void onCooperate() override;

    void onDefect() override;
};
*/

class GrimTrigger : public Strategy
{
public:
    GrimTrigger();
    bool triggered;

    void onCooperate() override;

    void onDefect() override;
};

class Pavlov : public Strategy
{
public:
    Pavlov();

    void onCooperate() override;

    void onDefect() override;
};

/*
class NPavlov : public Strategy
{
public:
    NPavlov();

    void onCooperate() override;

    void onDefect() override;
};
*/

/*
class AdaptivePavlov : public Strategy
{
public:
    AdaptivePavlov();

    void onCooperate() override;

    void onDefect() override;
};
*/

/*
class Reactive : public Strategy
{
public:
    Reactive();

    void onCooperate() override;

    void onDefect() override;
};

class MemoryOne : public Strategy
{
public:
    MemoryOne();

    void onCooperate() override;

    void onDefect() override;
};

class ZeroDeterminant : public Strategy
{
public:
    ZeroDeterminant();

    void onCooperate() override;

    void onDefect() override;
};

class Equalizer : public Strategy
{
public:
    Equalizer();

    void onCooperate() override;

    void onDefect() override;
};

class Extortionary : public Strategy
{
public:
    Extortionary();

    void onCooperate() override;

    void onDefect() override;
};

class Generous : public Strategy
{
public:
    Generous();

    void onCooperate() override;

    void onDefect() override;
};

class Generous : public Strategy
{
public:
    Good();

    void onCooperate() override;

    void onDefect() override;
};
*/

#endif