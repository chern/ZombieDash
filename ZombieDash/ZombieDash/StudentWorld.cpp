#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <list>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath) : GameWorld(assetPath) {
    m_player = nullptr;
    m_score = 0;
    m_level = 1;
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init() {
    m_player = new Penelope(200, 100, this);
    m_actors.emplace_back(new Wall(100, 150, this));
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
    m_player->doSomething();
    
    return GWSTATUS_CONTINUE_GAME;
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    // decLives();
    // return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::cleanUp() {
    delete m_player;
    m_player = nullptr;
    list<Actor*>::iterator actorsIter = m_actors.begin();
    while (actorsIter != m_actors.end()) {
        Actor* temp = *actorsIter;
        delete temp;
        temp = nullptr;
        actorsIter = m_actors.erase(actorsIter);
        // actorsIter++;
    }
}
