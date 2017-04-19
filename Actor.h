#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject
{
public:

	//basics
	Actor(int imageID, int startX, int startY, Direction startDirection, StudentWorld *SW, float size = 1.0, unsigned int depth = 0);
	virtual ~Actor();
	//accessors
	bool isActive() const;
	StudentWorld *getWorld() const;
	virtual bool isObstacle();
	virtual bool isPickupable();
	//mutators
	virtual void doSomething() = 0;
	void setInactive();
	virtual bool annoy(int HP = 0);
	void convertDirToCoord(Direction dir, int &x, int &y);

private:

	bool m_isActive;
	StudentWorld *p_StudentWorld;
};

class Dirt:public Actor
{
public:

	Dirt(int startX, int startY, StudentWorld *SW);
	virtual ~Dirt();
	//accessors
	virtual bool isObstacle();
	//mutators
	virtual void doSomething();
};

class Human :public Actor
{
public:

	//basics
	Human(int imageID, int startX, int startY, Direction startDirection, int HP, StudentWorld *SW);
	virtual ~Human();
	//accessors
	int getHP() const;
	//mutators
	virtual bool annoy(int HP = 0) = 0;
	virtual bool setLeaveOilField() = 0;
	void decHP(int HP);
	
private:
	
	int m_HP;
};

class FrackMan :public Human
{
public:

	//basics
	FrackMan(int startX, int startY, StudentWorld *SW);
	virtual ~FrackMan();
	//accessors
	int getWater() const;
	int getSonarCharge() const;
	int getGoldNuggets() const;
	//mutators
	void addWater();
	void addSonarCharge();
	void addGoldNuggets();
	virtual void doSomething();
	void turnOrMove(Direction dir);
	virtual bool setLeaveOilField();
	virtual bool annoy(int HP = 0);

private:
	void addSquirt();
	void useSonar();
	void dropBribe();
	int m_water;
	int m_charge;
	int m_nuggets;

};

class Protester : public Human
{
public:

	Protester(int imageID, int startX, int startY, int HP, StudentWorld *SW);
	virtual ~Protester();

	//accessors
	int getRestCount();
	bool getLeaveOilField();
	virtual bool canUseRadar();

	//mutators
	void addRestCount(int count);
	void setRestCount(int count);
	virtual void doSomething();
	void setNext(int x, int y);
	virtual bool setLeaveOilField();
	void turnThenMove(char dir = 'n');
	virtual bool useRadar() = 0;
	
	virtual void getBribed() = 0;
	virtual bool annoy(int HP = 0);

private:

	//within doSomething()
	void leaveOilField();
	bool isFacingFrackMan(int x, int y, Direction dir);
	void moveToNewDirection();
	bool movePerpendicular();
	bool stepForward();
	//others
	int calcRestCount();
	//data members
	int m_nRest;
	int m_nSquares;
	int m_restCount;
	int m_shoutCount;
	int m_perpendicularCount;
	bool m_leaveOilField;
	int m_xNext;
	int m_yNext;
};

class RegularProtester:public Protester
{
public:
	RegularProtester(int startX, int startY, StudentWorld *SW);
	virtual ~RegularProtester();

	//mutators
	virtual bool useRadar();
	virtual void getBribed();
private:

};

class HardcoreProtester :public Protester
{
public:
	HardcoreProtester(int startX, int startY, StudentWorld *SW);
	virtual ~HardcoreProtester();

	//accessors
	virtual bool canUseRadar();

	//mutators
	virtual bool useRadar();
	virtual void getBribed();
private:

};


class Boulder : public Actor
{
public:

	Boulder(int startX, int startY, StudentWorld *SW);
	virtual ~Boulder();
	//accessors
	virtual bool isObstacle();
	//mutators
	virtual void doSomething();

private:
	int m_tickUntilFall;
	bool m_dirtUnder;


};

class Squirt : public Actor
{
public:
	Squirt(int startX, int startY, Direction startDirection, StudentWorld *SW);
	virtual ~Squirt();

	//mutator
	virtual void doSomething();
	void convertDirection(int &cx, int &cy);

private:
	int m_travelDistance;
};

class ActivatingObject : public Actor
{
public:
	ActivatingObject(int imageID, int startX, int startY, StudentWorld *SW, bool ticking = false, int tick = -1);
	virtual ~ActivatingObject();
	virtual void doSomething();
	virtual void checkTickLeft();
	void showItself();
	virtual bool isPickupable();
	

private:
	virtual void activate() = 0;
	bool m_canYouSeeThis;
	int m_tickLeft;
	bool m_isTicking;
};

class OilBarrel:public ActivatingObject
{
public:
	OilBarrel(int startX, int startY, StudentWorld *SW);
	virtual ~OilBarrel();
	virtual void activate();

};

class GoldNugget :public ActivatingObject
{
public:
	GoldNugget(int startX, int startY, StudentWorld *SW);
	virtual ~GoldNugget();
	virtual void activate();
};

class Bribe :public ActivatingObject
{
public:
	Bribe(int startX, int startY, StudentWorld *SW);
	virtual ~Bribe();
	virtual void doSomething();
	virtual void activate();
private:

};

class SonarKit :public ActivatingObject
{
public:
	SonarKit(int startX, int startY, StudentWorld *SW);
	virtual ~SonarKit();
	virtual void activate();
private:
};

class WaterPool :public ActivatingObject
{
public:
	WaterPool(int startX, int startY, StudentWorld *SW);
	virtual ~WaterPool();
	virtual void activate();

private:
};

#endif // ACTOR_H_
