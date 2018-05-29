#include "SpriteAnimation.hpp"

SpriteAnimation::SpriteAnimation(SpriteSheet *spriteSheet, float durationSeconds, SpriteAnimMode playbackMode, int startSpriteIndex, int endSpriteIndex):m_spriteSheet(spriteSheet)
{
	m_durationSeconds = durationSeconds;
	m_playbackMode = playbackMode;
	m_spriteStartIndex = startSpriteIndex;
	m_spriteEndIndex = endSpriteIndex;
	m_elapsedSeconds = 0.0f;
}

SpriteAnimation::SpriteAnimation(SpriteSheet *spriteSheet, float durationSeconds, std::vector<int> spriteIndexes):m_spriteIndexes(spriteIndexes),m_spriteSheet(spriteSheet)
{
	m_spriteSheet = spriteSheet;
	m_elapsedSeconds = 0.0f;
	m_durationSeconds = durationSeconds;
}

SpriteAnimation::SpriteAnimation()
{

}

void SpriteAnimation::Update(float deltaSeconds)
{
	if(m_isPaused)
	{
		return;
	}
	m_elapsedSeconds += deltaSeconds;
	if(m_elapsedSeconds > m_durationSeconds)
	{
		m_elapsedSeconds = 0;
		m_isFinished = true;
	}
}

AABB2 SpriteAnimation::GetCurrentTexCoords() const
{
	int size = static_cast<int>(m_spriteIndexes.size());
	int currentIndex = static_cast<int>((m_elapsedSeconds/m_durationSeconds*static_cast<float>(size)));
	if(currentIndex > m_spriteIndexes.size())
	{
		currentIndex = 0;
	}
	return m_spriteSheet->GetTexCoordsForSpriteIndex(m_spriteIndexes.at(currentIndex));
}

void SpriteAnimation::Pause()
{
	m_isPaused = true;
}

void SpriteAnimation::Resume()
{
	m_isPaused = false;
}

void SpriteAnimation::Reset()
{
	m_elapsedSeconds = 0;
	m_isFinished = false;
}

float SpriteAnimation::GetSecondsRemaining() const
{
	return m_durationSeconds - m_elapsedSeconds;
}

float SpriteAnimation::GetFractionElapsed() const
{
	return m_elapsedSeconds/m_durationSeconds;
}

float SpriteAnimation::GetFractionRemaining() const
{
	return (m_durationSeconds-m_elapsedSeconds)/m_durationSeconds;
}

void SpriteAnimation::SetSecondsElapsed(float secondsElapsed)
{
	m_elapsedSeconds = secondsElapsed;
}

void SpriteAnimation::SetFractionElapsed(float fractionElapsed)
{
	m_elapsedSeconds = m_durationSeconds*(fractionElapsed);
}

int SpriteAnimation::getCurrentIndexPlaying()
{
	int size = static_cast<int>(m_spriteIndexes.size());
	int currentIndex = static_cast<int>((m_elapsedSeconds/m_durationSeconds*static_cast<float>(size)));
	return currentIndex;
}

