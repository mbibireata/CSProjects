#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <algorithm>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
        //player = new Iceman(this);
        statusText = "";
	}
    
    StudentWorld();

    virtual int init();
	
    virtual int move();

    virtual void cleanUp();
	
    virtual ~StudentWorld();
    
        //My functions
    
    void setDisplayText();
    
    //Player interactors
    Iceman* getPlayer(){
        return player;
    }
    
    
    int getMaxRows() const;
    int getMaxCols() const;
    
        //random coordinate generator
    //inclusive 
    void randomCoords(int minX, int minY, int maxX, int maxY, int& randomX, int& randomY);
    bool isValidPlacement(int x, int y);
    
    void goodyRandomCoords(int& randomX, int& randomY);
    bool goodyValidPlacement(int x, int y);
    
    
        //Ice interactors
    virtual void removeIce(int playerX, int playerY);
    virtual bool hasIce(int playerX, int playerY);
    virtual bool hasIce(Actor::Direction dir, int x, int y);
    
        //Boulder interactors
    std::vector<Actor*> getGameObjects();
    
    double distanceToPlayer(Actor* actor); 
    
    virtual bool checkBoulder(int x, int y);
    virtual bool canMove(int x, int y);
    virtual bool canMove(Actor::Direction dir, int x, int y);
    
    void decBarrels();
    
    int getBarrels();
    
    void addSquirt(int x, int y);
    
    void setGChance();
    
    int getGChance();
    
    void setRandChance();
    
    int getRandChance();
    
    int getTickCounter() const;
    
    void pathToSource(int x, int y, GraphObject::Direction (*directions)[61]);
    
private:
    std::vector<Actor*> gameObjects;
    static const int MAXROWS = 60;
    static const int MAXCOLS = 64;
    static const int tunnelBegin = 30;
    static const int tunnelEnd = 33;
    static const int tunnelDepthBegin = 4;
    static const int tunnelDepthEnd = 59;
    std::string statusText;

    
    Ice* livingIceBlocks[MAXCOLS][MAXROWS];
    Iceman* player;
    Boulder* boulder;
    
    int barrelsLeft;
    
    //protestor spawning 
    int tickCounter;
    
    //1 in gChance chance that a new goodie gets added on a particular tick
    int gChance;
    int randChance;
    
    const double sonarChance = .2;
    const double waterChance = .8;
    
};







#endif // STUDENTWORLD_H_
