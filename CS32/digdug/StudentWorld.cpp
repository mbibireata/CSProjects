#include "StudentWorld.h"
#include <sstream>
#include <iomanip>
#include <string>
#include <ctime> 
#include <cstdlib>
#include <algorithm>
#include <cmath>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

int StudentWorld::init()
{
    //generate random number seed based on system time
    srand(static_cast<unsigned int>(time(nullptr)));
    setGChance();
    tickCounter = 0;
    player = new Iceman(this);
    
    for (int i = 0; i<MAXCOLS; ++i){
        for (int j = 0; j<MAXROWS; ++j){
            if (i>=tunnelBegin && i<=tunnelEnd && j>=tunnelDepthBegin&&j<=tunnelDepthEnd) continue;
            livingIceBlocks[i][j]=new Ice(i,j);
        }
    }
    
    int numBoulders = std::min(static_cast<int>(getLevel())/2 + 2,9);
    
    int numGoldNuggets = std::max(5 - static_cast<double>(getLevel())/2,2.);
    int numBarrels = std::min(2 + static_cast<int>(getLevel()), 21);
    barrelsLeft = numBarrels; 
    
    int randomX;
    int randomY;
    
    for (int i = 0; i < numBoulders; ++i){
        randomCoords(0, 20, 60, 56, randomX, randomY); //sets randomX and randomY
        gameObjects.push_back(new Boulder(this, randomX, randomY));
    }
    
    for (int i = 0; i < numGoldNuggets; ++i){
        randomCoords(0, 0, 60, 56, randomX, randomY);
        gameObjects.push_back(new GoldNugget(this, randomX, randomY));
    }
    
    for (int i = 0; i < numBarrels; ++i) {
        randomCoords(0, 0, 60, 56, randomX, randomY);
        gameObjects.push_back(new OilBarrel(this, randomX, randomY));
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

//Yet To Implement:

int StudentWorld::move(){
    setDisplayText();
    
    //Give each actor a chance to do something
    if (player->getActiveStatus()){
        player->doSomething();
    }
    
    for (const auto& a : gameObjects){
        a->doSomething(); 
    }
    
    vector<Actor*>::iterator it = gameObjects.begin();
    
    
    while (it != gameObjects.end()) {
        if ((*it)->isMarkedForDelete()) {
            delete (*it);
            it = gameObjects.erase(it);
        }
        else ++it;
    }
    
    int randX, randY;
    setRandChance();
    //if ticker hits, spawn an item
    if (getRandChance() == getLevel() * 25 + 1){
        int i = rand() % 100;
        if (i < 20) {
            gameObjects.push_back(new SonarKit(this, 0, MAXROWS));
        }
        else {
            goodyRandomCoords(randX, randY);
            gameObjects.push_back(new Water(this, randX, randY));
        }
    }
    
    //spawn a protestor
    if (tickCounter == 0) {
        gameObjects.push_back(new RegularProtestor(this, IID_PROTESTER, MAXCOLS-4, MAXROWS));
    }
    else {
        int T = max(25, 200 - static_cast<int>(getLevel()));
        int P = min(15, 2 + static_cast<int>(getLevel() * 1.5));
        
        if (tickCounter % T == 0 && tickCounter < T * P) {
            gameObjects.push_back(new RegularProtestor(this, IID_PROTESTER, MAXCOLS-4, MAXROWS));
        }
    }
    ++tickCounter;
    
    //Remove newly-dead actors after each tick
    //removeDeadGameObjects(); //delete dead game objects
    
    //return the proper result
    if (!player->getActiveStatus()){
        if (getLives()>0){
            decLives();
            if (player->getHealth()<=0){
                playSound(SOUND_PLAYER_GIVE_UP);
            }
            return GWSTATUS_PLAYER_DIED;
        }
    }
    
    //If player collected all barrels on the current level
    //return the result that the player finished the level
    if (getBarrels()==0){
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    else return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
    delete player;
    for (int i = 0; i<MAXCOLS; ++i){
        for (int j = 0; j<MAXROWS; ++j){
            delete livingIceBlocks[i][j];
        }
    }
    std::vector<Actor*>::iterator it = gameObjects.begin();
    while (it!=gameObjects.end()){
        delete(*it);
        it = gameObjects.erase(it);
    }
}

StudentWorld::~StudentWorld() {
    delete player;
    for (int i = 0; i<MAXCOLS; ++i){
        for (int j = 0; j<MAXROWS; ++j){
            delete livingIceBlocks[i][j];
        }
    }
}

void StudentWorld::removeIce(int playerX, int playerY){
    if (playerX >= MAXCOLS || playerY >= MAXROWS) return;
    for (int i = playerX; i <= playerX+3 && i < MAXCOLS; ++i){
        for(int j = playerY; j<=playerY+3 && j < MAXROWS;++j){
            delete livingIceBlocks[i][j];
            livingIceBlocks[i][j]=nullptr;
        }
    }
}

bool StudentWorld::hasIce(int playerX, int playerY){
    if (playerX >= MAXCOLS || playerY >= MAXROWS) return 0;
    for (int i = playerX; i <= playerX+3 && i < MAXCOLS; ++i){
        for(int j = playerY; j<=playerY+3 && j < MAXROWS;++j){
            if (livingIceBlocks[i][j]!=nullptr)
                return 1;
        }
    }
    return 0;
}

bool StudentWorld::hasIce(Actor::Direction dir, int x, int y) {
    switch(dir) {
        case GraphObject::up:
            y += 1;
            break;
        case GraphObject::down:
            y -= 1;
            break;
        case GraphObject::left:
            x -= 1;
            break;
        case GraphObject::right:
            x += 1;
            break;
        default: break;
    }
    
    return hasIce(x,y);
}

    //RNG Implementations
void StudentWorld::randomCoords(int minX, int minY, int maxX, int maxY, int& randomX, int& randomY){
    do {
    randomX = minX + rand() % (maxX - minX + 1);
    randomY = minY + rand() % (maxY - minY + 1);
    } while (!isValidPlacement(randomX, randomY));
}

bool StudentWorld::isValidPlacement(int x, int y){
    //TODO: check placement
    if (x+3 >= tunnelBegin && x <= tunnelEnd) return false;
    for (const auto& a : gameObjects){
        int dx = a->getX() - x;
        int dy = a->getY() - y;
        if (dx*dx + dy*dy <= 36) return false;
    }
    return true;
}

void StudentWorld::goodyRandomCoords(int& randX, int& randY){
    do {
    randX = rand() % MAXCOLS;
    randY = rand() % MAXROWS + 1;
    } while (!goodyValidPlacement(randX, randY));
}

bool StudentWorld::goodyValidPlacement(int x, int y){
    if (hasIce(x,y)) return 0;
    else return 1;
}

//returns true if the desired coordinates will overlap with a boulder
bool StudentWorld::checkBoulder(int x, int y){
    for(const auto& a : getGameObjects()){
        for (int i = x; i<=x+3; ++i){
            for(int j = y; j<=y+3; ++j){
                if (i >= a->getX() && i<a->getX()+3 && j >= a->getY() && j< a->getY()+3){
                    return 1;
                }
            }
        }
    }
    return 0;
}

bool StudentWorld::canMove(int x, int y) {
    if (x < 0 || y < 0 || x > MAXCOLS-4 || y > MAXROWS){
        return false;
    }
    for (const auto& a : gameObjects){
        if (a->isImpenetrable()) {
            int dx = x - a->getX();
            
            int dy = y - a->getY();
            double distance = sqrt(static_cast<double>(dx)*dx + dy*dy);
            if (distance<=3.) return false; 
        }
    }
        
    
    return true;
}

bool StudentWorld::canMove(Actor::Direction dir, int x, int y) {
    switch(dir) {
        case GraphObject::up:
            y += 1;
            break;
        case GraphObject::down:
            y -= 1;
            break;
        case GraphObject::left:
            x -= 1;
            break;
        case GraphObject::right:
            x += 1;
            break;
        default: break;
    }
    
    return canMove(x,y); 
}



double StudentWorld::distanceToPlayer(Actor* actor) {
    double dx = player->getX() - actor->getX();
    double dy = player->getY() - actor->getY();
    return sqrt(dx*dx + dy*dy); 
}



void StudentWorld::setDisplayText(){
    
    int level = getLevel();
    int health = player->getHealth();
    int water = player->getSquirtsLeft();
    int gold = player->getCurrentGold();
    int lives = getLives(); 
    int score = getScore();
    int sonars = player->getSonarChargesLeft();
    int oil = getBarrels();
    
    ostringstream ossLevel;
    ossLevel.fill(' ');
    ossLevel << setw(2) << level;
    string niceLevel = ossLevel.str();
    
    ostringstream ossHealth;
    ossHealth.fill(' ');
    ossHealth << setw(3) << health;
    string niceHealth = ossHealth.str();
    
    ostringstream ossWater;
    ossWater.fill(' ');
    ossWater << setw(2) << water;
    string niceWater = ossWater.str();
    
    
    ostringstream ossGold;
    ossGold.fill(' ');
    ossGold << setw(2) << gold;
    string niceGold = ossGold.str();
    
    
    ostringstream ossOil;
    ossOil.fill(' ');
    ossOil << setw(2) << oil;
    string niceOil = ossOil.str();
    
    
    ostringstream ossSonar;
    ossSonar.fill(' ');
    ossSonar << setw(2) << sonars;
    string niceSonar = ossSonar.str();
    
    
    ostringstream ossScore;
    ossScore.fill('0');
    ossScore << setw(6) << score;
    string niceScore = ossScore.str();
    
    
    // Next, create a string from your statistics, of the form: // Lvl: 52 Lives: 3 Hlth: 80% Wtr: 20 Gld: 3 Oil Left: 2 Sonar: 1 Scr: 321000
    statusText = "Lvl: " + niceLevel + "  Lives: " + to_string(lives) + "  Hlth: " + to_string(static_cast<int>((static_cast<double>(health)/10)*100)) +
            "%" + "  Wtr: " + niceWater + "  Gld: " + niceGold + "  Oil Left: " + niceOil + "  Sonar: " + niceSonar
            + "  Scr: " + niceScore;
    // Finally, update the display text at the top of the screen with your // newly created stats
    setGameStatText(statusText);
    
    return;
}

void StudentWorld::pathToSource(int x, int y, GraphObject::Direction (*directions)[61]){
    int cols = MAXCOLS;
    int rows = MAXROWS+1;
    int distance[cols][rows];
    
    // Initialize distance array:
    //     -1: blocked path
    //   1000: large initial distance to source
    for (int i = 0; i < cols; ++i){
        for (int j = 0; j < rows; ++j) {
            //printf("%2d %2d %d %d\n", i, j, hasIce(i,j), canMove(i,j));
            distance[i][j] = (hasIce(i,j) || !canMove(i,j)) ? -1 : 1000;
            directions[i][j] = GraphObject::none;
        }
    }
    
    // Source distance set to 0
    distance[x][y] = 0;
    
    // update neighbors until no distance improves
    bool updated = true;
    while (updated) {
        updated = false;
        for (int i = 0; i < cols; ++i){
            for (int j = 0; j < rows; ++j) {
                if (distance[i][j] < 0) {
                    continue;
                }
                //printf("At %2d %2d %d\n", i, j, distance[i][j]);
                std::vector<std::pair<int, GraphObject::Direction> > neighbors;
                if (i > 0 && distance[i-1][j] >= 0 && distance[i][j] > distance[i-1][j] + 1) {
                    distance[i][j] = distance[i-1][j] + 1;
                    directions[i][j] = GraphObject::left;
                    updated = true;
                    //printf("Updated to %d dir left\n", distance[i][j]);
                }
                if (j > 0 && distance[i][j-1] >= 0 && distance[i][j] > distance[i][j-1] + 1) {
                    distance[i][j] = distance[i][j-1] + 1;
                    directions[i][j] = GraphObject::down;
                    updated = true;
                    //printf("Updated to %d dir down\n", distance[i][j]);
                }
                if (i + 1 < cols && distance[i+1][j] >= 0 && distance[i][j] > distance[i+1][j] + 1) {
                    distance[i][j] = distance[i+1][j] + 1;
                    directions[i][j] = GraphObject::right;
                    updated = true;
                    //printf("Updated to %d dir right\n", distance[i][j]);
                }
                if (j + 1 < rows && distance[i][j+1] >= 0 && distance[i][j] > distance[i][j+1] + 1) {
                    distance[i][j] = distance[i][j+1] + 1;
                    directions[i][j] = GraphObject::up;
                    updated = true;
                    //printf("Updated to %d dir up\n", distance[i][j]);
                }
                
            }
        }
    }
}

int StudentWorld::getMaxRows() const {
    return MAXROWS;
}

int StudentWorld::getMaxCols() const {
    return MAXCOLS;
}

vector<Actor*> StudentWorld::getGameObjects() {
    return gameObjects;
}

void StudentWorld::decBarrels() {
    --barrelsLeft;
}

int StudentWorld::getBarrels() {
    return barrelsLeft;
}

void StudentWorld::addSquirt(int x, int y) {
    int dir = player->getDirection();
    if (dir == GraphObject::right){
        x = player->getX()+4;
        y = player->getY();
    }
    else if (dir == GraphObject::left) {
        x = player->getX() - 4;
        y = player->getY();
    }
    else if (dir == GraphObject::up) {
        x = player->getX();
        y = player->getY() + 4;
    }
    else if (dir == GraphObject::down) {
        x = player->getX();
        y = player->getY() - 4;
    }
    gameObjects.push_back(new Squirt(this, x, y));
}

void StudentWorld::setGChance() {
    gChance = static_cast<int>(getLevel()) * 25  + 300;
}

int StudentWorld::getGChance() {
    return gChance;
}

void StudentWorld::setRandChance() {
    randChance = rand() % getGChance();
}

int StudentWorld::getRandChance() {
    return randChance;
}

int StudentWorld::getTickCounter() const {
    return tickCounter;
}


