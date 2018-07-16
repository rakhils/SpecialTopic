#pragma once
//-----------------------------------------------------------------------------------------------
//#include "ThirdParty/fmod/fmod.hpp"
#include <string>
#include <vector>
#include <map>
#include "Engine/ThirdParty/fmod/fmod.hpp"
#include "Engine/TinyXml2/tinyxml2.h"

//-----------------------------------------------------------------------------------------------
typedef   size_t SoundID;
typedef   size_t SoundPlaybackID;
constexpr size_t MISSING_SOUND_ID = (size_t)(-1); // for bad SoundIDs and SoundPlaybackIDs
//-----------------------------------------------------------------------------------------------
class AudioSystem;
using namespace tinyxml2;
struct GroupSound_t
{
	std::vector<SoundID>     m_soundIDs;
	std::vector<std::string> m_soundPaths;
	std::vector<float>       m_weights;
	
	void Add(SoundID id, std::string path, float weight)
	{
		m_soundIDs.push_back(id);
		m_soundPaths.push_back(path);
		m_weights.push_back(weight);
	}

	float GetTotalWeights()
	{
		float weightSum = 0.f;
		for (size_t weightIndex = 0; weightIndex < m_weights.size();weightIndex++)
		{
			weightSum += m_weights.at(weightIndex);
		}
		return weightSum;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
public:

	std::map<std::string, GroupSound_t> m_audioGroupMap;
	AudioSystem();
	virtual ~AudioSystem();

public:
	virtual void				BeginFrame();
	virtual void				EndFrame();
	void						LoadSoundsFromFile(std::string xmlPath);
	virtual SoundID				CreateOrGetSound( const std::string& soundFilePath );
	virtual SoundPlaybackID		PlaySound( SoundID soundID, bool isLooped=false, float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false );
	virtual void				StopSound( SoundPlaybackID soundPlaybackID );
	virtual void				PauseSound(SoundPlaybackID soundPlaybackID);
	virtual void				ResumeSound(SoundPlaybackID soundPlaybackID);
	virtual void				SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume );	// volume is in [0,1]
	virtual void				SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance );	// balance is in [-1,1], where 0 is L/R centered
	virtual void				SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed );		// speed is frequency multiplier (1.0 == normal)
	void						LoadSoundGroup(std::string grpname, std::string filename,float weight);
	void						PlaySoundFromGroup(std::string grpname);
	virtual void				ValidateResult( FMOD_RESULT result );

	static AudioSystem*			s_audioSystem;
	static AudioSystem*			GetInstance();
protected:
	FMOD::System*						m_fmodSystem;
	std::map< std::string, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;
};

