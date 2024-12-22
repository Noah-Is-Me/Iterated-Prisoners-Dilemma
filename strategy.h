#ifndef STRATEGY_H
#define STRATEGY_H

#include <string>
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

class TitForTwoTats : public Strategy
{
public:
    TitForTwoTats();
    int turnNum;

    void onCooperate() override;

    void onDefect() override;
};

class GrimTrigger : public Strategy
{
public:
    GrimTrigger();
    bool triggered;

    void onCooperate() override;

    void onDefect() override;
};

class PavLov : public Strategy
{
public:
    PavLov();

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

#endif