#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class GameWorld;
class StudentWorld;

const double PI = 3.14159265358979323846264338327950288419716939937510;

class Actor: public GraphObject{
    public:
    Actor(int imageID, double startX, double startY, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld);
    // Virtual functions
    virtual ~Actor();
    virtual void doSomething()=0;
    virtual bool isAffectedByHolyWater() const;
    virtual void HolyWaterDamage(int inflicted);
    int getLane() const;
    // Nonvirtual functions
    bool isAlive();
    bool isCollisionAvoidant() const;
    void changeAlive(bool status);
    void checkOutofBounds();
    StudentWorld* getStudentWorld() const;
    private:
        bool m_collisionAvoidance;
        bool m_alive;
        StudentWorld* m_StudentWorld; // pass this into constructor
};

//------------------------- PLAYER DECLARATION
class Player: public Actor{
    public:
        Player(int imageID, double startX, double startY, bool isCollisionAvoidant, bool isAlive, double speed, int dir, double size, unsigned int depth, int holyWater, int health, StudentWorld* sWorld);
        // Virtual function
        virtual ~Player();
        virtual void doSomething();
        void damage(int inflicted);
        //Mutators
        void setSpeed(int speed);

        // Nonvirtual helper functions
        void move();
        void borderCheck();
        void borderHit();
        double getSpeed();
        int getHolyWater();
        void getSpun();
        void increaseCharge(int charges);
        int getHealth();
    private:
        int m_health;
        int m_holyWater;
        double m_forwardSpeed;
    
};


//------------------------- AGENT DECLARATION
class Agent: public Actor{
    public:
    Agent(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, double movePlan, int health, StudentWorld* sWorld);
    // Virtual function
    virtual ~Agent();
    virtual void doSomething()=0;
    virtual bool isAffectedByHolyWater() const;
    // Helper nonvirtual functions
    void moveAgent();
    void changeHorizSpeed(double speed);
    void changeVertSpeed(double speed);
    double getHorizSpeed();
    double getVertSpeed();
    int getHealth();
    void damage(int inflicted);
    void decVertSpeed(double speed);
    void decMovementPlan();
    void changeMovementPlan(double newPlan);
    virtual void changeMovement();
    double getMovementPlan();
    private:
        double m_horizontalSpeed;
        double m_vertSpeed;
        double m_movementPlan;
        int m_health;
};


//------------------------- ZOMBIE CAB DECLARATION
class ZombieCab: public Agent{
    public:
    ZombieCab(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, double movePlan, int health, bool damagedPlayer, StudentWorld* sWorld);
    virtual ~ZombieCab();
    virtual void doSomething();
    virtual void HolyWaterDamage(int inflicted);
    virtual void changeMovement();
    private:
    bool damagedPlayerPrior();
    void setDamagedPlayer(bool damaged);
    void storeObject(Actor *a);
    bool hasDamagedPlayer;
    Actor* getObject();
    Actor* m_object;
};

//------------------------- HUMAN PEDESTRIAN DECLARATION
class HumanPed: public Agent{
    public:
        HumanPed(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, double movePlan, int health, StudentWorld* sWorld);
    // Virtual functions
    virtual ~HumanPed();
    virtual void doSomething();
    virtual void HolyWaterDamage(int inflicted);
    // Helper nonvirtual functions
};

//------------------------- ZOMBIE PED DECLARATION
class ZombiePed: public Agent{
    public:
        ZombiePed(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, double movePlan, int health, int GruntTicks, StudentWorld* sWorld);
    // Virtual functions
    virtual ~ZombiePed();
    virtual void doSomething();
    virtual void HolyWaterDamage(int inflicted);
    int getTicksTillGrunt();
    void decTicksTillGrunt();
    void resetTicks();
    private:
        int ticksTillGrunt;
};

//------------------------- BORDER LINE OBJECT
class BorderLine: public Actor{
    public:
    BorderLine(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld);
    // Virtual functions
    virtual ~BorderLine();
    virtual void doSomething();
    private:
    double m_horizontalSpeed;
    double m_vertSpeed;
};

//------------------------- HOLY WATER PROJECTILE
class HolyWaterProjectile: public Actor{
    public:
    HolyWaterProjectile(int imageID, double startX, double startY, double maxDistance, double distanceTravelled, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld);
    // Virtual functions
    virtual ~HolyWaterProjectile();
    virtual void doSomething();
    // Helper non virtual
    void incDistance(int distance);
    double getDistance();
    double getMaxDistance();
    private:
    double m_maxDistance;
    double m_distanceTravelled;
};

//------------------------- OBJECT DECLARATION
class Object: public Actor{
    public:
    Object(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld);
    // Virtual functions
    virtual ~Object();
    virtual void doSomething();
    bool ObjectAction(int SOUND_ID, int scoreIncrease);
    virtual void uniqueAction()=0;
    private:
    double m_horizontalSpeed;
    double m_vertSpeed;
};

//------------------------- HEALING GOODIE DECLARATION
class HealingGoodie: public Object{
    public:
    HealingGoodie(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld);
    // Virtual functions
    virtual ~HealingGoodie();
    virtual void uniqueAction();
    virtual bool isAffectedByHolyWater() const;
    virtual void HolyWaterDamage(int inflicted);
};

//------------------------- LOST SOULS DECLARATION
class LostSouls: public Object{
    public:
    LostSouls(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld);
    // Virtual functions
    virtual ~LostSouls();
    virtual void uniqueAction();
};

//------------------------- OIL SLICK DECLARATION
class OilSlick: public Object{
    public:
    OilSlick(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld);
// Virtual functions
    virtual ~OilSlick();
    virtual void uniqueAction();
};


//------------------------- HOLY WATER GOODIE REFILL DECLARATIOn
class HolyWaterRefill: public Object{
    public:
    HolyWaterRefill(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, bool isCollisionAvoidant, bool isAlive, int dir, double size, unsigned int depth, StudentWorld* sWorld);
// Virtual functions
    virtual ~HolyWaterRefill();
    virtual void uniqueAction();
    virtual bool isAffectedByHolyWater() const;
    virtual void HolyWaterDamage(int inflicted);
};

#endif // ACTOR_H_

