// William Chern, UCLA CS 32 Project 3 ("Zombie Dash"), Winter 2019

#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

const int DEAD_KILLED = 1;
const int DEAD_SAVED_USED = 2;

class Actor: public GraphObject {
public:
    Actor(int imageID, int startX, int startY, Direction startDir, int depth, StudentWorld* sw);
    virtual void doSomething() = 0;
    bool alive() const;
    void setDead(int deadID);
    StudentWorld* getStudentWorld() const;
    virtual bool blocksMovement() const;
    virtual bool canBeInfected() const; // by vomit
    virtual bool canBeDamaged() const; // by flame
    virtual bool blocksFlames() const;
    virtual bool canFall() const;
    virtual bool canSetOffLandmine() const;
    virtual bool canBeDetonated() const;
private:
    StudentWorld* m_studentWorld;
    bool m_alive;
    virtual void setDeadSpecialized(int deadID);
};

class Agent: public Actor {
public:
    Agent(int imageID, int startX, int startY, StudentWorld* sw);
    virtual bool blocksMovement() const;
    virtual bool canBeDamaged() const; // by flame
    virtual bool canFall() const;
    virtual bool canSetOffLandmine() const;
protected:
    bool attemptToMoveTo(int x, int y, Direction d);
private:
    virtual void computeDestinationCoordinates(int& destX, int& destY, Direction d) = 0;
};

class Human: public Agent {
public:
    Human(int imageID, int startX, int startY, StudentWorld* sw);
    virtual bool canBeInfected() const;
    int infections() const;
    void infect();
protected:
    bool infected() const;
    void vaccinate();
private:
    bool m_infected;
    int m_infections;
    virtual void playInfectedSoundIfApplicable();
};

class Penelope: public Human {
public:
    Penelope(int startX, int startY, StudentWorld* sw);
    virtual void doSomething();
    int getNumLandmines() const;
    int getNumFlamethrowerCharges() const;
    int getNumVaccines() const;
    void addVaccines(int num);
    void addLandmines(int num);
    void addFlamethrowerCharges(int num);
private:
    int m_vaccines;
    int m_landmines;
    int m_flamethrowerCharges;
    void deployFlames();
    void deployLandmine();
    void useVaccine();
    virtual void setDeadSpecialized(int deadID);
    virtual void computeDestinationCoordinates(int& destX, int& destY, Direction d);
};

class Citizen: public Human {
public:
    Citizen(int startX, int startY, StudentWorld* sw);
    virtual void doSomething();
private:
    bool m_paralyzed; // used for "paralysis ticks"
    virtual void setDeadSpecialized(int deadID);
    virtual void playInfectedSoundIfApplicable();
    virtual void computeDestinationCoordinates(int& destX, int& destY, Direction d);
    void attemptToFollowPlayer();
    void attemptToFleeFromZombie();
};

class Zombie: public Agent {
public:
    Zombie(int startX, int startY, StudentWorld* sw);
    virtual void doSomething();
protected:
    void setMovementPlanDistance(int mv);
    int movementPlanDistance() const;
private:
    bool m_paralyzed;
    int m_movementPlanDistance;
    void computeVomitCoordinates(int& vx, int& vy);
    virtual void computeDestinationCoordinates(int& destX, int& destY, Direction d);
    virtual void determineNewMovementPlan() = 0;
    virtual void setDeadSpecialized(int deadID);
    virtual void setZombieDead() = 0;
};

class DumbZombie: public Zombie {
public:
    DumbZombie(int startX, int startY, StudentWorld* sw);
private:
    virtual void determineNewMovementPlan();
    virtual void setZombieDead();
};

class SmartZombie: public Zombie {
public:
    SmartZombie(int startX, int startY, StudentWorld* sw);
private:
    virtual void determineNewMovementPlan();
    virtual void setZombieDead();
};

class Wall: public Actor {
public:
    Wall(int x, int y, StudentWorld* sw);
    virtual void doSomething();
    virtual bool blocksMovement() const;
    virtual bool blocksFlames() const;
};

class FallIntoObject: public Actor {
public:
    FallIntoObject(int imageID, int x, int y, int depth, StudentWorld* sw);
};

class Exit: public FallIntoObject {
public:
    Exit(int x, int y, StudentWorld* sw);
    virtual void doSomething();
    virtual bool blocksFlames() const;
};

class Pit: public FallIntoObject {
public:
    Pit(int x, int y, StudentWorld* sw);
    virtual void doSomething();
};

class Goodie: public Actor {
public:
    Goodie(int imageID, int x, int y, StudentWorld* sw);
    virtual void doSomething();
    virtual bool canBeDamaged() const; // by flame
private:
    virtual void setDeadSpecialized(int deadID);
    virtual void giveSpecializedGoodie() = 0;
};

class VaccineGoodie: public Goodie {
public:
    VaccineGoodie(int x, int y, StudentWorld* sw);
private:
    virtual void giveSpecializedGoodie();
};

class GasCanGoodie: public Goodie {
public:
    GasCanGoodie(int x, int y, StudentWorld* sw);
private:
    virtual void giveSpecializedGoodie();
};

class LandmineGoodie: public Goodie {
public:
    LandmineGoodie(int x, int y, StudentWorld* sw);
private:
    virtual void giveSpecializedGoodie();
};

class Projectile: public Actor {
public:
    Projectile(int imageID, int x, int y, Direction dir, StudentWorld* sw);
    virtual void doSomething();
private:
    int m_ticks;
    virtual void inflictSpecializedDamage() = 0;
};

class Flame: public Projectile {
public:
    Flame(int x, int y, Direction dir, StudentWorld* sw);
private:
    virtual void inflictSpecializedDamage();
};

class Vomit: public Projectile {
public:
    Vomit(int x, int y, Direction dir, StudentWorld* sw);
private:
    virtual void inflictSpecializedDamage();
};

class Landmine: public Actor {
public:
    Landmine(int x, int y, StudentWorld* sw);
    virtual void doSomething();
    virtual bool canBeDamaged() const;
    virtual bool canBeDetonated() const;
    void detonate();
private:
    int m_safetyTicks;
    bool m_active;
};

#endif // ACTOR_H_
