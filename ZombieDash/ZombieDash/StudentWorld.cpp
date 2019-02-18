// William Chern, UCLA CS 32 Project 3 ("Zombie Dash"), Winter 2019

#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw
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
    loadLevel();
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
    if (m_player->alive())
        m_player->doSomething();
    else
        return GWSTATUS_PLAYER_DIED;

    list<Actor*>::iterator actorsIter = m_actors.begin();
    while (actorsIter != m_actors.end()) {
        Actor* thisActor = *actorsIter;
        if (thisActor->alive()) {
            thisActor->doSomething();
            
            // if Penelope dies during this tick, return GWSTATUS_PLAYER_DIED
            if (!m_player->alive()) {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            
            // if Penelope completed the current level, return GWSTATUS_FINISHED_LEVEL
            
        }
        actorsIter++;
    }
    
    // Remove newly dead actors after each tick
    
    // Update the game status line
    // setGameStatText();
    
    return GWSTATUS_CONTINUE_GAME;
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
        // cout << "Erasing actor" << endl;
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

void StudentWorld::loadLevel() {
    Level lev(assetPath());
    
    ostringstream oss;  // oss is a name of our choosing.
    oss.fill('0');
    oss << "level" << setw(2) << getLevel() << ".txt";
    cout << oss.str() << endl;
    string levelFile = oss.str();
    
    // string levelFile = "level01.txt";
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found)
        cerr << "Cannot find " << levelFile << " data file" << endl;
    else if (result == Level::load_fail_bad_format)
        cerr << levelFile << " is improperly formatted" << endl;
    else if (result == Level::load_success) {
        cout << "Successfully loaded " << levelFile << endl;
        
        for (int x = 0; x < LEVEL_WIDTH; x++) {
            for (int y = 0; y < LEVEL_HEIGHT; y++) {
                Level::MazeEntry ge = lev.getContentsOf(x, y);
                int gameCoordinateX = x * SPRITE_WIDTH;
                int gameCoordinateY = y * SPRITE_HEIGHT;
                switch (ge) {
                    case Level::player:
                        m_player = new Penelope(gameCoordinateX, gameCoordinateY, this);
                        cout <<"Location (" << x << "," << y << ") is where Penelope starts" << endl;
                        break;
                    case Level::wall:
                        m_actors.emplace_back(new Wall(gameCoordinateX, gameCoordinateY, this));
                        cout << "Location (" << x << "," << y << ") holds a wall" << endl;
                        break;
                    case Level::empty:
                        // cout << "Location (" << x << "," << y << ") is empty" << endl;
                    default:
                        break;
                }
            }
        }
    }
}
