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
    virtual bool blocksFlames() const = 0;
    virtual bool canFall() const = 0;
private:
    bool m_alive;
    StudentWorld* m_studentWorld;
};

class Human: public Actor {
public:
    Human(int imageID, int startX, int startY, StudentWorld* sw);
    virtual bool blocksMovement() const;
    virtual bool canBeInfected() const;
    virtual bool canBeDamaged() const;
    virtual bool blocksFlames() const;
    virtual bool canFall() const;
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

class Citizen: public Human {
public:
    Citizen(int startX, int startY, StudentWorld* sw);
    virtual void doSomething();
private:
    unsigned long m_ticks; // used for "paralysis ticks"
};

class Zombie: public Actor {
public:
    Zombie(int startX, int startY, StudentWorld* sw);
    virtual void doSomething();
    virtual bool blocksMovement() const;
    virtual bool canBeInfected() const; // by vomit
    virtual bool canBeDamaged() const; // by flame
    virtual bool blocksFlames() const;
    virtual bool canFall() const;
private:
    unsigned long m_ticks;
    int m_movementPlanDistance;
};

class DumbZombie: public Zombie {
public:
    DumbZombie(int startX, int startY, StudentWorld* sw);
private:
};

class SmartZombie: public Zombie {
public:
    SmartZombie(int startX, int startY, StudentWorld* sw);
private:
};

class Wall: public Actor {
public:
    Wall(int x, int y, StudentWorld* sw);
    virtual void doSomething();
    virtual bool blocksMovement() const;
    virtual bool canBeInfected() const;
    virtual bool canBeDamaged() const;
    virtual bool blocksFlames() const;
    virtual bool canFall() const;
private:
};

class FallIntoObject: public Actor {
public:
    FallIntoObject(int imageID, int x, int y, int depth, StudentWorld* sw);
    virtual bool blocksMovement() const;
    virtual bool canBeInfected() const;
    virtual bool canBeDamaged() const;
    virtual bool canFall() const;
};

class Exit: public FallIntoObject {
public:
    Exit(int x, int y, StudentWorld* sw);
    virtual void doSomething();
    virtual bool blocksFlames() const;
private:
};

class Pit: public FallIntoObject {
public:
    Pit(int x, int y, StudentWorld* sw);
    virtual void doSomething();
    virtual bool blocksFlames() const;
};

#endif // ACTOR_H_
