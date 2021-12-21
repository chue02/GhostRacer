#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

class Player;

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_GhostRacer = nullptr;
    border_y = 0;
    m_souls = 0;
    m_bonus = 5000;
}

Player* StudentWorld::getPlayer(){
    return m_GhostRacer;
}

int StudentWorld::init()
{
    m_bonus = 5000;
    m_souls = 2 * getLevel()+5;
    m_GhostRacer = new Player(IID_GHOST_RACER, 128, 32, false, true, 0, 90, 4.0, 0, 10, 100, this);
    int N = VIEW_HEIGHT/SPRITE_HEIGHT;
    for (int i=0; i < N; i++){
        Actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, i*SPRITE_HEIGHT, 0, -4, false, true, 0, 2.0, 2, this));
        Actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i*SPRITE_HEIGHT, 0, -4, false, true, 0, 2.0, 2, this));
    }
    int M = VIEW_HEIGHT/(4*SPRITE_HEIGHT);
    for (int i=0; i < M; i++){
        Actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, LEFT_EDGE + YELLOW_LINE_X, i*(4*SPRITE_HEIGHT), 0, -4, false, true, 0, 2.0, 2, this));
        Actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, RIGHT_EDGE - YELLOW_LINE_X, i*(4*SPRITE_HEIGHT), 0, -4, false, true, 0, 2.0, 2, this));
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_GhostRacer->doSomething(); // Start with GhostRacer
    vector<Actor*>:: iterator it;
    it = Actors.begin();
    while (it != Actors.end()){
        if ((*it)->isAlive()){
            (*it)->doSomething();
            if (!m_GhostRacer->isAlive()){ // If an object kills GhostRacer, end the level
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (soulsLeft()==0){
                playSound(SOUND_FINISHED_LEVEL);
                increaseScore(m_bonus);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
        it++; // If the level continues, go thorugh the next object
    }
    it = Actors.begin();
    while (it != Actors.end()){
        if (!(*it)->isAlive()){
            delete *it;
            it = Actors.erase(it);
        }
        else
            it++; // if it's not dead, skip it
    }
    addBorderLines();
    
    // Add all new objects
    if (addSouls()==0)
        Actors.push_back(new LostSouls(IID_SOUL_GOODIE, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, 0, -4, false, true, 0, 4, 2, this));
    if (addOilSlick()==0)
        Actors.push_back(new OilSlick(IID_OIL_SLICK, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, 0, -4, false, true, 0, randInt(2, 5), 2, this));
    if (addHolyWaterRefill()==0)
        Actors.push_back(new HolyWaterRefill(IID_HOLY_WATER_GOODIE, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, 0, -4, false, true, 90, 2.0, 2, this));
    if (addHumanPed()==0)
        Actors.push_back(new HumanPed(IID_HUMAN_PED, randInt(0, VIEW_WIDTH), VIEW_HEIGHT, 0, -4, true, true, 0, 2, 0, 0, 2, this));
    if (addZombie()==0)
        Actors.push_back(new ZombiePed(IID_ZOMBIE_PED, randInt(0, VIEW_WIDTH), VIEW_HEIGHT, 0, -4, true, true, 90, 4, 0, 0, 2, 0, this));
    addCab();
    
    // Update scoreboard
    ostringstream scoreText;
    scoreText << "Score: " << getScore() << "  Lvl: " << getLevel() << "  Souls2Save: " << soulsLeft() << "  Lives: " << getLives() << "  Health: " << getPlayer()->getHealth() << "  Sprays: " << getPlayer()->getHolyWater() << "  Bonus: " << getBonus();
    setGameStatText(scoreText.str());
    decBonus();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_GhostRacer;
    vector<Actor*>:: iterator it;
    it = Actors.begin();
    while (it != Actors.end()){
        delete *it;
        it = Actors.erase(it);
    }
}

// Helper functions

StudentWorld::~StudentWorld(){
    cleanUp();
}

void StudentWorld::decBonus(){
    if (m_bonus > 0)
        m_bonus--;
}

int StudentWorld::getBonus(){
    return m_bonus;
}

bool StudentWorld::OverlapAlgo(Actor *a, Actor* b){
    double delta_x = abs(a->getX() - b->getX());
    double delta_y = abs(a->getY() - b->getY());
    double radius_sum = a->getRadius() + b->getRadius();
    if (delta_x < radius_sum * .25 && delta_y < radius_sum * .6)
        return true;
    return false;
}
bool StudentWorld::doesOverlap(Actor *a){ // Goes through every object in vector to see if it overlaps with object in parameter
    vector<Actor*>:: iterator it;
    it = Actors.begin();
    while (it != Actors.end()){
        if(OverlapAlgo(a, *it)){
            if ((*it)->isAffectedByHolyWater()){ // Since the projectile is the only thing that needs this object, we can always call this if statement and be fine
                (*it)->HolyWaterDamage(1);
                return true;
            }
        }
        it++;
    }
    return false;
}

bool StudentWorld::doesOverlapPlayer(Actor *a){
    // While there are many similarities to the previous function with this one, I couldn't figure out how to consolidate this into one function
    // The advantages of doing it this way are that we always know what the second object will be (it's player), and we don't have to loop through everything, rather just make sure that if this object overlaps the player, then do this
    if (OverlapAlgo(a, getPlayer()))
        return true;
    return false;
}

void StudentWorld::addBorderLines(){
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    double m_deltaY = new_border_y - (get_borderY());
    double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
    double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
    if (m_deltaY >=SPRITE_HEIGHT){
        Actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, new_border_y, 0, -4, false, true, 0, 2.0, 2, this));
        Actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE, new_border_y, 0, -4, false, true, 0, 2.0, 2, this));
    }
    if (m_deltaY >= 4*SPRITE_HEIGHT){
        Actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, LEFT_EDGE + YELLOW_LINE_X, new_border_y, 0, -4, false, true, 0, 2.0, 2, this));
        Actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, RIGHT_EDGE - YELLOW_LINE_X, new_border_y, 0, -4, false, true, 0, 2.0, 2, this));
    }
}


double StudentWorld::get_borderY(){
    return border_y;
}

void StudentWorld::set_borderY(double y){
    border_y = y;
}

void StudentWorld::addActor(Actor *a){
    Actors.push_back(a);
}

Actor* StudentWorld::isCollisionAvoidant(int lane){ // Only checks for collision avoidant objects in the lane
    vector<Actor*>:: iterator it = Actors.begin();
    while (it != Actors.end()){
        if ((*it)->isCollisionAvoidant() && (*it)->getLane() == lane)
            return *it;
        it++;
    }
    return nullptr;
}

// I couldn't figure out how to consolidate the next two functions into one, since the the getY() comparison seemed so critical and it had to be unique
Actor* StudentWorld::isCollisionAvoidantFront(Actor *a){
   vector<Actor*>:: iterator it = Actors.begin();
    while (it != Actors.end()){
        if ((*it)->isCollisionAvoidant() && (*it)->getY() > a->getY() && (*it)->getLane() == a->getLane())
            return *it;
        it++;
    }
    return nullptr;
}

Actor* StudentWorld::isCollisionAvoidantBehind(Actor *a){
   vector<Actor*>:: iterator it = Actors.begin();
    while (it != Actors.end()){
        if ((*it)->isCollisionAvoidant() && (*it)->getY() < a->getY() && (*it)->getLane() == a->getLane()){
            return *it;
        }
        it++;
    }
    return nullptr;
}
// Player functions

// Agent functions
int StudentWorld::addHumanPed(){
    int ChanceHumanPed = max(200 - getLevel() * 10, 30);
    return randInt(0, ChanceHumanPed-1);
}

// Object functions

// souls
void StudentWorld::soulSaved(){
    m_souls--;
}

int StudentWorld::soulsLeft(){
    return m_souls;
}

int StudentWorld::addSouls(){
    return randInt(0, 99);
}

// oilslick
int StudentWorld::addOilSlick(){
    int chanceOilSlick = max(150 - getLevel() * 10, 40);
    return randInt(0, chanceOilSlick-1);
}

// holy water refill
int StudentWorld::addHolyWaterRefill(){
    int chanceHolyWater = 100 + 10 * getLevel();
    return randInt(0, chanceHolyWater-1);
}

// since zombiecab/zombieped have the same probabilty, we can consolidate this into one function
int StudentWorld::addZombie(){
    int chanceZombiePed = max(100 - getLevel()*10, 20);
    return randInt(0, chanceZombiePed-1);
}

// cab
void StudentWorld::addCab(){
    if (addZombie()!=0)
        return;
    int chosenLane = -1;
    double startX = -1;
    double startY = -1;
    double vertSpeed = -1;
    bool midChecked = false;
    bool leftChecked = false;
    int minCheck = 0;
    int maxCheck = 2;
    for (int i=0; i < 3; i++){
        int cur_lane = randInt(minCheck, maxCheck);
        if (midChecked == true & cur_lane == 1){
            if (leftChecked == false)
                cur_lane--;
            else
                cur_lane++;
        }
        if (cur_lane == 0)
            leftChecked = true;
        Actor* closestObject = isCollisionAvoidant(cur_lane);
        if (closestObject == nullptr || closestObject->getY() > VIEW_HEIGHT/3){
            if (getPlayer()->getLane() != cur_lane){
                chosenLane = cur_lane;
                startY = SPRITE_HEIGHT/2;
                vertSpeed = getPlayer()->getSpeed() + randInt(2,4);
                break;
            }
            
        }
        if (closestObject == nullptr || closestObject->getY() < VIEW_HEIGHT * 2/3){
            chosenLane = cur_lane;
            startY = VIEW_HEIGHT - SPRITE_HEIGHT/2;
            vertSpeed = getPlayer()->getSpeed() - randInt(2,4);
            break;
        }
    }
    if (chosenLane==-1)
        return;
    
    switch (chosenLane){
        case 0:
            startX = ROAD_CENTER - ROAD_WIDTH/3;
            break;
        case 1:
            startX = ROAD_CENTER;
            break;
        case 2:
            startX = ROAD_CENTER + ROAD_WIDTH/3;
            break;
        default:
            break;
    }
    Actors.push_back(new ZombieCab(IID_ZOMBIE_CAB, startX, startY, 0, vertSpeed, true, true, 90, 4, 0, 0, 3, false, this));
}
