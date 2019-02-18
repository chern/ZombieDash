#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <list>
using namespace std;

GameWorld* createStudentWorld(string assetPath) {
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath) : GameWorld(assetPath) {
    m_player = nullptr;
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init() {
    m_player = new Penelope(240, 240, this);
    m_actors.emplace_back(new Wall(64, 144, this));
    m_actors.emplace_back(new Wall(208, 192, this));
    m_actors.emplace_back(new Wall(0, 0, this));
    m_actors.emplace_back(new Wall(0, 240, this));
    m_actors.emplace_back(new Wall(240, 0, this));
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
    m_player->doSomething();

    list<Actor*>::iterator actorsIter = m_actors.begin();
    while (actorsIter != m_actors.end()) {
        Actor* thisActor = *actorsIter;
        if (thisActor->alive()) {
            thisActor->doSomething();
            
            // if Penelope dies during this tick, return GWSTATUS_PLAYER_DIED
            if (!m_player->alive())
                return GWSTATUS_PLAYER_DIED;
            
            // if Penelope completed the current level, return GWSTATUS_FINISHED_LEVEL
            
        }
        
        actorsIter++;
    }
    
    // Remove newly dead actors after each tick
    
    // Update the game status line
    // setGameStatText();
    
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

bool StudentWorld::canMoveTo(int x, int y) {
    list<Actor*>::iterator actorsIter = m_actors.begin();
    while (actorsIter != m_actors.end()) {
        Actor* a = *actorsIter;
        if (!a->blocksMovement()) {
            continue;
        } else {
            for (int callerX = x; callerX < x + SPRITE_WIDTH; callerX++) {
                for (int callerY = y; callerY < y + SPRITE_HEIGHT; callerY++) {
                    for (int actorX = a->getX(); actorX < a->getX()+SPRITE_WIDTH; actorX++) {
                        for (int actorY = a->getY(); actorY < a->getY()+SPRITE_HEIGHT; actorY++) {
                            if (callerX == actorX && callerY == actorY) {
                                return false;
                            }
                        }
                    }
                }
            }
        }
        actorsIter++;
    }
    return true;
}
