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
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    static double distance(int x1, int y1, int x2, int y2) ;
    bool playerCanMoveTo(int x, int y) const;
    bool agentCanMoveTo(Agent* ag, int destX, int destY) const;
    bool overlapsWithPlayer(int x, int y) const;
    bool overlapsWithCitizen(int x, int y) const;
    bool overlapsWithOrganism(int x, int y) const;
    Human* getNearestHuman(int x, int y) const;
    Citizen* getNearestCitizen(int x, int y) const;
    Zombie* getNearestZombie(int x, int y) const;
    double distanceToPlayer(int x, int y) const;
    double distanceToNearestZombie(int x, int y) const;
    int getPlayerX() const;
    int getPlayerY() const;
    int citizensRemaining() const;
    void markCitizenGone();
    int zombiesRemaining() const;
    void markZombieGone();
    void finishLevel();
    void addVaccinesToPlayer(int num);
    void addFlamethrowerChargesToPlayer(int num);
    void addLandminesToPlayer(int num);
    void inflictFlameDamageAround(int x, int y);
    void inflictVomitDamageAround(int x, int y);
    void addFlames(int num, int originalX, int originalY, Direction d);
    void addFlamesAround(int x, int y);
    void addActor(Actor* a);
private:
    std::list<Actor*> m_actors;
    Penelope* m_player;
    bool m_levelFinished;
    int m_citizens;
    int m_zombies;
    int loadLevel(int levelNum);
    bool canMoveTo(int fromX, int fromY, int toX, int toY) const;
    bool overlapsWith(int x1, int y1, int x2, int y2) const;
    bool overlapsWithAnyObject(int x, int y) const;
    bool overlapsWithFlameBlockingObject(int x, int y) const;
};

#endif // STUDENTWORLD_H_
