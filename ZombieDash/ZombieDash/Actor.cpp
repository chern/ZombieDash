// William Chern, UCLA CS 32 Project 3 ("Zombie Dash"), Winter 2019

#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>
#include <map>

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

bool Agent::attemptToMoveTo(int x, int y, Direction d) {
    if (getStudentWorld()->agentCanMoveTo(this, x, y)) {
        setDirection(d);
        moveTo(x, y);
        return true;
    }
    return false;
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
    if (!m_infected) {
        m_infected = true;
        playInfectedSoundIfApplicable();
    }
    m_infections++;
}

bool Human::infected() const {
    return m_infected;
}

void Human::vaccinate() {
    m_infected = false;
    m_infections = 0;
}

void Human::playInfectedSoundIfApplicable() {
    return;
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
        int destX = getX();
        int destY = getY();
        bool directionalKeyPressed = false;
        switch (ch) {
            case KEY_PRESS_LEFT:
                // move Penelope to the left
                setDirection(left);
                directionalKeyPressed = true;
                break;
            case KEY_PRESS_RIGHT:
                // move Penelope to the right
                setDirection(right);
                directionalKeyPressed = true;
                break;
            case KEY_PRESS_UP:
                // move Penelope up
                setDirection(up);
                directionalKeyPressed = true;
                break;
            case KEY_PRESS_DOWN:
                // move Penelope down
                setDirection(down);
                directionalKeyPressed = true;
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
        if (directionalKeyPressed) {
            computeDestinationCoordinates(destX, destY, getDirection());
            if (getStudentWorld()->playerCanMoveTo(destX, destY))
                moveTo(destX, destY);
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
    getStudentWorld()->addActor(new Landmine(getX(), getY(), getStudentWorld()));
}

void Penelope::useVaccine() {
    if (m_vaccines <= 0)
        return; // do nothing
    m_vaccines--;
    vaccinate();
}

void Penelope::computeDestinationCoordinates(int& destX, int& destY, Direction d) {
    destX = getX();
    destY = getY();
    switch (d) {
        case up:
            destY = getY() + 4;
            break;
        case down:
            destY = getY() - 4;
            break;
        case left:
            destX = getX() - 4;
            break;
        case right:
            destX = getX() + 4;
            break;
    }
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
            int zombieChance = randInt(1, 10);
            if (zombieChance <= 7)
                getStudentWorld()->addActor(new DumbZombie(getX(), getY(), getStudentWorld()));
            else
                getStudentWorld()->addActor(new SmartZombie(getX(), getY(), getStudentWorld()));
            return;
        }
    }
    if (m_paralyzed) {
        m_paralyzed = false;
        return; // paralysis tick
    } else {
        m_paralyzed = true;
    }
    // Citizen must determine its distance to Penelope
    double distanceToPlayer = getStudentWorld()->distanceToPlayer(getX(), getY());
    // Citizen must determine its distance to the nearest zombie
    double distanceToNearestZombie = getStudentWorld()->distanceToNearestZombie(getX(), getY()); // will be -1 if there are no zombies
    if ((getStudentWorld()->zombiesRemaining() == 0 || distanceToNearestZombie == -1 || distanceToPlayer < distanceToNearestZombie) && distanceToPlayer <= 80) { // wants to follow Penelope
        attemptToFollowPlayer();
    } else if (getStudentWorld()->zombiesRemaining() != 0 && distanceToNearestZombie != -1 && distanceToNearestZombie <= 80) { // wants to run away from Zombie
        attemptToFleeFromZombie();
    }
}

void Citizen::setDeadSpecialized(int deadID) {
    getStudentWorld()->markCitizenGone();
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

void Citizen::playInfectedSoundIfApplicable() {
    getStudentWorld()->playSound(SOUND_CITIZEN_INFECTED);
}

void Citizen::computeDestinationCoordinates(int& destX, int& destY, Direction d) {
    destX = getX();
    destY = getY();
    switch (d) {
        case up:
            destY = getY() + 2;
            break;
        case down:
            destY = getY() - 2;
            break;
        case left:
            destX = getX() - 2;
            break;
        case right:
            destX = getX() + 2;
            break;
    }
}

void Citizen::attemptToFollowPlayer() {
    int destX = getX();
    int destY = getY();
    if (getX() == getStudentWorld()->getPlayerX() || getY() == getStudentWorld()->getPlayerY()) {
        Direction destDir = getDirection();
        if (getX() == getStudentWorld()->getPlayerX()) {
            if (getY() < getStudentWorld()->getPlayerY()) {
                destDir = up;
            } else {
                destDir = down;
            }
            computeDestinationCoordinates(destX, destY, destDir);
        } else {
            if (getX() < getStudentWorld()->getPlayerX()) {
                destDir = right;
            } else {
                destDir = left;
            }
            computeDestinationCoordinates(destX, destY, destDir);
        }
        if (attemptToMoveTo(destX, destY, destDir))
            return;
    } else {
        Direction horizontalOption;
        Direction verticalOption;
        if (getX() < getStudentWorld()->getPlayerX())
            horizontalOption = right;
        else
            horizontalOption = left;
        
        if (getY() < getStudentWorld()->getPlayerY())
            verticalOption = up;
        else
            verticalOption = down;
        
        int randomDirection = randInt(1, 2);
        Direction chosenDirection;
        Direction otherDirection;
        if (randomDirection == 1) {
            chosenDirection = horizontalOption;
            otherDirection = verticalOption;
        } else {
            chosenDirection = verticalOption;
            otherDirection = horizontalOption;
        }
        computeDestinationCoordinates(destX, destY, chosenDirection);
        if (attemptToMoveTo(destX, destY, chosenDirection))
            return;
        else {
            computeDestinationCoordinates(destX, destY, otherDirection);
            if(attemptToMoveTo(destX, destY, otherDirection))
                return;
        }
    }
}

void Citizen::attemptToFleeFromZombie() {
    double distanceToCurrentNearestZombie = getStudentWorld()->distanceToNearestZombie(getX(), getY()); // will be -1 if no zombies
    
    std::map<int, double> potentialDirections;
    potentialDirections[90] = -1;
    potentialDirections[270] = -1;
    potentialDirections[180] = -1;
    potentialDirections[0] = -1;
    bool canGetFurtherAwayFromZombies = false;
    std::map<Direction, double>::iterator potentDirIt = potentialDirections.begin();
    while (potentDirIt != potentialDirections.end()) {
        int destX = getX();
        int destY = getY();
        Direction dir = right;
        switch (potentDirIt->first) {
            case 90:
                dir = up;
                break;
            case 270:
                dir = down;
                break;
            case 180:
                dir = left;
                break;
            case 0:
                dir = right;
                break;
        }
        computeDestinationCoordinates(destX, destY, dir);
        if (getStudentWorld()->agentCanMoveTo(this, destX, destY)) {
            double dist = getStudentWorld()->distanceToNearestZombie(destX, destY);
            if (dist != -1) {
                potentDirIt->second = dist;
                if (dist > distanceToCurrentNearestZombie) {
                    canGetFurtherAwayFromZombies = true;
                }
            }
        }
        potentDirIt++;
    }
    
    if (canGetFurtherAwayFromZombies) {
        Direction chosenDirection = getDirection();
        double furthest = distanceToCurrentNearestZombie;
        potentDirIt = potentialDirections.begin();
        while (potentDirIt != potentialDirections.end()) {
            if (potentDirIt->second != -1 && potentDirIt->second > furthest) {
                furthest = potentDirIt->second;
                switch (potentDirIt->first) {
                    case 90:
                        chosenDirection = up;
                        break;
                    case 270:
                        chosenDirection = down;
                        break;
                    case 180:
                        chosenDirection = left;
                        break;
                    case 0:
                        chosenDirection = right;
                        break;
                }
            }
            potentDirIt++;
        }
        setDirection(chosenDirection);
        int destX = getX();
        int destY = getY();
        computeDestinationCoordinates(destX, destY, chosenDirection);
        moveTo(destX, destY);
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
            getStudentWorld()->addActor(new Vomit(vomitX, vomitY, getDirection(), getStudentWorld()));
            getStudentWorld()->playSound(SOUND_ZOMBIE_VOMIT);
            return;
        }
    }
    
    if (m_movementPlanDistance <= 0)
        determineNewMovementPlan();
    
    int destX = getX();
    int destY = getY();
    computeDestinationCoordinates(destX, destY, getDirection());
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

void Zombie::computeDestinationCoordinates(int& destX, int& destY, Direction d) {
    destX = getX();
    destY = getY();
    switch (d) {
        case up:
            destY = getY() + 1;
            break;
        case down:
            destY = getY() - 1;
            break;
        case left:
            destX = getX() - 1;
            break;
        case right:
            destX = getX() + 1;
            break;
    }
}

void Zombie::setDeadSpecialized(int deadID) {
    getStudentWorld()->markZombieGone();
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
        int randomDirection = randInt(1, 4);
        int vacX = getX();
        int vacY = getY();
        switch (randomDirection) {
            case 1: // up
                vacY += SPRITE_HEIGHT;
                break;
            case 2: // down
                vacY -= SPRITE_HEIGHT;
                break;
            case 3: // left
                vacX -= SPRITE_WIDTH;
                break;
            case 4: // right
                vacX += SPRITE_WIDTH;
                break;
        }
        getStudentWorld()->addActor(new VaccineGoodie(vacX, vacY, getStudentWorld()));
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
    if (getStudentWorld()->overlapsWithCitizen(getX(), getY())) {
        Citizen* nearestCitizen = getStudentWorld()->getNearestCitizen(getX(), getY());
        if (nearestCitizen != nullptr) {
            nearestCitizen->setDead(DEAD_SAVED_USED);
        }
    }
    
    // 2. Determine if exit overlaps with Penelope AND there are no citizens left
    if (getStudentWorld()->citizensRemaining() == 0 && getStudentWorld()->overlapsWithPlayer(getX(), getY())) {
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
    if (getStudentWorld()->overlapsWithAgent(getX(), getY())) {
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
    getStudentWorld()->addActor(new Pit(getX(), getY(), getStudentWorld()));
}
