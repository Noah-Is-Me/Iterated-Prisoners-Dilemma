#include "strategy.h"
#include "helper.h"

Move Strategy::onMove(Move move) const
{
    return move == cooperate ? onCooperate() : onDefect();
}

Strategy::~Strategy() = default;

TitForTat::TitForTat()
{
    name = "Tit-for-Tat";
    firstMove = cooperate;
}

Move TitForTat::onCooperate() const
{
    return cooperate;
}

Move TitForTat::onDefect() const
{
    return defect;
}

ForgivingTitForTat::ForgivingTitForTat()
{
    name = "Forgiving Tit-for-Tat";
    firstMove = cooperate;
}

Move ForgivingTitForTat::onCooperate() const
{
    return cooperate;
}

Move ForgivingTitForTat::onDefect() const
{
    if (randomChance(0.2))
        return cooperate;
    else
        return defect;
}

AlwaysDefect::AlwaysDefect()
{
    name = "Always Defect";
    firstMove = defect;
}

Move AlwaysDefect::onCooperate() const
{
    return defect;
}

Move AlwaysDefect::onDefect() const
{
    return defect;
}

AlwaysCooperate::AlwaysCooperate()
{
    name = "Always Cooperate";
    firstMove = cooperate;
}

Move AlwaysCooperate::onCooperate() const
{
    return cooperate;
}

Move AlwaysCooperate::onDefect() const
{
    return cooperate;
}

Random::Random()
{
    name = "Random";
    firstMove = randomChance(0.5) ? cooperate : defect;
}

Move Random::onCooperate() const
{
    return randomChance(0.5) ? cooperate : defect;
}

Move Random::onDefect() const
{
    return randomChance(0.5) ? cooperate : defect;
}
ProbabilityCooperator::ProbabilityCooperator()
{
    name = "Probability Cooperator";
    firstMove = randomChance(0.8) ? cooperate : defect;
}

Move ProbabilityCooperator::onCooperate() const
{
    if (randomChance(0.8))
    {
        return cooperate;
    }
    else
        return defect;
}

Move ProbabilityCooperator::onDefect() const
{
    if (randomChance(0.8))
    {
        return cooperate;
    }
    else
        return defect;
}

ProbabilityDefector::ProbabilityDefector()
{
    name = "Probability Defector";
    firstMove = randomChance(0.8) ? defect : cooperate;
}

Move ProbabilityDefector::onCooperate() const
{
    if (randomChance(0.8))
    {
        return defect;
    }
    else
        return cooperate;
}

Move ProbabilityDefector::onDefect() const
{
    if (randomChance(0.8))
    {
        return defect;
    }
    else
        return cooperate;
}

SuspiciousTitForTat::SuspiciousTitForTat()
{
    name = "Suspicious Tit-for-Tat";
    firstMove = defect;
}

Move SuspiciousTitForTat::onCooperate() const
{
    return cooperate;
}

Move SuspiciousTitForTat::onDefect() const
{
    return defect;
}

GenerousTitForTat::GenerousTitForTat()
{
    name = "Generous Tit-for-Tat";
    firstMove = cooperate;
}

Move GenerousTitForTat::onCooperate() const
{
    return cooperate;
}

Move GenerousTitForTat::onDefect() const
{
    if (randomChance(
            std::min(
                (1.0 - 1.0 * (def_cop - cop_cop) / 1.0 * (cop_cop - cop_def)),
                (0.0 + 1.0 * (cop_cop - def_def) / 1.0 * (def_cop - def_def)))))
    {
        return cooperate;
    }
    else
        return defect;
}

GradualTitForTat::GradualTitForTat()
{
    name = "Gradual Tit-for-Tat";
    firstMove = cooperate;

    continueDefecting = 0;
    highestDefectStreak = 0;
}

Move GradualTitForTat::onCooperate() const
{
    continueDefecting--;

    if (continueDefecting > 0)
        return defect;
    else
    {
        return cooperate;
    }
}

Move GradualTitForTat::onDefect() const
{
    highestDefectStreak++;
    continueDefecting = highestDefectStreak;

    return defect;
}

ImperfectTitForTat::ImperfectTitForTat() const
{
    name = "Randomized Tit-for-Tat";
    firstMove = cooperate;
}

Move ImperfectTitForTat::onCooperate() const
{
    if (randomChance(0.10))
    {
        return defect;
    }
    else
        return cooperate;
}

Move ImperfectTitForTat::onDefect() const
{
    if (randomChance(0.10))
    {
        return cooperate;
    }
    else
        return defect;
}

TitForTwoTats::TitForTwoTats()
{
    name = "Tit-for-Two-Tats";
    firstMove = cooperate;
    consecutiveDefectCount = 0;
}

Move TitForTwoTats::onCooperate() const
{
    consecutiveDefectCount = 0;
    return cooperate;
}

Move TitForTwoTats::onDefect() const
{
    consecutiveDefectCount++;
    if (consecutiveDefectCount > 2)
        return defect;
    else
        return cooperate;
}

TwoTitsForTat::TwoTitsForTat()
{
    name = "Two-Tits-for-Tat";
    firstMove = cooperate;
    defectAgain = false;
}

Move TwoTitsForTat::onCooperate() const
{
    if (defectAgain)
        return defect;
    else
        return cooperate;

    defectAgain = false;
}

Move TwoTitsForTat::onDefect() const
{
    defectAgain = true;
    return defect;
}

/*
OmegaTitForTat::OmegaTitForTat()const
{
    name = "Omega Tit-for-Tat";
    firstMove = cooperate;
}

 Move OmegaTitForTat::onCooperate()const
{
}

 Move OmegaTitForTat::onDefect()const
{
}
*/

GrimTrigger::GrimTrigger()
{
    name = "Grim Trigger";
    firstMove = cooperate;
    triggered = false;
}

Move GrimTrigger::onCooperate() const
{
    return triggered ? defect : cooperate;
}

Move GrimTrigger::onDefect() const
{
    triggered = true;
    return defect;
}

Pavlov::Pavlov()
{
    name = "Pavlov";
    firstMove = cooperate;
}

Move Pavlov::onCooperate() const
{
    return previousMove; // no change
}

Move Pavlov::onDefect() const
{
    if (previousMove == defect)
        return cooperate;
    else
        return defect;
}

/*
NPavlov::NPavlov()const
{
    name = "n-Pavlov";
    firstMove = cooperate;
}

 Move NPavlov::onCooperate()const
{
}

 Move NPavlov::onDefect()const
{
}
*/

/*
AdaptivePavlov::AdaptivePavlov()const
{
    name = "Adaptive Pavlov";
    firstMove = cooperate;
}

 Move AdaptivePavlov::onCooperate()const
{
}

 Move AdaptivePavlov::onDefect()const
{
}
*/

/*
Reactive::Reactive()const
{
    name = "Reactive";
    firstMove = cooperate;
}

 Move Reactive::onCooperate()const
{
}

 Move Reactive::onDefect()const
{
}

MemoryOne::MemoryOne()const
{
    name = "Memory-one";
    firstMove = cooperate;
}

 Move MemoryOne::onCooperate()const
{
}

 Move MemoryOne::onDefect()const
{
}

ZeroDeterminant::ZeroDeterminant()const
{
    name = "Zero Determinant";
    firstMove = cooperate;
}

 Move ZeroDeterminant::onCooperate()const
{
}

 Move ZeroDeterminant::onDefect()const
{
}

Equalizer::Equalizer()const
{
    name = "Equalizer";
    firstMove = cooperate;
}

 Move Equalizer::onCooperate()const
{
}

 Move Equalizer::onDefect()const
{
}

Extortionary::Extortionary()const
{
    name = "Extortionary";
    firstMove = cooperate;
}

 Move Extortionary::onCooperate()const
{
}

 Move Equalizer::onDefect()const
{
}

Generous::Generous()const
{
    name = "Generous";
    firstMove = cooperate;
}

 Move Generous::onCooperate()const
{
}

 Move Generous::onDefect()const
{
}

Generous::Equalizer()const
{
    name = "Generous";
    firstMove = cooperate;
}

 Move Generous::onCooperate()const
{
}

 Move Generous::onDefect()const
{
}

*/
