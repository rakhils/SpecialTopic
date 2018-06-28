#pragma once
#include "Engine/SceneManagement/Scene.hpp"
#include "Game/GamePlay/Entity/Tank.hpp"
#include "Engine/Time/StopWatch.hpp"
/*\class  : Level1		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/1/2018 4:18:57 PM
* \contact: srsrakhil@gmail.com
*/
class Tank;
class Map;
class SceneLevel1 : public Scene
{

public:
	//Member_Variables
	Tank * m_playerTank = nullptr;
	GameObject			    *m_soundTestObj = nullptr;
	Map					    *m_map = nullptr;
	bool				   m_init = false;
	OrbitCamera			   *m_ocamera = nullptr;
	PerspectiveCamera	   *m_pcamera = nullptr;

	float					m_cameraRadius = 30;
	float					m_cameraPhi	   = 0;
	float					m_cameraTheta  = -20;

	//Static_Member_Variables

	//Methods
	SceneLevel1();
	SceneLevel1(std::string str);
	~SceneLevel1();
	 
	void InitScene();
	void CreatePlayer();
	void CreateMap();
	void CreateWater();
	void CreateTerrain();

	void Update(float deltaTime);

	void Render();
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