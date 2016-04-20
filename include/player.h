#ifndef PLAYER_H
#define PLAYER_H

#include <array>
#include <vector>
#include <string>
#include <SDL.h>
#include <iostream>
#include "gameobject.h"
#include "GLFunctions.h"
#include <math.h>
#include "utilityfunctions.h"
#include "mesh.h"


class Player : public GameObject
{
private:
	Vec4 m_lastActiveRotation;
	Vec4 m_nextPosition;
	float m_currentSpeed;
	int m_life;
	// 0 if the button is currently not pressed, 1 otherwise
  std::array <char,4> m_keyPressed;
  std::array <char,4> m_previousKeyPressed;
	Mesh * m_playerMesh;
	float m_size;
  bool m_shoot;
	bool m_willCollide;

public:
	Player(Vec4 const _position, Vec4 const _rotation, float _speed, bool _active, int _life, Mesh *_playerMesh);
	~Player(){}
	void updatePosition();
	void updateRotation();
	void checkCollision(bool _collided);
	void draw() const;
	void shoot(bool _shoot) { m_shoot = _shoot; }
	bool didShoot() const { return m_shoot; }
	//int life() const {}
	//void life(int const _life) {}
	void input(SDL_Event & _event);
	float getCollisionLimitX() const { return  m_collisionLimit_x; }
	float getCollisionLimitZ() const { return  m_collisionLimit_z; }
	Vec4 getNextPosition() const { return m_nextPosition; }
	void willCollide(bool _willCollide) { m_willCollide = _willCollide; }
};

#endif // PLAYER_H
