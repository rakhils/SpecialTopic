#include "Game/GamePlay/Utils/ScoreCard.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
// CONSTRUCTOR
ScoreCard::ScoreCard()
{

}

// DESTRUCTOR
ScoreCard::~ScoreCard()
{

}

void ScoreCard::operator=(ScoreCard &scard)
{
	m_resourceFoodCollected		= scard.m_resourceFoodCollected;
	m_resourceStoneCollected	= scard.m_resourceStoneCollected;
	m_resourceWoodCollected		= scard.m_resourceWoodCollected;
	m_housesBuilt				= scard.m_housesBuilt;
	m_armySpawnersBuilt			= scard.m_armySpawnersBuilt;
	m_civiliansSpawned			= scard.m_civiliansSpawned;
	m_shortRangeArmySpawned		= scard.m_shortRangeArmySpawned;
	m_longRangeArmySpawned		= scard.m_longRangeArmySpawned;
	m_housesAttacked			= scard.m_housesAttacked;
	m_armySpawnersAttacked		= scard.m_armySpawnersAttacked;
	m_civiilansAttacked			= scard.m_civiilansAttacked;
	m_shortRangeArmyAttacked	= scard.m_shortRangeArmyAttacked;
	m_longRangeArmyAttacked		= scard.m_longRangeArmyAttacked;
	m_townCenterAttacked		= scard.m_townCenterAttacked;
	m_housesDestroyed			= scard.m_housesDestroyed;
	m_armySpawnersDestroyed		= scard.m_armySpawnersDestroyed;
	m_civiliansKilled			= scard.m_civiliansKilled;
	m_shortRangeArmyKilled		= scard.m_shortRangeArmyKilled;
	m_longRangeArmyKilled		= scard.m_longRangeArmyKilled;
	m_townCenterDestroyed		= scard.m_townCenterDestroyed;
	m_totalSteps				= scard.m_totalSteps;
	m_scoreRate				    = scard.m_scoreRate;
	m_interimScore				= scard.m_interimScore;
	m_bonusScore				= scard.m_bonusScore;
	m_totalScore				= scard.m_totalScore;
	m_alive						= scard.m_alive;
	m_health					= scard.m_health;
	m_negativeScores			= scard.m_negativeScores;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/15
*@purpose : Resets score
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ScoreCard::Reset()
{
	m_createdTime = static_cast<float>(GetCurrentTimeSeconds());
	m_scoreCalculated = false;
	m_resourceFoodCollected = 0;
	m_resourceStoneCollected = 0;
	m_resourceWoodCollected = 0;
	m_housesBuilt = 0;
	m_armySpawnersBuilt = 0;
	m_civiliansSpawned = 0;
	m_shortRangeArmySpawned = 0;
	m_longRangeArmySpawned = 0;
	m_housesAttacked = 0;
	m_armySpawnersAttacked = 0;
	m_civiilansAttacked = 0;
	m_shortRangeArmyAttacked = 0;
	m_longRangeArmyAttacked = 0;
	m_townCenterAttacked = 0;
	m_housesDestroyed = 0;
	m_armySpawnersDestroyed = 0;
	m_civiliansKilled = 0;
	m_shortRangeArmyKilled = 0;
	m_longRangeArmyKilled = 0;
	m_townCenterDestroyed = 0;
	m_totalSteps = 0;
	m_bonusScore = 0;
	m_interimScore = 0;
	m_scoreRate = 0.f;
	m_totalScore = 0;
	m_alive = 0;
	m_health = 0;
	m_negativeScores = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ScoreCard::CalculateTotalScore()
{
	if (m_scoreCalculated)
	{
		return;
	}
	m_totalSteps = static_cast<float>(GetCurrentTimeSeconds()) - m_createdTime;
	m_scoreCalculated = true;
	m_totalScore =
			(m_resourceFoodCollected  *		 m_resourceFoodCollectedPoints  +
			m_resourceStoneCollected  *		 m_resourceStoneCollectedPoints +
			m_resourceWoodCollected   *		 m_resourceWoodCollectedPoints  +
			m_housesBuilt			  *		 m_housesBuiltPoints			+
			m_armySpawnersBuilt		  *		 m_armySpawnersBuiltPoints		+
			m_civiliansSpawned		  *		 m_civiliansSpawnedPoints		+
			m_shortRangeArmySpawned   *		 m_shortRangeArmySpawnedPoints	+
			m_longRangeArmySpawned    *		 m_longRangeArmySpawnedPoints	+
			m_housesAttacked		  *		 m_housesAttackedPoints			+
			m_armySpawnersAttacked	  *		 m_armySpawnerAttackedPoints	+
			m_civiilansAttacked		  *		 m_civiilansAttackedPoints		+
			m_shortRangeArmyAttacked  *		 m_shortRangeArmyAttackedPoints +
			m_longRangeArmyAttacked	  *		 m_longRangeArmyAttackedPoints  +
			m_townCenterAttacked	  *		 m_townCenterAttackedPoints		+
			m_housesDestroyed		  *		 m_housesDestroyedPoints		+
			m_armySpawnersDestroyed	  *		 m_armySpawnerDestroyedPoints	+
			m_civiliansKilled		  *		 m_civiliansKilledPoints		+
			m_shortRangeArmyKilled	  *		 m_shortRangeArmyKilledPoints	+
			m_longRangeArmyKilled	  *		 m_longRangeArmyKilledPoints	+
			m_townCenterDestroyed	  *		 m_townCenterDestroyedPoints);
	if (m_entity != nullptr)
	{
		m_totalScore += m_entity->m_health* m_healthPoints;
		if (m_entity->m_health > 0)
		{
			m_alive = 1;
			m_totalScore += m_alivePoints;
		}
		else
		{
			m_alive = 0;
		}
	}
	int totalFrameEquivalent = RangeMapInt(m_totalSteps, 0, 1000, 1000, 0);
	m_bonusScore = totalFrameEquivalent;
	//m_scoreCalculated += m_bonusScore;
	m_scoreCalculated += totalFrameEquivalent;
	m_scoreCalculated += m_scoreRate;
	m_scoreCalculated -= m_negativeScores;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ScoreCard::GetTotalScore()
{
	int totalScoreLocal = 0;
	totalScoreLocal =
			(m_resourceFoodCollected  *		 m_resourceFoodCollectedPoints  +
			m_resourceStoneCollected  *		 m_resourceStoneCollectedPoints +
			m_resourceWoodCollected   *		 m_resourceWoodCollectedPoints  +
			m_housesBuilt			  *		 m_housesBuiltPoints			+
			m_armySpawnersBuilt		  *		 m_armySpawnersBuiltPoints		+
			m_civiliansSpawned		  *		 m_civiliansSpawnedPoints		+
			m_shortRangeArmySpawned   *		 m_shortRangeArmySpawnedPoints	+
			m_longRangeArmySpawned    *		 m_longRangeArmySpawnedPoints	+
			m_housesAttacked		  *		 m_housesAttackedPoints			+
			m_armySpawnersAttacked	  *		 m_armySpawnerAttackedPoints	+
			m_civiilansAttacked		  *		 m_civiilansAttackedPoints		+
			m_shortRangeArmyAttacked  *		 m_shortRangeArmyAttackedPoints +
			m_longRangeArmyAttacked	  *		 m_longRangeArmyAttackedPoints  +
			m_townCenterAttacked	  *		 m_townCenterAttackedPoints		+
			m_housesDestroyed		  *		 m_housesDestroyedPoints		+
			m_armySpawnersDestroyed	  *		 m_armySpawnerDestroyedPoints	+
			m_civiliansKilled		  *		 m_civiliansKilledPoints		+
			m_shortRangeArmyKilled	  *		 m_shortRangeArmyKilledPoints	+
			m_longRangeArmyKilled	  *		 m_longRangeArmyKilledPoints	+
			m_townCenterDestroyed	  *		 m_townCenterDestroyedPoints	+
			m_negativeScores);
	return totalScoreLocal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ScoreCard::CalculateInterimScore(bool forced,int prevInterimScore)
{
	if(m_totalSteps % 250 == 0 || forced)
	{
		int totalScoreLocal			= GetTotalScore();
		m_interimScore				= totalScoreLocal;
		int currentInterimScore     = m_interimScore - prevInterimScore;
		if(m_scoreRate == 0)
		{
			m_scoreRate = currentInterimScore;
		}
		else
		{
			m_scoreRate = (m_scoreRate + currentInterimScore) / 2.f;
		}
		return true;
	}
	return false;
}

void ScoreCard::UpdateScoreResourceFoodCollected(int count)
{
	if(m_entity != nullptr)
	{
		int totalFoodCount = m_entity->GetMyFoodCount();
		int totalWoodCount = m_entity->GetMyWoodCount();
		int totalStoneCount = m_entity->GetMyStoneCount();
		if(totalFoodCount <= 30)
		{
			count = RangeMapInt(totalFoodCount, 0, 30, 4, 1);
		}
		else
		{
			count = 0;
			if(totalFoodCount > 30 && totalStoneCount > 30 && totalWoodCount > 30)
			{
				count = 1;
			}
		}
	}
	m_resourceFoodCollected += count;
}
void ScoreCard::UpdateScoreResourceWoodCollected(int count)
{
	if (m_entity != nullptr)
	{
		int totalFoodCount = m_entity->GetMyFoodCount();
		int totalWoodCount = m_entity->GetMyWoodCount();
		int totalStoneCount = m_entity->GetMyStoneCount();
		if (totalWoodCount <= 30)
		{
			count = RangeMapInt(totalWoodCount, 0, 30, 3, 1);
		}
		else
		{
			count = 0;
			if (totalFoodCount > 30 && totalStoneCount > 30 && totalWoodCount > 30)
			{
				count = 1;
			}
		}
	}
	m_resourceWoodCollected += count;
}
void ScoreCard::UpdateScoreResourceStoneCollected(int count)
{
	if (m_entity != nullptr && m_entity != nullptr)
	{
		int totalFoodCount = m_entity->GetMyFoodCount();
		int totalWoodCount = m_entity->GetMyWoodCount();
		int totalStoneCount = m_entity->GetMyStoneCount();
		if (totalStoneCount <= 30)
		{
			count = RangeMapInt(totalStoneCount, 0, 30, 3, 1);
		}
		else
		{
			count = 0;
			if (totalFoodCount > 30 && totalStoneCount > 30 && totalWoodCount > 30)
			{
				count = 1;
			}
		}
	}
	m_resourceStoneCollected += count;
}
void ScoreCard::UpdateScoreHousesBuilt(int count)
{

	if(m_entity != nullptr && m_entity->m_map->m_gameStats.m_numOfHousesTeam1 > g_maxHousesBuilt && m_entity->m_teamID == 1)
	{
		m_negativeScores += 5;
		return;
	}
	if (m_entity != nullptr && m_entity->m_map->m_gameStats.m_numOfHousesTeam2 > g_maxHousesBuilt && m_entity->m_teamID == 2)
	{
		m_negativeScores += 5;
		return;
	}
	m_housesBuilt += count;
}
void ScoreCard::UpdateScoreArmySpawnersBuilt(int count)
{
	if(m_entity != nullptr && m_entity->m_teamID == 1)
	{
		if (m_entity->m_map->m_gameStats.m_numOfArmySpawnerTeam1 > g_maxArmySpawnerBuilt)
		{
			m_negativeScores += 5;
			return;
		}
	}
	if (m_entity != nullptr && m_entity->m_teamID == 2)
	{
		if (m_entity->m_map->m_gameStats.m_numOfArmySpawnerTeam2> g_maxArmySpawnerBuilt)
		{
			m_negativeScores += 5;
			return;
		}
	}
	m_armySpawnersBuilt += count;
}
void ScoreCard::UpdateScoreCiviliansSpawned(int count)
{
	if (m_entity != nullptr && m_entity->m_map->m_gameStats.m_numOfCiviliansTeam1 > g_maxUnits && m_entity->m_teamID == 1)
	{
		m_negativeScores += 5;
		return;
	}
	if (m_entity != nullptr && m_entity->m_map->m_gameStats.m_numOfCiviliansTeam2 > g_maxUnits && m_entity->m_teamID == 2)
	{
		m_negativeScores += 5;
		return;
	}
	if (m_entity != nullptr && m_entity->m_map->m_gameStats.m_numOfCiviliansTeam1 > 5 && m_entity->m_teamID == 1)
	{
		m_negativeScores += 5;
		return;
	}
	if (m_entity != nullptr && m_entity->m_map->m_gameStats.m_numOfCiviliansTeam2 > 5 && m_entity->m_teamID == 2)
	{
		m_negativeScores += 5;
		return;
	}
	m_civiliansSpawned += count;
}
void ScoreCard::UpdateScoreShortRangeArmySpawned(int count)
{
	if (m_entity != nullptr && m_entity->m_map->m_gameStats.m_numOfShortRangeArmyTeam1 > g_maxUnits && m_entity->m_teamID == 1)
	{
		m_negativeScores += 5;
		return;
	}
	if (m_entity != nullptr && m_entity->m_map->m_gameStats.m_numOfShortRangeArmyTeam2 > g_maxUnits && m_entity->m_teamID == 2)
	{
		m_negativeScores += 5;
		return;
	}
	if (m_entity != nullptr && m_entity->m_map->m_gameStats.m_numOfShortRangeArmyTeam1 > 5 && m_entity->m_teamID == 1)
	{
		m_negativeScores += 5;
		return;
	}
	if (m_entity != nullptr && m_entity->m_map->m_gameStats.m_numOfShortRangeArmyTeam2 > 5 && m_entity->m_teamID == 2)
	{
		m_negativeScores += 5;
		return;
	}
	m_shortRangeArmySpawned += count;
}
void ScoreCard::UpdateScoreLongRangeArmySpawned(int count)
{
	if (m_entity != nullptr && m_entity->m_map->m_gameStats.m_numOfLongRangeArmyTeam1 > g_maxUnits && m_entity->m_teamID == 1)
	{
		m_negativeScores += 5;
		return;
	}
	if (m_entity != nullptr && m_entity->m_map->m_gameStats.m_numOfLongRangeArmyTeam2 > g_maxUnits && m_entity->m_teamID == 2)
	{
		m_negativeScores += 5;
		return;
	}
	if (m_entity != nullptr && m_entity->m_map->m_gameStats.m_numOfLongRangeArmyTeam1 > 5 && m_entity->m_teamID == 1)
	{
		m_negativeScores += 5;
		return;
	}
	if (m_entity != nullptr && m_entity->m_map->m_gameStats.m_numOfLongRangeArmyTeam2 > 5 && m_entity->m_teamID == 2)
	{
		m_negativeScores += 5;
		return;
	}
	m_longRangeArmySpawned += count;
}
//////////////////
void ScoreCard::UpdateScoreHousesAttacked(int count)
{
	m_housesAttacked += count;
}
void ScoreCard::UpdateScoreArmySpawnersAttacked(int count)
{
	m_armySpawnersAttacked += count;
}
void ScoreCard::UpdateScoreCiviliansAttacked(int count)
{
	m_civiilansAttacked += count;
}
void ScoreCard::UpdateScoreShortRangeArmyAttacked(int count)
{
	m_shortRangeArmyAttacked += count;
}
void ScoreCard::UpdateScoreLongRangeArmyAttacked(int count)
{
	m_longRangeArmyAttacked += count;
}

void ScoreCard::UpdateScoreTownCenterAttacked(int count)
{
	m_townCenterAttacked += count;
}

///////////////////
void ScoreCard::UpdateScoreHousesDestroyed(int count)
{
	m_housesDestroyed += count;
}
void ScoreCard::UpdateScoreArmySpawnersDestroyed(int count)
{
	m_armySpawnersDestroyed += count;
}
void ScoreCard::UpdateScoreCiviliansKilled(int count)
{
	m_civiliansKilled += count;
}
void ScoreCard::UpdateScoreShortRangeArmyKilled(int count)
{
	m_shortRangeArmyKilled += count;
}
void ScoreCard::UpdateScoreLongRangeArmyKilled(int count)
{
	m_longRangeArmyKilled += count;
}

void ScoreCard::UpdateScoreTownCenterDestroyed(int count)
{
	m_townCenterDestroyed += count;
}
