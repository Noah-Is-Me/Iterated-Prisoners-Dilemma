#include "strategy.h"
#include "helper.h"

void Strategy::onMove(Move move)
{
    move == cooperate ? onCooperate() : onDefect();
}

void Strategy::reset()
{
    nextMove = firstMove;
    points = 0;
}

Strategy::~Strategy() = default;

TitForTat::TitForTat()
{
    name = "Tit-for-Tat";
    firstMove = cooperate;
}

void TitForTat::onCooperate()
{
    nextMove = cooperate;
}

void TitForTat::onDefect()
{
    nextMove = defect;
}

ForgivingTitForTat::ForgivingTitForTat()
{
    name = "Forgiving Tit-for-Tat";
    firstMove = cooperate;
}

void ForgivingTitForTat::onCooperate()
{
    nextMove = cooperate;
}

void ForgivingTitForTat::onDefect()
{
    if (randomChance(0.2))
        nextMove = cooperate;
    else
        nextMove = defect;
}

AlwaysDefect::AlwaysDefect()
{
    name = "Always Defect";
    firstMove = defect;
}

void AlwaysDefect::onCooperate()
{
    nextMove = defect;
}

void AlwaysDefect::onDefect()
{
    nextMove = defect;
}

TitForTwoTats::TitForTwoTats()
{
    name = "Tit-for-Two-Tats";
    firstMove = cooperate;
    turnNum = 0;
}

void TitForTwoTats::onCooperate()
{
    turnNum++;
    nextMove = defect;
}

void TitForTwoTats::onDefect()
{
    if (turnNum++ < 2)
        nextMove = cooperate;
    else
        nextMove = defect;
}

GrimTrigger::GrimTrigger()
{
    name = "Grim Trigger";
    firstMove = cooperate;
    triggered = false;
}

void GrimTrigger::onCooperate()
{
    nextMove = triggered ? defect : cooperate;
}

void GrimTrigger::onDefect()
{
    triggered = true;
    nextMove = defect;
}

PavLov::PavLov()
{
    name = "PavLov";
    firstMove = cooperate;
}

void PavLov::onCooperate()
{
    return; // no change
}

void PavLov::onDefect()
{
    if (nextMove == cooperate)
        nextMove = defect;
    else
        nextMove = cooperate;
}

AlwaysCooperate::AlwaysCooperate()
{
    name = "Always Cooperate";
    firstMove = cooperate;
}

void AlwaysCooperate::onCooperate()
{
    nextMove = cooperate;
}

void AlwaysCooperate::onDefect()
{
    nextMove = cooperate;
}

Random::Random()
{
    name = "Random";
    firstMove = randomChance(0.5) ? cooperate : defect;
}

void Random::onCooperate()
{
    nextMove = randomChance(0.5) ? cooperate : defect;
}

void Random::onDefect()
{
    nextMove = randomChance(0.5) ? cooperate : defect;
}