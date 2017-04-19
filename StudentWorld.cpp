#include "StudentWorld.h"
#include <string>
#include <queue>
#include <stack>
#include <vector>
#include <random>
#include "Actor.h"
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{
	
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

int StudentWorld::init()
{
	m_nBarrels = 0;
	m_nTotalBarrels = 0;
	m_timeSinceLastProtester = 0;
	m_nMaxProtester = min(15, 2 + getLevel() * 1.5);
	m_nProtesters = 0;
	
	for (int i = 0; i < 64; i++) //fill entire field with dirt
		for (int j = 0; j < 64; j++)
			p_Dirt[i][j] = new Dirt(i, j, this);
	for (int i = 30; i < 34; i++) //delete the dirt for  middle tunnel
		for (int j = 4; j < 60; j++)
		{
			delete p_Dirt[i][j];
			p_Dirt[i][j] = nullptr;
		}
	for (int i = 0; i < 64; i++) //delete the dirt for top
		for (int j = 60; j < 64; j++)
		{
			delete p_Dirt[i][j];
			p_Dirt[i][j] = nullptr;
		}

	p_FrackMan = new FrackMan(30, 60, this); //create Frackman

	int nBoulders = 0;
	int B = min((getLevel() / 2) + 2, 6);
	while (nBoulders != B) //Boulder formation
	{
		int x = randInt(0, 60);
		int y = randInt(20, 56);
		bool overLap = false;

		for (unsigned int k = 0; k < p_Actor.size(); k++)
		{
			int dx = x - p_Actor[k]->getX();
			int dy = y - p_Actor[k]->getY();
			if (p_Actor[k]->isObstacle() == true && sumOfSquares(dx, dy) <= 36)
			{
				overLap = true;
				break;
			}
		}

		if (overLap == false && !isOnMineshaft(x))
		{
			p_Actor.push_back(new Boulder(x, y, this));
			removeDirt(x, y);
			nBoulders++;
		}
	}



	int nNuggets = 0;
	int G = max(5 - getLevel() / 2, 2);
	while (nNuggets != G) //Gold nugget formulation
	{
		int x = randInt(0, 60);
		int y = randInt(0, 56);

		if (!overlap(x, y) && !isOnMineshaft(x))
		{
			p_Actor.push_back(new GoldNugget(x, y, this));
			nNuggets++;
		}
	}

	int nBarrels = 0;
	m_nTotalBarrels = min(2 + getLevel(), 20);
	while (nBarrels != m_nTotalBarrels)
	{
		int x = randInt(0, 60);
		int y = randInt(0, 56);

		if (!overlap(x, y) && !isOnMineshaft(x))
		{
			p_Actor.push_back(new OilBarrel(x, y, this));
			nBarrels++;
		}
	}


	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{

	setDisplayText();

	if (m_timeSinceLastProtester > 0)
		m_timeSinceLastProtester--;

	else if (m_nProtesters < m_nMaxProtester)
	{
		int probabilityOfHardcore = min(90, getLevel() * 10 + 30);
		if (successOrFail(probabilityOfHardcore) == true)
		{
			p_Actor.push_back(new HardcoreProtester(60, 60, this));
		}
		else
		{
			p_Actor.push_back(new RegularProtester(60, 60, this));
		}

		m_timeSinceLastProtester = max(25, 200 - getLevel());
		m_nProtesters++;
	}

	int G = getLevel() * 25 + 300;
	int bound = randInt(1, G);
	if (bound == G)
	{
		int probSonar = randInt(1, 5);
		if (probSonar == 1)
			p_Actor.push_back(new SonarKit(0, 60, this));
		else
			addPool();

	}
	


	int M = 16 + (getLevel() * 2); //functions for protesters (cellphone signal radar and path to take when it is leaving the oil field)
	pathToNext(true, p_FrackMan->getX(), p_FrackMan->getY(), M);
	pathToNext();

	if (p_FrackMan->isActive()) //frackman does something
	{
		p_FrackMan->doSomething();
	}

	for (unsigned int i = 0; i < p_Actor.size(); i++) //each actor does something
	{
		if (p_Actor[i]->isActive())
			p_Actor[i]->doSomething();
	}
	
	if (!p_FrackMan->isActive()) //check if frackman died
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	if (m_nBarrels == m_nTotalBarrels) //check if completed level
		return GWSTATUS_FINISHED_LEVEL;

	for (int i = 0; i < p_Actor.size(); i++) //check if actor died
	{
		if (!p_Actor[i]->isActive())
		{
			if (p_Actor[i]->annoy())
				m_nProtesters--;

			delete p_Actor[i];
			p_Actor.erase(p_Actor.begin() + i);
			i--;
		}
	}

	return GWSTATUS_CONTINUE_GAME;
	
}

void StudentWorld::cleanUp()
{
	delete p_FrackMan;

	m_nBarrels = 0;
	m_nTotalBarrels = 0;
	m_timeSinceLastProtester = 0;
	m_nMaxProtester = 0;
	m_nProtesters = 0;

	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 64; j++)
		{
			delete p_Dirt[i][j];
		}

	for (unsigned int j = 0; j < p_Actor.size(); j++)
		delete p_Actor[j];

	p_Actor.clear();


}

bool StudentWorld::removeDirt(int x, int y)
{
	bool hasDigged = false;
	for (int i = x; i < x + 4; i++)
	{
		for (int j = y; j < y + 4; j++)
		{
			
			if (i < 64 && j < 64 && p_Dirt[i][j] != nullptr)
			{
				hasDigged = true;
				delete p_Dirt[i][j];
				p_Dirt[i][j] = nullptr; //fix: play the sound
			}
		}
	}
	return hasDigged;
}

bool StudentWorld::isThereObstacle(int x, int y, Actor *a)
{
	if (x < 0 || x > 60 || y < 0 || y > 60)
	{
		return true;
	}

	for (int i = x - 3; i < x + 4; i++)
	{
		for (int j = y - 3; j < y + 4; j++)
		{
			if (i < 0 || j < 0 || i >= 64 || j >= 64)
				continue;

			if (a != p_FrackMan && i >= x && j >= y && p_Dirt[i][j] != nullptr)
				return true;
		}
	}

	for (unsigned int k = 0; k < p_Actor.size(); k++)
	{
		if (p_Actor[k]->isObstacle() && a != p_Actor[k])
		{
			int dx = x - p_Actor[k]->getX();
			int dy = y - p_Actor[k]->getY();
			if (sumOfSquares(dx, dy) <= 9)
				return true;
		}
	}
	return false;
}

void StudentWorld::pathToNext(bool radar, int x, int y, int maxStep)
{
	queue<Coord> coordQueue;


	int arr[64][64];
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 64; j++)
		{
			if (p_Dirt[i][j] != nullptr)
				arr[i][j] = -1;
			else
				arr[i][j] = 0;
		}

	int nProtester = 0;

	for (unsigned int j = 0; j < p_Actor.size(); j++)
	{
		if (p_Actor[j]->annoy() && dynamic_cast<Protester*>(p_Actor[j])->getRestCount() == 0) //if protester, if restcount = 0, and if in state of leaving oil field
		{
			if ((!radar && dynamic_cast<Protester*>(p_Actor[j])->getLeaveOilField()) || (radar && dynamic_cast<Protester*>(p_Actor[j])->canUseRadar()))
			{
				arr[p_Actor[j]->getX()][p_Actor[j]->getY()] += 1;
				nProtester++;
			}
		}
	}

	coordQueue.push(Coord(x, y));
	int currentSquare = 1;
	int nextSquare = 0;
	int numIterations = 0;
	int step = 0;
	int nFound = 0;

	while (!coordQueue.empty())
	{
		Coord current = coordQueue.front();
		coordQueue.pop();

		if (current.y() < 63 && arr[current.x()][current.y() + 1] != -1 && !isThereObstacle(current.x(), current.y() + 1) ) //north
		{
			coordQueue.push(Coord(current.x(), current.y() + 1));
			nextSquare++;
			if (arr[current.x()][current.y() + 1] != 0)
			{
				int nTemp = 0;
				for (unsigned int k = 0; k < p_Actor.size(); k++)
				{
					if (p_Actor[k]->getX() == current.x() && p_Actor[k]->getY() == current.y() + 1 && ((!radar && dynamic_cast<Protester*>(p_Actor[k])->getLeaveOilField()) || (radar && dynamic_cast<Protester*>(p_Actor[k])->canUseRadar())))
					{
						Actor *p = p_Actor[k];
						dynamic_cast<Protester*>(p)->setNext(current.x(), current.y());
						nFound++;
						nTemp++;
						if (nTemp >= arr[current.x()][current.y() + 1])
							break;
					}
				}
			}
			arr[current.x()][current.y() + 1] = -1;
		}

		if (current.y() > 0 && arr[current.x()][current.y() - 1] != -1 && !isThereObstacle(current.x(), current.y() - 1)) //south
		{
			coordQueue.push(Coord(current.x(), current.y() - 1));
			nextSquare++;
			if (arr[current.x()][current.y() - 1] != 0)
			{
				int nTemp = 0;
				for (unsigned int k = 0; k < p_Actor.size(); k++)
				{
					if (p_Actor[k]->getX() == current.x() && p_Actor[k]->getY() == current.y() - 1 && ((!radar && dynamic_cast<Protester*>(p_Actor[k])->getLeaveOilField()) || (radar && dynamic_cast<Protester*>(p_Actor[k])->canUseRadar())))
					{
						Actor *p = p_Actor[k];
						dynamic_cast<Protester*>(p)->setNext(current.x(), current.y());
						nFound++;
						nTemp++;
						if (nTemp >= arr[current.x()][current.y() - 1])
							break;
					}
				}
			}
			arr[current.x()][current.y() - 1] = -1;
		}

		if (current.x() > 0 && arr[current.x() - 1][current.y()] != -1 && !isThereObstacle(current.x() - 1, current.y())) //west
		{
			coordQueue.push(Coord(current.x() - 1, current.y()));
			nextSquare++;
			if (arr[current.x() - 1][current.y()] != 0)
			{
				int nTemp = 0;
				for (unsigned int k = 0; k < p_Actor.size(); k++)
				{
					if (p_Actor[k]->getX() == current.x() - 1 && p_Actor[k]->getY() == current.y() && ((!radar && dynamic_cast<Protester*>(p_Actor[k])->getLeaveOilField()) || (radar && dynamic_cast<Protester*>(p_Actor[k])->canUseRadar())))
					{
						Actor *p = p_Actor[k];
						dynamic_cast<Protester*>(p)->setNext(current.x(), current.y());
						nFound++;
						nTemp++;

						if (nTemp >= arr[current.x() - 1][current.y()])
							break;
					}
				}
			}
			arr[current.x() - 1][current.y()] = -1;
		}

		if ( current.x() < 63 && arr[current.x() + 1][current.y()] != -1 && !isThereObstacle(current.x() + 1, current.y())) //east
		{
			coordQueue.push(Coord(current.x() + 1, current.y()));
			nextSquare++;
			if (arr[current.x() + 1][current.y()] != 0)
			{
				int nTemp = 0;
				for (unsigned int k = 0; k < p_Actor.size(); k++)
				{
					if (p_Actor[k]->getX() == current.x() + 1 && p_Actor[k]->getY() == current.y() && ((!radar && dynamic_cast<Protester*>(p_Actor[k])->getLeaveOilField()) || (radar && dynamic_cast<Protester*>(p_Actor[k])->canUseRadar())))
					{
						Actor *p = p_Actor[k];
						dynamic_cast<Protester*>(p)->setNext(current.x(), current.y());
						nFound++;
						nTemp++;
						if (nTemp >= arr[current.x() + 1][current.y()])
							break;
					}
				}
			}
			arr[current.x() + 1][current.y()] = -1;
		}

		numIterations++;
		if (numIterations == currentSquare)
		{
			step++;
			currentSquare = nextSquare;
			numIterations = 0;
			nextSquare = 0;
		}

		if (radar && step >= maxStep)
			break;

		if (nFound >= nProtester)
			break;
	}
} 


bool StudentWorld::causeAnnoyance(int ID, int HP, int x, int y)
{
	if (ID == IID_PLAYER)
	{
		p_FrackMan->annoy(HP);
		return true;
	}
		
	
	else
	{
		bool annoyedOne = false;
		for (unsigned int i = 0; i < p_Actor.size(); i++)
		{
			if (p_Actor[i]->annoy() && !dynamic_cast<Protester*>(p_Actor[i])->getLeaveOilField() && p_Actor[i]->getX() == x && p_Actor[i]->getY() == y)
			{
				p_Actor[i]->annoy(HP);
				annoyedOne = true;
			}
		}
		return annoyedOne;
	}

}

bool StudentWorld::isInDirectLine(Actor *origin, char &dir)
{
	int x = origin->getX();
	int y = origin->getY();

	if (origin->getY() == p_FrackMan->getY())
	{
		bool hitLeft = false;
		bool hitRight = false;
		for (int h = 0; h < 64; h++)
		{
			if (!hitLeft)
			{
				if (x - h < 0 || isThereObstacle(x - h, y))
					hitLeft = true;
				else if (x - h == p_FrackMan->getX())
				{
					dir = 'l';
					return true;
				}
			}
			if (!hitRight)
			{
				if (x + h > 63 || isThereObstacle(x + h, y))
					hitRight = true;
				else if (x + h == p_FrackMan->getX())
				{
					dir = 'r';
					return true;
				}
			}
			if (hitLeft && hitRight)
				break;
		}
		return false;
	}

	if (origin->getX() == p_FrackMan->getX())
	{
		bool hitUp = false;
		bool hitDown = false;
		for (int v = 0; v < 64; v++)
		{
			if (!hitDown)
			{
				if (y - v < 0 || isThereObstacle(x, y - v))
					hitDown = true;
				else if (y - v == p_FrackMan->getY())
				{
					dir = 'd';
					return true;
				}
			}
			if (!hitUp)
			{
				if (y + v > 63 || isThereObstacle(x, y + v))
					hitUp = true;
				else if (y + v == p_FrackMan->getY())
				{
					dir = 'u';
					return true;
				}
			}
			if (hitDown && hitUp)
				break;
		}
		return false;
	}
	return false;


}

int StudentWorld::randInt(int min, int max)
{
	if (max < min)
		swap(max, min);
	static random_device rd;
	static mt19937 generator(rd());
	uniform_int_distribution<> distro(min, max);
	return distro(generator);
}

bool StudentWorld::dirtUnderneath(Actor *a)
{
	for (int i = 0; i < 4; i++)
	{
		if (p_Dirt[a->getX() + i][a->getY() - 1] != nullptr)
			return true;
	}
	return false;

}

void StudentWorld::annoySurroundings(Actor *a, int HP)
{
	int x = a->getX();
	int y = a->getY();

	bool annoyedOne = false;
	for (int k = 0; k < p_Actor.size(); k++)
	{
		int dx = x - p_Actor[k]->getX();
		int dy = y - p_Actor[k]->getY();
		if (p_Actor[k]->annoy(-1) && sumOfSquares(dx, dy) <= 9)
		{
			p_Actor[k]->annoy(HP);
			annoyedOne = true;
		}
	}

	if (a->isObstacle() && isThereHuman(a, true, 3))
	{
		p_FrackMan->annoy(HP);
	}

	if (annoyedOne == true && !a->isObstacle())
		a->setInactive();

/*	for (int i = x - 3; i < x + 4; i++)
		for (int j = y - 3; j < y + 4; j++)
		{
			if (i < 0 || i > 63 || j < 0 || j > 63)
				continue;

			if (causeAnnoyance(IID_PROTESTER, HP, i, j))
				annoyedOne = true;

			if (a->isObstacle())
			{
				if (p_FrackMan->getX() == i && p_FrackMan->getY() == j)
				{
					causeAnnoyance(IID_PLAYER, HP);
				}
			}
		}

	if (!a->isObstacle() && annoyedOne == true)
		a->setInactive(); */
}

void StudentWorld::storeNewActor(Actor *a)
{
	p_Actor.push_back(a);
}

bool StudentWorld::isThereHuman(Actor *a, bool lookForFrackMan, int range)
{

	if (lookForFrackMan == true)
	{
		int dx = a->getX() - p_FrackMan->getX();
		int dy = a->getY() - p_FrackMan->getY();
		if (sumOfSquares(dx, dy) <= square(range))
			return true;

		else
			return false;
	}
	
	else
	{
		for (unsigned int k = 0; k < p_Actor.size(); k++)
		{
			if (p_Actor[k]->annoy() && !dynamic_cast<Protester*>(p_Actor[k])->getLeaveOilField())
			{
				int dx = a->getX() - p_Actor[k]->getX();
				int dy = a->getY() - p_Actor[k]->getY();
				if (sumOfSquares(dx, dy) <= square(range))
				{
					dynamic_cast<Protester*>(p_Actor[k])->getBribed();
					return true;
				}
			}
		}
		return false;
	}
		
}

int StudentWorld::sumOfSquares(int dx, int dy)
{
	return square(dx) + square(dy);
}

int StudentWorld::square(int n)
{
	return n * n;
}

int StudentWorld::max(int n1, int n2)
{
	if (n1 > n2)
		return n1;
	else
		return n2;
}

void StudentWorld::addToInventory(int ID)
{
	switch (ID)
	{
	case IID_GOLD:
		p_FrackMan->addGoldNuggets();
		break;
	case IID_SONAR:
		p_FrackMan->addSonarCharge();
		break;
	case IID_WATER_POOL:
		p_FrackMan->addWater();
		break;
	}
}

void StudentWorld::sonarHidden(Actor *a)
{

	for (unsigned int i = 0; i < p_Actor.size(); i++)
	{
		int dx = a->getX() - p_Actor[i]->getX();
		int dy = a->getY() - p_Actor[i]->getY();
		if (p_Actor[i]->isPickupable() == true && sumOfSquares(dx, dy) < 144)
		{
			ActivatingObject *o = dynamic_cast<ActivatingObject*>(p_Actor[i]);
			o->showItself();
		}
	}
		
}

int StudentWorld::min(int n1, int n2)
{
	if (n1 < n2)
		return n1;

	else
		return n2;
}

bool StudentWorld::overlap(int x, int y, int range)
{
	for (unsigned int k = 0; k < p_Actor.size(); k++)
	{
		int dx = x - p_Actor[k]->getX();
		int dy = y - p_Actor[k]->getY();
		if (sumOfSquares(dx, dy) <= square(range))
		{
			return true;
		}
	}
	return false;
}

bool StudentWorld:: isOnMineshaft(int x)
{
	for (int i = 27; i < 34; i++)
	{
		if (x == i)
			return true;
	}

	return false;
}

void StudentWorld::setDisplayText()
{
	string score = numToStr(getScore());
	string level = numToStr(getLevel());
	string lives = numToStr(getLives());
	string health = numToStr((p_FrackMan->getHP() / 10.0) * 100);
	string squirts = numToStr(p_FrackMan->getWater());
	string gold = numToStr(p_FrackMan->getGoldNuggets());
	string sonar = numToStr(p_FrackMan->getSonarCharge());
	string barrelsLeft = numToStr(m_nTotalBarrels - m_nBarrels);

	adjustText(score, 6, "0");
	adjustText(level, 2, " ");
	adjustText(health, 3, " ");
	adjustText(squirts, 2, " ");
	adjustText(gold, 2, " ");
	adjustText(sonar, 2, " ");
	adjustText(barrelsLeft, 2, " ");

	string s = "  ";
	string result = "Scr: " + score + s + "Lvl: " + level + s + "Lives: " + lives + s + "Hlth: " + health + "%" + s + "Wtr: " + squirts + s + "Gld: " + gold + s + "Sonar: " + sonar + s + "Oil Left: " + barrelsLeft;

	setGameStatText(result);

}

void StudentWorld::barrelFound()
{
	m_nBarrels++;
}

void StudentWorld::adjustText(string &str, int length, string attach)
{
	string result;
	if (str.size() < length)
	{
		for (unsigned int i = 0; i < length - str.size(); i++)
			result += attach;
	}

	result += str;
	str = result;
}

string StudentWorld::numToStr(int num)
{
	if (num == 0)
		return "0";

	stack<char> c;
	while (num > 0)
	{
		int thisDigit = num % 10;
		switch (thisDigit)
		{
		case 0:
			c.push('0');
			break;
		case 1:
			c.push('1');
			break;
		case 2:
			c.push('2');
			break;
		case 3:
			c.push('3');
				break;
		case 4:
			c.push('4');
			break;
		case 5:
			c.push('5');
			break;
		case 6:
			c.push('6');
			break;
		case 7:
			c.push('7');
			break;
		case 8:
			c.push('8');
			break;
		case 9:
			c.push('9');
			break;
		}

		num /= 10;
	}

	string result;
	while (!c.empty())
	{
		result += c.top();
		c.pop();
	}

	return result;
}

bool StudentWorld::successOrFail(int probability)
{
	int bound = randInt(1, 100);

	if (bound <= probability)
		return true;
	else
		return false;
}

void StudentWorld::addPool()
{
	bool added = false;

	while (added == false)
	{
		int x = randInt(0, 60);
		int y = randInt(0, 60);
		if (!isThereObstacle(x, y) && !overlap(x, y))
		{
			p_Actor.push_back(new WaterPool(x, y, this));
			added = true;
		}
	}
}

void StudentWorld::whereIsFrackMan(int &x, int &y)
{
	x = p_FrackMan->getX();
	y = p_FrackMan->getY();
}
