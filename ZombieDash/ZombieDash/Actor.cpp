// William Chern, UCLA CS 32 Project 3 ("Zombie Dash"), Winter 2019

#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// ACTOR
Actor::Actor(int imageID, int startX, int startY, Direction startDir, int depth, StudentWorld* sw): GraphObject(imageID, startX, startY, startDir, depth) {
    m_studentWorld = sw;
    m_alive = true;
}

bool Actor::alive() const {
    return m_alive;
}

void Actor::setDead() {
    m_alive = false;
}

StudentWorld* Actor::getStudentWorld() const {
    return m_studentWorld;
}

// HUMAN
Human::Human(int imageID, int startX, int startY, StudentWorld* sw): Actor(imageID, startX, startY, right, 0, sw) {
    m_infected = false;
    m_infections = 0;
}

bool Human::blocksMovement() const {
    return true;
}

bool Human::canBeInfected() const {
    return true;
}

bool Human::canBeDamaged() const {
    return true;
}

bool Human::blocksFlames() const {
    return false;
}

bool Human::canFall() const {
    return true;
}

bool Human::infected() const {
    return m_infected;
}

int Human::infections() const {
    return m_infections;
}

void Human::infect() {
    if (m_infections == 0)
        m_infected = true;
    m_infections++;
}

// PENELOPE
Penelope::Penelope(int startX, int startY, StudentWorld* sw): Human(IID_PLAYER, startX, startY, sw) {
    m_landmines = 0;
    m_flamethrowerCharges = 0;
    m_vaccines = 0;
}

void Penelope::doSomething() {
    if (!alive())
        return;
    if (infected()) {
        infect();
        if (infections() >= 500) {
            setDead();
            getStudentWorld()->playSound(SOUND_PLAYER_DIE);
            return;
        }
    }
    
    int ch;
    if (getStudentWorld()->getKey(ch)) {
        // The user hit a key during this tick
        switch (ch) {
            case KEY_PRESS_LEFT:
                // move Penelope to the left
                setDirection(left);
                if (getStudentWorld()->canMoveTo(getX()-4, getY()))
                    moveTo(getX()-4, getY());
                break;
            case KEY_PRESS_RIGHT:
                // move Penelope to the right
                setDirection(right);
                if (getStudentWorld()->canMoveTo(getX()+4, getY()))
                    moveTo(getX()+4, getY());
                break;
            case KEY_PRESS_UP:
                // move Penelope up
                setDirection(up);
                if (getStudentWorld()->canMoveTo(getX(), getY()+4))
                    moveTo(getX(), getY()+4);
                break;
            case KEY_PRESS_DOWN:
                // move Penelope down
                setDirection(down);
                if (getStudentWorld()->canMoveTo(getX(), getY()-4))
                    moveTo(getX(), getY()-4);
                break;
            case KEY_PRESS_SPACE:
                // add flames in front of Penelope
                if (m_flamethrowerCharges > 0) {
                    
                }
                break;
            case KEY_PRESS_TAB:
                // landmines
                if (m_landmines > 0) {
                    
                }
                break;
            case KEY_PRESS_ENTER:
                // vaccinate
                if (m_vaccines > 0) {
                    
                }
                break;
        }
    }
}

int Penelope::getNumLandmines() const {
    return m_landmines;
}

int Penelope::getNumFlamethrowerCharges() const {
    return m_flamethrowerCharges;
}

int Penelope::getNumVaccines() const {
    return m_vaccines;
}

void Penelope::addVaccines(int num) {
    m_vaccines += num;
}

void Penelope::addLandmines(int num) {
    m_landmines += num;
}

void Penelope::addFlamethrowerCharges(int num) {
    m_flamethrowerCharges += num;
}

// CITIZEN
Citizen::Citizen(int startX, int startY, StudentWorld* sw): Human(IID_CITIZEN, startX, startY, sw) {
    m_ticks = 0;
}

void Citizen::doSomething() {
    m_ticks++;
    if(!alive())
        return;
    if (infected()) {
        infect();
        if (infections() >= 500) {
            setDead();
            getStudentWorld()->playSound(SOUND_ZOMBIE_BORN);
            getStudentWorld()->increaseScore(-1000);
            // introduce a new zombie object at the same (x,y) coordinate
                // 70% chance of DumbZombie
                // 30% chance of SmartZombie
            return;
        }
    }
    if (m_ticks % 2 == 0)
        return; // paralysis tick
    // Citizen must determine its distance to Penelope
    // Citizen must determine its distance to the nearest zombie
}

// ZOMBIE
Zombie::Zombie(int startX, int startY, StudentWorld* sw): Actor(IID_ZOMBIE, startX, startY, right, 0, sw) {
    m_ticks = 0;
    m_movementPlanDistance = 0;
}

void Zombie::doSomething() {
    m_ticks++;
    if (!alive())
        return;
    if (m_ticks % 2 == 0)
        return; // paralysis tick
}

bool Zombie::blocksMovement() const {
    return true;
}

bool Zombie::canBeInfected() const {
    return false;
}

bool Zombie::canBeDamaged() const {
    return true;
}

bool Zombie::blocksFlames() const {
    return false;
}

bool Zombie::canFall() const {
    return true;
}

// DUMB ZOMBIE
DumbZombie::DumbZombie(int startX, int startY, StudentWorld* sw): Zombie(startX, startY, sw) {}

// SMART ZOMBIE
SmartZombie::SmartZombie(int startX, int startY, StudentWorld* sw): Zombie(startX, startY, sw) {}

// WALL
Wall::Wall(int x, int y, StudentWorld* sw): Actor(IID_WALL, x, y, right, 0, sw) {}

void Wall::doSomething() {
    return;
}

bool Wall::blocksMovement() const {
    return true;
}

bool Wall::canBeInfected() const {
    return false;
}

bool Wall::canBeDamaged() const {
    return false;
}

bool Wall::blocksFlames() const {
    return true;
}

bool Wall::canFall() const {
    return false;
}

// FALL INTO OBJECT
FallIntoObject::FallIntoObject(int imageID, int x, int y, int depth, StudentWorld* sw): Actor(imageID, x, y, right, depth, sw) {}

bool FallIntoObject::blocksMovement() const {
    return false;
}

bool FallIntoObject::canBeInfected() const {
    return false;
}

bool FallIntoObject::canBeDamaged() const {
    return false;
}

bool FallIntoObject::canFall() const {
    return false;
}

// EXIT
Exit::Exit(int x, int y, StudentWorld* sw): FallIntoObject(IID_EXIT, x, y, 1, sw) {}

void Exit::doSomething() {
    // 1. Determine if exit overlaps with a citizen
    
    
    // 2. Determine if exit overlaps with Penelope AND there are no citizens left
    if (getStudentWorld()->overlapsWithPlayer(getX(), getY()) && getStudentWorld()->citizensRemaining() == 0) {
        getStudentWorld()->playSound(SOUND_LEVEL_FINISHED);
        getStudentWorld()->finishLevel();
    }
}

bool Exit::blocksFlames() const {
    return true;
}

// PIT
Pit::Pit(int x, int y, StudentWorld* sw): FallIntoObject(IID_PIT, x, y, 0, sw) {}

void Pit::doSomething() {
    return;
}

bool Pit::blocksFlames() const {
    return false;
}

// GOODIE
Goodie::Goodie(int imageID, int x, int y, StudentWorld* sw): Actor(imageID, x, y, right, 1, sw) {}

void Goodie::doSomething() {
    if (!alive())
        return;
    if (getStudentWorld()->overlapsWithPlayer(getX(), getY())) {
        getStudentWorld()->increaseScore(50);
        this->setDead();
        getStudentWorld()->playSound(SOUND_GOT_GOODIE);
        giveSpecializedGoodie();
    }
}

bool Goodie::blocksMovement() const {
    return false;
}

bool Goodie::canBeInfected() const {
    return false;
}

bool Goodie::canBeDamaged() const {
    return true;
}

bool Goodie::blocksFlames() const {
    return false;
}

bool Goodie::canFall() const {
    return false;
}

// VACCINE GOODIE
VaccineGoodie::VaccineGoodie(int x, int y, StudentWorld *sw): Goodie(IID_VACCINE_GOODIE, x, y, sw) {}

void VaccineGoodie::giveSpecializedGoodie() {
    getStudentWorld()->addVaccinesToPlayer(1);
}

// GAS CAN GOODIE
GasCanGoodie::GasCanGoodie(int x, int y, StudentWorld* sw): Goodie(IID_GAS_CAN_GOODIE, x, y, sw) {}

void GasCanGoodie::giveSpecializedGoodie() {
    getStudentWorld()->addFlamethrowerChargersToPlayer(5);
}

// LANDMINE GOODIE
LandmineGoodie::LandmineGoodie(int x, int y, StudentWorld* sw): Goodie(IID_LANDMINE_GOODIE, x, y, sw) {}

void LandmineGoodie::giveSpecializedGoodie() {
    getStudentWorld()->addLandminesToPlayer(2);
}
