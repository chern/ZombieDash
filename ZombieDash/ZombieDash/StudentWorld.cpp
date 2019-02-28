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
    m_levelFinished = false;
    m_citizens = 0;
    m_zombies = 0;
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init() {
    m_levelFinished = false;
    m_citizens = 0;
    m_zombies = 0;
    return loadLevel(getLevel());
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
    oss << "Score: ";
    if (getScore() >= 0)
        oss << setw(6) << getScore() << spaceSeparator;
    else
        oss << "-" << setw(5) << abs(getScore()) << spaceSeparator;
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
}

bool StudentWorld::canMoveTo(int fromX, int fromY, int toX, int toY) const {
    for (int x1 = fromX; x1 < fromX + SPRITE_WIDTH; x1++) {
        for (int y1 = fromY; y1 < fromY + SPRITE_HEIGHT; y1++) {
            for (int x2 = toX; x2 < toX + SPRITE_WIDTH; x2++) {
                for (int y2 = toY; y2 < toY + SPRITE_HEIGHT; y2++) {
                    if (x1 == x2 && y1 == y2)
                        return false;
                }
            }
        }
    }
    return true;
}

bool StudentWorld::playerCanMoveTo(int x, int y) const {
    list<Actor*>::const_iterator actorsIter = m_actors.cbegin();
    while (actorsIter != m_actors.cend()) {
        Actor* a = *actorsIter;
        if (a->alive() && a->blocksMovement()) {
            if (!canMoveTo(x, y, a->getX(), a->getY()))
                return false;
        }
        actorsIter++;
    }
    return true;
}

bool StudentWorld::agentCanMoveTo(Agent* ag, int destX, int destY) const {
    if (!canMoveTo(destX, destY, getPlayerX(), getPlayerY()))
        return false;
    list<Actor*>::const_iterator actorsIter = m_actors.cbegin();
    while (actorsIter != m_actors.cend()) {
        Actor* a = *actorsIter;
        if (a->alive() && ag != a && a->blocksMovement()) {
            if (!canMoveTo(destX, destY, a->getX(), a->getY()))
                return false;
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
    
    if ((pow(deltaX, 2) + pow(deltaY, 2)) <= pow(10, 2))
        return true;
    return false;
}

bool StudentWorld::overlapsWithPlayer(int x, int y) const {
    return overlapsWith(x, y, getPlayerX(), getPlayerY());
}

bool StudentWorld::overlapsWithCitizen(int x, int y) const {
    list<Actor*>::const_iterator actorsIter = m_actors.cbegin();
    while (actorsIter != m_actors.cend()) {
        Actor* a = *actorsIter;
        if (a->alive() && a->canBeInfected() && overlapsWith(x, y, a->getX(), a->getY()))
            return true;
        // only citizens (and Penelope) can be infected
        actorsIter++;
    }
    return false;
}

bool StudentWorld::overlapsWithAgent(int x, int y) const {
    if (overlapsWith(x, y, getPlayerX(), getPlayerY()))
        return true;
    list<Actor*>::const_iterator actorsIter = m_actors.cbegin();
    while (actorsIter != m_actors.cend()) {
        Actor* a = *actorsIter;
        if (a->alive() && a->canSetOffLandmine() && overlapsWith(x, y, a->getX(), a->getY()))
            return true;
        // only Agents (citizens, Penelope, zombies) can set off landmines
        actorsIter++;
    }
    return false;
}

double StudentWorld::distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

int StudentWorld::getPlayerX() const {
    return m_player->getX();
}

int StudentWorld::getPlayerY() const {
    return m_player->getY();
}

Agent* StudentWorld::getNearestAgent(int x, int y) const {
    Agent* nearestAgent = m_player;
    double nearestDistance = distance(nearestAgent->getX() + SPRITE_WIDTH/2, nearestAgent->getY() + SPRITE_HEIGHT/2, x + SPRITE_WIDTH/2, y + SPRITE_HEIGHT/2);
    list<Actor*>::const_iterator actorsIter = m_actors.cbegin();
    while (actorsIter != m_actors.cend()) {
        Actor* a = *actorsIter;
        if (a->alive() && a->canFall() && a->canSetOffLandmine()) {
            double tempDistance = distance(a->getX() + SPRITE_WIDTH/2, a->getY() + SPRITE_HEIGHT/2, x + SPRITE_WIDTH/2, y + SPRITE_HEIGHT/2);
            if (tempDistance < nearestDistance) {
                nearestAgent = static_cast<Agent*>(a);
                nearestDistance = tempDistance;
            }
        }
        actorsIter++;
    }
    return nearestAgent;
}

Human* StudentWorld::getNearestHuman(int x, int y) const {
    Human* nearestHuman = m_player;
    double nearestDistance = distance(nearestHuman->getX() + SPRITE_WIDTH/2, nearestHuman->getY() + SPRITE_HEIGHT/2, x + SPRITE_WIDTH/2, y + SPRITE_HEIGHT/2);
    list<Actor*>::const_iterator actorsIter = m_actors.cbegin();
    while (actorsIter != m_actors.cend()) {
        Actor* a = *actorsIter;
        if (a->alive() && a->canBeInfected()) {
            double tempDistance = distance(a->getX() + SPRITE_WIDTH/2, a->getY() + SPRITE_HEIGHT/2, x + SPRITE_WIDTH/2, y + SPRITE_HEIGHT/2);
            if (tempDistance < nearestDistance) {
                nearestHuman = static_cast<Human*>(a);
                nearestDistance = tempDistance;
            }
        }
        actorsIter++;
    }
    return nearestHuman;
}

Citizen* StudentWorld::getNearestCitizen(int x, int y) const {
    if (citizensRemaining() <= 0)
        return nullptr;
    else {
        Citizen* nearestCitizen = nullptr;
        double nearestDistance = 100000;
        list<Actor*>::const_iterator actorsIter = m_actors.cbegin();
        while (actorsIter != m_actors.cend()) {
            Actor* a = *actorsIter;
            if (a->alive() && a->canBeInfected()) {
                double tempDistance = distance(a->getX() + SPRITE_WIDTH/2, a->getY() + SPRITE_HEIGHT/2, x + SPRITE_WIDTH/2, y + SPRITE_HEIGHT/2);
                if (tempDistance < nearestDistance) {
                    nearestCitizen = static_cast<Citizen*>(a);
                    nearestDistance = tempDistance;
                }
            }
            actorsIter++;
        }
        return nearestCitizen;
    }
}

Zombie* StudentWorld::getNearestZombie(int x, int y) const {
    if (zombiesRemaining() <= 0)
        return nullptr;
    
    Zombie* nearestZombie = nullptr;
    double nearestDistance = 1000000;
    list<Actor*>::const_iterator actorsIter = m_actors.cbegin();
    while (actorsIter != m_actors.cend()) {
        Actor* a = *actorsIter;
        if (a->alive() && !a->canBeInfected() && a->blocksMovement() && a->canBeDamaged() && a->canFall() && a->canSetOffLandmine()) {
            double tempDistance = distance(a->getX() + SPRITE_WIDTH/2, a->getY() + SPRITE_HEIGHT/2, x + SPRITE_WIDTH/2, y + SPRITE_HEIGHT/2);
            if (tempDistance < nearestDistance) {
                nearestZombie = static_cast<Zombie*>(a);
                nearestDistance = tempDistance;
            }
        }
        actorsIter++;
    }
    return nearestZombie;
}

double StudentWorld::distanceToPlayer(int x, int y) const {
    return distance(x + SPRITE_WIDTH/2, y + SPRITE_HEIGHT/2, getPlayerX() + SPRITE_WIDTH/2, getPlayerY() + SPRITE_HEIGHT/2);
}

double StudentWorld::distanceToNearestZombie(int x, int y) const {
    if (zombiesRemaining() <= 0)
        return -1;
    Zombie* nearestZombie = getNearestZombie(x, y);
    if (nearestZombie != nullptr)
        return distance(x + SPRITE_WIDTH/2, y + SPRITE_HEIGHT/2, nearestZombie->getX() + SPRITE_WIDTH/2, nearestZombie->getY() + SPRITE_HEIGHT/2);
    else
        return -1;
}

int StudentWorld::citizensRemaining() const {
    return m_citizens;
}

void StudentWorld::markCitizenGone() {
    m_citizens--;
}

int StudentWorld::zombiesRemaining() const {
    return m_zombies;
}

void StudentWorld::markZombieGone() {
    m_zombies--;
}

void StudentWorld::finishLevel() {
    m_levelFinished = true;
}

void StudentWorld::addVaccinesToPlayer(int num) {
    m_player->addVaccines(num);
}

void StudentWorld::addFlamethrowerChargesToPlayer(int num) {
    m_player->addFlamethrowerCharges(num);
}

void StudentWorld::addLandminesToPlayer(int num) {
    m_player->addLandmines(num);
}

void StudentWorld::inflictFlameDamageAround(int x, int y) {
    if (overlapsWithPlayer(x, y)) {
        m_player->setDead(DEAD_KILLED);
        playSound(SOUND_PLAYER_DIE);
    }
    list<Actor*>::iterator actorsIter = m_actors.begin();
    while (actorsIter != m_actors.end()) {
        Actor* a = *actorsIter;
        if (a->alive() && a->canBeDamaged() && overlapsWith(x, y, a->getX(), a->getY())) {
            a->setDead(DEAD_KILLED);
            if (a->canBeDetonated())
                static_cast<Landmine*>(a)->detonate();
        }
        actorsIter++;
    }
}

void StudentWorld::inflictVomitDamageAround(int x, int y) {
    if (overlapsWithPlayer(x, y))
        m_player->infect();
    list<Actor*>::iterator actorsIter = m_actors.begin();
    while (actorsIter != m_actors.end()) {
        Actor* a = *actorsIter;
        if (a->alive() && a->canBeInfected() && overlapsWith(x, y, a->getX(), a->getY())) {
            static_cast<Human*>(a)->infect();
        }
        actorsIter++;
    }
}

void StudentWorld::addFlames(int num, int originalX, int originalY, Direction dir) {
    bool continuePlacingFlames = true; // if flames encounter a wall
    for (int i = 1; continuePlacingFlames && i <= num; i++) {
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
        else
            continuePlacingFlames = false;
    }
}

void StudentWorld::addFlamesAround(int x, int y) {
    m_actors.emplace_back(new Flame(x, y, Actor::up, this));
    // north
    if (!overlapsWithFlameBlockingObject(x, y + SPRITE_HEIGHT))
        m_actors.emplace_back(new Flame(x, y + SPRITE_HEIGHT, Actor::up, this));
    // northeast
    if (!overlapsWithFlameBlockingObject(x + SPRITE_WIDTH, y + SPRITE_HEIGHT))
        m_actors.emplace_back(new Flame(x + SPRITE_WIDTH, y + SPRITE_HEIGHT, Actor::up, this));
    // east
    if (!overlapsWithFlameBlockingObject(x + SPRITE_WIDTH, y))
        m_actors.emplace_back(new Flame(x + SPRITE_WIDTH, y, Actor::up, this));
    // southeast
    if (!overlapsWithFlameBlockingObject(x + SPRITE_WIDTH, y - SPRITE_HEIGHT))
        m_actors.emplace_back(new Flame(x + SPRITE_WIDTH, y - SPRITE_HEIGHT, Actor::up, this));
    // south
    if (!overlapsWithFlameBlockingObject(x, y - SPRITE_HEIGHT))
        m_actors.emplace_back(new Flame(x, y - SPRITE_HEIGHT, Actor::up, this));
    // southwest
    if (!overlapsWithFlameBlockingObject(x - SPRITE_WIDTH, y - SPRITE_HEIGHT))
        m_actors.emplace_back(new Flame(x - SPRITE_WIDTH, y - SPRITE_HEIGHT, Actor::up, this));
    // west
    if (!overlapsWithFlameBlockingObject(x - SPRITE_WIDTH, y))
        m_actors.emplace_back(new Flame(x - SPRITE_WIDTH, y, Actor::up, this));
    // northwest
    if (!overlapsWithFlameBlockingObject(x - SPRITE_WIDTH, y + SPRITE_HEIGHT))
        m_actors.emplace_back(new Flame(x - SPRITE_WIDTH, y + SPRITE_HEIGHT, Actor::up, this));
}

void StudentWorld::addActor(Actor* a) {
    m_actors.emplace_back(a);
}

bool StudentWorld::overlapsWithAnyObject(int x, int y) const {
    if (overlapsWithPlayer(x, y))
        return true;
    list<Actor*>::const_iterator actorsIter = m_actors.cbegin();
    while (actorsIter != m_actors.cend()) {
        Actor* a = *actorsIter;
        if (a->alive() && overlapsWith(x, y, a->getX(), a->getY()))
            return true;
        actorsIter++;
    }
    return false;
}

// loop through actors, check if any items that block flames are at (x, y)
bool StudentWorld::overlapsWithFlameBlockingObject(int x, int y) const {
    list<Actor*>::const_iterator actorsIter = m_actors.cbegin();
    while (actorsIter != m_actors.cend()) {
        Actor* a = *actorsIter;
        if (a->alive() && a->blocksFlames() && overlapsWith(x, y, a->getX(), a->getY()))
            return true;
        actorsIter++;
    }
    return false;
}

int StudentWorld::loadLevel(int levelNum) {
    Level lev(assetPath());
    
    ostringstream oss;
    oss.fill('0');
    oss << "level" << setw(2) << levelNum << ".txt";
    cout << oss.str() << endl;
    string levelFile = oss.str();
    
    // string levelFile = "level01.txt";
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (levelNum > 99 || result == Level::load_fail_file_not_found) {
        cerr << "Cannot find " << levelFile << " data file" << endl;
        return GWSTATUS_PLAYER_WON;
    } else if (result == Level::load_fail_bad_format) {
        cerr << levelFile << " is improperly formatted" << endl;
        return GWSTATUS_LEVEL_ERROR;
    } else if (result == Level::load_success) {
        cout << "Successfully loaded " << levelFile << endl;
        
        for (int fileX = 0; fileX < LEVEL_WIDTH; fileX++) {
            for (int fileY = 0; fileY < LEVEL_HEIGHT; fileY++) {
                Level::MazeEntry ge = lev.getContentsOf(fileX, fileY);
                int gameX = fileX * SPRITE_WIDTH;
                int gameY = fileY * SPRITE_HEIGHT;
                switch (ge) {
                    case Level::player:
                        delete m_player;
                        m_player = nullptr;
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
    return GWSTATUS_CONTINUE_GAME;
}

