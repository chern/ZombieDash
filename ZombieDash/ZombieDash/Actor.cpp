// William Chern, UCLA CS 32 Project 3 ("Zombie Dash"), Winter 2019

#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// ACTOR
Actor::Actor(int imageID, int startX, int startY, Direction startDir, int depth, StudentWorld* sw): GraphObject(imageID, startX, startY, startDir, depth) {
    m_studentWorld = sw;
    m_alive = true;
}

bool Actor::alive() const {
    return m_alive;
}

void Actor::setDead(int deadID) {
    m_alive = false;
    setDeadSpecialized(deadID);
}

StudentWorld* Actor::getStudentWorld() const {
    return m_studentWorld;
}

bool Actor::blocksMovement() const {
    return false;
}

bool Actor::canBeInfected() const {
    return false;
}

bool Actor::canBeDamaged() const {
    return false;
}

bool Actor::blocksFlames() const {
    return false;
}

bool Actor::canFall() const {
    return false;
}

bool Actor::canSetOffLandmine() const {
    return false;
}

bool Actor::canBeDetonated() const {
    return false;
}

void Actor::setDeadSpecialized(int deadID) {
    return;
}

// AGENT
Agent::Agent(int imageID, int startX, int startY, StudentWorld* sw): Actor(imageID, startX, startY, right, 0, sw) {}

bool Agent::blocksMovement() const {
    return true;
}

bool Agent::canBeDamaged() const {
    return true;
}

bool Agent::canFall() const {
    return true;
}

bool Agent::canSetOffLandmine() const {
    return true;
}

// HUMAN
Human::Human(int imageID, int startX, int startY, StudentWorld* sw): Agent(imageID, startX, startY, sw) {
    m_infected = false;
    m_infections = 0;
}

bool Human::canBeInfected() const {
    return true;
}

int Human::infections() const {
    return m_infections;
}

void Human::infect() {
    if (!m_infected)
        m_infected = true;
    m_infections++;
}

bool Human::infected() const {
    return m_infected;
}

void Human::vaccinate() {
    m_infected = false;
    m_infections = 0;
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
            setDead(DEAD_KILLED);
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
                if (getStudentWorld()->playerCanMoveTo(getX()-4, getY()))
                    moveTo(getX()-4, getY());
                break;
            case KEY_PRESS_RIGHT:
                // move Penelope to the right
                setDirection(right);
                if (getStudentWorld()->playerCanMoveTo(getX()+4, getY()))
                    moveTo(getX()+4, getY());
                break;
            case KEY_PRESS_UP:
                // move Penelope up
                setDirection(up);
                if (getStudentWorld()->playerCanMoveTo(getX(), getY()+4))
                    moveTo(getX(), getY()+4);
                break;
            case KEY_PRESS_DOWN:
                // move Penelope down
                setDirection(down);
                if (getStudentWorld()->playerCanMoveTo(getX(), getY()-4))
                    moveTo(getX(), getY()-4);
                break;
            case KEY_PRESS_SPACE:
                // add flames in front of Penelope
                if (m_flamethrowerCharges > 0) {
                    deployFlames();
                }
                break;
            case KEY_PRESS_TAB:
                // landmines
                if (m_landmines > 0) {
                    deployLandmine();
                }
                break;
            case KEY_PRESS_ENTER:
                // vaccinate
                if (m_vaccines > 0) {
                    useVaccine();
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

void Penelope::deployFlames() {
    if (m_flamethrowerCharges <= 0)
        return; // do nothing
    m_flamethrowerCharges--;
    getStudentWorld()->playSound(SOUND_PLAYER_FIRE);
    getStudentWorld()->addFlames(3, getX(), getY(), getDirection());
}

void Penelope::deployLandmine() {
    if (m_landmines <= 0)
        return; // do nothing
    m_landmines--;
    getStudentWorld()->addLandmine(getX(), getY());
}

void Penelope::useVaccine() {
    if (m_vaccines <= 0)
        return; // do nothing
    m_vaccines--;
    vaccinate();
}

// CITIZEN
Citizen::Citizen(int startX, int startY, StudentWorld* sw): Human(IID_CITIZEN, startX, startY, sw) {
    m_paralyzed = false;
}

void Citizen::doSomething() {
    if(!alive())
        return;
    if (infected()) {
        infect();
        if (infections() >= 500) {
            setDead(DEAD_KILLED);
            getStudentWorld()->playSound(SOUND_ZOMBIE_BORN);
            // introduce a new zombie object at the same (x,y) coordinate
                // 70% chance of DumbZombie
                // 30% chance of SmartZombie
            return;
        }
    }
    if (m_paralyzed) {
        m_paralyzed = false;
        return; // paralysis tick
    } else {
        m_paralyzed = true;
    }
    // TODO: Citizen must determine its distance to Penelope
    // TODO: Citizen must determine its distance to the nearest zombie
}

void Citizen::setDeadSpecialized(int deadID) {
    switch (deadID) {
        case DEAD_KILLED:
            getStudentWorld()->increaseScore(-1000);
            getStudentWorld()->playSound(SOUND_CITIZEN_DIE);
            break;
        case DEAD_SAVED_USED:
            getStudentWorld()->increaseScore(500);
            getStudentWorld()->playSound(SOUND_CITIZEN_SAVED);
            break;
    }
}

// ZOMBIE
Zombie::Zombie(int startX, int startY, StudentWorld* sw): Agent(IID_ZOMBIE, startX, startY, sw) {
    m_paralyzed = false;
    m_movementPlanDistance = 0;
}

void Zombie::doSomething() {
    if (!alive())
        return;
    if (m_paralyzed) {
        m_paralyzed = false;
        return; // paralysis tick
    } else {
        m_paralyzed = true;
    }
    
    // compute vomit coordinates
    int vomitX = getX();
    int vomitY = getY();
    computeVomitCoordinates(vomitX, vomitY);
    // if vomit coordinates overlap with citizen or Penelope, 1/3 chance of vomiting
    if (getStudentWorld()->overlapsWithPlayer(vomitX, vomitY) || getStudentWorld()->overlapsWithCitizen(vomitX, vomitY)) {
        int chance = randInt(1, 3);
        if (chance > 1) { // 1 in 3 chance of vomit
            getStudentWorld()->addVomit(vomitX, vomitY, getDirection());
            getStudentWorld()->playSound(SOUND_ZOMBIE_VOMIT);
            return;
        }
    }
    
    if (m_movementPlanDistance <= 0)
        determineNewMovementPlan();
    
    int destX = getX();
    int destY = getY();
    computeDestinationCoordinates(destX, destY);
    if (getStudentWorld()->agentCanMoveTo(this, destX, destY)) {
        moveTo(destX, destY);
        m_movementPlanDistance--;
    } else {
        setMovementPlanDistance(0);
    }
}

void Zombie::setMovementPlanDistance(int mv) {
    m_movementPlanDistance = mv;
}

int Zombie::movementPlanDistance() const {
    return m_movementPlanDistance;
}

void Zombie::computeVomitCoordinates(int& vx, int& vy) {
    switch (getDirection()) {
        case up:
            vx = getX();
            vy = getY() + SPRITE_HEIGHT;
            break;
        case down:
            vx = getX();
            vy = getY() - SPRITE_HEIGHT;
            break;
        case left:
            vx = getX() - SPRITE_WIDTH;
            vy = getY();
            break;
        case right:
            vx = getX() + SPRITE_WIDTH;
            vy = getY();
            break;
    }
}

void Zombie::computeDestinationCoordinates(int& destX, int& destY) {
    switch (getDirection()) {
        case up:
            destX = getX();
            destY = getY() + 1;
            break;
        case down:
            destX = getX();
            destY = getY() - 1;
            break;
        case left:
            destX = getX() - 1;
            destY = getY();
            break;
        case right:
            destX = getX() + 1;
            destY = getY();
            break;
    }
}

void Zombie::setDeadSpecialized(int deadID) {
    if (deadID == DEAD_KILLED) {
        setZombieDead();
        getStudentWorld()->playSound(SOUND_ZOMBIE_DIE);
    }
}

// DUMB ZOMBIE
DumbZombie::DumbZombie(int startX, int startY, StudentWorld* sw): Zombie(startX, startY, sw) {}

void DumbZombie::determineNewMovementPlan() {
    setMovementPlanDistance(randInt(3, 10));
    switch (randInt(1, 4)) {
        case 1:
            setDirection(up);
            break;
        case 2:
            setDirection(down);
            break;
        case 3:
            setDirection(left);
            break;
        case 4:
            setDirection(right);
            break;
    }
}

void DumbZombie::setZombieDead() {
    getStudentWorld()->increaseScore(1000);
    // 1 in 10 DumbZombies will drop a vaccine goodie
    int chance = randInt(1, 10);
    if (chance == 1) {
        getStudentWorld()->addVaccineGoodie(getX(), getY());
    }
}

// SMART ZOMBIE
SmartZombie::SmartZombie(int startX, int startY, StudentWorld* sw): Zombie(startX, startY, sw) {}

void SmartZombie::determineNewMovementPlan() {
    setMovementPlanDistance(randInt(3, 10));
    Human* nearestHuman = getStudentWorld()->getNearestHuman(getX(), getY());
    double distance = sqrt(pow(getX() + SPRITE_WIDTH - nearestHuman->getX(), 2) + pow(getY() + SPRITE_HEIGHT - nearestHuman->getY() , 2));
    if (distance > 80) {
        switch (randInt(1, 4)) {
            case 1:
                setDirection(up);
                break;
            case 2:
                setDirection(down);
                break;
            case 3:
                setDirection(left);
                break;
            case 4:
                setDirection(right);
                break;
        }
    } else {
        if (getX() == nearestHuman->getX()) {
            if (getY() < nearestHuman->getY())
                setDirection(up);
            else
                setDirection(down);
        } else if (getY() == nearestHuman->getY()) {
            if (getX() < nearestHuman->getX())
                setDirection(right);
            else
                setDirection(left);
        } else {
            Direction horizontalOption;
            Direction verticalOption;
            if (getX() < nearestHuman->getX())
                horizontalOption = right;
            else
                horizontalOption = left;
            if (getY() < nearestHuman->getY())
                verticalOption = up;
            else
                verticalOption = down;
            int d = randInt(1, 2);
            if (d == 1)
                setDirection(horizontalOption);
            else
                setDirection(verticalOption);
        }
    }
}

void SmartZombie::setZombieDead() {
    getStudentWorld()->increaseScore(2000);
}

// WALL
Wall::Wall(int x, int y, StudentWorld* sw): Actor(IID_WALL, x, y, right, 0, sw) {}

void Wall::doSomething() {
    return;
}

bool Wall::blocksMovement() const {
    return true;
}

bool Wall::blocksFlames() const {
    return true;
}

// FALL INTO OBJECT
FallIntoObject::FallIntoObject(int imageID, int x, int y, int depth, StudentWorld* sw): Actor(imageID, x, y, right, depth, sw) {}

// EXIT
Exit::Exit(int x, int y, StudentWorld* sw): FallIntoObject(IID_EXIT, x, y, 1, sw) {}

void Exit::doSomething() {
    // TODO: 1. Determine if exit overlaps with a citizen
    
    
    // 2. Determine if exit overlaps with Penelope AND there are no citizens left
    if (getStudentWorld()->overlapsWithPlayer(getX(), getY()) && getStudentWorld()->citizensRemaining() == 0) {
        getStudentWorld()->playSound(SOUND_LEVEL_FINISHED);
        // getStudentWorld()->playSound(SOUND_CITIZEN_SAVED);
        getStudentWorld()->finishLevel();
    }
}

bool Exit::blocksFlames() const {
    return true;
}

// PIT
Pit::Pit(int x, int y, StudentWorld* sw): FallIntoObject(IID_PIT, x, y, 0, sw) {}

void Pit::doSomething() {
    // TODO: if person/zombie overlaps it, destroy it as though it were damaged by a flame
    
    
    return;
}

// GOODIE
Goodie::Goodie(int imageID, int x, int y, StudentWorld* sw): Actor(imageID, x, y, right, 1, sw) {}

void Goodie::doSomething() {
    if (!alive())
        return;
    if (getStudentWorld()->overlapsWithPlayer(getX(), getY())) {
        this->setDead(DEAD_SAVED_USED);
    }
}

bool Goodie::canBeDamaged() const {
    return true;
}

void Goodie::setDeadSpecialized(int deadID) {
    if (deadID == DEAD_SAVED_USED) {
        getStudentWorld()->increaseScore(50);
        getStudentWorld()->playSound(SOUND_GOT_GOODIE);
        giveSpecializedGoodie();
    }
}

// VACCINE GOODIE
VaccineGoodie::VaccineGoodie(int x, int y, StudentWorld *sw): Goodie(IID_VACCINE_GOODIE, x, y, sw) {}

void VaccineGoodie::giveSpecializedGoodie() {
    getStudentWorld()->addVaccinesToPlayer(1);
}

// GAS CAN GOODIE
GasCanGoodie::GasCanGoodie(int x, int y, StudentWorld* sw): Goodie(IID_GAS_CAN_GOODIE, x, y, sw) {}

void GasCanGoodie::giveSpecializedGoodie() {
    getStudentWorld()->addFlamethrowerChargesToPlayer(5);
}

// LANDMINE GOODIE
LandmineGoodie::LandmineGoodie(int x, int y, StudentWorld* sw): Goodie(IID_LANDMINE_GOODIE, x, y, sw) {}

void LandmineGoodie::giveSpecializedGoodie() {
    getStudentWorld()->addLandminesToPlayer(2);
}

// PROJECTILE
Projectile::Projectile(int imageID, int x, int y, Direction dir, StudentWorld* sw): Actor(imageID, x, y, dir, 0, sw) {
    m_ticks = 0;
}

void Projectile::doSomething() {
    m_ticks++;
    if (!alive())
        return;
    if (m_ticks >= 2) {
        setDead(DEAD_SAVED_USED);
        return;
    }
    inflictSpecializedDamage();
}

// FLAME
Flame::Flame(int x, int y, Direction dir, StudentWorld* sw): Projectile(IID_FLAME, x, y, dir, sw) {}

void Flame::inflictSpecializedDamage() {
    getStudentWorld()->inflictFlameDamageAround(getX(), getY());
}

// VOMIT
Vomit::Vomit(int x, int y, Direction dir, StudentWorld* sw): Projectile(IID_VOMIT, x, y, dir, sw) {}

void Vomit::inflictSpecializedDamage() {
    getStudentWorld()->inflictVomitDamageAround(getX(), getY());
}

// LANDMINE
Landmine::Landmine(int x, int y, StudentWorld* sw): Actor(IID_LANDMINE, x, y, right, 1, sw) {
    m_safetyTicks = 30;
    m_active = false;
}

void Landmine::doSomething() {
    if (!alive())
        return;
    if (!m_active) {
        m_safetyTicks--;
        if (m_safetyTicks <= 0)
            m_active = true;
        return;
    }
    // must determine if landmine overlaps with a citizen, Penelope, or zombie
    if (getStudentWorld()->overlapsWithOrganism(getX(), getY())) {
        detonate();
    }
}

bool Landmine::canBeDamaged() const {
    return true;
}

bool Landmine::canBeDetonated() const {
    return true;
}

void Landmine::detonate() {
    setDead(DEAD_SAVED_USED);
    getStudentWorld()->playSound(SOUND_LANDMINE_EXPLODE);
    getStudentWorld()->addFlamesAround(getX(), getY());
    getStudentWorld()->addPit(getX(), getY());
}
