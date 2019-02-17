#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor: public GraphObject {
public:
    Actor(int imageID, int startX, int startY, Direction startDir, int depth, StudentWorld* sw);
    virtual void doSomething() = 0;
    bool alive() const;
    StudentWorld* getStudentWorld() const;
    void setDead();
private:
    bool m_alive;
    StudentWorld* m_studentWorld;
};

class Human: public Actor {
public:
    Human(int imageID, int startX, int startY, Direction startDir, int depth, StudentWorld* sw);
    bool infected() const;
    int infections() const;
    void infect();
    // uninfect?
private:
    bool m_infected;
    int m_infections;
};

class Penelope: public Human {
public:
    Penelope(int startX, int startY, StudentWorld* sw);
    virtual void doSomething();
private:
    int m_landmines;
    int m_flamethrowerCharges;
    int m_vaccines;
};

#endif // ACTOR_H_
