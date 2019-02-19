// William Chern, UCLA CS 32 Project 3 ("Zombie Dash"), Winter 2019

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
    virtual bool blocksMovement() const = 0;
    virtual bool canBeInfected() const = 0; // by vomit
    virtual bool canBeDamaged() const = 0; // by flame
private:
    bool m_alive;
    StudentWorld* m_studentWorld;
};

class Human: public Actor {
public:
    Human(int imageID, int startX, int startY, Direction startDir, int depth, StudentWorld* sw);
    virtual bool blocksMovement() const;
    virtual bool canBeInfected() const;
    virtual bool canBeDamaged() const;
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
    int getNumLandmines() const;
    int getNumFlamethrowerCharges() const;
    int getNumVaccines() const;
private:
    int m_landmines;
    int m_flamethrowerCharges;
    int m_vaccines;
};

class Wall: public Actor {
public:
    Wall(int x, int y, StudentWorld* sw);
    virtual void doSomething();
    virtual bool blocksMovement() const;
    virtual bool canBeInfected() const;
    virtual bool canBeDamaged() const;
private:
};

class FallIntoObject: public Actor {
public:
    FallIntoObject(int imageID, int x, int y, int depth, StudentWorld* sw);
    virtual bool blocksMovement() const;
    virtual bool canBeInfected() const;
    virtual bool canBeDamaged() const;
};

class Exit: public FallIntoObject {
public:
    Exit(int x, int y, StudentWorld* sw);
    virtual void doSomething();
private:
};

#endif // ACTOR_H_
