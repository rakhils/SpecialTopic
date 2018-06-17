#pragma once
#include <string>
#include "Engine/Core/Component.hpp"
#include "Engine/Audio/AudioSystem.hpp"
/*\class  : AudioComponent		   
* \group  : <GroupName>	   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman	   
* \version: 2.0	   
* \date   : 5/20/2018 12:57:30 AM
* \contact: srsrakhil@gmail.com
*/
 
class AudioComponent : public Component
{

public:
	//Member_Variables
	SoundID				m_soundID;
	SoundPlaybackID		m_soundPlayBackID;
	bool				m_hasBegunPlaying		= false;
	bool				m_isPlaying				= false;
	bool				m_isPaused				= false;
	bool				m_isStopped				= false;
	bool				m_is3DSound				= false;
	float				m_maxDistanceFor0Sound  = 100;
	Vector3				m_position;

	//Static_Member_Variables

	//Methods

	AudioComponent();
	AudioComponent(std::string filename);
	~AudioComponent();

	void AssignSoundFile(std::string filename);
	void Play();
	
	void PauseSound();
	void ResumeSound();
	void StopSound();

	void Update(float deltatime);
	//Static_Methods

protected:
	//Member_Variables
	//Static_Member_Variables
	//Method
	//Static_Methods

private:
	//Member_Variables
	//Static_Member_Variables
	//Methods
	//Static_Methods

};