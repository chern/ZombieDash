// William Chern, UCLA CS 32 Project 3 ("Zombie Dash"), Winter 2019

#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld {
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool canMoveTo(int x, int y) const;
    bool overlapsWithPlayer(int x, int y) const;
    int citizensRemaining() const;
    int zombiesRemaining() const;
    void finishLevel();
    void addVaccinesToPlayer(int num);
    void addFlamethrowerChargersToPlayer(int num);
    void addLandminesToPlayer(int num);
    void inflictFlameDamageAround(int x, int y);
    void inflictVomitDamageAround(int x, int y);
    void addFlames(int num, int originalX, int originalY, Direction d);
private:
    std::list<Actor*> m_actors;
    Penelope* m_player;
    int m_citizens;
    int m_zombies;
    bool m_levelFinished;
    void loadLevel();
    bool overlapsWith(int x1, int y1, int x2, int y2) const;
    bool overlapsWithFlameBlockingObject(int x, int y) const;
};

#endif // STUDENTWORLD_H_
