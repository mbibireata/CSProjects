#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <algorithm>
#include <queue>

class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor: public GraphObject{
public:
    Actor(int ID, int startX, int startY, Direction dir, double size, int depth);
    
    virtual void doSomething();
    
    virtual StudentWorld* getWorld();

    virtual void setstwPtr(StudentWorld* world);
    double distanceTo(Actor* actor);
    virtual ~Actor();
    
    //getters and setters
    void setActive();
    void setInactive();
    bool getActiveStatus();
    
    void markForDelete();
    bool isMarkedForDelete();
    
    void setImpenetrable();
    bool isImpenetrable();
    
    int getHealth();
    bool getKillable();
    
    //mutators
    void setHealth(int amount);
    
    virtual void decHealth(int amount);
    
    void setKillable(bool canDie);
    
    
    
private:
    bool isActive;
    StudentWorld* stwptr;
    bool killable;
    bool markedForDelete;
    bool impenetrable;
    int health;
};


class Ice : public Actor {
public:
    Ice();
    Ice(int x, int y);
    virtual ~Ice();
};


class Iceman: public Actor{
public:
    Iceman(StudentWorld* world);
    virtual void doSomething();
    virtual void destroyIce();
    virtual void move(int key, int dx, int dy);
    
    
    //getters
    int getSquirtsLeft();
    
    int getSonarChargesLeft();
    
    int getCurrentGold();
    
    //mutators
    virtual void increaseSonarCharges();
    
    virtual void increaseCurrentGold();
    
    virtual void decSquirts();
    
    virtual void incSquirts();
    
    virtual void decSonarCharges();
    
    ~Iceman();
    
private:
    const int startX = 30;
    const int startY = 60;
    
    int squirtsLeft;
    int sonarChargesLeft;
    int currentGold;
};

class Protestor: public Actor{
private:
    enum state {moving, chasing, leaving};
    state currentState;
    int timeSinceMove;
    int timeBetweenMoves;
    int distToMove;
    
    bool atExit;
    bool nearIceman;
    bool canAnnoyAgain;
    
    int timeSinceAnnoy;
    int timeBetweenAnnoy;
    
    int lastTurnTick;
    int stunnedTicks;
    
public:
    Protestor(StudentWorld* world, int ID, int x, int y);
    virtual void doSomething();
    virtual ~Protestor(){}
    
    //getters and setters
    virtual int getTimeSinceMove();
    virtual int getTimeBetweenMoves();
    void setState(state input);
    
    //mutators
    virtual void resetTimeSinceMove();
    virtual void incTimeSinceMove();
    virtual bool searchForIceman();
    
    void isAtExit();
    
    virtual void hurtIceman();
    
    virtual bool canMoveToIceman();
    
    void move(Direction dir);
    
    Direction calculateDirection();
    
    Direction pickDirection();
    
    int calculateDistanceToMove(Direction dir);
    
    bool shouldTurn(Direction& newDir);
    
    virtual void decHealth(int amount);
    
};

class RegularProtestor:public Protestor{
public:
    RegularProtestor(StudentWorld* world, int ID, int x, int y);
    virtual void hurtIceman();
    virtual bool searchForIceman();
    virtual bool canMoveToIceman(int targetX, int targetY, int currentX, int currentY);
    virtual ~RegularProtestor();
};

class HardcoreProtestor:public Protestor{
};

class Item:public Actor{
public:
    Item(StudentWorld* world, int ID, int x, int y);
    virtual void doSomething();
    virtual ~Item();
};

class Boulder:public Actor{
public:
    Boulder(StudentWorld* world, int x, int y);
    int getAmount();
    virtual void doSomething();
    virtual ~Boulder();
private:
    int amount;
    enum boulderState {stable, waiting, falling};
    boulderState currentState;
    int timer;
};

class GoldNugget:public Item{
public:
    GoldNugget(StudentWorld* world, int x, int y);
    virtual void doSomething();
    virtual ~GoldNugget(){}
    
private:
    bool protestorPickup;
};

class OilBarrel:public Item{
public:
    OilBarrel(StudentWorld* world, int x, int y);
    virtual void doSomething(); 
    virtual ~OilBarrel(){}

};

class Water:public Item{
public:
    Water(StudentWorld* world, int x, int y);
    virtual void doSomething();
    ~Water();
    
private:
    int timer;
    int ticksAllowed; 
};

class Squirt:public Item{
public:
    Squirt(StudentWorld* world, int x, int y);
    virtual void doSomething();
    virtual ~Squirt();
    
private:
    //int m_imageID = IID_WATER_SPURT;
    int range;
    int distTraveled;
    bool hasShots;
};

class SonarKit:public Item{
public:
    SonarKit(StudentWorld* world, int x, int y);
    virtual void doSomething();
    virtual ~SonarKit();
private:
    int timer;
    int ticksAllowed; 
};


#endif // ACTOR_H_
