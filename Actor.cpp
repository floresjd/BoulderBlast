#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include "GameConstants.h"
#include <cstdlib>
using namespace std;
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp


//-----EXIT >> from Actor
void Exit::doSomething()
{
	Actor* pa = getWorld()->getContentsAt(getX(), getY(), this);
	if (pa != nullptr && isVisible() && pa->isPlayer())
		getWorld()->setFinishedLevel();
}
//-----END EXIT


//---------HOLE >> from Actor
void Hole::doSomething()
{
	if (!isAlive())
		return;
	Actor* pa = getWorld()->getContentsAt(getX(), getY(), this);
	if (pa != nullptr)
	{
		if (pa->isBoulder())
		{
			pa->setDead();
			setDead();
		}
	}
	return;
}
//----------END HOLE


//-------BULLET >> from Actor
void Bullet::doSomething()
{
	if (!isAlive())
		return;
	if (m_duringCurrTick)
	{
		m_duringCurrTick = false;
		return;
	}
	if (checkSquare())
	{
		setDead();
		return;
	}
	moveActor(getDirection());
	if (checkSquare())
	{
		setDead();
	}
}

//return true if bullet should not die
bool Bullet::checkSquare()
{
	return (getWorld()->checkBulletSqaure(getX(), getY()));
}
//------------------END BULLET


//-------FACTORY >> from Actor
void Factory::doSomething()
{
	bool kb = false;
	int x = rand() % 50;
	if (x == 10 && (getWorld()->countKbots(getX(), getY(), kb) < 3) && !kb)
	{
		getWorld()->playSound(SOUND_ROBOT_BORN);
		if (!m_angry)
			getWorld()->addActor(new Kleptobot(getX(), getY(), getWorld(), Direction::right));
		else
			getWorld()->addActor(new AngryKleptobot(getX(), getY(), getWorld(), Direction::right));
	}
}
//-------------END FACTORY

//-----PICKABLE >> from Actor
void Pickable::increasePlayerScore()
{
	getWorld()->increaseScore(m_worth);
}
//-----END PICKABLE

//------JEWEL >> from pickable >> from actor
void Jewel::doSomething()
{
	if (!isAlive() && !isVisible())
		return;
	Actor* pa = getWorld()->getContentsAt(getX(), getY(), this);
	if (pa != nullptr)
	{
		if (pa->isPlayer())
		{
			increasePlayerScore();
			setDead();
			getWorld()->playSound(SOUND_GOT_GOODIE);
			getWorld()->jewelPickedup();
		}
	}
	return;
}
//-------END JEWEL

//----GOODIE >> from Pickable >> from Actor
//------END GOODIE


//-----AMMO >> from Goodie >> from Pickable >> from Actor
void Ammo::doSomething()
{
	if (!isAlive() || !isVisible())
		return;
	Actor* pa = getWorld()->getContentsAt(getX(), getY(), this);
	if (pa != nullptr)
	{
		if (pa->isPlayer())
		{
			increasePlayerScore();
			getWorld()->increasePlayerAmmo(20);
			setDead();
		}
	}
	return;
}
//-----END AMMO

//-----EXTRALIFE >> from Goodie >> from Pickable >> from Actor
void ExtraLife::doSomething()
{

	if (!isAlive() || !isVisible())
		return;
	Actor* pa = getWorld()->getContentsAt(getX(), getY(), this);
	if (pa != nullptr)
	{
		if (pa->isPlayer())
		{
			increasePlayerScore();
			getWorld()->incLives();
			setDead();
		}
	}
	return;
}
//----END EXTRALIFE


//-----RESTOREHEALTH >> from Goodie >> Pickable >> Actor
void RestoreHealth::doSomething()
{

	if (!isAlive() || !isVisible())
		return;
	Actor* pa = getWorld()->getContentsAt(getX(), getY(), this);
	if (pa != nullptr)
	{
		if (pa->isPlayer())
		{
			increasePlayerScore();
			getWorld()->restorePlayerHealth();
			setDead();
		}
	}
	return;
}
//-----END RESTOREHEALTH

//-----ACTOR  (base)
bool Actor::moveActor(Direction d)
{
	int X = getX(), Y = getY();
	switch (d)
	{
	case Direction::up:		if (canActorMoveTo(X, Y + 1))	moveTo(X, Y + 1);	break;
	case Direction::down:	if (canActorMoveTo(X, Y - 1))	moveTo(X, Y - 1);	break;
	case Direction::right:	if (canActorMoveTo(X + 1, Y))	moveTo(X + 1, Y);	break;
	case Direction::left:	if (canActorMoveTo(X - 1, Y))	moveTo(X - 1, Y);	break;
	}
	//true false if previous x and y equal current == player did not move
	return !(getX() == X && getY() == Y);
}

void Actor::damage()
{
	m_health -= 2;
	if (m_health <= 0)
	{
		setDead();
	}
	return;
}
//-----END ACTOR

//-----MOVEABLEACTOR >> Actor
//-----END MOVEABLEACTOR


//----BOULDER >> MoveableActor >> Actor
bool Boulder::canActorMoveTo(int x, int y) const
{
	return getWorld()->canBoulderMoveTo(x, y);
}

bool Boulder::push(Direction d)
{
	return moveActor(d);
}
//-----END BOULDER


//-----SHOOTER >> MoveableActor >> Actor
void Shooter::shoot(bool thisTick) const
{
	Direction shooterD = getDirection();
	int shooterX = getX(), shooterY = getY();
	switch(shooterD)
	{
	case Direction::up:		getWorld()->addActor(new Bullet(shooterX, shooterY + 1, getWorld(), Direction::up, thisTick));	break;
	case Direction::down:	getWorld()->addActor(new Bullet(shooterX, shooterY - 1, getWorld(), Direction::down, thisTick));	break;
	case Direction::right:	getWorld()->addActor(new Bullet(shooterX + 1, shooterY, getWorld(), Direction::right, thisTick));	break;
	case Direction::left:	getWorld()->addActor(new Bullet(shooterX - 1, shooterY, getWorld(), Direction::left, thisTick));	break;
	}
}
//-----END SHOOTER

//------PLAYER >> Shooter >> MoveableActor >> Actor
void Player::doSomething()
{
	if (!isAlive())
		return;
	int input;
	if (getWorld()->getKey(input))
	{
		int X = getX(), Y = getY();
		switch (input)
		{
		case KEY_PRESS_UP:		setDirection(Direction::up);	moveActor(Direction::up);	break;
		case KEY_PRESS_DOWN:	setDirection(Direction::down);	moveActor(Direction::down);	break;
		case KEY_PRESS_RIGHT:	setDirection(Direction::right);	moveActor(Direction::right); break;
		case KEY_PRESS_LEFT:	setDirection(Direction::left);	moveActor(Direction::left);	break;
		case KEY_PRESS_ESCAPE:	setDead();	return;
		case KEY_PRESS_SPACE:
			if (canShoot())
			{
				shoot(false);
				getWorld()->playSound(SOUND_PLAYER_FIRE);
				--m_ammo;
			}
			break;
		}
	}
}

bool Player::canActorMoveTo(int x, int y) const
{
	return getWorld()->canPlayerMoveTo(x, y);
}

void Player::damage()
{
	Actor::damage();
	getWorld()->playSound(SOUND_PLAYER_IMPACT);
	if (!isAlive())
		getWorld()->playSound(SOUND_PLAYER_DIE);
}
//------END PLAYER

//-----ROBOT >> Shooter >> MoveableActor >> Actor
bool Robot::canActorMoveTo(int x, int y) const
{
	return (getWorld()->canRobotMoveTo(x, y));
}

bool Robot::canShootAtPlayer() const
{
	return (getWorld()->existsClearShotAtPlayer(getX(), getY(), getDirection()));
}

void Robot::damage()
{
	Actor::damage();
	getWorld()->playSound(SOUND_ROBOT_IMPACT);
	if (!isAlive())
	{
		getWorld()->increaseScore(m_bonus);
		getWorld()->playSound(SOUND_ROBOT_DIE);
	}
}

bool Robot::canRobotMove()
{
	if (m_ticks == 1)
	{
		setTicks();
		return true;
	}
	--m_ticks;
	return false;
}

void Robot::setTicks()
{
	m_ticks = ((28 - getWorld()->getLevel()) / 4);
	if (m_ticks < 3)
		m_ticks = 3;
}
//--------END ROBOT


//-----SNARLBOT >> Robot >> Shooter >> MoveableActor >> Actor
void Snarlbot::doSomething()
{
	if (!isAlive() || !canRobotMove())
		return;
	if (canShootAtPlayer())
	{
		shoot();	return;
	}
	//else
	if (!moveActor(getDirection()))
		reverseDirection();
}

void Snarlbot::reverseDirection()
{
	switch (getDirection())
	{
	case Direction::up:		setDirection(Direction::down);	break;
	case Direction::down:	setDirection(Direction::up);	break;
	case Direction::left:	setDirection(Direction::right);	break;
	case Direction::right:	setDirection(Direction::left);	break;
	}
}
//----END SNARLBOT


//-------KLEPTOBOT >> Robot >> Shooter >> MoveableActor >> Actor
void Kleptobot::doSomething()
{
	if (!isAlive() || !canRobotMove())	
		return;
	if (!haveGoodie() && pickedUpGoodie())
		return;
	moveKbot();
}

void Kleptobot::damage()
{
	Robot::damage();
	if (!isAlive() && haveGoodie())
	{
		switch (m_goodieID)
		{
		case IID_AMMO:			getWorld()->addActor(new Ammo(getX(), getY(), getWorld()));			return;
		case IID_EXTRA_LIFE:	getWorld()->addActor(new ExtraLife(getX(), getY(), getWorld()));	return;
		case IID_RESTORE_HEALTH:getWorld()->addActor(new RestoreHealth(getX(), getY(), getWorld()));return;
		}
	}
}

bool Kleptobot::pickedUpGoodie()
{
	Actor* p = getWorld()->checkGoodie(getX(), getY());
	int x = rand() % 10;
	if (p != nullptr && x == 5)
	{
		getWorld()->playSound(SOUND_ROBOT_MUNCH);
		m_goodie = true;
		m_gx = p->getX();
		m_gy = p->getY();
		p->setDead();
		m_goodieID = p->getID();
		return true;
	}
	return false;
}

void Kleptobot::moveKbot()
{
	bool moved = false;
	if (m_dbt != 0)
	{
		--m_dbt;
		if (moved = moveActor(getDirection()))
		{
			return;
		}
	}
	if (!moved || m_dbt == 0)
	{
		setDistanceBeforeTurning();
		int arr[4] = { 0, 1, 2, 3 };
		int d, k = 4;
		for (;;)
		{
			d = rand() % k;
			switch (arr[d])
			{
			case 0: setDirection(Direction::up); break;
			case 1: setDirection(Direction::down); break;
			case 2: setDirection(Direction::right); break;
			case 3: setDirection(Direction::left); 	break;
			}
			moved = moveActor(getDirection());
			if (moved)	return;
			for (int i = 0; i < k; i++)
			{
				if (d == i)
				{	
					arr[i] = arr[k - 1]; 
					--k; 
					break;	
				}
			}
			if (k == 0)
			{
				return;
			}
		}
	}
}

void Kleptobot::setDistanceBeforeTurning()
{
	m_dbt = rand() % 6 + 1;
}
//-----END KLEPTOBOT


//----ANGRYKLEPTOBOT >> Kleptobot >> Robot >> Shooter >> MoveableActor >> Actor
void AngryKleptobot::doSomething()
{
	if (!isAlive() || !canRobotMove())	return;
	if (!haveGoodie() && pickedUpGoodie())	return;
	if (canShootAtPlayer())
	{	shoot();	return;		}
	moveKbot();
}
//----END ANGRYKLEPTOBOT
