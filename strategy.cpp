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

ProbabilityCooperator::ProbabilityCooperator()
{
    name = "Probability Cooperator";
    firstMove = randomChance(0.8) ? cooperate : defect;
}

void ProbabilityCooperator::onCooperate()
{
    if (randomChance(0.8))
    {
        nextMove = cooperate;
    }
    else
        nextMove = defect;
}

void ProbabilityCooperator::onDefect()
{
    if (randomChance(0.8))
    {
        nextMove = cooperate;
    }
    else
        nextMove = defect;
}

ProbabilityDefector::ProbabilityDefector()
{
    name = "Probability Defector";
    firstMove = randomChance(0.8) ? defect : cooperate;
}

void ProbabilityDefector::onCooperate()
{
    if (randomChance(0.8))
    {
        nextMove = defect;
    }
    else
        nextMove = cooperate;
}

void ProbabilityDefector::onDefect()
{
    if (randomChance(0.8))
    {
        nextMove = defect;
    }
    else
        nextMove = cooperate;
}

SuspiciousTitForTat::SuspiciousTitForTat()
{
    name = "Suspicious Tit-for-Tat";
    firstMove = defect;
}

void SuspiciousTitForTat::onCooperate()
{
    nextMove = cooperate;
}

void SuspiciousTitForTat::onDefect()
{
    nextMove = defect;
}

GenerousTitForTat::GenerousTitForTat()
{
    name = "Generous Tit-for-Tat";
    firstMove = cooperate;
}

void GenerousTitForTat::onCooperate()
{
    nextMove = cooperate;
}

void GenerousTitForTat::onDefect()
{
    if (randomChance(
            std::min(
                (1.0 - 1.0 * (def_cop - cop_cop) / 1.0 * (cop_cop - cop_def)),
                (0.0 + 1.0 * (cop_cop - def_def) / 1.0 * (def_cop - def_def)))))
    {
        nextMove = cooperate;
    }
    else
        nextMove = defect;
}

GradualTitForTat::GradualTitForTat()
{
    name = "Gradual Tit-for-Tat";
    firstMove = cooperate;

    continueDefecting = 0;
    highestDefectStreak = 0;
}

void GradualTitForTat::onCooperate()
{
    continueDefecting--;

    if (continueDefecting > 0)
        nextMove = defect;
    else
    {
        nextMove = cooperate;
    }
}

void GradualTitForTat::onDefect()
{
    highestDefectStreak++;
    continueDefecting = highestDefectStreak;

    nextMove = defect;
}

ImperfectTitForTat::ImperfectTitForTat()
{
    name = "Randomized Tit-for-Tat";
    firstMove = cooperate;
}

void ImperfectTitForTat::onCooperate()
{
    if (randomChance(0.10))
    {
        nextMove = defect;
    }
    else
        nextMove = cooperate;
}

void ImperfectTitForTat::onDefect()
{
    if (randomChance(0.10))
    {
        nextMove = cooperate;
    }
    else
        nextMove = defect;
}

TitForTwoTats::TitForTwoTats()
{
    name = "Tit-for-Two-Tats";
    firstMove = cooperate;
    consecutiveDefectCount = 0;
}

void TitForTwoTats::onCooperate()
{
    consecutiveDefectCount = 0;
    nextMove = cooperate;
}

void TitForTwoTats::onDefect()
{
    consecutiveDefectCount++;
    if (consecutiveDefectCount > 2)
        nextMove = defect;
    else
        nextMove = cooperate;
}

TwoTitsForTat::TwoTitsForTat()
{
    name = "Two-Tits-for-Tat";
    firstMove = cooperate;
    defectAgain = false;
}

void TwoTitsForTat::onCooperate()
{
    if (defectAgain)
        nextMove = defect;
    else
        nextMove = cooperate;

    defectAgain = false;
}

void TwoTitsForTat::onDefect()
{
    defectAgain = true;
    nextMove = defect;
}

/*
OmegaTitForTat::OmegaTitForTat()
{
    name = "Omega Tit-for-Tat";
    firstMove = cooperate;
}

void OmegaTitForTat::onCooperate()
{
}

void OmegaTitForTat::onDefect()
{
}
*/

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

Pavlov::Pavlov()
{
    name = "Pavlov";
    firstMove = cooperate;
}

void Pavlov::onCooperate()
{
    return; // no change
}

void Pavlov::onDefect()
{
    if (nextMove == defect)
        nextMove = cooperate;
    else
        nextMove = defect;
}

/*
NPavlov::NPavlov()
{
    name = "n-Pavlov";
    firstMove = cooperate;
}

void NPavlov::onCooperate()
{
}

void NPavlov::onDefect()
{
}
*/

/*
AdaptivePavlov::AdaptivePavlov()
{
    name = "Adaptive Pavlov";
    firstMove = cooperate;
}

void AdaptivePavlov::onCooperate()
{
}

void AdaptivePavlov::onDefect()
{
}
*/

/*
Reactive::Reactive()
{
    name = "Reactive";
    firstMove = cooperate;
}

void Reactive::onCooperate()
{
}

void Reactive::onDefect()
{
}

MemoryOne::MemoryOne()
{
    name = "Memory-one";
    firstMove = cooperate;
}

void MemoryOne::onCooperate()
{
}

void MemoryOne::onDefect()
{
}

ZeroDeterminant::ZeroDeterminant()
{
    name = "Zero Determinant";
    firstMove = cooperate;
}

void ZeroDeterminant::onCooperate()
{
}

void ZeroDeterminant::onDefect()
{
}

Equalizer::Equalizer()
{
    name = "Equalizer";
    firstMove = cooperate;
}

void Equalizer::onCooperate()
{
}

void Equalizer::onDefect()
{
}

Extortionary::Extortionary()
{
    name = "Extortionary";
    firstMove = cooperate;
}

void Extortionary::onCooperate()
{
}

void Equalizer::onDefect()
{
}

Generous::Generous()
{
    name = "Generous";
    firstMove = cooperate;
}

void Generous::onCooperate()
{
}

void Generous::onDefect()
{
}

Generous::Equalizer()
{
    name = "Generous";
    firstMove = cooperate;
}

void Generous::onCooperate()
{
}

void Generous::onDefect()
{
}

*/
