#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <queue>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor Implementations;
Actor::Actor(int ID, int startX, int startY, Direction dir, double size, int depth):GraphObject(ID,startX,startY,dir,size,depth),
isActive(false),markedForDelete(false),impenetrable(false){
    setActive();
    setVisible(true);
}

void Actor::setActive() {
    isActive = true;
}

void Actor::setInactive(){
    isActive = false;
}

bool Actor::getActiveStatus(){
    return isActive;
}

void Actor::doSomething(){}

Actor::~Actor(){}

double Actor::distanceTo(Actor* actor) {
    int dx = actor->getX() - getX();
    int dy = actor->getY() - getY();
    
    return sqrt(dx*dx + dy*dy);
}

StudentWorld* Actor::getWorld(){
    return stwptr;
}

void Actor::setstwPtr(StudentWorld* world) {
    stwptr = world;
}

void Actor::markForDelete() {
    markedForDelete = true;
}

bool Actor::isMarkedForDelete() {
    return markedForDelete;
}

void Actor::setImpenetrable() {
    impenetrable = true;
}

bool Actor::isImpenetrable() {
    return impenetrable;
}

int Actor::getHealth() {
    return health;
}

void Actor::setHealth(int amount) {
    health = amount;
}

void Actor::decHealth(int amount) {
    health -= amount;
}

void Actor::setKillable(bool canDie) {
    killable = canDie;
}

bool Actor::getKillable() {
    return killable;
}

//Ice Implementations: 
Ice::Ice():Actor(IID_ICE,0,0, GraphObject::right, .25, 3){}

Ice::Ice(int x, int y):Actor(IID_ICE, x, y, GraphObject::right, .25, 3){}

Ice::~Ice(){}

//Iceman Implementations:
Iceman::Iceman(StudentWorld* world):Actor(IID_PLAYER, 30, 60, GraphObject::right, 1., 0), squirtsLeft(5), sonarChargesLeft(1), currentGold(0){
    setHealth(10);
    setstwPtr(world);
}

void Iceman::move(int key, int dx, int dy){
    if(getDirection()!=GraphObject::left && key == KEY_PRESS_LEFT) setDirection(GraphObject::left);
    else if (getDirection()!=GraphObject::right && key == KEY_PRESS_RIGHT) setDirection(GraphObject::right);
    else if (getDirection()!=GraphObject::up && key == KEY_PRESS_UP) setDirection(GraphObject::up);
    else if (getDirection()!=GraphObject::down && key == KEY_PRESS_DOWN) setDirection(GraphObject::down);
    
    else {
        int newX = getX() + dx;
        int newY = getY() + dy;
        if (getWorld()->canMove(newX, newY)){
            moveTo(newX, newY); 
        }
        else moveTo(getX(), getY());
    }
    
}

void Iceman::doSomething(){
    if (getHealth() <= 0) setInactive();
    
    if (!getActiveStatus()) return;
    destroyIce();
    
    int ch;
    if (getWorld()->getKey(ch)==true){
        switch (ch){
            case KEY_PRESS_ESCAPE:
                setInactive(); 
                break;
            
            case KEY_PRESS_SPACE:
                if (getSquirtsLeft() > 0) {
                    decSquirts();
                    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                    getWorld()->addSquirt(getX(),getY());
                }
                break;
            case KEY_PRESS_TAB:
                //TODO drop gold
                break;
            case KEY_PRESS_LEFT:
                move(ch,-1,0);
                break;
            case KEY_PRESS_RIGHT:
                move(ch,1,0);
                break;
            case KEY_PRESS_UP:
                move(ch,0,1);
                break;
            case KEY_PRESS_DOWN:
                move(ch,0,-1);
                break;
            case 'z':
            case 'Z':
                if (getSonarChargesLeft() > 0){
                    getWorld()->playSound(SOUND_SONAR);
                    decSonarCharges();
                    for(auto& x : getWorld()->getGameObjects()) {
                        if (getWorld()->distanceToPlayer(x) <= 12.){
                            x->setVisible(true);
                            
                        }
                    }
                    
                }
                break;
        }
    }
}


void Iceman::destroyIce(){
    if (getWorld()->hasIce(getX(), getY())){
        getWorld()->playSound(SOUND_DIG);
    }
    getWorld()->removeIce(getX(),getY());
    
}

int Iceman::getSquirtsLeft() {
    return squirtsLeft;
}

int Iceman::getSonarChargesLeft() {
    return sonarChargesLeft;
}

int Iceman::getCurrentGold() {
    return currentGold;
}

void Iceman::increaseSonarCharges() {
    ++sonarChargesLeft;
}

void Iceman::decSonarCharges() {
    --sonarChargesLeft;
}

void Iceman::increaseCurrentGold() {
    ++currentGold;
}

void Iceman::decSquirts() {
    --squirtsLeft;
}

void Iceman::incSquirts() {
    ++squirtsLeft;
}

Iceman::~Iceman(){}

//Boulder Implementations: 
Boulder::Boulder(StudentWorld* world, int x, int y):Actor(IID_BOULDER,x,y,GraphObject::down,1.,1), currentState(stable){
    setstwPtr(world);
    setImpenetrable();
    getWorld()->removeIce(getX(),getY());
}

void Boulder::doSomething(){
    switch(currentState){
        case stable:
            if (!getWorld()->hasIce(getX(),getY()-1)){
                currentState = waiting;
                timer = 0;
                
            }
            break;
            
        case waiting:
            ++timer;
            if (timer == 30) {
                getWorld()->playSound(SOUND_FALLING_ROCK);
                currentState = falling;
            }
            break;
            
        case falling:
            int x, y;
            x = getX();
            y = getY();
            if (!getWorld()->hasIce(x,y-1) && y > 0){
                moveTo(x,y-1);
            }
            else markForDelete();
            if (getWorld()->distanceToPlayer(this) <= 3.){
                getWorld()->getPlayer()->decHealth(100);
                getWorld()->getPlayer()->setInactive();
                break;
            }
            for(auto& x : getWorld()->getGameObjects()) {
                
                if(x->getKillable() && distanceTo(x) <= 3.) {
                    x->decHealth(100);
                }
            }
            break;
    }
}

int Boulder::getAmount() {
    return amount;
}

Boulder::~Boulder(){}

//Item class implementations
Item::Item(StudentWorld* world, int ID, int x, int y):Actor(ID,x,y,GraphObject::right,1.,2){
    setstwPtr(world);
    setVisible(false);
}

void Item::doSomething(){
    
}


Item::~Item(){}

//Oil Barrel Implementations

OilBarrel::OilBarrel(StudentWorld* world, int x, int y):Item(world, IID_BARREL, x, y){}

void OilBarrel::doSomething(){
    double distance = getWorld()->distanceToPlayer(this);
    
    if (distance <= 4.) {
        setVisible(true);
    }
    
    if (distance <= 3.) {
        getWorld()->playSound(SOUND_FOUND_OIL);
        markForDelete();
        getWorld()->increaseScore(1000);
        getWorld()->decBarrels(); 
    }
}

//Gold Nugget Implementations
GoldNugget::GoldNugget(StudentWorld* world, int x, int y):Item(world, IID_GOLD, x, y), protestorPickup(false){}

void GoldNugget::doSomething() {
    if (!protestorPickup) {
        double distance = getWorld()->distanceToPlayer(this);
        
        if (distance <= 4.) {
            setVisible(true);
        }
        
        if (distance <= 3.) {
            getWorld()->playSound(SOUND_GOT_GOODIE);
            markForDelete();
            getWorld()->increaseScore(10);
            getWorld()->getPlayer()->increaseCurrentGold();
        }
    }
}

//Sonar Kit Implementations
SonarKit::SonarKit(StudentWorld* world, int x, int y):Item(world, IID_SONAR, x, y) {
    timer = 0;
    ticksAllowed = std::max(100, 300 - 10 * static_cast<int>(getWorld()->getLevel()));
    setVisible(true); 
}

void SonarKit::doSomething() {
    if (timer < ticksAllowed){
        ++timer;
        double distance = getWorld()->distanceToPlayer(this);
    
        if (distance <= 4.) {
            setVisible(true);
        }
        
        if (distance <= 3.) {
            getWorld()->playSound(SOUND_GOT_GOODIE);
            markForDelete();
            getWorld()->increaseScore(75);
            getWorld()->getPlayer()->increaseSonarCharges();
        }
    }
    
    else markForDelete();
}

SonarKit::~SonarKit(){}

//Squirt Implementations
Squirt::Squirt(StudentWorld* world, int x, int y):Item(world, IID_WATER_SPURT, x, y),hasShots(true), range(4), distTraveled(0){
    setDirection(getWorld()->getPlayer()->getDirection());
    if (getWorld()->hasIce(getX(),getY())||getWorld()->getMaxRows()+3<getY()||getWorld()->getMaxCols()-3<getX()||getX()<0||getY()<0) setVisible(false);
    else setVisible(true);
}

void Squirt::doSomething() {
    int dir = getDirection();
    
    if (distTraveled < 4){
        ++distTraveled; 
        switch(dir) {
            case GraphObject::up:
                if (getWorld()->canMove(getX(),getY()+1)) moveTo(getX(),getY()+1);
                break;
            case GraphObject::down:
                if (getWorld()->canMove(getX(),getY()-1)) moveTo(getX(),getY()-1);
                break;
            case GraphObject::left:
                if (getWorld()->canMove(getX()-1,getY())) moveTo(getX()-1,getY());
                break;
            case GraphObject::right:
                if (getWorld()->canMove(getX()+1,getY())) moveTo(getX()+1,getY());
                break;
            default:
                break;
        }
    }
    else {
        markForDelete();
    }
    
    for (auto& x : getWorld()->getGameObjects()) {
        if (x->getKillable() && distanceTo(x) <= 3. && hasShots) {
            x->decHealth(2);
            hasShots = false;
        }
    }
}

Squirt::~Squirt(){}

//Water Pool Implementations
Water::Water(StudentWorld* world, int x, int y):Item(world, IID_WATER_POOL, x, y), timer(0){
    setVisible(true);
    ticksAllowed = std::max(100, 300 - 10*static_cast<int>(getWorld()->getLevel()));
}

void Water::doSomething() {
    if (timer < ticksAllowed) {
        ++timer;
        double distance  = getWorld()->distanceToPlayer(this);
        
        if (distance <= 3.) {
            getWorld()->playSound(SOUND_GOT_GOODIE);
            markForDelete();
            for (int i = 0; i < 5; ++i) {
                getWorld()->getPlayer()->incSquirts();
            }
            getWorld()->increaseScore(100);
        }
    }
    else markForDelete(); 
}

Water::~Water(){}

//Protestor Implementations
Protestor::Protestor(StudentWorld* world, int ID, int x, int y):Actor(ID,x,y,GraphObject::left,1.,0),lastTurnTick(0),stunnedTicks(0){
    setActive();
    setstwPtr(world); 
    setVisible(true);
    
    currentState = moving;
    resetTimeSinceMove();
    setKillable(true);
    timeBetweenMoves = std::max(0, 3 - static_cast<int>(getWorld()->getLevel())/4);
    distToMove = calculateDistanceToMove(getDirection());
    atExit = false;
    timeBetweenAnnoy = 15;
    timeSinceAnnoy = 15;
}

void Protestor::doSomething(){
    //return immediately if dead
    if (!getActiveStatus()) return;
    
    if (stunnedTicks > 0) {
        --stunnedTicks;
        return;
    }
    
    //check internal move timer
    if (timeSinceMove < timeBetweenMoves){
        ++timeSinceMove;
        return;
    }
    
    //reset internal move timer
    else {
        resetTimeSinceMove();
        ++timeSinceAnnoy;
    }
    
    //set state to leaving when dies
    if (getHealth() <= 0){
        if (currentState!=leaving) {
            getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        }
        currentState = leaving;
    }
    if (currentState == leaving) {
        //mark for deletion by studentWorld if at exit
        int exitX = 60;
        int exitY = 60;
        if (getX() == exitX && getY() == exitY) {
            markForDelete();
            return;
        }
        
        //queue based maze search to exit
        Direction directions[64][61];
        getWorld()->pathToSource(exitX, exitY, directions);
        Direction dir = directions[getX()][getY()];
        setDirection(dir);
        move(dir);
        
        
        //return after moving one step towards exit
        return;
    }
    
    if (getWorld()->distanceToPlayer(this)<=4.) {
        setDirection(calculateDirection());
        if (timeSinceAnnoy < timeBetweenAnnoy) {
            ++timeSinceAnnoy;
        }
        else canAnnoyAgain = true;
        if (canAnnoyAgain) {
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            hurtIceman();
            timeSinceAnnoy = 0;
            canAnnoyAgain = false;
        }
        return;
    }
    
    if (searchForIceman()) {
        distToMove = 0;
        if (getWorld()->distanceToPlayer(this) > 4.) {
            setDirection(calculateDirection());
            move(calculateDirection());
        }
    }
    
    else {
        --distToMove;
        Direction newDir;
        if (shouldTurn(newDir)) {
            //change direction
            setDirection(newDir);
            distToMove = calculateDistanceToMove(getDirection());
            move(getDirection());
            lastTurnTick = getWorld()->getTickCounter();
        }
        
        else if (distToMove <= 0) {
            setDirection(pickDirection());
            distToMove = calculateDistanceToMove(getDirection());
        }
        
        //TODO: perpendicular condition
        
        else if(getWorld()->canMove(getDirection(),getX(),getY()) && !getWorld()->hasIce(getDirection(), getX(), getY())) {
            move(getDirection());
            --distToMove;
        }
        
        else {
            setDirection(pickDirection());
            distToMove = calculateDistanceToMove(getDirection());
        }
    }
    
}

bool Protestor::searchForIceman() {
    return 0;
}

void Protestor::move(Direction dir){
    switch(dir){
        case GraphObject::up:
            if (getWorld()->canMove(getX(), getY()+1) && !getWorld()->hasIce(getX(), getY()+1)) {
                moveTo(getX(),getY()+1);
            }
            break;
        case GraphObject::down:
            if (getWorld()->canMove(getX(), getY()-1) && !getWorld()->hasIce(getX(), getY()-1)) {
                moveTo(getX(), getY()-1);
            }
            break;
        case GraphObject::right:
            if(getWorld()->canMove(getX()+1, getY()) && !getWorld()->hasIce(getX()+1, getY())) {
                moveTo(getX()+1, getY());
            }
            break;
        case GraphObject::left:
            if(getWorld()->canMove(getX()-1, getY()) && !getWorld()->hasIce(getX()-1, getY())) {
                moveTo(getX()-1, getY());
            }
            break;
        default: break;
    }
    
}

Actor::Direction Protestor::calculateDirection() {
    if (getY() == getWorld()->getPlayer()->getY()){
        if (getX() - getWorld()->getPlayer()->getX() > 0) return left;
        else return right;
    }
    
    if (getX() == getWorld()->getPlayer()->getX()){
        if (getY() - getWorld()->getPlayer()->getY() > 0) return down;
        else return up;
    }
    
    if (getY() == getWorld()->getPlayer()->getY()+4){
        if (getX() - getWorld()->getPlayer()->getX() > 0) return left;
        else return right;
    }
    
    if (getX() == getWorld()->getPlayer()->getX()+4){
        if (getY() - getWorld()->getPlayer()->getY() > 0) return down;
        else return up;
    }
    
    if (abs(getY() - getWorld()->getPlayer()->getY()) > abs(getX() - getWorld()->getPlayer()->getX())){
        if (getY() - getWorld()->getPlayer()->getY() > 0) return down;
        else return up;
    }
    
    if (abs(getX() - getWorld()->getPlayer()->getX()) > abs(getY() - getWorld()->getPlayer()->getY())){
        if (getX() - getWorld()->getPlayer()->getX() > 0) return left;
        else return right;
    }
    
    return getDirection();
}

Actor::Direction Protestor::pickDirection() {
    int directions[4] = {1, 1, 1, 1};
    //0 is up
    //1 is down
    //2 is left
    //3 is right
    
    
    if (getWorld()->hasIce(getX()+1,getY()) || !getWorld()->canMove(getX()+1, getY())) {
        directions[3]=0;
    }
    if (getWorld()->hasIce(getX()-1,getY()) || !getWorld()->canMove(getX()-1, getY())){
        directions[2]=0;
    }
    if(getWorld()->hasIce(getX(),getY() - 1) || !getWorld()->canMove(getX(), getY()-1)){
        directions[1] = 0;
    }
    if(getWorld()->hasIce(getX(),getY()+1) || !getWorld()->canMove(getX(), getY()+1)){
        directions[0] = 0;
    }
    
    bool foundMove = false;
    Actor::Direction direction = left;
    while (!foundMove) {
        int random = rand() % 4;
        if(directions[random] != 0){
            foundMove = true;
            switch(random){
                case 0:
                    direction = up;
                    break;
                case 1:
                    direction = down;
                    break;
                case 2:
                    direction = left;
                    break;
                case 3:
                    direction = right;
                    break;
                default:
                    direction = left;
                    break;
            }
        }
    }
    return direction;
}

int Protestor::calculateDistanceToMove(Direction dir) {
    switch(dir) {
        case up:
            return getWorld()->getMaxRows() - getY();
            break;
        case down:
            return getY() - 1;
            break;
        case left:
            return getX() - 1;
            break;
        case right:
            return getWorld()->getMaxCols() - getX();
            break;
        default: return 0;
    }
}

bool Protestor::shouldTurn(Direction& newDir) {
    Direction dir = getDirection();
    
    if ((getWorld()->getTickCounter() - lastTurnTick)/timeBetweenMoves < 200) {
        return false;
    }
    
    if (dir == left || dir == right) {
        bool canUp = getWorld()->canMove(up, getX(), getY()) && !getWorld()->hasIce(up, getX(), getY());
        bool canDown = getWorld()->canMove(down, getX(), getY()) && !getWorld()->hasIce(down, getX(), getY());
        if (canUp && canDown) {
            newDir = (rand() % 2) ? up : down;
            return true;
        }
        else if (canUp) {
            newDir = up;
            return true;
        }
        else if (canDown) {
            newDir = down;
            return true;
            
        }
        else {
            return false;
        }
    }
    else if (dir == up || dir == down) {
        bool canLeft = getWorld()->canMove(left, getX(), getY()) && !getWorld()->hasIce(left, getX(), getY());
        bool canRight = getWorld()->canMove(right, getX(), getY()) && !getWorld()->hasIce(right, getX(), getY());
        if (canLeft && canRight) {
            newDir = (rand() % 2) ? left : right;
            return true;
        }
        else if (canLeft) {
            newDir = left;
            return true;
        }
        else if (canRight) {
            newDir = right;
            return true;
            
        }
        else {
            return false;
        }
    }
    return false;
}

void Protestor::decHealth(int amount) {
    Actor::decHealth(amount);
    getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
    
    //number of ticks it will be stunned for
    stunnedTicks = std::max(50, 100 - (int)getWorld()->getLevel() * 10);
    
}

int Protestor::getTimeSinceMove() {
    return timeSinceMove;
}

int Protestor::getTimeBetweenMoves() {
    return timeBetweenMoves;
}

void Protestor::resetTimeSinceMove() {
    timeSinceMove = 0;
}

void Protestor::incTimeSinceMove() {
    ++timeSinceMove;
}

void Protestor::setState(state input) {
    currentState = input;
}

void Protestor::hurtIceman(){}

bool Protestor::canMoveToIceman() {
    return 1;
}

//Regular Protestor Implementations
RegularProtestor::RegularProtestor(StudentWorld* world, int ID, int x, int y):Protestor(world, IID_PROTESTER, x, y){
    setHealth(5);
}

void RegularProtestor::hurtIceman() {
    getWorld()->getPlayer()->decHealth(2); 
}

bool RegularProtestor::searchForIceman() {
    int protX = getX();
    int protY = getY();
    bool validX, validY;
    
    if (protX == getWorld()->getPlayer()->getX()) {
        validX = true;
    }
    else validX = false;
    
    if (protY == getWorld()->getPlayer()->getY()) {
        validY = true;
    }
    else validY = false;
    
    
    
    if ((validX || validY) && canMoveToIceman(getWorld()->getPlayer()->getX(), getWorld()->getPlayer()->getY(), getX(), getY())) return 1;
    else return 0;
}

bool RegularProtestor::canMoveToIceman(int targetX, int targetY, int currentX, int currentY) {
    for(;currentY > targetY; --currentY){
        if (getWorld()->hasIce(currentX,currentY)) return 0;
    }
    for(;currentX > targetX; --currentX) {
        if (getWorld()->hasIce(currentX, currentY)) return 0;
    }
    for (; currentX < targetX; ++currentX) {
        if (getWorld()->hasIce(currentX,currentY)) return 0;
    }
    for (; currentY < targetY; ++currentY) {
        if (getWorld()->hasIce(currentX, currentY)) return 0;
    }
    
    return 1;
    
}

RegularProtestor::~RegularProtestor(){}
/*
void Actor::doSomething(){
    if (!isActive) return;
    
}
 */




