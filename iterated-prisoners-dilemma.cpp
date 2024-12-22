#include <stdio.h>
#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>

enum Move
{
    cooperate,
    defect
};

class Strategy
{
public:
    std::string name;
    int points;

    Move firstMove;
    Move nextMove;

    virtual void onCooperate() = 0;
    virtual void onDefect() = 0;

    // Virtual destructor to allow proper cleanup of derived class objects
    virtual ~Strategy() = default;
};

class TitForTat : public Strategy
{
public:
    TitForTat()
    {
        name = "Tit-for-Tat";
        nextMove = cooperate;
    }

    void onCooperate() override
    {
        nextMove = cooperate;
    }

    void onDefect() override
    {
        nextMove = defect;
    }
};

class ForgivingTitForTat : public Strategy
{
public:
    ForgivingTitForTat()
    {
        name = "Forgiving Tit-for-Tat";
        nextMove = cooperate;
    }

    void onCooperate() override
    {
        nextMove = cooperate;
    }

    void onDefect() override
    {

        nextMove = defect;
    }
};

int main()
{
    printf("hello world!");

    std::unordered_map<std::string, Strategy> strategies;

    for (int i = 0; i < 1; i++)
    {
        // runIteration(tit_for_tat, forgiving_tit_for_tat);
    }

    return 1;
}

/*
void runIteration(Strategy s1, Strategy s2)
{
    Move s1Move = s1.nextMove;
    Move s2Move = s2.nextMove;

    switch (s1Move)
    {
    case cooperate:
        s2.onCooperate();
    }
}
*/