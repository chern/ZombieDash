#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// ACTOR

Actor::Actor(int imageID, int startX, int startY, Direction startDir, int depth, StudentWorld* sw): GraphObject(imageID, startX, startY, startDir, depth) {
    m_studentWorld = sw;
    m_alive = true;
}

void Actor::setDead() {
    m_alive = false;
}

StudentWorld* Actor::studentWorld() const {
    return m_studentWorld;
}

// HUMAN

Human::Human(int imageID, int startX, int startY, Direction startDir, int depth, StudentWorld* sw): Actor(imageID, startX, startY, startDir, depth, sw) {
    m_infected = false;
    m_infections = 0;
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
    }
}
