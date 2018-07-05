
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtil.hpp"
//-----------------------------------------------------------------------------------------------
// To disable audio entirely (and remove requirement for fmod.dll / fmod64.dll) for any game,
//	#define ENGINE_DISABLE_AUDIO in your game's Code/Game/EngineBuildPreferences.hpp file.
//
// Note that this #include is an exception to the rule "engine code doesn't know about game code".
//	Purpose: Each game can now direct the engine via #defines to build differently for that game.
//	Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
//
#include "Engine/EngineBuildPreferences.hpp"
//#if !defined( ENGINE_DISABLE_AUDIO )
//-----------------------------------------------------------------------------------------------
// Link in the appropriate FMOD static library (32-bit or 64-bit)
//
#if defined( _WIN64 )
#pragma comment( lib, "Engine/ThirdParty/fmod/fmod64_vc.lib" )
#else
#pragma comment( lib, "Engine/ThirdParty/fmod/fmod_vc.lib" )
#endif
//-----------------------------------------------------------------------------------------------
// Initialization code based on example from "FMOD Studio Programmers API for Windows"
//

AudioSystem *			AudioSystem::s_audioSystem = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : Gets static instance of audio system
*@param   : NIL
*@return  : AudioSystem static pointer
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AudioSystem* AudioSystem::GetInstance()
{
	if(s_audioSystem == nullptr)
	{
		s_audioSystem = new AudioSystem();
	}
	return s_audioSystem;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
AudioSystem::AudioSystem()
	: m_fmodSystem( nullptr )
{
	FMOD_RESULT result;
	result = FMOD::System_Create( &m_fmodSystem );
	ValidateResult( result );

	result = m_fmodSystem->init( 512, FMOD_INIT_NORMAL, nullptr );
	ValidateResult( result );

	//LoadSoundsFromFile("Data\\Definitions\\Sounds.xml");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//DESTRUCTOR
AudioSystem::~AudioSystem()
{
	FMOD_RESULT result = m_fmodSystem->release();
	ValidateResult( result );

	m_fmodSystem = nullptr; // #Fixme: do we delete/free the object also, or just do this?
}

//-----------------------------------------------------------------------------------------------
void AudioSystem::BeginFrame()
{
	m_fmodSystem->update();
}

//-----------------------------------------------------------------------------------------------
void AudioSystem::EndFrame()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/28
*@purpose : Load all sounds from definitions
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AudioSystem::LoadSoundsFromFile(std::string xmlPath)
{
	TinyXMLDocument doc;
	doc.LoadFile(xmlPath.c_str());
	XMLElement *xmlRootElement = doc.FirstChildElement("SoundDefinition");
	XMLElement *xmlChildElement = xmlRootElement->FirstChildElement("Sounds");
	for (; xmlChildElement != nullptr; xmlChildElement = xmlChildElement->NextSiblingElement())
	{
		const char* name = xmlChildElement->Attribute("name");
		XMLElement *xmlFilePathElement = xmlChildElement->FirstChildElement("File");

		for (; xmlFilePathElement != nullptr; xmlFilePathElement = xmlFilePathElement->NextSiblingElement())
		{
			const char* filePath = xmlFilePathElement->Attribute("path");
			LoadSoundGroup(name, filePath);
		}
	}
}

//-----------------------------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGetSound( const std::string& soundFilePath )
{
	std::map< std::string, SoundID >::iterator found = m_registeredSoundIDs.find( soundFilePath );
	if( found != m_registeredSoundIDs.end() )
	{
		return found->second;
	}
	else
	{
		FMOD::Sound* newSound = nullptr;
		m_fmodSystem->createSound( soundFilePath.c_str(), FMOD_DEFAULT, nullptr, &newSound );
		if( newSound )
		{
			SoundID newSoundID = m_registeredSounds.size();
			m_registeredSoundIDs[ soundFilePath ] = newSoundID;
			m_registeredSounds.push_back( newSound );
			return newSoundID;
		}
	}

	return MISSING_SOUND_ID;
}

//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::PlaySound( SoundID soundID, bool isLooped, float volume, float balance, float speed, bool isPaused )
{
	size_t numSounds = m_registeredSounds.size();
	if( soundID < 0 || soundID >= numSounds )
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if( !sound )
		return MISSING_SOUND_ID;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( sound, nullptr, isPaused, &channelAssignedToSound );
	if( channelAssignedToSound )
	{
		int loopCount = isLooped ? -1 : 0;
		unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channelAssignedToSound->setMode(playbackMode);
		channelAssignedToSound->getFrequency( &frequency );
		channelAssignedToSound->setFrequency( frequency * speed );
		channelAssignedToSound->setVolume( volume );
		channelAssignedToSound->setPan( balance );
		channelAssignedToSound->setLoopCount( loopCount );
	}

	return (SoundPlaybackID) channelAssignedToSound;
}

//-----------------------------------------------------------------------------------------------
void AudioSystem::StopSound( SoundPlaybackID soundPlaybackID )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->stop();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : Pause sound
*@param   : SoundID
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AudioSystem::PauseSound(SoundPlaybackID soundPlaybackID)
{
	if (soundPlaybackID == MISSING_SOUND_ID)
	{
		ERROR_RECOVERABLE("WARNING: attempt to set volume on missing sound playback ID!");
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setPaused(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : resumes the sound playing
*@param   : Sound ID
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AudioSystem::ResumeSound(SoundPlaybackID soundPlaybackID)
{
	if (soundPlaybackID == MISSING_SOUND_ID)
	{
		ERROR_RECOVERABLE("WARNING: attempt to set volume on missing sound playback ID!");
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setPaused(false);
}

//-----------------------------------------------------------------------------------------------
// Volume is in [0,1]
//
void AudioSystem::SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setVolume( volume );
}

//-----------------------------------------------------------------------------------------------
// Balance is in [-1,1], where 0 is L/R centered
//
void AudioSystem::SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set balance on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setPan( balance );
}

//-----------------------------------------------------------------------------------------------
// Speed is frequency multiplier (1.0 == normal)
//	A speed of 2.0 gives 2x frequency, i.e. exactly one octave higher
//	A speed of 0.5 gives 1/2 frequency, i.e. exactly one octave lower
//
void AudioSystem::SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set speed on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	float frequency;
	FMOD::Sound* currentSound = nullptr;
	channelAssignedToSound->getCurrentSound( &currentSound );
	if( !currentSound )
		return;

	int ignored = 0;
	currentSound->getDefaults( &frequency, &ignored );
	channelAssignedToSound->setFrequency( frequency * speed );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AudioSystem::LoadSoundGroup(std::string grpname, std::string filename)
{
	CreateOrGetSound(filename);
	std::map<std::string, std::vector<std::string>>::iterator it;
	it = m_audioGroupMap.find(grpname);
	if(it == m_audioGroupMap.end())
	{
		std::vector<std::string> filenames;
		filenames.push_back(filename);
		m_audioGroupMap[grpname] = filenames;
		return;
	}
	//std::vector<std::string> filenames = it->second;
	it->second.push_back(filename);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : Plays random song from group
*@param   : group name
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AudioSystem::PlaySoundFromGroup(std::string grpname)
{
	std::map<std::string, std::vector<std::string>>::iterator it;
	it = m_audioGroupMap.find(grpname);
	if (it != m_audioGroupMap.end())
	{
		std::vector<std::string> filenames = it->second;
		int random = GetRandomIntLessThan(static_cast<int>(filenames.size()));
		SoundID id = CreateOrGetSound(filenames.at(random));
		PlaySound(id);
	}
}

//-----------------------------------------------------------------------------------------------
void AudioSystem::ValidateResult( FMOD_RESULT result )
{
	if( result != FMOD_OK )
	{
		ERROR_RECOVERABLE( Stringf( "Engine/Audio SYSTEM ERROR: Got error result code %i - error codes listed in fmod_common.h\n", (int) result ) );
	}
}

//#endif // !defined( ENGINE_DISABLE_AUDIO )
