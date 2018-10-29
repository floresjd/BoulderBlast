#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <iostream>
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

const int SNARBOT_BOUNS = 100;
const int KLEPTOBOT_BOUNS = 10;
const int ANGRY_KLEPTOBOT_BONUS = 20;
const int JEWEL_BONUS = 50;
const int AMMO_BOUNS = 100;
const int HEALTH_BOUNS = 500;
const int EXTRA_LIFE_BONUS = 1000;
const int EXIT_BONUS = 2000;
const int RESTORE_HEALTH_BONUS = 500;

#include "GameConstants.h"
class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int imageID, int sX, int sY, StudentWorld* pSW, int health, Direction sDirection = none)
		:GraphObject(imageID, sX, sY, sDirection), p_sw(pSW), m_alive(true), m_health(health)
	{	
	}
	virtual ~Actor()
	{}
	//-----non virtual
	void restoreHealth()
	{
		m_health = 20;
	}
	bool moveActor(Direction d);
	StudentWorld* getWorld() const
	{
		return p_sw;
	}
	void setDead()
	{
		m_alive = false;
	}
	bool isAlive() const
	{
		return m_alive;
	}
	int getHealth() const
	{
		return m_health;
	}

	//-----pure virtual
	virtual void doSomething() = 0;

	//----virtual functions that can change
	virtual bool canActorMoveTo(int x, int y) const
	{
		return false;
	}
	virtual void damage();
	virtual bool push(Direction d)
	{
		return false;
	}

	//-----virtual functions asking if actor is x
	virtual bool shootThrough() const
	{
		return true;
	}
	virtual bool isWall() const
	{
		return false;
	}
	virtual bool isBoulder() const
	{
		return false;
	}
	virtual bool isActor() const
	{
		return false;
	}
	virtual bool isMoveableActor() const
	{
		return false;
	}
	virtual bool isHole() const
	{
		return false;
	}
	virtual bool isBullet() const
	{
		return false;
	}
	virtual bool isGoodie() const
	{
		return false;
	}
	virtual bool isExit() const
	{
		return false;
	}
	virtual bool isKleptoBot() const
	{
		return false;
	}
	virtual bool isPlayer() const
	{
		return false;
	}
	virtual bool isFactory() const
	{
		return false;
	}
	virtual bool isPickable() const
	{
		return false;
	}
private:
	StudentWorld* p_sw;
	bool m_alive;
	int m_health;
};

class Wall : public Actor
{
public:
	Wall(int sX, int sY, StudentWorld* pSW)
		:Actor(IID_WALL, sX, sY, pSW, -1)
	{		setVisible(true);	}
	virtual ~Wall()
	{}
	virtual void doSomething()
	{		return;	}
	virtual bool isWall() const
	{		return true;	}
	virtual bool shootThrough() const
	{		return false;	}
};

class Exit : public Actor
{
public:
	Exit( int sX, int sY, StudentWorld* pSW)
		:Actor(IID_EXIT, sX, sY, pSW, -1)
	{
		setVisible(false);
	}
	virtual ~Exit()
	{}
	virtual void doSomething();
	bool checkPlayer() const
	{
		return false; 
	}
	virtual bool isExit() const
	{
		return true;
	}
};

class Hole : public Actor
{
public:
	Hole(int sX, int sY, StudentWorld* pSW)
		:Actor(IID_HOLE, sX, sY, pSW, -1)
	{
		setVisible(true);
	}
	virtual ~Hole()
	{}
	virtual void doSomething();
	virtual bool shootThrough()
	{
		return true;
	}
	virtual bool isHole() const
	{
		return true;
	}
};

class Bullet : public Actor
{
public:
	Bullet(int sX, int sY, StudentWorld* pSW, Direction d, bool currTick)
		:Actor(IID_BULLET, sX, sY, pSW, 2, d), m_duringCurrTick(currTick)
	{
		setVisible(true);
	}
	virtual ~Bullet()
	{}
	virtual void doSomething();
	virtual bool canActorMoveTo(int x, int y) const
	{		return true;	}
	virtual bool shootThrough()
	{		return true;	}
private:
	bool checkSquare();
	bool m_duringCurrTick;
};

class Factory : public Actor
{
public:
	Factory(int sX, int sY, StudentWorld* pSW, bool angry = false)
		:Actor(IID_ROBOT_FACTORY, sX, sY, pSW, 0), m_angry(angry)
	{
		setVisible(true);
	}
	~Factory()
	{}
	virtual void doSomething();
	bool shootThrough()
	{
		return false;
	}
	virtual bool isFactory() const
	{
		return true;
	}
private:
	bool m_angry;
};

class Pickable : public Actor
{
public:
	Pickable(int imageID, int sX, int sY, StudentWorld* pSW, int worth)
		:Actor(imageID, sX, sY, pSW, -1), m_worth(worth)
	{	}
	~Pickable()
	{}
	void increasePlayerScore();
	virtual bool shootThrough()
	{
		return true;
	}
	virtual bool isPickable() const
	{
		return true;
	}
private:
	unsigned int m_worth;
};

class Jewel : public Pickable
{
public:
	Jewel(int sX, int sY, StudentWorld* pSW)
		:Pickable(IID_JEWEL, sX, sY, pSW, JEWEL_BONUS)
	{
		setVisible(true);
	}
	virtual ~Jewel()
	{}
	virtual void doSomething();
};

class Goodie : public Pickable
{
public:
	Goodie(int imageID, int sX, int sY, StudentWorld* pSW, int worth)
		:Pickable(imageID, sX, sY, pSW, worth)
	{
		setVisible(true);
	}
	virtual ~Goodie()
	{}
	virtual bool isGoodie() const
	{
		return true;
	}
};

class Ammo : public Goodie
{
public:
	Ammo(int sX, int sY, StudentWorld* pSW)
		:Goodie(IID_AMMO, sX, sY, pSW, AMMO_BOUNS)
	{}
	virtual ~Ammo()
	{}
	virtual void doSomething();
};

class ExtraLife : public Goodie
{
public:
	ExtraLife(int sX, int sY, StudentWorld* pSW)
		:Goodie(IID_EXTRA_LIFE, sX, sY, pSW, EXTRA_LIFE_BONUS)
	{	}
	virtual ~ExtraLife()
	{}
	virtual void doSomething();
};

class RestoreHealth : public Goodie
{
public:
	RestoreHealth(int sX, int sY, StudentWorld* pSW)
		:Goodie(IID_RESTORE_HEALTH, sX, sY, pSW, RESTORE_HEALTH_BONUS)
	{	}
	virtual ~RestoreHealth()
	{}
	virtual void doSomething();
};

class MoveableActor : public Actor
{
public:
	MoveableActor(int imageID, int sX, int sY, StudentWorld* pSW, int health, Direction sDirection = none)
		:Actor(imageID, sX, sY, pSW, health, sDirection)
	{}
	~MoveableActor()
	{}
	virtual bool shootThrough() const
	{		return false;	}
	virtual bool isMoveableActor() const
	{		return true;	}
};

class Boulder : public MoveableActor
{
public:
	Boulder(int sX, int sY, StudentWorld* pSW)
		:MoveableActor(IID_BOULDER, sX, sY, pSW, 10)
	{
		setVisible(true);
	}
	virtual ~Boulder()
	{}
	virtual void doSomething()
	{
		return;
	}
	virtual bool push(Direction d);
	virtual bool canActorMoveTo(int x, int y) const;
	virtual bool isBoulder() const
	{
		return true;
	}
};

class Shooter : public MoveableActor
{
public:
	Shooter(int imageID, int sX, int sY, StudentWorld* pSW, int health, Direction sDirection)
		:MoveableActor(imageID, sX, sY, pSW, health, sDirection)
	{}
	virtual ~Shooter()
	{}
	void shoot(bool thisTick = true) const;
};

class Player : public Shooter
{
public:
	Player(int sX, int sY, StudentWorld* pSW)
		:Shooter(IID_PLAYER, sX, sY, pSW, 20, Direction::right), m_ammo(20)
	{
		setVisible(true);
	}
	virtual ~Player()
	{}
	virtual void doSomething();
	virtual bool canActorMoveTo(int x, int y) const;
	virtual void damage();

	int getAmmo() const
	{
		return m_ammo;
	}
	void increaseAmmo(int n)
	{
		m_ammo += n;
	}
	virtual bool isPlayer() const
	{
		return true;
	}
private:
	bool canShoot() const
	{
		return m_ammo != 0;
	}
	int m_ammo;
};


class Robot : public Shooter
{
public:
	Robot(int imageID, int sX, int sY, StudentWorld* pSW, int health, Direction d, int b)
		:Shooter(imageID, sX, sY, pSW, health, d), m_bonus(b)
	{
		setVisible(true);
		setTicks();
	}
	virtual bool canActorMoveTo(int x, int y) const;
	bool canShootAtPlayer() const;

	virtual void damage();
	bool canRobotMove();
private:
	void setTicks();
	int m_bonus;
	int m_ticks;
};

class Snarlbot : public Robot
{
public:
	Snarlbot(int sX, int sY, StudentWorld* pSW, Direction d)
		:Robot(IID_SNARLBOT, sX, sY, pSW, 10, d, SNARBOT_BOUNS)
	{
	}
	virtual void doSomething();
	void reverseDirection();
};

class Kleptobot : public Robot
{
public:
	Kleptobot(int sX, int sY, StudentWorld* pSW, Direction d, int imageID = IID_KLEPTOBOT, int health = 5, int bonus = KLEPTOBOT_BOUNS)
		:Robot(imageID, sX, sY, pSW, health, d, bonus), m_goodie(false)
	{
		setDistanceBeforeTurning();
	}
	virtual void doSomething();
	virtual void damage();
	bool pickedUpGoodie();
	void moveKbot();
	bool haveGoodie() const
	{
		return m_goodie;
	}
	virtual bool isKleptoBot() const
	{
		return true;
	}
private:
	int m_dbt;
	void setDistanceBeforeTurning();
	bool m_goodie;
	int m_gx;
	int m_gy;
	int m_goodieID;
};

class AngryKleptobot : public Kleptobot
{
public:
	AngryKleptobot(int sX, int sY, StudentWorld* pSW, Direction d)
		:Kleptobot(sX, sY, pSW, d, IID_ANGRY_KLEPTOBOT, 8, ANGRY_KLEPTOBOT_BONUS)
	{
	}
	virtual void doSomething();
};

//NEED ROBOTS, ROBOTS FACTORY

#endif // ACTOR_H_
