#ifndef LEVEL_H
#define LEVEL_H

#include <iostream>
#include <vector>
#include <string>
#include <SDL.h>
#include <math.h>
#include "GLFunctions.h"
#include "utilityfunctions.h"
#include "player.h"
#include "obstacle.h"
#include <typeinfo>

#ifdef __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

class Level
{
private:
	int m_mapWidth;
	int m_mapHeight;
	int m_ObstacleNumber;
	float m_cellSize;
	Vec4 m_position;
	Player *m_player;
	std::vector<std::vector<char>> m_map;
	std::vector<Obstacle> m_obstacles;
	void generateMap();
	void loadMap();
public:
	Level(int _height, int _width, int _obstacles, Player* _player);
	Level(std::string _address, Player* _player);
	void draw() const;
	bool wallCollision(GameObject * _gameObject);
};

#endif // LEVEL_H