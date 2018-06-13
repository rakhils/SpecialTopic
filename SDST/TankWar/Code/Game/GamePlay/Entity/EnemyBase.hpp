#pragma once
#include "Engine/GameObject/GameObject.hpp"
/*\class  : EnemyBase
*		   
* \group  : <GroupName>
*		   
* \brief  :
*		   
* \TODO:  :
*		   
* \note   :
*		   
* \author : Rakhil Soman
*		   
* \version: 1.0
*		   
* \date   : 6/10/2018 5:45:37 PM
*
* \contact: srsrakhil@gmail.com
*/
class EnemyTank;
class EnemyBase : public GameObject
{

public:
	//Member_Variables
	std::vector<EnemyTank*> m_tanks;
	//Static_Member_Variables

	//Methods
	EnemyBase();
	EnemyBase(std::string name, Vector3 position,Vector3 dimensions);
	~EnemyBase();
	void SpawnEnemy(Scene *scene);
	void Update(float deltaTime);

	static void AddEnemyBase(std::string name,Vector3 position,Vector3 dimensions,Scene *scene);
	static void CreateEnemy(std::string baseName,Scene *scene);
	static void UpdateEnemyBase(float deltaTime);
	static std::map<std::string, EnemyBase*> s_enemyBases;
	
	//Static_Methods

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};