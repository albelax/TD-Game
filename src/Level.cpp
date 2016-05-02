#include "Level.h"

Level::Level(int _width, int _height, int _obstacles, Player* _player) :
	m_mapWidth(_width),m_mapHeight(_height),m_ObstacleNumber(_obstacles)
{
	m_map.resize(m_mapHeight);
	std::vector<std::vector<char>>::iterator it;
	for (it = m_map.begin(); it != m_map.end(); ++it)
	{
		(*it).resize(m_mapWidth);
	}
	generateMap();
}


Level::Level(std::string _address, Player* _player, std::vector<Mesh *> _meshes, int _cellSize)
{
	m_player = _player;
	m_position = Vec4(0,0,0,1);
	m_cellSize = _cellSize;
	int obstacle;
	int bullet;
	for (int i = 0; i < _meshes.size(); ++i)
	{
		if(_meshes[i]->name() == "obstacle")
			obstacle = i;
		if(_meshes[i]->name() == "bullet")
			bullet = i;
	}
	std::vector<std::string> mapFile = utilityFunctions::loadFromFile(_address);
	int count = 0;
	for (int i = 0; i < mapFile.size(); ++i)
	{
		if (utilityFunctions::split(mapFile[i])[0] == "w")
			m_mapWidth = std::stoi(utilityFunctions::split(mapFile[i])[1]);
		else if (utilityFunctions::split(mapFile[i])[0] == "h")
		{
			m_mapHeight = std::stoi(utilityFunctions::split(mapFile[i])[1]);
		}
		else
		{
			std::vector<char> tempVec;
			for (int j = 0; j < utilityFunctions::split(mapFile[i]).size(); ++j)
			{
				std::string tempLine = utilityFunctions::split(mapFile[i])[j];
				if(tempLine == "0")
				{
					tempVec.push_back('0');
				}
				else if(tempLine == "1")
				{
					tempVec.push_back('1');
				}
				else if(tempLine == "2")
				{
					tempVec.push_back('2');
					m_obstacles.push_back(Obstacle(Vec4(j*m_cellSize+1.5f, 0, i*m_cellSize - m_cellSize*1.5f),Vec4(0,0,0,1),0,true,0, _meshes[obstacle],6));
					m_Satellites.push_back(EnemySatellite(
						Vec4(m_obstacles.back().getPosition().m_x + m_obstacles.back().getCollisionLimit_x(),0,m_obstacles.back().getPosition().m_z + m_obstacles.back().getCollisionLimit_z()),
						m_obstacles.back().getPosition(),_meshes[0],m_player,0.01f,true,5));
				}
				else if(tempLine == "3")
				{
					tempVec.push_back('3');
					m_obstacles.push_back(Obstacle(Vec4(j*m_cellSize+1.5f, 0, i*m_cellSize - m_cellSize*1.5f),Vec4(0,0,0,1),0,true,0, _meshes[obstacle],8));
					m_Satellites.push_back(EnemySatellite(
						Vec4(m_obstacles.back().getPosition().m_x + m_obstacles.back().getCollisionLimit_x(),0,m_obstacles.back().getPosition().m_z + m_obstacles.back().getCollisionLimit_z()),
						m_obstacles.back().getPosition(), _meshes[0],m_player,0.01f,true,5));
				}
				else if(tempLine == "4")
				{
					tempVec.push_back('4');
					m_obstacles.push_back(Obstacle(Vec4(j*m_cellSize+1.5f, 0, i*m_cellSize - m_cellSize*1.5f),Vec4(0,0,0,1),0,true,0, _meshes[obstacle],12));
					m_Satellites.push_back(EnemySatellite(
						Vec4(m_obstacles.back().getPosition().m_x + m_obstacles.back().getCollisionLimit_x(),0,m_obstacles.back().getPosition().m_z + m_obstacles.back().getCollisionLimit_z()),
						m_obstacles.back().getPosition(),_meshes[0],m_player,0.01f,true,5));
				}
				else if (tempLine == "e")
				{
					tempVec.push_back('e');
					m_enemies.push_back(Enemy(Vec4(j*m_cellSize+2, 0, i*m_cellSize - m_cellSize*1.5f),Vec4(0,0,0,1),0.5f,true,4,_meshes[0],m_player));
				}
			}
			m_map.push_back(tempVec);
			tempVec.clear();
		}
		++count;
	}
	for (int i = 0; i < 50; ++i)
	{
		m_bullets.push_back(Bullet(Vec4(0,0,0,1),Vec4(0,0,0,1),0.5f,false, _meshes[bullet]));
	}
	for (int i = 0; i < m_enemies.size(); ++i)
	{
		m_objects.push_back(&m_enemies[i]);
	}
	for (int i = 0; i < m_Satellites.size(); ++i)
	{
		m_objects.push_back(&m_Satellites[i]);
	}
	for (int i = 0; i < m_obstacles.size(); ++i)
	{
		m_objects.push_back(&m_obstacles[i]);
	}

	int w = m_mapWidth/6;
	int h = m_mapHeight/6;
	for (int i=  0; i < w; ++i)
	{
		for (int j = 0; j < h; ++j)
		{
			m_meshes.push_back( Mesh("models/level/" + std::to_string(i) + std::to_string(j) + ".obj","level"));
		}
	}
}

void Level::generateMap()
{
	srand((int)time(NULL));
	std::vector<int> taken;

	int obstacles = m_ObstacleNumber;
	while (obstacles > 0)
	{
		int w = rand()%4;
		int h = rand()%4;
		int x = rand()%m_mapWidth;
		int y = rand()%m_mapHeight;
		bool patchFree = true;

		if (w < 3)
			w = 3;
		if (h < 3)
			h = 3;
		if ( x + w > m_mapWidth)
			x -= w;
		if ( y + h > m_mapWidth)
			y -= h;

		if (taken.size() == 0)
		{
			taken.push_back(x+w);
			taken.push_back(y+h);
		}

		for (int i = 0; i < taken.size(); i+=2)
		{
			if (x+w == i || y+h == i+1)
				patchFree = false;
		}

		if(patchFree)
		{
			for (int i = 0; i < h; ++i)
			{
				for (int j = 0; j < w; ++j)
				{
					m_map[x+j][y+i] = '1';
				}
			}
			--obstacles;
		}
	}
	std::vector<std::vector<char>>::iterator it;
	for (it = m_map.begin(); it != m_map.end(); ++it)
	{
		std::vector<char>::iterator it_2;
		for (it_2 = (*it).begin(); it_2 != (*it).end(); ++it_2)
		{
			if ( (*it_2) != '1' )
				(*it_2) = '0';
			std::cout << (*it_2) << " ";
		}
		std::cout << std::endl;
	}
}


bool Level::wallCollision(GameObject * _gameObject, Vec4 _pos)
{
	int xPositive = static_cast<int>((_pos.m_x + _gameObject->getCollisionLimit_x())/(m_cellSize));
	int zPositive = static_cast<int>((_pos.m_z + _gameObject->getCollisionLimit_z())/(m_cellSize));
	int xNegative = static_cast<int>((_pos.m_x - _gameObject->getCollisionLimit_x())/(m_cellSize));
	int zNegative = static_cast<int>((_pos.m_z - _gameObject->getCollisionLimit_z())/(m_cellSize));

	if(m_map[zPositive][xPositive] == '0' || m_map[zNegative][xNegative] == '0' || m_map[zPositive][xNegative] == '0' || m_map[zNegative][xPositive] == '0')
		return true;
	return false;
}

void Level::update()
{
  if(m_player != nullptr)
  {
    m_player->updateRotation();
    m_player->updatePosition();
    m_player->updatehealthBar();
  }

	std::vector<GameObject*>::iterator it;
	for (it = m_objects.begin(); it != m_objects.end(); ++it)
	{
    if(std::abs(m_player->getPosition().m_x - (*it)->getPosition().m_x) < 50 && std::abs(m_player->getPosition().m_z - (*it)->getPosition().m_z) < 50)
		{
			(*it)->updatePosition();
			(*it)->updateRotation();
		}
	}
	Collisions();
  activateBullets();
}

void Level::draw() const
{
  m_player->draw();

	for (int i = 0; i < m_objects.size(); ++i)
	{
		if(std::abs(m_player->getPosition().m_x - m_objects[i]->getPosition().m_x) < 50 && std::abs(m_player->getPosition().m_z - m_objects[i]->getPosition().m_z) < 50 )
		{
			m_objects[i]->draw();
		}
	}
  for (int i = 0; i < m_bullets.size(); ++i)
  {
    if(m_bullets[i].active())
      m_bullets[i].draw();
  }
  drawMap();
}


void Level::drawMap() const
{
//  float x = m_cellSize/2;
//  float z = m_cellSize/2;

//  glColor3f(1, 0, 0);

//  for (int i = 0; i < m_map.size(); ++i)
//  {
//    for (int j = 0; j < m_map[i].size(); ++j)
//    {
//      if ( m_map[i][j] == '0' )
//      {
//				if (std::abs(m_player->getPosition().m_x - j*m_cellSize) < 100
//				 && std::abs(m_player->getPosition().m_z - i*m_cellSize) < 100)
//        {
//          glColor4f(0.2f,0.3f,0.6f,1);
//          glPushMatrix();
//            glTranslatef(x,0,z);
//            GLFunctions::cube(m_cellSize, 1, m_cellSize);
//          glPopMatrix();
//        }
//      }
//      x += m_cellSize;
//    }
//    z += m_cellSize;
//    x = m_cellSize/2;
//  }
	int x = m_cellSize*3;
	int z = m_cellSize*3;

	for (int i = 0; i < m_meshes.size(); ++i)
	{
			if (std::abs(m_player->getPosition().m_x - x) < 100
			&& std::abs(m_player->getPosition().m_z - z) < 100)
			{
				//glColor4f(0.2f,0.3f,0.6f,1);
					glColor3f(0.623529f, 0.698039f, 0.65098f);
				glPushMatrix();
					glTranslatef(x,0,z);
					m_meshes[i].draw(m_cellSize);
				glPopMatrix();
			}
		if ((i+1) % 5 == 0)
		{
			x = m_cellSize*3;
			z += m_cellSize*6;
			//std::cout <<i+1 << std::endl;
		}
		else
		{
			x += m_cellSize*6;
			//std::cout <<"1" << std::endl;
		}
	}
}

void Level::activateBullets()
{
	for(int i = 0; i< m_bullets.size(); ++i)
  {
		if(m_bullets[i].active() == false && m_player->canShoot() && m_player->pressedKeys()[4] == '1')
    {
      m_bullets[i].setParent(m_player);
      m_bullets[i].active(true);
			m_player->canShoot(false);
			return;
    }

    for (int i = 0; i < m_objects.size(); ++i)
    {
      if((std::abs(m_player->getPosition().m_x - m_objects[i]->getPosition().m_x) < 30
        && std::abs(m_player->getPosition().m_z - m_objects[i]->getPosition().m_z) < 30)
        && dynamic_cast<Enemy*>(m_objects[i]))
      {
        Enemy* temp_enemy = dynamic_cast<Enemy*>(m_objects[i]);
        if(temp_enemy->canShoot())
        {
          m_bullets[i].setParent(temp_enemy);
          m_bullets[i].active(true);
          temp_enemy->canShoot(false);
        }
      }
    }
  }
}

void Level::Collisions()
{
	std::vector<GameObject*>::iterator it;

	// collision detection between enemies and player, and enemies and bullets
	for (it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		if(std::abs(m_player->getPosition().m_x - (*it)->getPosition().m_x) < 50 && std::abs(m_player->getPosition().m_z - (*it)->getPosition().m_z) < 50)
		{
			// check collisions between player and the surrounding objects
			if (std::abs(m_player->getPosition().m_x - (*it)->getPosition().m_x) < m_player->getCollisionLimit_x() + (*it)->getCollisionLimit_x()
				&& std::abs(m_player->getPosition().m_z - (*it)->getPosition().m_z) < m_player->getCollisionLimit_z()+ (*it)->getCollisionLimit_z())
			{
				m_player->checkCollision(true);
			}
			// checks wether the player will collide with an enemy in the next update
			if (std::abs(m_player->getNextPosition().m_x - (*it)->getPosition().m_x) < m_player->getCollisionLimit_x() + (*it)->getCollisionLimit_x()
				&& std::abs(m_player->getNextPosition().m_z - (*it)->getPosition().m_z) < m_player->getCollisionLimit_z()+ (*it)->getCollisionLimit_z())
			{
				m_player->willCollide(true);
			}

			for(int j = 0; j< m_bullets.size(); ++j)
			{
				// collision between a bullet shot by the player and an enemy
				// if there is a collision and the life of the enemy is 0 the enemy will be deleted from the object list
				if (m_bullets[j].getParent() == m_player && std::abs(m_bullets[j].getPosition().m_x - (*it)->getPosition().m_x) < m_bullets[j].getCollisionLimit_x() + (*it)->getCollisionLimit_x()
				&& std::abs(m_bullets[j].getPosition().m_z - (*it)->getPosition().m_z) < m_bullets[j].getCollisionLimit_z() + (*it)->getCollisionLimit_z())
				{
					m_bullets[j].active(false);
					(*it)->life((*it)->life()-1);
					if((*it)->life() == 0)
          {
            // delete the enemy
						m_objects.erase(it);
            // restore player health
            m_player->resetLife();
          }
				}
				// collision between any bullet with any gameobject
				else if (m_bullets[j].getParent() != (*it) && std::abs(m_bullets[j].getPosition().m_x - (*it)->getPosition().m_x) < m_bullets[j].getCollisionLimit_x() + (*it)->getCollisionLimit_x()
					&& std::abs(m_bullets[j].getPosition().m_z - (*it)->getPosition().m_z) < m_bullets[j].getCollisionLimit_z() + (*it)->getCollisionLimit_z())
					m_bullets[j].active(false);
			}
		}
	}

	// checks collision between the playar and the walls
	if(wallCollision(m_player, m_player->getPosition()))
		m_player->checkCollision(true);
	if(wallCollision(m_player, m_player->getNextPosition()))
		m_player->willCollide(true);

	// checks collisions between bullets and the walls
	for(int i = 0; i< m_bullets.size(); ++i)
	{
		if(m_bullets[i].active())
		{
			m_bullets[i].updatePosition();
			if(wallCollision(&m_bullets[i], m_bullets[i].getPosition()))
			{
				m_bullets[i].active(false);
			}
      if (dynamic_cast<Enemy*>(m_bullets[i].getParent()) )
			{
				if (std::abs(m_bullets[i].getPosition().m_x - m_player->getPosition().m_x) < m_bullets[i].getCollisionLimit_x() + m_player->getCollisionLimit_x()
				&& std::abs(m_bullets[i].getPosition().m_z - m_player->getPosition().m_z) < m_bullets[i].getCollisionLimit_z()+ m_player->getCollisionLimit_z())
        {
					m_player->life(m_player->life()-1);
          m_bullets[i].active(false);
          if (m_player->life() == 0)
            m_player->active(false);
				}
			}
		}
	}
}


void Level::enemyCanShoot()
{
  std::vector<GameObject*>::iterator it;
  for (it = m_objects.begin(); it != m_objects.end(); ++it)
   {
    if(Enemy * temp = dynamic_cast<Enemy*>(*it))
    {
      temp->canShoot(true);
    }
  }
}
