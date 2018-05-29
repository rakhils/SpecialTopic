#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimationDefinition.hpp"
enum SpriteAnimMode
{
	SPRITE_ANIM_MODE_PLAY_TO_END,	// Play from time=0 to durationSeconds, then finish
	SPRITE_ANIM_MODE_LOOPING,		// Play from time=0 to end then repeat (never finish)
	SPRITE_ANIM_MODE_PINGPONG, 		// optional, play forwards, backwards, forwards...
	NUM_SPRITE_ANIM_MODES			  
};


class SpriteAnimation
{

public:

	SpriteSheet * m_spriteSheet = nullptr;
	std::vector<int> m_spriteIndexes;
	std::string m_name;
	
	int m_spriteStartIndex = 0;
	int m_spriteEndIndex   = 0;
	int m_playbackMode	   = 0;

	bool m_isFinished = false;
	bool m_isPlaying  = false;
	bool m_isPaused   = false;

	float m_durationSeconds = 0;
	float m_elapsedSeconds  = 0;
	
	SpriteAnimationDefinition *m_definition;

	SpriteAnimation();
	SpriteAnimation( SpriteSheet* spriteSheet, float durationSeconds,SpriteAnimMode playbackMode, int startSpriteIndex, int endSpriteIndex );

	SpriteAnimation(  SpriteSheet* spriteSheet, float durationSeconds,std::vector<int> spriteIndexes);

	void Update( float deltaSeconds );
	AABB2 GetCurrentTexCoords() const;	// Based on the current elapsed time
	const Texture* GetTexture() const	{ return m_spriteSheet->getTexture();}
	void Pause();					    // Starts unpaused (playing) by default
	void Resume();						// Resume after pausing
	void Reset();						// Rewinds to time 0 and starts (re)playing
	bool IsFinished() const				{ return m_isFinished; }
	bool IsPlaying() const				{ return m_isPlaying; }
	float GetDurationSeconds() const	{ return m_durationSeconds; }
	float GetSecondsElapsed() const		{ return m_elapsedSeconds; }
	float GetSecondsRemaining() const;
	float GetFractionElapsed() const;
	float GetFractionRemaining() const;
	void  SetSecondsElapsed( float secondsElapsed );	    // Jump to specific time
	void  SetFractionElapsed( float fractionElapsed );    // e.g. 0.33f for one-third in
	int   getCurrentIndexPlaying();
	
};
