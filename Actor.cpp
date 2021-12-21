#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <algorithm>
#include <string>
#include <iostream>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//---------------- ACTOR IMPLEMENTATION

Actor::Actor(int imageID, double startX, double startY, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld) : GraphObject(imageID,startX, startY,dir,size,depth){
    m_collisionAvoidance = isCollisionAvoidant;
    m_alive = isAlive;
    m_StudentWorld = sWorld;
};

Actor::~Actor(){
}

void Actor::changeAlive(bool status){
    m_alive = status;
}

bool Actor::isAffectedByHolyWater() const{
    return false; // almost all actors shouldn't be affected by holy water
}

void Actor::HolyWaterDamage(int inflicted){
    return; // most actors weren't affected by holy water, so I just made this return by default and had it changed in the subclasses
}
// Nonvirtual

// I needed these subseqeuent accessor functions to communicate between objects and StudentWorld
bool Actor::isAlive(){
    return m_alive;
}

StudentWorld* Actor::getStudentWorld() const{
    return m_StudentWorld;
}

bool Actor::isCollisionAvoidant() const{
    return m_collisionAvoidance;
}

int Actor::getLane() const{ // returns a corresponding number to each lane they're in (each lane is a range)
    if (getX() > LEFT_EDGE + YELLOW_LINE_X && getX() < RIGHT_EDGE - YELLOW_LINE_X)
        return 1;
    else if (getX() <= LEFT_EDGE + YELLOW_LINE_X)
        return 0;
    else if (getX() >= RIGHT_EDGE - YELLOW_LINE_X)
        return 2;
    return -1;
}

void Actor::checkOutofBounds(){
    if(getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
        changeAlive(false);
}

//---------------- PLAYER IMPLEMENTAITON
Player::Player(int imageID, double startX, double startY, bool isCollisionAvoidant, bool isAlive, double speed, int dir, double size , unsigned int depth, int holyWater, int health, StudentWorld* sWorld) : Actor(imageID, startX, startY, isCollisionAvoidant, isAlive, dir, size, depth, sWorld){
    m_forwardSpeed = speed;
    m_health = health;
    m_holyWater = holyWater;
};


// Virtual
Player::~Player(){
}

void Player::doSomething(){
    if (isAlive()){
        borderCheck();
        int ch;
        if (getStudentWorld()->getKey(ch)){
            switch(ch){
                case KEY_PRESS_SPACE:
                    if (getHolyWater()<=0)
                        break;
                    getStudentWorld()->addActor(new HolyWaterProjectile(IID_HOLY_WATER_PROJECTILE, getStudentWorld()->getPlayer()->getX(), getStudentWorld()->getPlayer()->getY(), 160, 0, false, true, getStudentWorld()->getPlayer()->getDirection(), 1, 1, getStudentWorld()));
                    getStudentWorld()->playSound(SOUND_PLAYER_SPRAY);
                    increaseCharge(-1); // Sprays holy water by creating a new actor and decrementing charge
                    break;
                case KEY_PRESS_LEFT: // Following are movement algorithms
                    if (getDirection() < 114)
                        setDirection(getDirection()+8);
                    break;
                case KEY_PRESS_RIGHT:
                    if (getDirection() > 66)
                        setDirection(getDirection()-8);
                    break;
                case KEY_PRESS_UP:
                    if (getSpeed() < 5)
                        setSpeed(getSpeed()+1);
                    break;
                case KEY_PRESS_DOWN:
                    if (getSpeed() > -1)
                        setSpeed(getSpeed()-1);
                    break;
            }
        }
        move();
    }
    return;
}

// Non-virtual
void Player::move(){ // General move function for it to call per tick
    double max_shift_per_tick = 4;
    double direction = getDirection();
    double delta_x = cos(direction * PI/180) * max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x+delta_x, cur_y);
}

void Player::damage(int inflicted){
    if (inflicted < 0 && m_health - inflicted > 100){ // If inflicted is negative like in healing goodie, then heal the player to 100, unless it's already at 100 or near 100
        m_health = 100;
        return;
    }
    m_health -= inflicted;
    if (m_health <= 0){
        changeAlive(false);
        getStudentWorld()->playSound(SOUND_PLAYER_DIE);
    }
}

void Player::borderCheck(){
    //check if it's swerving off road with boundary objects
    // the bounds are constant anyway
    double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
    double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
    if (getX() <= LEFT_EDGE){
        if (getDirection()>90){  // FOR LEFT
            setDirection(82);
            borderHit();
        }
    }
    else if (getX() >= RIGHT_EDGE){
        if (getDirection()<90){
            setDirection(98);
            borderHit();
        }
    }
}

void Player::borderHit(){ // call this function to implement consequences of getting hit by border, as well as move player away from it
    damage(10);
    getStudentWorld()->playSound(SOUND_VEHICLE_CRASH);
    move();

}

double Player::getSpeed(){
    return m_forwardSpeed;
}

void Player::setSpeed(int speed){
    m_forwardSpeed = speed;
}

void Player::getSpun(){
    int magnitude = randInt(0,1);
    if (magnitude == 0) // if the magnitude isn't 1, then make it -1 so that way the direction can randomly be positive or negative
        magnitude = -1;
    int spinDirection = randInt(5, 20);
    if (getDirection() + magnitude * spinDirection < 60) // ensures player's direction can't be <60
        setDirection(60);
    else if (getDirection() + magnitude * spinDirection > 120) // ensure player's direction can't be >120
        setDirection(120);
    else
        setDirection(getDirection() + magnitude * spinDirection);
}

int Player::getHolyWater(){
    return m_holyWater;
}

void Player::increaseCharge(int charges){
    m_holyWater += charges;
}

int Player::getHealth(){
    return m_health;
}
//--------------------- AGENT IMPLEMENTATION
Agent::Agent(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, double movePlan, int health, StudentWorld* sWorld): Actor(imageID, startX, startY, isCollisionAvoidant, isAlive, dir, size, depth, sWorld){
    m_horizontalSpeed = horizSpeed;
    m_vertSpeed = vertSpeed;
    m_health = health;
    m_movementPlan = movePlan;
};

Agent::~Agent(){
};

bool Agent::isAffectedByHolyWater() const{ // all of the following agents are affected by holy water
    return true;
}

void Agent::moveAgent(){
    // every agent has the same move function, so this will be in every agent's doSomething
    int vert_speed = m_vertSpeed - getStudentWorld()->getPlayer()->getSpeed();
    int horiz_speed = m_horizontalSpeed;
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
    checkOutofBounds();
}

// Helper functions
void Agent::changeHorizSpeed(double speed){
    m_horizontalSpeed = speed;
}

double Agent::getHorizSpeed(){
    return m_horizontalSpeed;
}

double Agent::getVertSpeed(){
    return m_vertSpeed;
}

void Agent::decVertSpeed(double speed){
    m_vertSpeed -= speed; // needed for zombie cabs
}

void Agent::changeVertSpeed(double speed){
    m_vertSpeed = speed;
}

int Agent::getHealth(){
    return m_health; // I could've made Player an Agent since they have health in common, but I didn't have time to make player an Agent
}

void Agent::damage(int inflicted){
    m_health -= inflicted; // Like the previous function, this is a similar function in Player, but I didn't have time to merge them into the same hierarchy
    if (m_health <= 0)
        changeAlive(false);
}

void Agent::changeMovement(){
    int magnitude = randInt(0,1); // this and the following 2 lines could've been a general funciton in Actor, but I didn't have time to make it one
    if (randInt(0,1)==0)
        magnitude = -1;
    changeHorizSpeed(magnitude * randInt(1,3));
    changeMovementPlan(randInt(4,32));
    if (getHorizSpeed()<0) // reverse Agent's direction
        setDirection(180);
    else if (getHorizSpeed()>0)
        setDirection(0);
}

void Agent::decMovementPlan(){
    m_movementPlan--;
}

void Agent::changeMovementPlan(double newPlan){
    m_movementPlan = newPlan;
}
double Agent::getMovementPlan(){
    return m_movementPlan;
}


//--------------------- ZOMBIE CAB IMPLEMENTATION
ZombieCab::ZombieCab(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, double movePlan, int health, bool damagedPlayer, StudentWorld* sWorld): Agent(imageID, startX, startY, horizSpeed, vertSpeed, isCollisionAvoidant, isAlive, dir, size, depth, movePlan, health, sWorld){
    hasDamagedPlayer = damagedPlayer;
};

ZombieCab::~ZombieCab(){};

void ZombieCab::HolyWaterDamage(int inflicted){
    damage(inflicted);
    if(!isAlive()){
        getStudentWorld()->playSound(SOUND_VEHICLE_DIE);
        if (!getStudentWorld()->doesOverlapPlayer(this)){
            int chance = randInt(1, 5);
            if (chance==1)
                getStudentWorld()->addActor(new OilSlick(IID_OIL_SLICK, getX(), getY(), 0, -4, false, true, 0, randInt(2,5), 2, getStudentWorld()));
            getStudentWorld()->increaseScore(200);
        }
    }
    else
        getStudentWorld()->playSound(SOUND_VEHICLE_HURT);
}

bool ZombieCab::damagedPlayerPrior(){
    return hasDamagedPlayer;
}

void ZombieCab::setDamagedPlayer(bool damaged){
    hasDamagedPlayer = damaged;
}

void ZombieCab::storeObject(Actor* a){
     m_object = a;
}

void ZombieCab::changeMovement(){
    // since ZombieCab moves differently from the other agents, it neeeds its unique changeMovement function
    changeMovementPlan(randInt(4,32));
    changeVertSpeed(getVertSpeed() + randInt(-2,2));
}

Actor* ZombieCab::getObject(){
    return m_object;
}

void ZombieCab::doSomething(){
    if (!isAlive())
        return;
    if (getStudentWorld()->doesOverlapPlayer(this)){
        if(!damagedPlayerPrior()){
            getStudentWorld()->playSound(SOUND_VEHICLE_CRASH);
            getStudentWorld()->getPlayer()->damage(20);
            if (getX() <= getStudentWorld()->getPlayer()->getX()){
                changeHorizSpeed(-5);
                setDirection(120 + randInt(0, 19));
            }
            else if (getX() > getStudentWorld()->getPlayer()->getX()){
                changeHorizSpeed(5);
                setDirection(60 - randInt(0, 19));
            }
            setDamagedPlayer(true);
        }
    }
    moveAgent();
    if (getVertSpeed() > getStudentWorld()->getPlayer()->getSpeed()){
        storeObject(getStudentWorld()->isCollisionAvoidantFront(this));
        if (getObject() != nullptr){
            if (getObject()->getY()- getY() > 96){
                decVertSpeed(0.5);
                return;
            }
        }
    }
    if (getVertSpeed() <= getStudentWorld()->getPlayer()->getSpeed()){
        storeObject(getStudentWorld()->isCollisionAvoidantBehind(this));
        if (getObject() != nullptr){
            if (getObject()->getY()- getY() < 96){
                decVertSpeed(-0.5);
                return;
            }
        }
    }
    changeMovement();
}

//--------------------- HUMAN PEDESTRIAN IMPLEMENTATION

HumanPed::HumanPed(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, double movePlan, int health, StudentWorld* sWorld): Agent(imageID, startX, startY, horizSpeed, vertSpeed, isCollisionAvoidant, isAlive, dir, size, depth, movePlan, health, sWorld){
};

HumanPed::~HumanPed(){
};


void HumanPed::HolyWaterDamage(int inflicted){ // never uses inflicted, but since HolyWaterDamage is a more generic function, it's easier to not have to function overload
    changeHorizSpeed(getHorizSpeed()*-1);
    if (getHorizSpeed()<0)
        setDirection(180);
    else
        setDirection(0);
}

void HumanPed::doSomething(){
    if (!isAlive())
        return;
    if (getStudentWorld()->doesOverlapPlayer(this)){
        getStudentWorld()->getPlayer()->changeAlive(false); // insta kill the player
        return;
    }
    moveAgent();
    decMovementPlan();
    if (getMovementPlan() > 0)
        return;
    changeMovement();
}

//--------------------- ZOMBIE PED IMPLEMENTATION
ZombiePed::ZombiePed(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, double movePlan, int health, int GruntTicks, StudentWorld* sWorld): Agent(imageID, startX, startY, horizSpeed, vertSpeed, isCollisionAvoidant, isAlive, dir, size, depth, movePlan, health, sWorld){
    ticksTillGrunt = GruntTicks;
};

ZombiePed::~ZombiePed(){};

int ZombiePed::getTicksTillGrunt(){
    return ticksTillGrunt;
}

void ZombiePed::resetTicks(){
    ticksTillGrunt = 20;
}

void ZombiePed::decTicksTillGrunt(){
    ticksTillGrunt--;
}

void ZombiePed::doSomething(){
    if (!isAlive())
        return;
    if (getStudentWorld()->doesOverlapPlayer(this)){
        getStudentWorld()->getPlayer()->damage(5);
        damage(2);
        if (!isAlive())
            getStudentWorld()->playSound(SOUND_PED_DIE);
        return;
    }
    if (abs(getX() - getStudentWorld()->getPlayer()->getX()) <= 30 && getY() > getStudentWorld()->getPlayer()->getY()){
        setDirection(270);{
            if (getX() < getStudentWorld()->getPlayer()->getX())
                changeHorizSpeed(1);
            else if (getX() > getStudentWorld()->getPlayer()->getX())
                changeHorizSpeed(-1);
            else
                changeHorizSpeed(0);
            decTicksTillGrunt();
            if (getTicksTillGrunt() <= 0){
                getStudentWorld()->playSound(SOUND_ZOMBIE_ATTACK);
                resetTicks();
            }
        }
    }
    moveAgent();
    if (getMovementPlan() > 0){
        decMovementPlan();
        return;
    }
    else
        changeMovement();
}

void ZombiePed::HolyWaterDamage(int inflicted){
    damage(inflicted);
    if(!isAlive()){
        getStudentWorld()->playSound(SOUND_PED_DIE);
        if (!getStudentWorld()->doesOverlapPlayer(this)){
            int chance = randInt(1, 5);
            if (chance==1)
                getStudentWorld()->addActor(new HealingGoodie(IID_HEAL_GOODIE, getX(), getY(), 0, -4, false, true, 0, 1, 2, getStudentWorld()));
            getStudentWorld()->increaseScore(150);
        }
    }
    else
        getStudentWorld()->playSound(SOUND_PED_HURT);
}

//--------------------- BORDERLINE IMPLEMENTATION
BorderLine::BorderLine(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld) : Actor(imageID, startX, startY, isCollisionAvoidant, isAlive, dir, size, depth, sWorld){
    m_horizontalSpeed = horizSpeed;
    m_vertSpeed = vertSpeed;
    if (startX ==  ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH/3 || startX ==  ROAD_CENTER - ROAD_WIDTH / 2 -
        ROAD_WIDTH/3)
        getStudentWorld()->set_borderY(startY);
};

// Virtual functions

BorderLine::~BorderLine(){
}

void BorderLine::doSomething(){
    // Like Agent's movement, a lot of this could've been done under one shared funciton in actor, however some lines have to be in a specific order for BorderLines to spawn properly
    int vert_speed = m_vertSpeed - getStudentWorld()->getPlayer()->getSpeed();
    int horiz_speed = m_horizontalSpeed;
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    if (getStudentWorld()->get_borderY() == getY()){ // If it's time to add a new border (at the Y location of the newest border), and if the X is equal to the white borders (so that way we won't add an extra yellow border), then replace Y in StudentWorld to the same as the Y in the newest border to be added
        if (getX() ==  ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH/3){
            getStudentWorld()->set_borderY(new_y);
        }
    }
    moveTo(new_x, new_y);
    checkOutofBounds();
}

//--------------------- HOLY WATER PROJECTILE IMPLEMENTATION
HolyWaterProjectile::HolyWaterProjectile(int imageID, double startX, double startY, double maxDistance, double distanceTravelled, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld): Actor(imageID, startX, startY, isCollisionAvoidant, isAlive, dir, size, depth, sWorld){
    m_maxDistance = maxDistance;
    m_distanceTravelled = distanceTravelled;
};

HolyWaterProjectile::~HolyWaterProjectile(){
};

void HolyWaterProjectile::incDistance(int distance){
    m_distanceTravelled+=distance;
}

double HolyWaterProjectile::getDistance(){
    return m_distanceTravelled;
}

double HolyWaterProjectile::getMaxDistance(){
    return m_maxDistance;
}

void HolyWaterProjectile::doSomething(){
    if (!isAlive())
        return;
    if (getStudentWorld()->doesOverlap(this)){
        changeAlive(false);
        return;
    }
    else{
        incDistance(SPRITE_HEIGHT);
        moveForward(SPRITE_HEIGHT);
    }
    checkOutofBounds();
    if (!isAlive())
        return;
    if (getDistance() >= getMaxDistance())
        changeAlive(false);
    
    
}
//--------------------- OBJECT IMPLEMENTATION
Object::Object(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld) : Actor(imageID, startX, startY, isCollisionAvoidant, isAlive, dir, size, depth, sWorld){
    m_horizontalSpeed = horizSpeed;
    m_vertSpeed = vertSpeed;
};

// virtual function
Object::~Object(){
}

void Object::doSomething(){
    // This could've been in actor, but I there wasn't enough time
    int vert_speed = m_vertSpeed - getStudentWorld()->getPlayer()->getSpeed();
    int horiz_speed = m_horizontalSpeed;
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
    checkOutofBounds();
    if (!isAlive())
        return;
    uniqueAction(); // every object does something different, so call this funciton
}

bool Object::ObjectAction(int SOUND_ID, int scoreIncrease){
    // most objects do the same thing when it overlaps a player, so call this function if it does overlap the player and play sound and increase score
    if(getStudentWorld()->doesOverlapPlayer(this)){
        changeAlive(false);
        getStudentWorld()->playSound(SOUND_ID);
        getStudentWorld()->increaseScore(scoreIncrease);
        return true;
    }
    return false;
}


//--------------------  HEALING GOODIE IMPLEMENTATION
HealingGoodie::HealingGoodie(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld): Object(imageID, startX,startY, horizSpeed, vertSpeed, isCollisionAvoidant, isAlive, dir, size, depth, sWorld){};

HealingGoodie::~HealingGoodie(){
}

void HealingGoodie::uniqueAction(){
    if(ObjectAction(SOUND_GOT_GOODIE, 250)){ // if there's overlap, do its unique action
        getStudentWorld()->getPlayer()->damage(-10);
    }
}

bool HealingGoodie::isAffectedByHolyWater() const{
    return true;
}

void HealingGoodie::HolyWaterDamage(int inflicted){
    changeAlive(false); // since this really doesn't have health, all it needs to do is despawn when it gets hit with holy water
}


//--------------------- LOST SOULS IMPLEMENTATION
LostSouls::LostSouls(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld): Object(imageID, startX,startY, horizSpeed, vertSpeed, isCollisionAvoidant, isAlive, dir, size, depth, sWorld){};

LostSouls::~LostSouls(){
}

void LostSouls::uniqueAction(){
    if (ObjectAction(SOUND_GOT_SOUL, 100)){
        getStudentWorld()->soulSaved();
    }
    setDirection(getDirection()-10);
}

//--------------------- OIL SLICK IMPLEMENTATION
OilSlick::OilSlick(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld): Object(imageID, startX,startY, horizSpeed, vertSpeed, isCollisionAvoidant, isAlive, dir, size, depth, sWorld){};

OilSlick::~OilSlick(){
};

void OilSlick::uniqueAction(){
    // Since oilsick doesn't increase score, nor does it die when it overlaps player, don't call ObjectAction
    if (getStudentWorld()->doesOverlapPlayer(this)){
        getStudentWorld()->getPlayer()->getSpun();
        getStudentWorld()->playSound(SOUND_OIL_SLICK);
    }
}

//--------------------  HOLY WATER REFILL IMPLEMENTATION
HolyWaterRefill::HolyWaterRefill(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld): Object(imageID, startX,startY, horizSpeed, vertSpeed, isCollisionAvoidant, isAlive, dir, size, depth, sWorld){};

HolyWaterRefill::~HolyWaterRefill(){
};

void HolyWaterRefill::uniqueAction(){
    if (ObjectAction(SOUND_GOT_GOODIE, 50)){
        getStudentWorld()->getPlayer()->increaseCharge(10);
    }
}

bool HolyWaterRefill::isAffectedByHolyWater() const{
    // same thing with healing goodie
    return true;
}

void HolyWaterRefill::HolyWaterDamage(int inflicted){
    changeAlive(false);
}
