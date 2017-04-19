#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
//
// Actor Implementation

Actor::Actor(int imageID, int startX, int startY, Direction startDirection, StudentWorld *SW, float size, unsigned int depth)
	:GraphObject(imageID, startX, startY, startDirection, size, depth), m_isActive(true), p_StudentWorld(SW)
{
	setVisible(true);
}

Actor::~Actor()
{};


bool Actor::isActive() const
{
	return m_isActive;
}

StudentWorld* Actor::getWorld() const
{
	return p_StudentWorld;
}

bool Actor::isObstacle()
{
	return false;
}

bool Actor::isPickupable()
{
	return false;
}

void Actor::setInactive()
{
	m_isActive = false;
}

bool Actor::annoy(int HP)
{
	return false;
}

void Actor::convertDirToCoord(Direction dir, int &cx, int &cy)
{
	switch (dir)
	{
	case left:
		cx = -1;
		cy = 0;
		break;
	case right:
		cx = 1;
		cy = 0;
		break;
	case up:
		cx = 0;
		cy = 1;
		break;
	case down:
		cx = 0;
		cy = -1;
	}

}

// End of Actor Implementation

//Dirt Implementation

Dirt::Dirt(int startX, int startY, StudentWorld *SW)
	:Actor(IID_DIRT, startX, startY, right, SW, 0.25, 3)
{
}

Dirt::~Dirt() {};

bool Dirt::isObstacle()
{
	return true;
}

void Dirt::doSomething()
{
	return;
}

// End of Dirt Implementation

//Human Implementation

Human::Human(int imageID, int startX, int startY, Direction startDirection, int HP, StudentWorld *SW)
	:Actor(imageID, startX, startY, startDirection, SW), m_HP(HP) {}

Human::~Human() {}

int Human::getHP() const
{
	return m_HP;
}


void Human::decHP(int HP)
{
	m_HP -= HP;
}
// End of Human Implementation

//FrackMan Implementation

FrackMan::FrackMan(int startX, int startY, StudentWorld *SW)
	:Human(IID_PLAYER, startX, startY, right, 10, SW)
{
	m_water = 5;
	m_charge = 1;
	m_nuggets = 0;
}

FrackMan::~FrackMan() {}

int FrackMan::getWater() const
{
	return m_water;
}

int FrackMan::getSonarCharge() const
{
	return m_charge;
}

int FrackMan::getGoldNuggets() const
{
	return m_nuggets;
}



void FrackMan::addWater()
{
	m_water += 5;
}

void FrackMan::addSonarCharge()
{
	m_charge++;
}

void FrackMan::addGoldNuggets()
{
	m_nuggets++;
}

void FrackMan::doSomething() //imcomplete
{
	if (!isActive())
	return;
	if (getWorld()->removeDirt(getX(), getY()) == true)
		getWorld()->playSound(SOUND_DIG);

	if (true)
	{
		int keyInput;
		if (getWorld()->getKey(keyInput))
		{
			switch(keyInput)
			{
			case KEY_PRESS_ESCAPE:
				setInactive();
				break;
			case KEY_PRESS_SPACE: 
				addSquirt();
				break;
			case KEY_PRESS_LEFT:
				turnOrMove(left);
				break;
			case KEY_PRESS_RIGHT:
				turnOrMove(right);
				break;
			case KEY_PRESS_UP:
				turnOrMove(up);
				break;
			case KEY_PRESS_DOWN:
				turnOrMove(down);
				break;
			case 'z':
			case 'Z':
				if (m_charge > 0)
					useSonar();
				break;
			case KEY_PRESS_TAB:
				if (m_nuggets > 0)
					dropBribe();
				break;

			}
		}
	}
		


}

void FrackMan::turnOrMove(Direction dir)
{
	int cx, cy;
	convertDirToCoord(dir, cx, cy);

	if (getDirection() == dir)
	{
		if (!getWorld()->isThereObstacle(getX() + cx, getY() + cy, this))
			moveTo(getX() + cx, getY() + cy);
		else
			moveTo(getX(), getY());
	}
		
	else
		setDirection(dir);
}

bool FrackMan::setLeaveOilField()
{
	return false;
}

bool FrackMan::annoy(int HP)
{
	if (HP == 0)
		return true;

	decHP(HP);

	if (getHP() <= 0)
	{
		setInactive();
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
	}

	return true;
}

void FrackMan::addSquirt()
{
	if (m_water <= 0)
		return;

	int cx, cy;
	convertDirToCoord(getDirection(), cx, cy);
	cx *= 4;
	cy *= 4;

	if (!getWorld()->isThereObstacle(getX() + cx, getY() + cy))
	{
		Squirt *s = new Squirt(getX() + cx, getY() + cy, getDirection(), getWorld());
		getWorld()->storeNewActor(s);
	}

	getWorld()->playSound(SOUND_PLAYER_SQUIRT);
	m_water--;
	//m_water--;
}

void FrackMan::useSonar()
{
	m_charge--;
	getWorld()->sonarHidden(this);
	getWorld()->playSound(SOUND_SONAR);

}

void FrackMan::dropBribe()
{
	m_nuggets--;
	
	if (m_water <= 0)
		return;

	Bribe *b = new Bribe(getX(), getY(), getWorld());
	getWorld()->storeNewActor(b);
}
//End of Frackman implementation

//Protester Implementation

Protester::Protester(int imageID, int startX, int startY, int HP, StudentWorld *SW)
	:Human(imageID, startX, startY, left, HP, SW), m_leaveOilField(false) //fix:: test purposes!!
{
	m_restCount = 0;
	m_nSquares = getWorld()->randInt(8, 60);
	//fix:set private members
	m_xNext = getX();
	m_yNext = getY();
	m_shoutCount = 0;
	m_perpendicularCount = 200;
}

Protester:: ~Protester()
{

}

int Protester::getRestCount()
{
	return m_restCount;
}

bool Protester::getLeaveOilField()
{
	return m_leaveOilField;
}

bool Protester::canUseRadar()
{
	return false;
}


void Protester::addRestCount(int count)
{
	m_restCount += count;
}

void Protester::setRestCount(int count)
{
	m_restCount = count;
}
void Protester::doSomething()
{
	if (!isActive())
		return;

	if (m_restCount > 0)
	{
		m_restCount--;
		return;
	}

	else
	{
		m_restCount = calcRestCount();
		m_perpendicularCount--;
	}


	if (m_leaveOilField) //if protester needs to leave the oilfield
	{
			leaveOilField();
			return;
	}

	if (m_shoutCount > 0)
	{
		m_shoutCount--;
		return;
	}


	int x, y;
	getWorld()->whereIsFrackMan(x, y);
	if (getWorld()->isThereHuman(this, true, 4) && isFacingFrackMan(x, y, getDirection())) //frackman within range && Protester is facing frackman
	{
		
		if (m_shoutCount <= 0)
		{
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			getWorld()->causeAnnoyance(IID_PLAYER);
			m_shoutCount = 15;
		}

		return;
	}


	if (!getWorld()->isThereHuman(this, true, 4))
	{
		if (useRadar())
			return;
		
		char dir;
		if (getWorld()->isInDirectLine(this, dir))
		{
			turnThenMove(dir);
			m_nSquares = 0;
			return;
		}
	}
		
	m_nSquares--;
	if (m_nSquares <= 0)
	{
		moveToNewDirection();
		m_nSquares = getWorld()->randInt(8, 60);
		return;
	}

	if (!movePerpendicular() && !stepForward())
	{
		m_nSquares = 0;
		m_restCount = 0;
		return;
	}
}

void Protester::setNext(int x, int y)
{
	m_xNext = x;
	m_yNext = y;
}

bool Protester::setLeaveOilField()
{
	m_leaveOilField = true;

	return true;
}


void Protester::turnThenMove(char dir)
{

	if (dir != 'n')
	{
		int cx, cy;
		Direction direction;
		switch (dir)
		{
		case 'l':
			cx = -1;
			cy = 0;
			direction = left;
			break;
		case 'r':
			cx = 1;
			cy = 0;
			direction = right;
			break;
		case 'u':
			cx = 0;
			cy = 1;
			direction = up;
			break;
		case 'd':
			cx = 0;
			cy = -1;
			direction = down;
		}
		setDirection(direction);
		moveTo(getX() + cx, getY() + cy);
		setNext(getX(), getY());
		return;
	}

	if (m_xNext == getX() + 1)
	{
		setDirection(right);
		moveTo(m_xNext, m_yNext);
	}

	if (m_xNext == getX() - 1)
	{
		setDirection(left);
		moveTo(m_xNext, m_yNext);
	}
		
	if (m_yNext == getY() + 1)
	{
		setDirection(up);
		moveTo(m_xNext, m_yNext);
	}
		
	if (m_yNext == getY() - 1)
	{
		setDirection(down);
		moveTo(m_xNext, m_yNext);
	}
	setNext(getX(), getY());
}

void Protester::moveToNewDirection()
{
	int dir = getWorld()->randInt(0, 3);
	switch (dir)
	{
	case 0:
		if (!getWorld()->isThereObstacle(getX() + 1, getY()))
			turnThenMove('r');

		break;
	case 1:
		if (!getWorld()->isThereObstacle(getX() - 1, getY()))
			turnThenMove('l');
		break;
	case 2:
		if (!getWorld()->isThereObstacle(getX(), getY() - 1))
			turnThenMove('d');
		break;
	case 3:
		if (!getWorld()->isThereObstacle(getX(), getY() + 1))
			turnThenMove('u');
		break;
	}
}

bool Protester::movePerpendicular()
{
	if (m_perpendicularCount > 0)
		return false;

	if (getDirection() == up || getDirection() == down)
	{
		if (!getWorld()->isThereObstacle(getX() - 1, getY()))
		{
			turnThenMove('l');
			m_perpendicularCount = 200;
			return true;
		}

		if (!getWorld()->isThereObstacle(getX() + 1, getY()))
		{
			turnThenMove('r');
			m_perpendicularCount = 200;
			return true;
		}
	}

	if (getDirection() == left || getDirection() == right)
	{
		if (!getWorld()->isThereObstacle(getX(), getY() - 1))
		{
			turnThenMove('d');
			m_perpendicularCount = 200;
			return true;
		}

		if (!getWorld()->isThereObstacle(getX(), getY() + 1))
		{
			turnThenMove('u');
			m_perpendicularCount = 200;
			return true;
		}
	}
	return false;
}

bool Protester::stepForward()
{
	Direction dir = getDirection();
	int cx = 0;
	int cy = 0;
	convertDirToCoord(dir, cx, cy);
	if (!getWorld()->isThereObstacle(getX() + cx, getY() + cy))
	{
		moveTo(getX() + cx, getY() + cy);
		setNext(getX(), getY());
		return true;
	}

	else
		return false;

}

void Protester::leaveOilField()
{
	if (getX() == 60 && getY() == 60)
	{
		setInactive();
		return;
	}

	else
		turnThenMove();
	
}

bool Protester::annoy(int HP)
{
	if (HP == -1 && m_leaveOilField)
		return false;

	if (HP == 0 || HP == -1)
		return true;

	decHP(HP);
	
	if (getHP() > 0)
	{
		getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		int count = getWorld()->max(50, 100 - (getWorld()->getLevel() * 10));
		setRestCount(count);
	}

	else
	{
		m_leaveOilField = true;
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		setRestCount(0);
		if (HP == 100)
			getWorld()->increaseScore(500);
		else if (canUseRadar())
		{
			getWorld()->increaseScore(250);
		}
		else
		{
			getWorld()->increaseScore(100);
		}
			
	}

	return true;
}
bool Protester::isFacingFrackMan(int x, int y, Direction dir)
{
	int lowerX, lowerY, upperX, upperY;

	switch (dir)
	{
	case left:
		lowerX = -4;
		upperX = -1;
		lowerY = -4;
		upperY = 4;
		break;
	case right:
		lowerX = 1;
		upperX = 4;
		lowerY = -4;
		upperY = 4;
		break;
	case down:
		lowerX = -4;
		upperX = 4;
		lowerY = -4;
		upperY = -1;
		break;
	case up:
		lowerX = -4;
		upperX = 4;
		lowerY = 1;
		upperY = 4;
		break;
	}

	if (x >= getX() + lowerX && x <= getX() + upperX && y >= getY() + lowerY && y <= getY() + upperY)
		return true;

	else
		return false;
}

int Protester::calcRestCount()
{
	int N = getWorld()->max(0, 3 - (getWorld()->getLevel() / 4));
	return N;
}

// End of Protester Implementation

//Regular Protester Implementation

RegularProtester::RegularProtester(int startX, int startY, StudentWorld *SW)
	:Protester(IID_PROTESTER, startX, startY, 5, SW) {}

RegularProtester::~RegularProtester()
{

}

bool RegularProtester::useRadar()
{
	return false;
}

void RegularProtester::getBribed()
{
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getWorld()->increaseScore(25);
	setLeaveOilField();
}
//End of Regular Protester Implementation

//HardCore Protester Implementation
HardcoreProtester::HardcoreProtester(int startX, int startY, StudentWorld *SW)
	:Protester(IID_HARD_CORE_PROTESTER, startX, startY, 20, SW) {}

HardcoreProtester::~HardcoreProtester()
{

}

bool HardcoreProtester::canUseRadar()
{
	return true;
}

bool HardcoreProtester::useRadar()
{
	int x = getX();
	int y = getY();
	turnThenMove();
	if (x == getX() && y == getY())
		return false;
	else
	{
		return true;
	}
		
}

void HardcoreProtester::getBribed()
{
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getWorld()->increaseScore(50);
	int possibleTick = 100 - (getWorld()->getLevel() * 10);
	setRestCount(getWorld()->max(possibleTick, 50));
}

//End of Hardcore Protester Implementation

//Boulder Implementation

Boulder::Boulder(int startX, int startY, StudentWorld *SW)
	:Actor(IID_BOULDER, startX, startY, down, SW, 1.0, 1)
{
	m_tickUntilFall = 30;
	m_dirtUnder = true;
}

Boulder::~Boulder()
{
	
}

bool Boulder::isObstacle()
{
	return true;
}

void Boulder::doSomething()
{
	if (!isActive())
		return;

	if (m_dirtUnder == true)
	{
		if (getWorld()->dirtUnderneath(this))
			return;

		else
		{
			m_dirtUnder = false;
			return;
		}
	}

	if (m_tickUntilFall > 0)
	{
		m_tickUntilFall--;
		return;
	}
	
	if (m_tickUntilFall <= 0)
	{
		if (m_tickUntilFall == 0)
		{
			getWorld()->playSound(SOUND_FALLING_ROCK);
			m_tickUntilFall--;
		}
			
		if (!getWorld()->isThereObstacle(getX(), getY() - 1, this))
		{
			moveTo(getX(), getY() - 1);
			getWorld()->annoySurroundings(this, 100);
		}

		else
			setInactive();
	}
}

Squirt::Squirt(int startX, int startY, Direction startDirection, StudentWorld *SW)
	:Actor(IID_WATER_SPURT, startX, startY, startDirection, SW, 1.0, 1)
{
	m_travelDistance = 4;
}

Squirt::~Squirt()
{

}

void Squirt::doSomething()
{
	if (!isActive())
		return;

	getWorld()->annoySurroundings(this, 2);
	if (!isActive())
		return; 

	if (m_travelDistance <= 0)
	{
		setInactive();
		return;
	}

	int cx, cy;
	convertDirection(cx, cy);
	if (getWorld()->isThereObstacle(getX() + cx, getY() + cy))
	{
		setInactive();
		return;
	}

	else
	{
		moveTo(getX() + cx, getY() + cy);
		m_travelDistance--;
		return;
	}

}

void Squirt::convertDirection(int &cx, int &cy)
{
	Direction dir = getDirection();
	switch (dir)
	{
	case left:
			cx = -1;
			cy = 0;
		break;
	case right:
			cx = 1;
			cy = 0;
		break;
	case down:
			cx = 0;
			cy = -1;
		break;
	case up:
			cx = 0;
			cy = 1;
		break;
	}
}

ActivatingObject::ActivatingObject(int imageID, int startX, int startY, StudentWorld *SW, bool ticking, int tick)
	:Actor(imageID, startX, startY, right, SW, 1.0, 2)
{
	setVisible(false);
	m_canYouSeeThis = false;
	m_isTicking = ticking;
	m_tickLeft = tick;
	int t = getWorld()->max(100, 300 - 10 * getWorld()->getLevel());
	if (m_tickLeft == -1)
		m_tickLeft = t;
}


ActivatingObject::~ActivatingObject()
{
}

void ActivatingObject::doSomething()
{
	if (!isActive())
		return;

	if (!m_canYouSeeThis && getWorld()->isThereHuman(this, true, 4))
	{
		showItself();
		checkTickLeft();
		return;
	}

	if (getWorld()->isThereHuman(this, true, 3))
	{
		activate();
		return;
	}

	else
	{
		checkTickLeft();
	}


	

	
}

void ActivatingObject::checkTickLeft()
{
	if (!m_isTicking)
		return;

	m_tickLeft--;
	if (m_tickLeft <= 0)
		setInactive();



}

void ActivatingObject::showItself()
{
	setVisible(true);
	m_canYouSeeThis = true;
}

bool ActivatingObject::isPickupable()
{
	return true;
}


OilBarrel::OilBarrel(int startX, int startY, StudentWorld *SW)
	:ActivatingObject(IID_BARREL, startX, startY, SW)
{

}

OilBarrel::~OilBarrel()
{

}

void OilBarrel::activate()
{
	setInactive();
	getWorld()->playSound(SOUND_FOUND_OIL);
	getWorld()->increaseScore(1000);
	getWorld()->barrelFound();
}

GoldNugget::GoldNugget(int startX, int startY, StudentWorld *SW)
	:ActivatingObject(IID_GOLD, startX, startY, SW)
{

}
GoldNugget::~GoldNugget()
{

}

void GoldNugget::activate()
{
	setInactive();
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->addToInventory(IID_GOLD);
	getWorld()->increaseScore(10);
}

Bribe::Bribe(int startX, int startY, StudentWorld *SW)
	:ActivatingObject(IID_GOLD, startX, startY, SW, true, 100)
{
	setVisible(true);
}

Bribe::~Bribe()
{

}

void Bribe::doSomething()
{
	if (!isActive())
		return;

	if (getWorld()->isThereHuman(this, false, 3))
	{
		setInactive();
	}

	else
	{
		checkTickLeft();
	}
}

void Bribe::activate()
{
	//nothing
}

SonarKit::SonarKit(int startX, int startY, StudentWorld *SW)
	:ActivatingObject(IID_SONAR, startX, startY, SW, true)
{
	setVisible(true);
}
SonarKit::~SonarKit()
{

}
void SonarKit::activate()
{
	setInactive();

	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->addToInventory(IID_SONAR);
	getWorld()->increaseScore(100);
}

WaterPool::WaterPool(int startX, int startY, StudentWorld *SW)
	:ActivatingObject(IID_WATER_POOL, startX, startY, SW, true)
{
	setVisible(true);
}
WaterPool::~WaterPool()
{
	
}
void WaterPool::activate()
{
	setInactive();

	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->addToInventory(IID_WATER_POOL);
	getWorld()->increaseScore(75);
}