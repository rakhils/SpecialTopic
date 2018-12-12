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
#include "Game/GamePlay/Entity/InvisibleEnemies.hpp"
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
	bool							m_init							= false;
	bool							gafirstsample					= false;
	bool							m_isDebugMode					= false;
	Camera*							m_camera						= nullptr;
	Texture*						m_textureBackground				= nullptr;
	AABB2							m_cameraQuads;
	AABB2							m_block;
	Vector2							m_dimensions;

	Vector2							m_lastKnownPosition;
	float							m_timeElapsedAfterKnownLocation = 0.f;
	float							m_lastTrainedTime				= 0;
	float							m_miniMapScaleFactor			= 1.f / 5.f;
	bool							m_slowerTraining				= false;
	float							m_traininigPeriod				= 2;
	int								m_generations					= 0;
	std::vector<Brick*>				m_bricks;
	std::vector<Pipe*>				m_pipes;
	std::vector<Pipe*>				m_viewablePipes;
	std::vector<Pit>				m_pits;
	std::vector<InvisibleEnemies*>  m_invisibleEnemies;
	std::vector<Mario*>		        m_marios;
	std::vector<MiniMapObject>		m_minimapObjs;
	std::vector<MiniMapObject>		m_minimapLastPos;

	Mario*							m_mario							= nullptr;
	int							    m_aliveMarioCount				= 0;
	//MINIMAP
	Vector2							m_miniMapPosition;
	AABB2							m_minimapAABB;
	AABB2							m_minimapLastPosAABB;
	int								m_minimapWidth					= 10;
	int								m_minimapHeight					= 10;

	bool							m_bestMode						= false;
	NeuralNetwork*					m_bestNeuralNet					= nullptr;
	float							m_bestFitness					= 0.f;
	float							m_lastBestFitness				= 0.f;
	float							m_lastBestMarioX				= 0.f;
	int								m_lastBestMarioJump				= 0;
	float							m_bestMarioX					= 0.f;
	int								m_bestMarioJump					= 0;

	Vector2							m_cameraMins;
	GeneticAlgorithm *				m_ga							= nullptr;
	GeneticAlgorithm *				m_gaString						= nullptr;

	Map(MapDefinition* def);

	void							InitCamera();
	void							InitGA();
	
	void							CreatePit(Vector2 position);
	void							CreateBricks(Vector2 position,Vector2 dimension,bool hidden,bool physics);
	void							CreatePipes(Vector2 position, Vector2 dimensions,bool isViewable);
	void							CreateGround();
	void							CreateCharacters();
	void							CreateMario();
	void							CreateInvisibleEnemies(Vector2 position, float radius);

	void							GetBestMario();
	void							Update(float deltaTime);
	void							UpdateCamera();
	void							UpdateBrick(float deltaTime);
	void							UpdatePipes(float deltaTime);
	void							UpdateMarios(float deltaTime);
	void							AddPipe(Vector2 mousePos);

	void							CheckForMarioOutOfBounds();
	void							QueryAndDie(float deltaTime);
	void							EndOnePlay();
	bool							IsAllDead();
	float							CalculateMaxFitness();
	float							CalculateMinFitness();
	float							CalculateFitnessSum();
	void							PickMostSuccessfullMario();
	void							SortMariosByFitnessSaveBest();
	Mario*							PickRandomMarioUsingFitness();

	void							Render();
	void							RenderMario();
	void							RenderBricks();
	void							RenderPipes();
	void							RenderGrid();
	void							RenderInvisibleEnemies();
	void							RenderNN();
};
