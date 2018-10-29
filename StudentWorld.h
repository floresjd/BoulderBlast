#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
#include <vector>
class GraphObject;
class Actor;
class Player;
class Level;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir), m_player(nullptr), m_nActors(0), m_bonus(1000), m_jewels(0), m_finishedLevel(false), m_exitShown(false)
	{
	}
	
	virtual ~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	void jewelPickedup()
	{		--m_jewels;	}
	void addActor(Actor* newActor);
	void setFinishedLevel()
	{		m_finishedLevel = true;	}
	//const function vvvv
	Actor* getContentsAt(int x, int y, const Actor* a) const;
	bool canBoulderMoveTo(int x, int y) const;
	bool canPlayerMoveTo(int x, int y) const;
	bool canRobotMoveTo(int x, int y) const;
	bool checkBulletSqaure(int x, int y) const;
	void increasePlayerAmmo(int n) const;
	void restorePlayerHealth() const;
	bool existsClearShotAtPlayer(int x, int y, GraphObject::Direction d) const;
	int countKbots(int x, int y, bool& kBot) const;
	Actor* checkGoodie(int x, int y) const;
private:
	//private functions
	bool isbetween(int a, int r, int p) const;
	void checkPlayerFinishedLevel() const;
	void addJewel()
	{		++m_jewels;	}
	bool gotAllJewels() const
	{		return m_jewels == 0;	}
	int jewelsCount() const
	{		return m_jewels;	}
	void showExit();
	void updateDisplay();
	int loadLevel(Level& lev);
	int resetLevel(int x);
	//private data memeber
	bool m_exitShown;
	bool m_finishedLevel;
	Player* m_player;
	std::vector<Actor*> m_actors;
	int m_nActors;
	std::string currLevel;
	unsigned int m_bonus;
	int m_jewels;
};

#endif // STUDENTWORLD_H_
