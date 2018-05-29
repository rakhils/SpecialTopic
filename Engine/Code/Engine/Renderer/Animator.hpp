#pragma once
#include <map>

#include "Engine/Renderer/IsoSprites.hpp"
#include "Engine/Core/Time.hpp"

/*\class  : Animator
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
* \date   : 3/11/2018 6:42:14 PM
*
* \contact: srsrakhil@gmail.com
*/
enum AnimatorLoopType
{
	CLAMP,
	LOOP,

};
struct Animation
{
	Animation::Animation(){}
	std::string m_isoSpriteId;
	IsoSprites  *m_isoSprite;
	float	    m_duration;
};
class AnimatorDefinition
{
public:
	std::string m_id;
	std::string m_loopTypeString;
	std::string m_srcFile;
	AnimatorLoopType m_loopType;
	std::vector<Animation*> m_animation;

	AnimatorDefinition(XMLElement &element);

};
class Renderer;
class Animator
{

public:
	//Member_Variables
	std::string id;
	AnimatorDefinition *m_definition = nullptr;
	std::string m_currentSpriteId    = "";
	IsoSprites *m_currentIsoSprite   = nullptr;
	Renderer   *renderer			 = nullptr;
	float m_animationStartTime;
	float m_currentTime;
	bool isComplete = false;
	//Static_Member_Variables
	static std::map<std::string, AnimatorDefinition*> s_animators;
	//Methods

	Animator();
	Animator(AnimatorDefinition *def);
	~Animator();

	void Update(float deltaTime);
	bool IsComplete();

	IsoSprites* GetCurrentSprite();
	IsoSprites* GetSpriteWithDirection(Vector2 direction);
	
	static void LoadDefinition(std::string filePath);
	static AnimatorDefinition* GetDefinition(std::string idname);
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