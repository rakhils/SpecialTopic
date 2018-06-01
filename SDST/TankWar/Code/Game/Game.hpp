#pragma once

#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleEmitter.hpp"

#include "Game/GamePlay/Maps/MapDefinition.hpp"
#include "Game/GamePlay/Tiles/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Maps/Map.hpp"

struct ObjectProperties
{
	Vector4 m_tint;

};
class Bullet;
class Asteroid;
class DebugDraw;
//class GameObject;
class ForwardRenderingPath;
class Renderable;
class Scene;
class Ship;
class Game
{
public:

	Map * m_map = nullptr;
	Mesh* m_cube = nullptr;
	bool  m_init = false;
	float m_angle = 0.f;
	int	  m_lightSelected = 0;
	float m_enable2D = false;
	float m_yValue = 0.f;
	float m_yValueDirection = 1.f;
	bool  isQuitTriggered = false;
	bool  m_isDebugDrawn = false;
	bool  m_isUsingNormalMap = false;
	int   m_lightIndex = 0;
	float m_renderMode = 1;
	float m_shipVelocity = 15;
	/*float m_specFactor = 1;
	float m_specPower = 1;*/
	Light *m_light = nullptr;

	Ship				   *m_ship;
	GameObject			   *m_testObject;
	GameObject			   *m_test1Object;
	GameObject			   *m_lightObj;
	
	GameObject			   *m_cameraObject;
	Ship					*m_enemyShip;
	int					   m_score = 0;
	std::vector<Bullet*>   m_bullets;
	std::vector<Bullet*>   m_enemyBullet;
	std::vector<Asteroid*> m_asteroids;
	Scene				   *m_scene;
	ForwardRenderingPath   *m_forwardRenderingPath = nullptr;
	float				   m_lastBulletSpawnTime;
	float				   m_bulletSpawnInterval = 0.25f;
	float				   m_lastEnemyBulletSpawnTime;
	//float m_innerAngle = 30;
	//float m_outerAngle = 90;

	bool m_cameraBodyVisible = false;

	Game();
	void Init();
	void CreateShip();
	void SetCameraFocusOnPosition();
	void CreateBullet(Vector3 position, Vector3 forward,bool isEnemy);
	void CreateAsteroid(Vector3 position,Vector3 forward,float radius);
	void Update(float deltaTime);
	
	static void OnCollisionEnter(Collider *collider);

	void Render();
	void RenderBullet();
	void RenderEnemyBullet();
	void RenderAsteroids();
	void Render1();
	void RenderDebug();	
};