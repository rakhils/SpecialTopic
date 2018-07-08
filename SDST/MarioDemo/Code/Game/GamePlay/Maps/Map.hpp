#pragma once
#include <string>
#include <map>
#include <vector>

#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"

#include "Game/GamePlay/Tiles/Tiles.hpp"
#include "Game/GamePlay/Maps/MapDefinition.hpp"
//#include "Game/GamePlay/MapGenSteps.hpp"
#include "Game/GamePlay/Entity/Brick.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Entity/Mario.hpp"
class GeneticAlgorithm;
struct MiniMapObject
{
	Rgba  m_color;
	float m_value;
};
struct Pit
{
	Vector2 m_position;
	Rgba    m_color;
};
class Pipe;
class Map
{
public:
	bool      m_init		= false;
	bool      gafirstsample = false;
	bool	  m_isDebugMode = false;
	Camera*   m_camera;
	AABB2     m_cameraQuads;
	AABB2     m_block;
	Texture*  m_textureBackground;
	Vector2   m_dimensions;
	Vector2   m_miniMapPosition;

	Vector2   m_lastKnownPosition;
	float     m_timeElapsedAfterKnownLocation;
	float     m_lastTrainedTime = 0;
	float     m_miniMapScaleFactor = 1 / 5.f;
	bool	  m_slowerTraining = false;
	float     m_traininigPeriod = 0;
	std::vector<Brick*> m_bricks;
	std::vector<Pipe*>  m_pipes;
	std::vector<Pit>    m_pits;

	Mario *m_mario;
	//MINIMAP
	AABB2					   m_minimapAABB;
	std::vector<MiniMapObject> m_minimapObjs;
	AABB2					   m_minimapLastPosAABB;
	std::vector<MiniMapObject> m_minimapLastPos;
	int m_minimapWidth  = 10;
	int m_minimapHeight = 10;
	GeneticAlgorithm *m_ga = nullptr;
	GeneticAlgorithm *m_gaString = nullptr;
	Map(MapDefinition* def);

	void InitCamera();
	void InitMiniMap();
	void InitGA();
	void SetMiniMapValues(std::vector<MiniMapObject> &m_minimap,IntVector2 pos, Rgba color);
	std::vector<float>& GetInputsFromMiniMap();
	
	void CreatePit(Vector2 position);
	void CreateBricks(Vector2 position,Vector2 dimension,bool hidden,bool physics);
	void CreatePipes(Vector2 position, Vector2 dimensions);
	void CreateGround();
	void CreateCharacters();
	void CreateMario();
	void InitScore();
	void InitTime();

	void Update(float deltaTime);
	void UpdateCamera();
	void UpdateMiniMap(float deltaTime);
	void UpdateBrick(float deltaTime);
	void UpdatePipes(float deltaTime);

	void TrainNN(bool isDead);

	void CheckForMarioOutOfBounds();
	void QueryAndDie(float deltaTime);
	void EndOnePlay();

	void Render();
	void RenderMiniMap(AABB2 aabbPos,std::vector<MiniMapObject>& minimap);
	void RenderMario();
	void RenderBricks();
	void RenderPipes();
	void RenderGrid();
	void RenderNN();
};
