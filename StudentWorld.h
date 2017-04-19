#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>

class Actor;
class FrackMan;
class Dirt;
class Boulder;
class Protester;
class GraphObject;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Coord
{
public:
	Coord(int x, int y) : m_x(x), m_y(y) {}
	int x() const { return m_x; }
	int y() const { return m_y; }
private:
	int m_x;
	int m_y;
};

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);

	virtual ~StudentWorld();

	virtual int init();

	virtual int move();

	virtual void cleanUp();

	bool removeDirt(int x, int y);

	bool isThereObstacle(int x, int y, Actor *a = nullptr);

	void pathToNext(bool radar = false, int x = 60, int y = 60, int maxStep = 0);

	bool causeAnnoyance(int ID, int HP = 2, int x = -1, int y = -1);

	bool isInDirectLine(Actor *origin, char &dir);

	int randInt(int min, int max);

	bool dirtUnderneath(Actor *a);

	void annoySurroundings(Actor *a, int HP);

	void storeNewActor(Actor *a);

	bool isThereHuman(Actor *a, bool lookForFrackMan, int range);

	int sumOfSquares(int dx, int dy);

	int square(int n);

	int max(int n1, int n2);

	void addToInventory(int ID);

	void sonarHidden(Actor *a);

	int min(int n1, int n2);

	bool overlap(int x, int y, int range = 6);

	bool isOnMineshaft(int x);

	void setDisplayText();

	void barrelFound();

	void adjustText(std::string &str, int length, std::string attach);

	std::string numToStr(int num);

	bool successOrFail(int probability);

	void addPool();

	void whereIsFrackMan(int &x, int &y);

private:
	FrackMan *p_FrackMan;
	Dirt *p_Dirt[64][64];
	std::vector<Actor*> p_Actor;
	int m_nTotalBarrels;
	int m_nBarrels;
	int m_timeSinceLastProtester;
	int m_nMaxProtester;
	int m_nProtesters;



};

#endif // STUDENTWORLD_H_
