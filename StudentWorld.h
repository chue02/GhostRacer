#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Player;

const double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
const double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
const double YELLOW_LINE_X = ROAD_WIDTH/3;

class StudentWorld : public GameWorld
{
public: 
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    virtual ~StudentWorld();
    // Helper functions
    double get_borderY();
    void set_borderY(double y);
    bool OverlapAlgo(Actor *a, Actor* b);
    bool doesOverlap(Actor* a);
    bool doesOverlapPlayer(Actor *a);
    Actor* isCollisionAvoidant(int lane);
    Actor* isCollisionAvoidantFront(Actor *a);
    Actor* isCollisionAvoidantBehind(Actor *a);
    void addActor(Actor *a);
    int getBonus();
    void decBonus();
    // object funcitons
    void addBorderLines();
    
    // AGENTS
    // human ped
    int addHumanPed();
    // GOODIES
    // souls
    void soulSaved();
    int soulsLeft();
    int addSouls();
    // oilslick
    int addOilSlick();
    // player stuff
    Player* getPlayer();
    // holy water refill
    int addHolyWaterRefill();
    // zombie 
    int addZombie();
    void addCab();

private:
    std::vector<Actor*> Actors;
    Player* m_GhostRacer;
    double border_y;
    int m_souls;
    int m_bonus;
};

#endif // STUDENTWORLD_H_
