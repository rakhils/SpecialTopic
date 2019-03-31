#include "Game/GamePlay/Enemy.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/BTreeTasks/BTreeSetRandomAttackLocation.hpp"
#include "Game/GamePlay/BTreeTasks/BTreeSetRandomWanderLocation.hpp"
#include "Game/GamePlay/BTreeTasks/BTreeAttackPlayerTask.hpp"
#include "Game/GamePlay/BTreeTasks/Decorators/BTreeBlackboardCondition.hpp"
#include "Game/GamePlay/BTreeTasks/Services/BTreeCheckAndSetEnemyState.hpp"
#include "Game/GamePlay/Entity/Player.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/AI/BehaviorTree/Selector.hpp"
#include "Engine/AI/BehaviorTree/Sequence.hpp"
#include "Engine/AI/BehaviorTree/SimpleParallel.hpp"
#include "Engine/AI/BehaviorTree/Tasks/BTreeTaskWaitBlackboardTime.hpp"
#include "Engine/Core/StringUtils.hpp"

// CONSTRUCTOR
Enemy::Enemy()
{
	m_position = Vector2(200,100);
}

// DESTRUCTOR
Enemy::~Enemy()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Enemy::InitBTree()
{
	m_btree = new BehaviorTree();
	InitBlackboardValues();

	Selector *rootSelector				 = new Selector("ROOT_SELECTOR",m_btree);
	Sequence *wanderSequence			 = new Sequence("SEQ_WANDER" ,	m_btree);
	Sequence *alertSequence				 = new Sequence("SEQ_ALERT"  ,	m_btree);
	Sequence *attackSelquence			 = new Sequence("SEQ_ATACK" ,	m_btree);
	Sequence *pursuitSequence			 = new Sequence("SEQ_PURSUIT",	m_btree);
	Sequence *retreatSequence			 = new Sequence("SEQ_RETREAT",	m_btree);

	rootSelector->m_children.push_back(wanderSequence);
	rootSelector->m_children.push_back(alertSequence);
	rootSelector->m_children.push_back(attackSelquence);
	rootSelector->m_children.push_back(pursuitSequence);
	rootSelector->m_children.push_back(retreatSequence);

	rootSelector->m_parent = m_btree->m_root;

	BTreeCheckAndSetEnemyState *checkAndSetEnemyState = new BTreeCheckAndSetEnemyState();
	checkAndSetEnemyState->m_btree = m_btree;
	checkAndSetEnemyState->m_name = "Check And Set EnemyState";
	rootSelector->m_services.push_back(checkAndSetEnemyState);

	wanderSequence->m_parent   = rootSelector;
	alertSequence->m_parent    = rootSelector;
	attackSelquence->m_parent  = rootSelector;
	pursuitSequence->m_parent  = rootSelector;
	retreatSequence->m_parent  = rootSelector;
	m_btree->m_root->m_children.push_back(rootSelector);

	BTreeBlackboardCondition *wanderDecorator		= new BTreeBlackboardCondition("BBCondition",m_btree);
	wanderDecorator->m_enemyStateTargetEnumValue	= (int)WANDER;
	BTreeBlackboardCondition *alertDecorator		= new BTreeBlackboardCondition("BBCondition",m_btree);
	alertDecorator->m_enemyStateTargetEnumValue		= (int)ALERT;
	BTreeBlackboardCondition *attackDecorator		= new BTreeBlackboardCondition("BBCondition",m_btree);
	attackDecorator->m_enemyStateTargetEnumValue	= (int)ATTACK;
	BTreeBlackboardCondition *pursuitDecorator		= new BTreeBlackboardCondition("BBCondition",m_btree);
	pursuitDecorator->m_enemyStateTargetEnumValue   = (int)PURSUIT;
	BTreeBlackboardCondition *retreatDecorator		= new BTreeBlackboardCondition("BBCondition",m_btree);
	retreatDecorator->m_enemyStateTargetEnumValue	= (int)RETREAT;


	wanderSequence->m_decorators.push_back(wanderDecorator);
	alertSequence->m_decorators.push_back(alertDecorator);
	attackSelquence->m_decorators.push_back(attackDecorator);
	pursuitSequence->m_decorators.push_back(pursuitDecorator);
	retreatSequence->m_decorators.push_back(retreatDecorator);

	BTreeSetRandomWanderLocation *setWanderLocationBTask = new BTreeSetRandomWanderLocation();
	setWanderLocationBTask->m_btree = m_btree;
	setWanderLocationBTask->m_parent = wanderSequence;
	wanderSequence->AddChild(setWanderLocationBTask);

	BTreeMoveTask *moveTask = new BTreeMoveTask(m_btree);
	moveTask->m_blackBoardKey = "WANDER_LOCATION";
	moveTask->m_parent = wanderSequence;
	moveTask->m_enemy = this;
	wanderSequence->AddChild(moveTask);

	/********************************************************************/
	SimpleParallel *parallelTaskAlert  = new SimpleParallel("PAR_ALRT",m_btree);
	parallelTaskAlert->m_parent = alertSequence;

	BTreeSetRandomAttackLocation *setRandomAttackBTask = new BTreeSetRandomAttackLocation();
	setRandomAttackBTask->m_btree = m_btree;
	setRandomAttackBTask->m_player = m_map->m_player;
	setRandomAttackBTask->m_parent = alertSequence;

	alertSequence->AddChild(setRandomAttackBTask);
	alertSequence->AddChild(parallelTaskAlert);

	BTreeTaskWaitBlackboardTime *waitBlackBoardTimeAlert = new BTreeTaskWaitBlackboardTime(m_btree);
	waitBlackBoardTimeAlert->m_parent = parallelTaskAlert;
	waitBlackBoardTimeAlert->m_blackBoardKey = "BBAlertWait";
	parallelTaskAlert->AddChild(waitBlackBoardTimeAlert);


	BTreeMoveTask *moveTaskAlert = new BTreeMoveTask(m_btree);
	moveTaskAlert->m_blackBoardKey = "ATTACK_LOCATION";
	moveTaskAlert->m_parent = parallelTaskAlert;
	moveTaskAlert->m_enemy = this;
	parallelTaskAlert->AddChild(moveTaskAlert);

	/********************************************************************/
	SimpleParallel *parallelTask  = new SimpleParallel("PAR_ATCK1",m_btree);
	SimpleParallel *parallelTask1 = new SimpleParallel("PAR_ATCK2",m_btree);

	parallelTask->m_parent = attackSelquence;
	parallelTask1->m_parent = attackSelquence;


	BTreeSetRandomAttackLocation *setRandomAttackBTask1 = new BTreeSetRandomAttackLocation();
	setRandomAttackBTask1->m_btree = m_btree;
	setRandomAttackBTask1->m_player = m_map->m_player;
	setRandomAttackBTask1->m_parent = attackSelquence;

	attackSelquence->AddChild(setRandomAttackBTask1);
	attackSelquence->AddChild(parallelTask);
	attackSelquence->AddChild(parallelTask1);

	BTreeMoveTask *moveTaskAttack1 = new BTreeMoveTask(m_btree);
	moveTaskAttack1->m_blackBoardKey = "ATTACK_LOCATION";
	moveTaskAttack1->m_parent = parallelTask;
	moveTaskAttack1->m_enemy = this;
	parallelTask->AddChild(moveTaskAttack1);

	BTreeAttackPlayerTask *attackPlayer = new BTreeAttackPlayerTask();
	attackPlayer->m_parent = parallelTask;
	attackPlayer->m_enemy = this;
	attackPlayer->m_map = m_map;
	attackPlayer->m_player = m_map->m_player;
	parallelTask->AddChild(attackPlayer);


	BTreeTaskWaitBlackboardTime *waitBlackBoardTime = new BTreeTaskWaitBlackboardTime(m_btree);
	waitBlackBoardTime->m_parent = parallelTask1;
	waitBlackBoardTime->m_blackBoardKey = "BBAttackWait";
	parallelTask1->AddChild(waitBlackBoardTime);


	BTreeAttackPlayerTask *attackPlayer1 = new BTreeAttackPlayerTask();
	attackPlayer1->m_parent = parallelTask1;
	attackPlayer1->m_enemy = this;
	attackPlayer1->m_map = m_map;
	attackPlayer1->m_player = m_map->m_player;
	parallelTask1->AddChild(attackPlayer1);
	/********************************************************************/

	BTreeMoveTask *moveTaskPursuit = new BTreeMoveTask(m_btree);
	moveTaskPursuit->m_blackBoardKey = "PURSUIT_LOCATION";
	moveTaskPursuit->m_parent = pursuitSequence;
	moveTaskPursuit->m_enemy = this;
	pursuitSequence->AddChild(moveTaskPursuit);

	/********************************************************************/

	BTreeMoveTask *moveTaskRetreat = new BTreeMoveTask(m_btree);
	moveTaskRetreat->m_blackBoardKey = "SPAWN_LOCATION";
	moveTaskRetreat->m_parent = retreatSequence;
	moveTaskRetreat->m_enemy = this;
	retreatSequence->AddChild(moveTaskRetreat);

	/********************************************************************/
	BehaviorTreeManager::CreateBehaviorTreeInstance(m_btree);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/29
*@purpose : Inits all black board values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Enemy::InitBlackboardValues()
{
	m_btree->m_blackBoard->SetValue("SPAWN_LOCATION",   m_position.ToString());
	m_btree->m_blackBoard->SetValue("PURSUIT_LOCATION", m_position.ToString());
	m_btree->m_blackBoard->SetValue("ENEMY_LOCATION",   m_position.ToString());
	m_btree->m_blackBoard->SetValue("PLAYER_LOCATION",  m_position.ToString());


	m_btree->m_blackBoard->SetValue("BBAttackWait", ToString(0.5f));
	m_btree->m_blackBoard->SetValue("BBAlertWait" , ToString(1.5f));
	m_btree->m_blackBoard->SetValue("ENEMY_STATE",  ToString((int)0));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Enemy::Update(float deltaTime)
{
	UpdateBlackboardValues();
	BehaviorTreeManager::Update(deltaTime);
	m_lastAttackTime -= deltaTime;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Enemy::UpdateBlackboardValues()
{
	m_btree->m_blackBoard->SetValue("ENEMY_LOCATION",  m_position.ToString());
	m_btree->m_blackBoard->SetValue("PLAYER_LOCATION", m_map->m_player->m_position.ToString());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Enemy::Render()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	g_theRenderer->DrawSolidCircle(m_position, g_alertRange,  Rgba::FADED_YELLOW);
	g_theRenderer->DrawSolidCircle(m_position, g_attackRange, Rgba::FADED_RED);
	g_theRenderer->DrawCircle(m_position.x, m_position.y, g_unitDistance/2.f, Rgba::YELLOW, 50);

	g_theRenderer->DrawSolidRectangle(Vector2(0,g_gameMapExtends.maxs.y + 25),Vector2(g_gameMapExtends.maxs.x,g_gameMapExtends.maxs.y + 25),
		Vector2(g_gameMapExtends.maxs.x,1000),Vector2(0,1000),Rgba::BLACK);

	delete defaultMaterial;
	Vector2 rootPosition(925, 925);
	m_btree->Render(rootPosition);
}
