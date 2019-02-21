// William Chern, UCLA CS 32 Project 3 ("Zombie Dash"), Winter 2019

#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw()
#include <string>
#include <cmath>
#include <list>
using namespace std;

GameWorld* createStudentWorld(string assetPath) {
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath): GameWorld(assetPath) {
    m_player = nullptr;
    m_citizens = 0;
    m_zombies = 0;
    m_levelFinished = false;
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init() {
    loadLevel();
    m_citizens = 0;
    m_zombies = 0;
    m_levelFinished = false;
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
    if (m_player->alive())
        m_player->doSomething();
    else
        return GWSTATUS_PLAYER_DIED;

    list<Actor*>::iterator actorsIter = m_actors.begin();
    while (actorsIter != m_actors.end()) {
        Actor* a = *actorsIter;
        if (a->alive()) {
            a->doSomething();
            
            // if Penelope dies during this tick, return GWSTATUS_PLAYER_DIED
            if (!m_player->alive()) {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            
            // if Penelope completed the current level, return GWSTATUS_FINISHED_LEVEL
            if (m_levelFinished)
                return GWSTATUS_FINISHED_LEVEL;
        }
        actorsIter++;
    }
    
    // Remove newly dead actors after each tick
    actorsIter = m_actors.begin();
    while (actorsIter != m_actors.end()) {
        Actor* a = *actorsIter;
        if (!a->alive()) {
            delete a;
            a = nullptr;
            actorsIter = m_actors.erase(actorsIter);
        } else
            actorsIter++;
    }
    
    // Update the game status line
    string spaceSeparator = "  ";
    ostringstream oss;
    oss.fill('0');
    oss << "Score: " << setw(6) << getScore() << spaceSeparator;
    oss << "Level: " << getLevel() << spaceSeparator;
    oss << "Lives: "  << getLives() << spaceSeparator;
    oss << "Vaccines: " << m_player->getNumVaccines() << spaceSeparator;
    oss << "Flames: " << m_player->getNumFlamethrowerCharges() << spaceSeparator;
    oss << "Mines: " << m_player->getNumLandmines() << spaceSeparator;
    oss << "Infected: " << m_player->infections();
    string gameStats = oss.str();
    setGameStatText(gameStats);
    // cout << gameStats << endl;
    
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
    m_citizens = 0;
    m_zombies = 0;
}

bool StudentWorld::canMoveTo(int x, int y) const {
    list<Actor*>::const_iterator actorsIter = m_actors.begin();
    while (actorsIter != m_actors.end()) {
        Actor* a = *actorsIter;
        if (a->blocksMovement()) {
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

bool StudentWorld::overlapsWith(int x1, int y1, int x2, int y2) const {
    double centerX1 = x1 + (SPRITE_WIDTH/2);
    double centerY1 = y1 + (SPRITE_HEIGHT/2);
    double centerX2 = x2 + (SPRITE_WIDTH/2);
    double centerY2 = y2 + (SPRITE_HEIGHT/2);
    
    double deltaX = abs(centerX2 - centerX1);
    double deltaY = abs(centerY2 - centerY1);
    
    if (pow(deltaX, 2) + pow(deltaY, 2) <= pow(10, 2))
        return true;
    return false;
}

bool StudentWorld::overlapsWithPlayer(int x, int y) const {
    return overlapsWith(x, y, m_player->getX(), m_player->getY());
}

int StudentWorld::citizensRemaining() const {
    return m_citizens;
}

int StudentWorld::zombiesRemaining() const {
    return m_zombies;
}

void StudentWorld::finishLevel() {
    m_levelFinished = true;
}

void StudentWorld::addVaccinesToPlayer(int num) {
    m_player->addVaccines(num);
}

void StudentWorld::addFlamethrowerChargersToPlayer(int num) {
    m_player->addFlamethrowerCharges(num);
}

void StudentWorld::addLandminesToPlayer(int num) {
    m_player->addLandmines(num);
}

void StudentWorld::inflictFlameDamageAround(int x, int y) {
    list<Actor*>::iterator actorsIter = m_actors.begin();
    while (actorsIter != m_actors.end()) {
        Actor* a = *actorsIter;
        if (a->canBeDamaged()) {
            if (overlapsWith(x, y, a->getX(), a->getY())) {
                a->setDead();
            }
        }
        actorsIter++;
    }
}

void StudentWorld::inflictVomitDamageAround(int x, int y) {
    list<Actor*>::iterator actorsIter = m_actors.begin();
    while (actorsIter != m_actors.end()) {
        Actor* a = *actorsIter;
        if (a->canBeInfected()) {
            if (overlapsWith(x, y, a->getX(), a->getY())) {
                a->infect();
            }
        }
        actorsIter++;
    }
}

void StudentWorld::addFlames(int num, int originalX, int originalY, Direction dir) {
    bool continuePlacingFlames = true; // if flames encounter a wall
    for (int i = 1; i <= num; i++) {
        int flameX = originalX;
        int flameY = originalY;
        switch (dir) {
            case Actor::up:
                flameY = originalY + (i * SPRITE_HEIGHT);
                break;
            case Actor::down:
                flameY = originalY - (i * SPRITE_HEIGHT);
                break;
            case Actor::left:
                flameX = originalX - (i * SPRITE_WIDTH);
                break;
            case Actor::right:
                flameX = originalX + (i * SPRITE_WIDTH);
                break;
            default:
                break;
        }
        if (continuePlacingFlames && !overlapsWithFlameBlockingObject(flameX, flameY))
            m_actors.emplace_back(new Flame(flameX, flameY, dir, this));
        else {
            continuePlacingFlames = false;
            break;
        }
    }
}

// loop through actors, check if any items that block flames are at (x, y)
bool StudentWorld::overlapsWithFlameBlockingObject(int x, int y) const {
    list<Actor*>::const_iterator actorsIter = m_actors.begin();
    while (actorsIter != m_actors.end()) {
        Actor* a = *actorsIter;
        if (a->blocksFlames()) {
            if (overlapsWith(x, y, a->getX(), a->getY()))
                return true;
        }
        actorsIter++;
    }
    return false;
}

void StudentWorld::loadLevel() {
    Level lev(assetPath());
    
    ostringstream oss;
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
        
        for (int fileX = 0; fileX < LEVEL_WIDTH; fileX++) {
            for (int fileY = 0; fileY < LEVEL_HEIGHT; fileY++) {
                Level::MazeEntry ge = lev.getContentsOf(fileX, fileY);
                int gameX = fileX * SPRITE_WIDTH;
                int gameY = fileY * SPRITE_HEIGHT;
                switch (ge) {
                    case Level::player:
                        m_player = new Penelope(gameX, gameY, this);
                        cout <<"Location (" << fileX << "," << fileY << ") is where Penelope starts" << endl;
                        break;
                    case Level::wall:
                        m_actors.emplace_back(new Wall(gameX, gameY, this));
                        // cout << "Location (" << fileX << "," << fileY << ") holds a wall" << endl;
                        break;
                    case Level::exit:
                        m_actors.emplace_back(new Exit(gameX, gameY, this));
                        cout << "Location (" << fileX << "," << fileY << ") has an exit" << endl;
                        break;
                    case Level::citizen:
                        m_actors.emplace_back(new Citizen(gameX, gameY, this));
                        m_citizens++;
                        cout << "Location (" << fileX << "," << fileY << ") starts with a citizen" << endl;
                        break;
                    case Level::dumb_zombie:
                        m_actors.emplace_back(new DumbZombie(gameX, gameY, this));
                        m_zombies++;
                        break;
                    case Level::smart_zombie:
                        m_actors.emplace_back(new SmartZombie(gameX, gameY, this));
                        m_zombies++;
                        break;
                    case Level::pit:
                        m_actors.emplace_back(new Pit(gameX, gameY, this));
                        cout << "Location (" << fileX << "," << fileY << ") has a pit" << endl;
                        break;
                    case Level::vaccine_goodie:
                        m_actors.emplace_back(new VaccineGoodie(gameX, gameY, this));
                        cout << "Location (" << fileX << "," << fileY << ") has a vaccine goodie" << endl;
                        break;
                    case Level::gas_can_goodie:
                        m_actors.emplace_back(new GasCanGoodie(gameX, gameY, this));
                        cout << "Location (" << fileX << "," << fileY << ") has a gas-can goodie" << endl;
                        break;
                    case Level::landmine_goodie:
                        m_actors.emplace_back(new LandmineGoodie(gameX, gameY, this));
                        cout << "Location (" << fileX << "," << fileY << ") has a landmine goodie" << endl;
                        break;
                    case Level::empty:
                        // cout << "Location (" << fileX << "," << fileY << ") is empty" << endl;
                    default:
                        break;
                }
            }
        }
    }
}

