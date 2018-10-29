#include "StudentWorld.h"
#include <string>
#include "GameConstants.h"
#include "Level.h"
#include "GraphObject.h"
#include "Actor.h"
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

int counter = 0;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp


void StudentWorld::updateDisplay()
{
	ostringstream dispText;
	dispText << "Score: ";
	dispText.fill('0');
	dispText << setw(7) << getScore();
	dispText << "  Level: ";
	dispText << setw(2) << getLevel();
	dispText << "  Lives: ";
	dispText.fill(' ');
	dispText << setw(2) << getLives();
	dispText << "  Health: ";
	dispText.fill(' ');
	dispText << setw(3) << m_player->getHealth() * 5;
	dispText << "%  Ammo: ";
	dispText << setw(3) << m_player->getAmmo();
	dispText << "  Bonus: ";
	dispText << setw(4) << m_bonus;
	string currText = dispText.str();
	setGameStatText(currText);
}


int StudentWorld::loadLevel(Level& lev)
{
	ostringstream levNum;
	levNum.fill('0');
	levNum << setw(2) << getLevel();
	currLevel = "Level" + levNum.str() + ".dat";
	Level::LoadResult result = lev.loadLevel(currLevel);
	if (result == Level::load_fail_file_not_found)
	{
		cerr << "Could not find " << "" << " data file" << endl;
		return -1;
	}
	else if (result == Level::load_fail_bad_format)
	{
		cerr << "Your level was improperly formatted" << endl;
		return 0;
	}
	else if (result == Level::load_success)
		cerr << "Successfully loaded level \n";
		return 1;
}


int StudentWorld::init()
{
	//load level
	Level lev(GameWorld::assetDirectory());
	int check = loadLevel(lev);

	//find and allocate for Actors
	for (int x = 0; x < VIEW_WIDTH; ++x)
	{
		for (int y = 0; y < VIEW_HEIGHT; ++y)
		{
			Level::MazeEntry coord = lev.getContentsOf(x, y);
			ostringstream oss_location;
			oss_location << x << "," << y;
			string location = oss_location.str();
			switch (coord)
			{
			case Level::empty:
				break;
			case Level::exit:
				++m_nActors;
				m_actors.push_back(new Exit(x, y, this));
				break;
			case Level::player:
				m_player = new Player(x, y, this);
				break;
			case Level::horiz_snarlbot:
				++m_nActors;
				m_actors.push_back(new Snarlbot(x, y, this, GraphObject::Direction::right));
				break;
			case Level::vert_snarlbot:
				++m_nActors;
				m_actors.push_back(new Snarlbot(x, y, this, GraphObject::Direction::down));
				break;
			case Level::kleptobot_factory:
				++m_nActors;
				m_actors.push_back(new Factory(x, y, this));
				break;
			case Level::angry_kleptobot_factory:
				++m_nActors;
				m_actors.push_back(new Factory(x, y, this, true));
				break;
			case Level::wall:
				++m_nActors;
				m_actors.push_back(new Wall(x, y, this));
				break;
			case Level::boulder:
				++m_nActors;
				m_actors.push_back(new Boulder(x, y, this));
				break;
			case Level::hole:
				++m_nActors;
				m_actors.push_back(new Hole(x, y, this));
				break;
			case Level::jewel:
				addJewel();
				++m_nActors;
				m_actors.push_back(new Jewel(x, y, this));
				break;
			case Level::restore_health:
				++m_nActors;
				m_actors.push_back(new RestoreHealth(x, y, this));
				break;
			case Level::extra_life:
				++m_nActors;
				m_actors.push_back(new ExtraLife(x, y, this));
				break;
			case Level::ammo:
				++m_nActors;
				m_actors.push_back(new Ammo(x, y, this));
				break;
			default:	cout << "---What??";	break;
			}
		}
	}

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::countKbots(int x, int y, bool& kBot) const
{
	int count = 0;
	for (int k = 0; k < m_nActors; ++k)
	{
		Actor* p = m_actors[k];
		int ax = p->getX(),
			ay = p->getY();
		if (p->isKleptoBot() && (ax <= x + 3 && ax >= x - 3 && ay <= y + 3 && ay >= y - 3))
		{
			if (ax == x && ay == y)
				kBot = true;
			++count;
		}
	}
	return count;
}

bool StudentWorld::isbetween(int a, int r, int p) const
{
	return ((a < r && a > p) || (a > r && a < p));
}

bool StudentWorld::existsClearShotAtPlayer(int x, int y, GraphObject::Direction d) const
{
	int px = m_player->getX(), py = m_player->getY();
	if (x != px && y != py)
		return false;
	bool facingPlayer = false;
	switch (d)
	{
	case GraphObject::Direction::up:	if (y < py)	facingPlayer = true;	break;
	case GraphObject::Direction::down:	if (y > py)	facingPlayer = true;	break;
	case GraphObject::Direction::right:	if (x < px)	facingPlayer = true;	break;
	case GraphObject::Direction::left:	if (x > px)	facingPlayer = true;	break;
	}
	if (!facingPlayer)	return false;
	if (x == px)
	{
		for (int k = 0; k < m_nActors; ++k)
		{
			Actor* p = m_actors[k];
			int ay = p->getY(),	ax = p->getX();
			if (ax == x && isbetween(ay, y, py) && !p->shootThrough())
				return false;
		}
		return true;
	}
	else if (y == py)
	{
		for (int k = 0; k < m_nActors; ++k)
		{
			Actor* p = m_actors[k];
			int ay = p->getY(),
				ax = p->getX();
			if (ay == y && isbetween(ax, x, px)  && !p->shootThrough())
				return false;
		}
		return true;
	}
	return false;
}

void StudentWorld::addActor(Actor* newActor)
{
	m_actors.push_back(newActor);	++m_nActors;	return;
}

Actor* StudentWorld::getContentsAt(int x, int y, const Actor* a) const
{
	if (a != m_player && m_player->getX() == x && m_player->getY() == y)
		return m_player;
	for (int k = 0; k < m_nActors; ++k)
	{
		if (a != m_actors[k] && m_actors[k]->getX() == x && m_actors[k]->getY() == y)
			return m_actors[k];
	}
	return nullptr;
}

Actor* StudentWorld::checkGoodie(int x, int y) const
{
	for (int k = 0; k < m_nActors; ++k)
		if (m_actors[k]->isGoodie() && x == m_actors[k]->getX() && y == m_actors[k]->getY())
			return m_actors[k];
	return nullptr;
}

bool StudentWorld::canBoulderMoveTo(int x, int y) const
{
	for (int k = 0; k < m_nActors; ++k)
	{
		Actor* p = m_actors[k];
		int ax = p->getX(),	ay = p->getY();
		if (x == ax && y == ay )
			if (p->isHole() || p->isBullet())	return true;
			else	return false;
	}
	return true;
}

bool StudentWorld::canPlayerMoveTo(int x, int y) const
{
	bool canMove = true;
	for (int k = 0; k < m_nActors; ++k)
	{
		Actor* p = m_actors[k];
		int ax = p->getX(),	ay = p->getY();
		if (x == ax && y == ay)
			if (p->isBoulder())
				return p->push(m_player->getDirection());
			else if (p->isFactory() || p->isMoveableActor() || p->isWall() || p->isHole())
				return false;
			else
				continue;
	}
	return canMove;
}

bool StudentWorld::checkBulletSqaure(int x, int y) const
{
	if (m_player->getX() == x && m_player->getY() == y)
	{
		m_player->damage();	return true;
	}
	bool beKilled = false;
	for (int k = 0; k < m_nActors; ++k)
	{
		Actor* p = m_actors[k];
		int ax = p->getX(),
			ay = p->getY();
		if (x == ax && y == ay)
			if (p->isMoveableActor())
			{
				p->damage();  return true;
			}
			else if (p->isWall())	return true;
			else if (p->isFactory())
			{
				beKilled = true;
			}
	}
	return beKilled;
}

bool StudentWorld::canRobotMoveTo(int x, int y) const
{
	if (m_player->getX() == x && m_player->getY() == y)
	{
		return false;
	}
	bool canMove = true;
	for (int k = 0; k < m_nActors; ++k)
	{
		Actor* p = m_actors[k];
		int ax = p->getX(),	ay = p->getY();
		if (x == ax && y == ay)
			if (p->isMoveableActor() || p->isWall() || p->isHole() || p->isFactory())
				return false;
			else if (p->isPickable() || p->isExit())
				continue;
	}
	return canMove;
}

void StudentWorld::showExit()
{
	if (m_exitShown)
		return;
	for (int k = 0; k < m_nActors; ++k)
	{
		if (m_actors[k]->isExit())
			m_actors[k]->setVisible(true);
	}
	m_exitShown = true;
	playSound(SOUND_REVEAL_EXIT);
}

int StudentWorld::resetLevel(int x)
{
	m_jewels = 0;
	switch (x)
	{
	case -1:	m_bonus = 1000;	decLives();
		return GWSTATUS_PLAYER_DIED;
	case 0:
		increaseScore(m_bonus + EXIT_BONUS);
		m_bonus = 1000;
		m_finishedLevel = false;
		playSound(SOUND_FINISHED_LEVEL);
		if (getLevel() == 99)
			return GWSTATUS_PLAYER_WON;
		return GWSTATUS_FINISHED_LEVEL;
	}
	return -1;
}

void StudentWorld::checkPlayerFinishedLevel() const
{
	for (int k = 0; k < m_nActors; ++k)
		if (m_actors[k]->isExit())
			m_actors[k]->doSomething();
}

int StudentWorld::move()
{
	updateDisplay();
	if (!m_player->isAlive())
		return resetLevel(-1);
		
	//tell all actors to do something
	for (int act = 0; act < m_nActors; ++act)
	{
		if (m_actors[act]->isAlive())
			m_actors[act]->doSomething();
		if (m_finishedLevel)
			return resetLevel(0);
		if (!m_player->isAlive())
			return resetLevel(-1);
	}
	m_player->doSomething();
	if (gotAllJewels())
	{
		showExit();
		checkPlayerFinishedLevel();
	}
	if (m_finishedLevel)
		return resetLevel(0);
	if (!m_player->isAlive())
		return resetLevel(-1);
	
	//remove dead object
	vector<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{ 
		if (!((*it)->isAlive()))
		{
			delete *it;
			--m_nActors;
			it = m_actors.erase(it);
			continue;
		}
		it++;
	}

	//reduce game bounus
	if (m_bonus > 0)
		--m_bonus;

	//check if player is dead
	if (!m_player->isAlive())
		return resetLevel(-1);
	
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	if (m_player == nullptr)
		cout << "can't follow nullptr\n";
	else
		delete m_player;

	//delete all the actors

	vector<Actor*>::iterator it = m_actors.begin();
	if (it == m_actors.end())
		cout << "There are no actors to cleanup\n";
	while (it != m_actors.end())
	{
		delete *it;
		--m_nActors;
		it++;
	}
	m_actors.clear();
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

void StudentWorld::increasePlayerAmmo(int n) const
{
	m_player->increaseAmmo(n);
}

void StudentWorld::restorePlayerHealth() const
{
	m_player->restoreHealth();
}