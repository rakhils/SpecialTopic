#include "Map.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/Physics/Collider/BoxCollider2D.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/AI/GA/StringGeneticAlgorithm.hpp"
#include "Engine/AI/GA/Abstract/GeneticAlgorithm.hpp"
#include "Engine/AI/GA/Chromosome.hpp"
#include "Engine/AI/GA/Gene.hpp"
#include "Engine/AI/GA/SimpleCharGene.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetGA.hpp"
#include "Engine/FileUtil/File.h"
#include "Engine/Core/StringUtils.hpp"

//#include "Engine/AI/NeuralNetwork/NeuralNetworkConstants.h"
#include "Engine/AI/NeuralNetwork/NeuralNetwork.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetworkGene.hpp"
#include "Engine/AI/GA/Gene.hpp"
#include "Game/GamePlay/Entity/Pipe.hpp"
#include "Game/GamePlay/Entity/Mario.hpp"
#include "Game/GameCommon.hpp"

Map::Map(MapDefinition *mapDef)
{
	if(g_controlMode)
	{
		m_bestMode = true;
	}
	m_dimensions.x = mapDef->m_width;
	m_dimensions.y = mapDef->m_heigth;
	for(int index = 0;index < mapDef->m_genSteps.size();index++)
	{
		mapDef->m_genSteps.at(index)->Run(*this);
	}
	CreateCharacters();
	InitMiniMap();
	InitGA();
	std::string fitness = GetFileContentAsString(g_marioFitnessFilePath.c_str());
	ToFloat(fitness, &m_bestFitness);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitCamera()
{
	int width  = Windows::GetInstance()->GetDimensions().x;
	int height = Windows::GetInstance()->GetDimensions().y;
	m_block = AABB2(Vector2::ZERO, 30, 30);
	m_camera = new OrthographicCamera();
	FrameBuffer *frameBuffer = new FrameBuffer();
	m_camera->m_defaultFrameBuffer = frameBuffer;
	m_camera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	m_camera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	m_camera->m_transform.SetLocalPosition(Vector3(static_cast<float>(width / 2), static_cast<float>(height / 2), 0));
	Camera::SetGameplayCamera(m_camera);
	Camera::SetCurrentCamera(m_camera);
	((OrthographicCamera*)m_camera)->ZoomIn(Vector2(300,0));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/20
*@purpose : Inits the minimap
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitMiniMap()
{
	m_minimapAABB = AABB2(Vector2(0, 0), 50, 50);
	m_minimapLastPosAABB = AABB2(Vector2(0, 0), 50, 50);
	for (int indexY = 0; indexY < m_minimapHeight; indexY++)
	{
		for (int indexX = 0; indexX < m_minimapWidth; indexX++)
		{
			MiniMapObject1 obj;
			obj.m_color = Rgba::BLACK;
			obj.m_value = 0.f;
			m_minimapObjs.push_back(obj);
			m_minimapLastPos.push_back(obj);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitGA()
{
	SetRandomSRAND();
	std::string target    = "hello world !!!";
	int   geneCount		  = 1;
	float mutationChance  = 0.01f;
	float crossOverChance = 1.f;
	int   totalPopulation   = 1;

	/*std::vector<Gene*> inputs;
	for(int index = 0;index < g_initialMarioCount;index++)
	{
		Gene* gene = new NeuralNetworkGene(0.01f);
		((NeuralNetworkGene*)gene)->m_neuralNet = m_mario->m_neuralNet;
		inputs.push_back(gene);
	}
	
	
	m_ga				  = new NeuralNetGA(totalPopulation, geneCount,crossOverChance, mutationChance,inputs);*/

	

	geneCount = 1;
	mutationChance = 0.01f;
	crossOverChance = 1.f;
	int totalGeneCountForString = target.length();
	int totalPopulationForString = 500;



	m_gaString = new StringGeneticAlgorithm(totalPopulationForString, totalGeneCountForString, crossOverChance, mutationChance);
	Chromosome *ch  = new Chromosome(totalGeneCountForString, mutationChance,SIMPLE_CHAR);
	for(int index = 0;index < target.length();index++)
	{
		Gene *gene = new SimpleCharGene(mutationChance);
		((SimpleCharGene*)gene)->m_char = target[index];
		ch->m_genes.at(index) = (gene);
	}
	m_gaString->SetTarget(ch);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreatePit(Vector2 position)
{
	Pit nPit;
	nPit.m_position = position;
	nPit.m_color    = Rgba::RED;
	m_pits.push_back(nPit);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateBricks(Vector2 position,Vector2 dimension,bool hidden,bool physics)
{
	int size = m_bricks.size();
	std::string name = ("brick_"+ToString(size));

	Brick *brick      = new Brick(name,EntityDefinition::GetDefinition("Brick"));
	brick->m_length   = dimension.x;
	brick->m_height   = dimension.y;
	brick->m_isHidden = hidden;
	brick->SetPosition(position);
	if(physics)
	{
		brick->AddBoxCollider2D(Vector3(0,0,0), Vector2(dimension.x/2.f, dimension.y/2.f), Vector3::FORWARD);
		brick->GetBoxCollider2D()->m_isStatic = true;
	}
	m_bricks.push_back(brick);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreatePipes(Vector2 position, Vector2 dimensions,bool isViewable)
{
	int size = m_pipes.size();
	std::string name = ("pipe_" + ToString(size));

	Pipe *pipe = new Pipe(name, EntityDefinition::GetDefinition("Pipe"));
	pipe->SetPosition(position);
	pipe->m_length = dimensions.x;
	pipe->m_height = dimensions.y;
	pipe->AddBoxCollider2D(Vector3(0, 0, 0), dimensions/2.f, Vector3::FORWARD);
	pipe->GetBoxCollider2D()->m_isStatic = true;
	if(isViewable)
	{
		m_viewablePipes.push_back(pipe);
	}
	m_pipes.push_back(pipe);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateGround()
{
	float stepSize = m_block.GetDimensions().x/2.f;
	for(int posX = stepSize/2.f;posX<7000;posX += stepSize)
	{
		Vector2 position(posX, 40);
		CreateBricks(position,m_block.GetDimensions(),true,false);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/24
*@purpose : Creats abd set all characters
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Map::CreateCharacters()
{
	InitCamera();
	CreateGround();
	CreateMario();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateMario()
{
	for (int index = 0; index < g_initialMarioCount; index++)
	{
		Mario *mario = new Mario(EntityDefinition::GetDefinition("Mario"));
		mario->SetPosition(Vector2(200, 400));
		//mario->AddRigidBody3DComponent();
		mario->m_length = mario->m_definition->m_length;
		mario->m_height = mario->m_definition->m_height;
		Vector2 colliderPosition(0, -mario->m_definition->m_height / 2 + mario->m_definition->m_physicsRadius);
		mario->AddCircleCollider(Vector3(colliderPosition, 0), mario->m_definition->m_physicsRadius);
		//((RigidBody3D*)mario->GetComponentByType(RIGID_BODY_3D))->m_friction = 0.8f;
		//((RigidBody3D*)mario->GetComponentByType(RIGID_BODY_3D))->m_useGravity = true;
		//((RigidBody3D*)mario->GetComponentByType(RIGID_BODY_3D))->m_gravity = Vector3(0, -10, 0);
		mario->m_map = this;
		m_marios.push_back(mario);
	}
	m_mario = m_marios.at(0);
	m_bestNeuralNet = new NeuralNetwork(m_mario->m_neuralNet->GetNumberOfInputs(), m_mario->m_neuralNet->GetNumberOfHiddenLayerNeuron(), m_mario->m_neuralNet->GetNumberOfOutputs());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/11
*@purpose : Create enemies
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateInvisibleEnemies(Vector2 position, float radius)
{
	std::string name = "InvisibleEnemy";
	name.append(ToString(m_invisibleEnemies.size()));
	InvisibleEnemies *enemy = new InvisibleEnemies(name,position,radius);
	m_invisibleEnemies.push_back(enemy);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::GetBestMario()
{
	m_mario->m_neuralNet->LoadFromFile(g_neuralNetFilePath.c_str());
}

void Map::Update(float deltaTime)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// NEURAL NET 
	


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ========== GA ============
	/*std::vector<float> inputs;
	for (int index = 0; index < m_minimapObjs.size(); index++)
	{
		inputs.push_back(m_minimapObjs.at(index).m_value);
	}
	((NeuralNetGA*)m_ga)->UpdateWithInputs(inputs);
	m_gaString->Epoch();
	
	Chromosome *chr = m_gaString->m_best;
	std::string temp;
	for(int index = 0;index < chr->m_genes.size();index++)
	{
		std::string ch(1,((SimpleCharGene*)(chr->m_genes.at(index)))->m_char);
		temp.append(ch);
	}
	//gafirstsample = true;
	DebugDraw::GetInstance()->DebugRenderLogf("Generation Count : %d", m_gaString->m_currentGenerationCount);
	DebugDraw::GetInstance()->DebugRenderLogf("TEXT			    : %s", temp.c_str());
	DebugDraw::GetInstance()->DebugRenderLogf("Best Fitness     : %f", m_gaString->m_best->GetTotalFitness(m_gaString->m_target));
	DebugDraw::GetInstance()->DebugRenderLogf("Avg Fitness      : %f", m_gaString->m_averageFitnessValue);
	DebugDraw::GetInstance()->DebugRenderLogf("Mutation Rate    : %f", m_gaString->m_mutationChance);
	DebugDraw::GetInstance()->DebugRenderLogf("=================================================", "");*/
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_textureBackground = Texture::CreateOrGetTexture("Data//Images//level1.png", true, false);

	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_N))
	{
		m_isDebugMode = m_isDebugMode ? false : true;
	}
	if(InputSystem::GetInstance()->WasLButtonJustPressed())
	{
		Vector2 mousePos = InputSystem::GetInstance()->GetMouseClientPosition();
		AddPipe(mousePos);
	}
	if (g_theInput->wasKeyJustReleased(InputSystem::KEYBOARD_Q))
	{
		EndOnePlay();
	}
	if(InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_Z))
	{
		((OrthographicCamera*)m_camera)->ZoomIn(Vector2(1000*deltaTime,1000*deltaTime));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_X))
	{
		((OrthographicCamera*)m_camera)->ZoomOut(Vector2(1000*deltaTime,1000*deltaTime));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_A))
	{
		((OrthographicCamera*)m_camera)->MoveLeft(deltaTime * 1200);
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_S))
	{
		((OrthographicCamera*)m_camera)->MoveDown(deltaTime*1200);
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_D))
	{
		((OrthographicCamera*)m_camera)->MoveRigth(deltaTime*1200);
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_W))
	{
		((OrthographicCamera*)m_camera)->MoveUp(deltaTime*1200);
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_R))
	{
		Vector3 position = m_mario->GetWorldPosition();
		m_mario->m_transform.SetLocalPosition(Vector3(position.x, 450, 0));
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_B))
	{
		m_bestMode = m_bestMode ? false : true;
		GetBestMario();
	}
	
	//m_mario->Update(deltaTime);
	UpdateCamera();
	UpdateMarios(deltaTime);
	if(IsAllDead() && !m_bestMode)
	{
		PickMostSuccessfulMario();
	}
	//UpdateMiniMap(deltaTime);
	//QueryAndDie(deltaTime);
	//CheckForMarioOutOfBounds();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateCamera()
{
	m_camera->SetOrthoProjection();
	if (!m_bestMode)
	{
		return;
	}
	float minX = Windows::GetInstance()->GetDimensions().x / 2.f;
	float minY = Windows::GetInstance()->GetDimensions().y / 2.f;

	float maxX = m_textureBackground->getDimensions().x - Windows::GetInstance()->GetDimensions().x / 2.f;
	float maxY = Windows::GetInstance()->GetDimensions().y / 2.f;

	float cameraX = m_mario->m_transform.GetWorldPosition().x;
	float cameraY = 0;

	cameraX = ClampFloat(cameraX, minX, maxX);
	cameraY = ClampFloat(cameraY, minY, maxY);
	m_cameraMins = Vector2(cameraX - minX , minY);
	m_camera->m_transform.SetLocalPosition(Vector3(cameraX, cameraY, 0));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateMiniMap(float deltaTime)
{
	float width   = static_cast<float>(Windows::GetInstance()->GetDimensions().x);
	float height  = static_cast<float>(Windows::GetInstance()->GetDimensions().y);

	Vector2 marioWorldPosition    = m_mario->m_transform.GetWorldPosition().GetXY();
	m_miniMapPosition.x			  = m_camera->m_transform.GetWorldPosition().x + width  / 2   - m_minimapAABB.GetDimensions().x*2;
	m_miniMapPosition.y			  = m_camera->m_transform.GetWorldPosition().y + height / 2   - m_minimapAABB.GetDimensions().y*2;
	m_miniMapPosition.y += 100;
	m_minimapAABB				  = AABB2(m_miniMapPosition, m_minimapAABB.GetDimensions().x/2.f, m_minimapAABB.GetDimensions().y/2.f);
	m_minimapLastPosAABB		  = AABB2(m_miniMapPosition + Vector2(0,-150), m_minimapAABB.GetDimensions().x/2.f , m_minimapAABB.GetDimensions().y/2.f);

	// SQUARE CAN TAKE X OR Y
	Vector2 minBounds			  = marioWorldPosition - Vector2::ONE * (m_block.GetDimensions().x/2.f) * (m_minimapWidth / 2.f);
	Vector2 maxBounds			  = marioWorldPosition + Vector2::ONE * (m_block.GetDimensions().x/2.f) * (m_minimapHeight/ 2.f);

	// MAKE ALL BLACK IN MINIMAP
	for(int index = 0;index < m_minimapObjs.size();index++)
	{
		m_minimapObjs.at(index).m_color = Rgba::CONSOLE_FADED_BLUE;
		m_minimapObjs.at(index).m_value = 0.0f;
		/*if (m_mario->IsGrounded(deltaTime))
		{
			m_minimapLastPos.at(index).m_color = Rgba::CONSOLE_FADED_BLUE;
			m_minimapLastPos.at(index).m_value = 0.75f;
		}*/
	}

	for(int index = 0;index < m_bricks.size();index++)
	{
		Vector2 brickPosition = m_bricks.at(index)->m_transform.GetWorldPosition().GetXY();
		if(brickPosition.x > minBounds.x && brickPosition.y > minBounds.y)
		{
			int relativeX = RoundToNearestInt((brickPosition.x - minBounds.x) / (m_block.GetDimensions().x/2.f));
			int relativeY = RoundToNearestInt((brickPosition.y - minBounds.y) / (m_block.GetDimensions().x/2.f));
			relativeY++;
			if(brickPosition.x < maxBounds.x && brickPosition.y < maxBounds.y)
			{	
				SetMiniMapValues(m_minimapObjs,IntVector2(relativeX, relativeY), Rgba::WHITE);
				if (m_mario->IsGrounded(deltaTime))
				{
					SetMiniMapValues(m_minimapLastPos, IntVector2(relativeX, relativeY), Rgba::WHITE);
				}
				//DebugDraw::GetInstance()->DebugRenderLogf("RELATIVE %d,%d", relativeX, relativeY);
			}
		}
	}	
	for (int index = 0; index < m_pits.size(); index++)
	{
		Vector2 pitPosition = m_pits.at(index).m_position;
		if (pitPosition.x > minBounds.x && pitPosition.y > minBounds.y)
		{
			int relativeX = RoundToNearestInt((pitPosition.x - minBounds.x) / (m_block.GetDimensions().x / 2.f));
			int relativeY = RoundToNearestInt((pitPosition.y - minBounds.y) / (m_block.GetDimensions().x / 2.f));
			relativeY++;
			if (pitPosition.x < maxBounds.x && pitPosition.y < maxBounds.y)
			{
				SetMiniMapValues(m_minimapObjs,IntVector2(relativeX, relativeY), m_pits.at(index).m_color);
				if (m_mario->IsGrounded(deltaTime))
				{
					SetMiniMapValues(m_minimapLastPos, IntVector2(relativeX, relativeY), m_pits.at(index).m_color);
				}
				//DebugDraw::GetInstance()->DebugRenderLogf("RELATIVE PIT %d,%d", relativeX, relativeY);
			}
		}
	}
	int positionX = static_cast<int>((marioWorldPosition.x - minBounds.x) / (m_block.GetDimensions().x / 2.f));
	int positionY = static_cast<int>((marioWorldPosition.y - minBounds.y) / (m_block.GetDimensions().x / 2.f));
	//DebugDraw::GetInstance()->DebugRenderLogf("RELATIVE MARIO %d,%d", positionX, positionY);
	SetMiniMapValues(m_minimapObjs,IntVector2(positionX, positionY), Rgba::GREEN);

	if(m_mario->IsGrounded(deltaTime))
	{
		SetMiniMapValues(m_minimapLastPos, IntVector2(positionX, positionY), Rgba::GREEN);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::SetMiniMapValues(std::vector<MiniMapObject1>&minimap, IntVector2 pos, Rgba color)
{
	int index = static_cast<int>(pos.y)*m_minimapHeight + static_cast<int>(pos.x);
	if(index < minimap.size())
	{
		minimap.at(index).m_color = color;
		if(color == Rgba::WHITE)
		{
			minimap.at(index).m_value = 0.75f;
		}
		else
		if (color == Rgba::RED)
		{
			minimap.at(index).m_value = 1;
		}
		else
		if (color == Rgba::GREEN)
		{
			minimap.at(index).m_value = -0.50;
		}
		else
		{
			minimap.at(index).m_value = 0.0;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : Gets float value from minimap
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<float>& Map::GetInputsFromMiniMap()
{
	std::vector<float> inputs;
	for(int index = 0;index < m_minimapObjs.size();index++)
	{
		inputs.push_back(m_minimapObjs.at(index).m_value);
	}
	return inputs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateBrick(float deltaTime)
{
	for(Brick *brick : m_bricks)
	{
		brick->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdatePipes(float deltaTime)
{
	for (Pipe *pipe : m_pipes)
	{
		pipe->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateMarios(float deltaTime)
{
	if(m_bestMode)
	{
		m_mario->Update(deltaTime);
		if(m_mario->m_isDead)
		{
			m_mario->ResetPosition();
		}
		return;
	}
	m_aliveMarioCount = 0;
	for(int index = 0;index < m_marios.size();index++)
	{
		if(!m_marios.at(index)->m_isDead)
		{
			m_aliveMarioCount++;
			m_marios.at(index)->Update(deltaTime);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/20
*@purpose : Updates NN
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::TrainNN(bool isDead)
{
	if(true)
	{
		m_mario->m_neuralNet->Mutate();
		return;
	}
	std::vector<float> knowOutputs;
	knowOutputs.push_back(0);
	knowOutputs.push_back(0);

	std::vector<float> inputs;
	for (int index = 0; index < m_minimapLastPos.size(); index++)
	{
		inputs.push_back(m_minimapLastPos.at(index).m_value);
	}
	((NeuralNetGA*)m_ga)->UpdateWithInputs(inputs);
	std::vector<float> NNOutputs;
	for (int index = 0; index < m_mario->m_neuralNet->m_outputs->m_neurons.size(); index++)
	{
		float value = m_mario->m_neuralNet->m_outputs->m_neurons.at(index).m_value;
		NNOutputs.push_back(value);
	}
	bool wasMovingRight = NNOutputs.at(0) > 0.5f ? true : false;
	bool wasJumping		= NNOutputs.at(1) > 0.5f ? true : false;

	if(isDead)
	{
		if(m_mario->m_deathType == DEATH_PIT)
		{
			//is Jumping
			if(wasJumping)
			{
				knowOutputs.at(1) = 0.f;
			}
			if(!wasJumping)
			{
				knowOutputs.at(1) = 1.f;
			}
		}
		if (m_mario->m_deathType == DEATH_IDLE)
		{
			if (!wasJumping)
			{
				knowOutputs.at(1) = 1.f;
			}
			if (wasJumping)
			{
				//knowOutputs.at(1) = 0.f;
			}
			if(!wasMovingRight)
			{
				knowOutputs.at(0) = 1.f;
			}
		}

	}
	else
	{
		return;
	}
	/*else
	{
		if (wasJumping)
		{
			knowOutputs.at(1) = 0.f;
		}
		if (!wasMovingRight)
		{
			knowOutputs.at(0) = 1.f;
		}
	}
	if (wasJumping)
	{
		//knowOutputs.at(1) = 0.f;
	}*/
	//knowOutputs.at(0) = 1.f;
	//DebugDraw::GetInstance()->DebugRenderLogf(1,"IS DEAD %s RIGHT %s LEFT %s JUMP %s", ToString(isDead), ToString(wasMovingRight), ToString(wasMovingLeft), ToString(wasJumping));
	m_mario->m_neuralNet->DoBackPropogation(knowOutputs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::AddPipe(Vector2 MousePos)
{
	if(true)
	{
		return;
	}
	/*	<Brick actor = "Pipe"  position = "1500,120"  dimension = "60,120" / >
		<Brick actor = "Brick" position = "1485,75"  dimension = "30,30" physics = "false" / >
		<Brick actor = "Brick" position = "1485,105"  dimension = "30,30" physics = "false" / >
		<Brick actor = "Brick" position = "1485,135"  dimension = "30,30" physics = "false" / >
		<Brick actor = "Brick" position = "1485,165"  dimension = "30,30" physics = "false" / >
		<Brick actor = "Brick" position = "1515,75"  dimension = "30,30" physics = "false" / >
		<Brick actor = "Brick" position = "1515,105"  dimension = "30,30" physics = "false" / >
		<Brick actor = "Brick" position = "1515,135"  dimension = "30,30" physics = "false" / >
		<Brick actor = "Brick" position = "1515,165"  dimension = "30,30" physics = "false" / >*/
	Vector2 pos(m_cameraMins.x + MousePos.x, 120);
	Vector2 centre(pos.x , pos.y);
	CreatePipes(centre, Vector2(64, 128),true);

	pos.y = 115 - 32;
	Vector2 brickPos1(pos.x - 16, pos.y + 16);
	Vector2 brickPos2(pos.x + 16, pos.y + 16);

	Vector2 brickPos3(pos.x - 16, pos.y + 32);
	Vector2 brickPos4(pos.x + 16, pos.y + 32);

	Vector2 brickPos5(pos.x - 16, pos.y + 48);
	Vector2 brickPos6(pos.x + 16, pos.y + 48);

	Vector2 brickPos7(pos.x - 16, pos.y + 90);
	Vector2 brickPos8(pos.x + 16, pos.y + 90);

	CreateBricks(brickPos1, Vector2(32, 32), true, true);
	CreateBricks(brickPos2, Vector2(32, 32), true, true);
	CreateBricks(brickPos3, Vector2(32, 32), true, true);
	CreateBricks(brickPos4, Vector2(32, 32), true, true);
	CreateBricks(brickPos5, Vector2(32, 32), true, true);
	CreateBricks(brickPos6, Vector2(32, 32), true, true);
	CreateBricks(brickPos7, Vector2(32, 32), true, true);
	CreateBricks(brickPos8, Vector2(32, 32), true, true);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/20
*@purpose : Checks if mario is out of bounds and respawn
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CheckForMarioOutOfBounds()
{
	if (m_mario->m_transform.GetWorldPosition().y < 0)
	{
		m_mario->m_deathType = DEATH_PIT;
		EndOnePlay();
	}
	if (m_mario->m_transform.GetWorldPosition().x > 6700)
	{
		m_mario->m_deathType = NONE;
		EndOnePlay();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/20
*@purpose : Checks condition and Let mario die
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::QueryAndDie(float deltaTime)
{
	m_timeElapsedAfterKnownLocation += deltaTime;
	if(m_timeElapsedAfterKnownLocation > 2)
	{
		Vector2 distance = (m_mario->m_transform.GetWorldPosition().GetXY() - m_lastKnownPosition);
		if(distance.GetLength() < m_block.GetDimensions().x * 2)
		{
			m_mario->m_deathType = DEATH_IDLE;
			EndOnePlay();
		}
		m_lastKnownPosition = m_mario->m_transform.GetWorldPosition().GetXY();
		m_timeElapsedAfterKnownLocation = 0.f;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/20
*@purpose : Ends the play and make AI learn from results
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::EndOnePlay()
{
	//DebugDraw::GetInstance()->DebugRenderLogf(1, "END ONE PLAY ");
	TrainNN(true);
	SetRandomSRAND();
	m_generations++;
	//m_mario->ResetWeight();
	m_mario->m_transform.SetLocalPosition(Vector3(200, 150, 0));
	m_lastTrainedTime = 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsAllDead()
{
	bool allDead = true;
	for(int index = 0;index < m_marios.size();index++)
	{
		if(!m_marios.at(index)->m_isDead)
		{
			allDead = false;
		}
	}
	return allDead;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Map::CalculateMaxFitness()
{
	float max = -1;
	for (int index = 0; index < m_marios.size(); index++)
	{
		float fitness = m_marios.at(index)->m_fitness;
		if(fitness > max)
		{
			max = fitness;
		}
	}
	return max;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Map::CalculateFitnessSum()
{
	float total = 0;
	for (int index = 0; index < m_marios.size(); index++)
	{
		total += m_marios.at(index)->m_fitness;
	}
	return total;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : Picks the best marios copy and mutate
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::PickMostSuccessfulMario()
{
	std::vector<Mario*> successFulMarios;
	SortMariosInOrderOfFitness();
	for(int index = 0;index < m_marios.size();index++)
	{
		Mario *mario = PickRandomMarioUsingFitness();
		successFulMarios.push_back(mario);
	}
	for (int index = 0; index < m_marios.size(); index++)
	{
		Mario *mario = m_marios.at(index);
		successFulMarios.at(index)->m_neuralNet->CopyWeightsTo(*(mario->m_neuralNet));
		mario->m_neuralNet->Mutate();
		
		m_marios.at(index)->ResetPosition();
		
	}
	m_generations++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::SortMariosInOrderOfFitness()
{
	for (int index = 0; index < m_marios.size(); index++)
	{
		for (int index1 = index + 1; index1 < m_marios.size(); index1++)
		{
			if (m_marios.at(index)->m_fitness < m_marios.at(index1)->m_fitness)
			{
				Mario *mario = m_marios.at(index);
				m_marios.at(index) = m_marios.at(index1);
				m_marios.at(index1) = mario;
			}
		}
	}
	m_lastBestFitness   = m_marios.at(0)->m_fitness;
	m_lastBestMarioJump = m_marios.at(0)->m_numOfJumps;
	m_lastBestMarioX    = m_marios.at(0)->m_transform.GetWorldPosition().x;
	if(m_marios.at(0)->m_fitness > m_bestFitness)
	{
		m_bestFitness   = m_marios.at(0)->m_fitness;
		m_bestMarioJump = m_marios.at(0)->m_numOfJumps;
		m_bestMarioX    = m_marios.at(0)->m_transform.GetWorldPosition().x;
		m_marios.at(0)->m_neuralNet->CopyWeightsTo(*m_bestNeuralNet);
		m_marios.at(0)->m_neuralNet->StoreToFile(g_neuralNetFilePath.c_str());
		FILE *fp = FileOpenForWrite(g_marioFitnessFilePath);
		FileAppendString(fp, ToString(m_bestFitness));
		FileClose(fp);
	}
	m_mario = m_marios.at(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Mario* Map::PickRandomMarioUsingFitness()
{
	int   maxIteration	   = 100;
	int   currentIteration = 0;
	float fitnessSum	   = CalculateFitnessSum();
	//float randomNum		   = GetRandomFloatInRange(0,fitnessSum);
	int randomNum = GetRandomIntInRange(0, g_maxMarioCountedForNextGen);
	/*int  index			 = 0;
	while(randomNum > 0)
	{
		for (index = 0; index < m_marios.size(); index++)
		{
			randomNum -= m_marios.at(index)->m_fitness;
			if(randomNum <= 0)
			{
				break;
			}
		}
	}*/

	return m_marios.at(randomNum);

	

	/*int num = ClampInt(g_maxMarioCountedForNextGen,0, static_cast<int>(m_marios.size() - 1));
	return m_marios.at(num);*/
	/*while (true)
	{
		int index = GetRandomIntLessThan(static_cast<int>(m_marios.size()));
		Mario *mario = m_marios.at(index);
		int r = GetRandomIntLessThan(CalculateMaxFitness());

		if (static_cast<float>(r) < CalculateFitnessSum())
		{
			return mario;
		}
		currentIteration++;
		if (currentIteration > maxIteration)
		{
			break;
		}
	}*/
	
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/22
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Map::Render()
{
	DebugDraw::GetInstance()->DebugRenderLogf("GENERATION COUNT %d", m_generations);
	DebugDraw::GetInstance()->DebugRenderLogf("ALIVE COUNT %d", m_aliveMarioCount);
	DebugDraw::GetInstance()->DebugRenderLogf("BEST FITNESS %f BEST JUMP %d BEST X %f", m_bestFitness,m_bestMarioJump,m_bestMarioX);
	DebugDraw::GetInstance()->DebugRenderLogf("LAST :: BEST FITNESS %f BEST JUMP %d BEST X %f", m_lastBestFitness, m_lastBestMarioJump, m_lastBestMarioX);

	if(g_controlMode)
	{
		Vector2 mariopos = m_mario->m_transform.GetWorldPosition().GetXY();
		DebugDraw::GetInstance()->DebugRenderLogf("MARIO POS %f, %f", mariopos.x, mariopos.y);
	}
	Camera::SetCurrentCamera(m_camera);
	Renderer::GetInstance()->BeginFrame();

	if(m_isDebugMode)
	{
		RenderNN();
		return;
	}


	AABB2 aabb(Vector2(0, 0), Vector2(6784, 1080));

	Material *defaultMaterial = Material::AquireResource("default");
	defaultMaterial->m_textures.at(0) = m_textureBackground;
	Texture::SetCurrentTexture(m_textureBackground);
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	AABB2 m_aabb2(Vector2(0, 0), Vector2(6784.f,480.f));
	g_theRenderer->DrawTexturedAABB(m_aabb2, m_textureBackground, Vector2(0,1), Vector2(1,0), Rgba::WHITE);
	delete defaultMaterial;

	RenderBricks();
	RenderPipes();
	RenderMario();
	RenderInvisibleEnemies();
	//RenderMiniMap(m_minimapAABB, m_minimapObjs);
	//RenderMiniMap(m_minimapLastPosAABB,m_minimapLastPos);

	//RenderGrid();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderMiniMap(AABB2 minmapAABB,std::vector<MiniMapObject1>& minimap)
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);

	Vector2 marioPosition = m_mario->m_transform.GetWorldPosition().GetXY();
	//Renderer::GetInstance()->DrawAABB(m_minimapAABB,Rgba::BLACK);
	Renderer::GetInstance()->DrawRectangle(minmapAABB);

	Vector2 minPosition = minmapAABB.mins;
	Vector2 marioMiniMapPosition = minmapAABB.GetCenter();
	for(int indexY = 0;indexY < m_minimapHeight;indexY++)
	{
		for(int indexX = 0;indexX < m_minimapWidth;indexX++)
		{
			int index = indexY * m_minimapHeight + indexX;
			Vector2 currentPos(indexX * 10, indexY * 10);
			Rgba color = minimap.at(index).m_color;
			if(color == Rgba::BLACK)
			{
				continue;
			}
			AABB2 miniMapobj(minPosition + currentPos,5,5);
			Renderer::GetInstance()->DrawAABB(miniMapobj,color);// , color);
		}
	}


	/*for (int brickIndex = 0; brickIndex < m_bricks.size(); brickIndex++)
	{
		Vector2 brickPosition = m_bricks.at(brickIndex)->m_transform.GetWorldPosition().GetXY();
		if (m_cameraQuads.IsPointInside(brickPosition))
		{
			Vector2 relativePosition =  brickPosition - marioPosition;
			relativePosition = relativePosition * m_miniMapScaleFactor;
			Vector2 absolutePosition = m_miniMapPosition + relativePosition;
			AABB2 brickMiniMap(absolutePosition, 10, 10);
			Renderer::GetInstance()->DrawAABB(brickMiniMap, Rgba::GREEN);
		}
	}*/
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderMario()
{
	if(m_bestMode)
	{
		m_mario->Render();
		return;
	}
	for(int index = 0;index < m_marios.size();index++)
	{
		m_marios.at(index)->Render();
	}
	//m_mario->Render();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/04
*@purpose : Render all bricks
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Map::RenderBricks()
{
	for (size_t brickIndex = 0; brickIndex < m_bricks.size(); brickIndex++)
	{
		if(m_bricks.at(brickIndex)->m_isHidden)
		{
			continue;
		}
		m_bricks.at(brickIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderPipes()
{
	/*for (size_t pipeIndex = 0; pipeIndex < m_pipes.size(); pipeIndex++)
	{
		if (m_pipes.at(pipeIndex)->m_isHidden)
		{
			continue;
		}
		m_pipes.at(pipeIndex)->Render();
	}*/
	Material *defaultMaterial = Material::AquireResource("default");
	Texture  *texture		  = Texture::CreateOrGetTexture("Data\\Images\\pipe.png", true, true);
	defaultMaterial->m_textures.at(0) = texture;
	Texture::SetCurrentTexture(texture);
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	for (size_t pipeIndex = 0; pipeIndex < m_viewablePipes.size(); pipeIndex++)
	{
		Pipe *pipe = m_viewablePipes.at(pipeIndex);
		Vector2 entityPosition = pipe->m_transform.GetWorldPosition().GetXY();
	

		AABB2 m_aabb2(entityPosition, pipe->m_length / 2, pipe->m_height / 2);

		g_theRenderer->DrawTexturedAABB(m_aabb2, texture, Vector2::ZERO, Vector2::ONE, Rgba::WHITE);
	}
	delete defaultMaterial;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderGrid()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	int incrementValue = 50;
	for(int x = 0;x< 1000;x+=incrementValue)
	{
		Renderer::GetInstance()->DrawLine(Vector2(x, 0), Vector2(x, 1000));
	}
	for (int y = 0; y < 1000; y += incrementValue)
	{
		Renderer::GetInstance()->DrawLine(Vector2(0, y), Vector2(1000, y));
	}
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderInvisibleEnemies()
{
	for (size_t index = 0; index < m_invisibleEnemies.size(); index++)
	{
		m_invisibleEnemies.at(index)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderNN()
{
	float xPosition = m_mario->m_transform.GetWorldPosition().x;
	float inputStartXPosition  = m_camera->m_transform.GetWorldPosition().x - -200;
	float hiddenStartXPosition = m_camera->m_transform.GetWorldPosition().x;
	float outputStartXPosition = m_camera->m_transform.GetWorldPosition().x + 200;
	Vector2 inputStart (inputStartXPosition, 100);
	Vector2 hiddenStart(hiddenStartXPosition,100);
	Vector2 outputStart(outputStartXPosition + 500,100);
	float   fontSize = 5;
	float   radius = 20;
	float   distanceBetweenNodes = 50;

	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);

	for(int inputIndex = 0;inputIndex < m_mario->m_neuralNet->m_inputs->m_neurons.size();inputIndex++)
	{
		Renderer::GetInstance()->DrawCircle(inputStart, radius);
		inputStart.y += distanceBetweenNodes;
	}
	for (int hiddenINdex = 0; hiddenINdex < m_mario->m_neuralNet->m_hiddenLayers->m_neurons.size(); hiddenINdex++)
	{
		Renderer::GetInstance()->DrawCircle(hiddenStart, radius);
		inputStart = Vector2(inputStartXPosition, 100);
		for (int inputIndex = 0; inputIndex < m_mario->m_neuralNet->m_inputs->m_neurons.size(); inputIndex++)
		{
			Renderer::GetInstance()->DrawLine(inputStart, hiddenStart);
			inputStart.y += distanceBetweenNodes;
		}
		hiddenStart.y += distanceBetweenNodes;
	}
	for (int outputIndex = 0; outputIndex < m_mario->m_neuralNet->m_outputs->m_neurons.size(); outputIndex++)
	{
		Renderer::GetInstance()->DrawCircle(outputStart, radius);
		hiddenStart = Vector2(hiddenStartXPosition, 100);
		for (int hiddenINdex = 0; hiddenINdex < m_mario->m_neuralNet->m_hiddenLayers->m_neurons.size(); hiddenINdex++)
		{
			Renderer::GetInstance()->DrawLine(hiddenStart, outputStart);
			hiddenStart.y += distanceBetweenNodes;
		}
		outputStart.y += distanceBetweenNodes;
	}
	inputStart  = Vector2(inputStartXPosition,  100);
	hiddenStart = Vector2(hiddenStartXPosition, 100);
	outputStart = Vector2(outputStartXPosition, 100);

	Material *defaulttextMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(defaulttextMaterial);
	for (int inputIndex = 0; inputIndex < m_mario->m_neuralNet->m_inputs->m_neurons.size(); inputIndex++)
	{
		float value = m_mario->m_neuralNet->m_inputs->m_neurons.at(inputIndex).m_value;
		Renderer::GetInstance()->DrawTextOnPoint(ToString(value).c_str(), 0, 3, inputStart - Vector2(fontSize,0), fontSize,Rgba::YELLOW);
		inputStart.y += distanceBetweenNodes;
	}
	for (int hiddenINdex = 0; hiddenINdex < m_mario->m_neuralNet->m_hiddenLayers->m_neurons.size(); hiddenINdex++)
	{
		float value = m_mario->m_neuralNet->m_hiddenLayers->m_neurons.at(hiddenINdex).m_value;
		Renderer::GetInstance()->DrawTextOnPoint(ToString(value).c_str(), 0, 3, hiddenStart - Vector2(fontSize, 0), fontSize, Rgba::YELLOW);
		hiddenStart.y += distanceBetweenNodes;
	}
	for (int outputIndex = 0; outputIndex < m_mario->m_neuralNet->m_outputs->m_neurons.size(); outputIndex++)
	{
		float value = m_mario->m_neuralNet->m_outputs->m_neurons.at(outputIndex).m_value;
		Renderer::GetInstance()->DrawTextOnPoint(ToString(value).c_str(), 0, 3, outputStart - Vector2(fontSize, 0), fontSize, Rgba::YELLOW);
		outputStart.y += distanceBetweenNodes;
	}
	delete defaulttextMaterial;
	delete defaultMaterial;
}

