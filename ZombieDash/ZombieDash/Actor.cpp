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
Human::Human(int imageID, int startX, int startY, Direction startDir, int depth, StudentWorld* sw): Actor(imageID, startX, startY, startDir, depth, sw) {
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
Penelope::Penelope(int startX, int startY, StudentWorld* sw): Human(IID_PLAYER, startX, startY, right, 0, sw) {
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

// EXIT
Exit::Exit(int x, int y, StudentWorld* sw): Actor(IID_EXIT, x, y, right, 1, sw) {}

void Exit::doSomething() {
    // 1. Determine whether or not exit overlaps with a citizen
    
    
    // 2. Determine whether or not exit overlaps with Penelope AND there are no citizens left
    if (getStudentWorld()->overlapsWithPlayer(getX(), getY()) && getStudentWorld()->citizensRemaining() == 0) {
        getStudentWorld()->playSound(SOUND_LEVEL_FINISHED);
        getStudentWorld()->finishLevel();
    }
}

bool Exit::blocksMovement() const {
    return false;
}

bool Exit::canBeInfected() const {
    return false;
}

bool Exit::canBeDamaged() const {
    return false;
}
