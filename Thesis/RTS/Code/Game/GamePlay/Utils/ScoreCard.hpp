#pragma once
#include "Engine/FileUtil/File.h"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Time.hpp"
/*\class  : ScoreCard		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 11/13/2018 4:59:08 PM
* \contact: srsrakhil@gmail.com
*/
class Entity;
struct ScoreCardByActions
{
	bool m_isSuccess = false;
	int  m_numOfAttackActions = 0;
	int  m_numOfExploreActions = 0;
	int  m_numOfFollowActions = 0;
	int  m_numOfPatrolActions = 0;
	int  m_numOfRetreatActions = 0;
	int  m_totalScore = 0;

	int  CalculateTotalScore()
	{
		m_totalScore = m_numOfAttackActions + m_numOfExploreActions + m_numOfFollowActions + m_numOfPatrolActions + m_numOfRetreatActions;
		return m_totalScore;
	}

	void Reset()
	{
		m_isSuccess = false;
		m_numOfAttackActions = 0;
		m_numOfExploreActions = 0;
		m_numOfFollowActions = 0;
		m_numOfPatrolActions = 0;
		m_numOfRetreatActions = 0;
		m_totalScore = 0;
	}

	std::string GetAsString()
	{
		std::string behaviorActions;
		behaviorActions     +=
			"TEAM STATS - " + ToString(m_isSuccess)           + "\n" +
			"ATTACK     - " + ToString(m_numOfAttackActions)  + "\n" +
			"EXPLORE    - " + ToString(m_numOfExploreActions) + "\n" +
			"FOLLOW     - " + ToString(m_numOfFollowActions)  + "\n" +
			"PATROL     - " + ToString(m_numOfPatrolActions)  + "\n" +
			"RETREAT    - " + ToString(m_numOfRetreatActions) + "\n" +
			"TOTAL      - " + ToString(m_numOfAttackActions   + m_numOfExploreActions + m_numOfFollowActions + m_numOfPatrolActions + m_numOfRetreatActions);
		return behaviorActions;
	}

	void ReadFromFile(char const *fileName)
	{
		std::string fileContent = GetFileContentAsString(fileName);
		std::vector<std::string> params;
		Split(params, fileContent, '\n');
		for(int index = 0;index < params.size();index++)
		{
			std::vector<std::string> paramValues;
			Split(paramValues, params.at(index), '-');
			if(StartsWith(paramValues.at(0),"ATTACK"))
			{
				paramValues.at(1) = TrimString(paramValues.at(1));
				ToInt(paramValues.at(1), &m_numOfAttackActions);
			}
			if (StartsWith(paramValues.at(0), "EXPLORE"))
			{
				paramValues.at(1) = TrimString(paramValues.at(1));
				ToInt(paramValues.at(1), &m_numOfExploreActions);
			}
			if (StartsWith(paramValues.at(0), "FOLLOW"))
			{
				paramValues.at(1) = TrimString(paramValues.at(1));
				ToInt(paramValues.at(1), &m_numOfFollowActions);
			}
			if (StartsWith(paramValues.at(0), "PATROL"))
			{
				paramValues.at(1) = TrimString(paramValues.at(1));
				ToInt(paramValues.at(1), &m_numOfPatrolActions);
			}
			if (StartsWith(paramValues.at(0), "RETREAT"))
			{
				paramValues.at(1) = TrimString(paramValues.at(1));
				ToInt(paramValues.at(1), &m_numOfRetreatActions);
			}
			if (StartsWith(paramValues.at(0), "TOTAL"))
			{
				paramValues.at(1) = TrimString(paramValues.at(1));
				ToInt(paramValues.at(1), &m_totalScore);
			}
		}
	}

	void SaveToFile(char const *fileName)
	{
		std::string fileContent = GetAsString();
		FileWrite(fileName, fileContent);
		std::string fileNameStr(fileName);
		std::vector<std::string> fileNameWithoutExtensionStr;
		Split(fileNameWithoutExtensionStr, fileNameStr, '.');

		std::string fileContents = ToString(m_numOfAttackActions)  + ","  +
								   ToString(m_numOfExploreActions) + ","  +
								   ToString(m_numOfFollowActions)  + ","  +
								   ToString(m_numOfPatrolActions)  + ","  +
								   ToString(m_numOfRetreatActions) + ","  +
								   ToString(m_totalScore); 
		FileWrite(fileNameWithoutExtensionStr.at(0) + "_TOTAL.txt", fileContents);
	}
	void SaveStatsToFile(char const *fileName)
	{
		std::string fileContent = ToString(m_totalScore) +"\n"+ GetAsString();
		FileWrite(fileName, fileContent);
		
		/*std::string fileNameStr(fileName);
		std::vector<std::string> fileNameWithoutExtensionStr;
		Split(fileNameWithoutExtensionStr, fileNameStr, '.');

		std::string fileContents = "TOTAL," + ToString(m_totalScore);
		FileWrite(fileNameWithoutExtensionStr.at(0) + "_TOTAL.txt", fileContents);*/
		//FileWrite(fileNameWithoutExtensionStr.at(0) + "_EXPLORE.txt", ToString(m_numOfAttackActions));
		//FileWrite(fileNameWithoutExtensionStr.at(0) + "_.txt", ToString(m_numOfAttackActions));
		//FileWrite(fileNameWithoutExtensionStr.at(0) + "_ATTACK.txt", ToString(m_numOfAttackActions));
		//FileWrite(fileNameWithoutExtensionStr.at(0) + "_ATTACK.txt", ToString(m_numOfAttackActions));

	}
};
class ScoreCard
{
public:
	Entity * m_entity = nullptr;
	float m_createdTime = static_cast<float>(GetCurrentTimeSeconds());
	bool  m_scoreCalculated = false;
	int m_resourceFoodCollected = 0;
	int m_resourceStoneCollected = 0;
	int m_resourceWoodCollected = 0;
	int m_housesBuilt = 0;
	int m_armySpawnersBuilt = 0;
	int m_civiliansSpawned = 0;
	int m_shortRangeArmySpawned = 0;
	int m_longRangeArmySpawned = 0;
	int m_housesAttacked = 0;
	int m_armySpawnersAttacked = 0;
	int m_civiilansAttacked = 0;
	int m_shortRangeArmyAttacked = 0;
	int m_longRangeArmyAttacked = 0;
	int m_townCenterAttacked = 0;
	int m_housesDestroyed = 0;
	int m_armySpawnersDestroyed = 0;
	int m_civiliansKilled = 0;
	int m_shortRangeArmyKilled = 0;
	int m_longRangeArmyKilled = 0;
	int m_townCenterDestroyed = 0;
	int m_totalSteps  = 0;

	int m_interimScore = 0;
	float m_scoreRate = 0.f;
	int m_negativeScores = 0;
	int m_bonusScore = 0;
	int m_totalScore = 0;
	int m_alive		 = 0;
	int m_health     = 0;


	int m_resourceFoodCollectedPoints = 1;
	int m_resourceStoneCollectedPoints = 1;
	int m_resourceWoodCollectedPoints = 1;
	int m_housesBuiltPoints = 10;
	int m_armySpawnersBuiltPoints = 16;
	int m_civiliansSpawnedPoints = 20;
	int m_shortRangeArmySpawnedPoints = 30;
	int m_longRangeArmySpawnedPoints = 32;
	int m_housesAttackedPoints = 2;
	int m_armySpawnerAttackedPoints = 3;
	int m_civiilansAttackedPoints = 4;
	int m_shortRangeArmyAttackedPoints = 6;
	int m_longRangeArmyAttackedPoints = 7;
	int m_townCenterAttackedPoints = 12;
	int m_housesDestroyedPoints = 4;
	int m_armySpawnerDestroyedPoints = 6;
	int m_civiliansKilledPoints = 8;
	int m_shortRangeArmyKilledPoints = 12;
	int m_longRangeArmyKilledPoints = 14;
	int m_townCenterDestroyedPoints = 1000;
	int m_alivePoints				= 100;
	int m_healthPoints				= 25;

	void operator=(ScoreCard &scard);

	void Reset();
	void CalculateTotalScore();
	int	 GetTotalScore();
	bool CalculateInterimScore(bool forced,int prevInterimScore);
	void SaveToFile(char const *fileName)
	{
		FILE *fp = FileOpenForWrite(fileName);
		FileAppendString(fp, ToString(m_totalScore) + "\n");
		FileAppendString(fp, "TOTAL_RESOURCE_FOOD_COLLECTED       : " + ToString(m_resourceFoodCollected) + "\n");
		FileAppendString(fp, "TOTAL_RESOURCE_STONE_COLLECTED      : " + ToString(m_resourceStoneCollected) + "\n");
		FileAppendString(fp, "TOTAL_RESOURCE_WOOD_COLLECTED       : " + ToString(m_resourceWoodCollected) + "\n");
		FileAppendString(fp, "TOTAL_HOUSES_BUILT                  : " + ToString(m_housesBuilt) + "\n");
		FileAppendString(fp, "TOTAL_ARMY_CENTRE_SPAWNED           : " + ToString(m_armySpawnersBuilt) + "\n");
		FileAppendString(fp, "TOTAL_CIVILIANS_SPAWNED             : " + ToString(m_civiliansSpawned) + "\n");
		FileAppendString(fp, "TOTAL_SHORT_RANGE_ARMY_SPAWNED      : " + ToString(m_shortRangeArmySpawned) + "\n");
		FileAppendString(fp, "TOTAL_LONG_RANGE_ARMY_SPAWNED       : " + ToString(m_longRangeArmySpawned) + "\n");

		FileAppendString(fp, "TOTAL_ATTACK_ON_HOUSES              : " + ToString(m_housesAttacked) + "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_ARMY_SPAWNERS       : " + ToString(m_armySpawnersAttacked) + "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_CIVILIANS           : " + ToString(m_civiilansAttacked) + "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_SHORT_RANGE_ARMIES  : " + ToString(m_shortRangeArmyAttacked) + "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_LONG_RANGE_ARMIES   : " + ToString(m_longRangeArmyAttacked) + "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_TOWNCENTERS         : " + ToString(m_townCenterAttacked) + "\n");

		FileAppendString(fp, "TOTAL_KILL_ON_HOUSES                : " + ToString(m_housesDestroyed) + "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_ARMY_SPAWNERS         : " + ToString(m_armySpawnersDestroyed) + "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_CIVILIANS             : " + ToString(m_civiliansKilled) + "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_SHORT_RANGE_ARMIES    : " + ToString(m_shortRangeArmyKilled) + "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_LONG_RANGE_ARMIES     : " + ToString(m_longRangeArmyKilled) + "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_TOWNCENTERS           : " + ToString(m_townCenterDestroyed) + "\n");

		FileAppendString(fp, "TOTAL_SCORE_RATE                    : " + ToString(m_scoreRate) + "\n");
		FileAppendString(fp, "TOTAL_NEGATIVE_SCORE                : " + ToString(m_negativeScores) + "\n");
		FileAppendString(fp, "TOTAL_BONUS_SCORE                   : " + ToString(m_bonusScore) + "\n");
		FileAppendString(fp, "TOTAL_FRAMES                        : " + ToString(m_totalSteps) + "\n");
		FileAppendString(fp, "ALIVE                               : " + ToString(m_alive) + "\n");
		FileAppendString(fp, "TOTAL_SCORE                         : " + ToString(m_totalScore) + "\n");
		FileClose(fp);
	}

	void UpdateScoreResourceFoodCollected(int count);
	void UpdateScoreResourceWoodCollected(int count);
	void UpdateScoreResourceStoneCollected(int count);
	void UpdateScoreHousesBuilt(int count);
	void UpdateScoreArmySpawnersBuilt(int count);
	void UpdateScoreCiviliansSpawned(int count);
	void UpdateScoreShortRangeArmySpawned(int count);
	void UpdateScoreLongRangeArmySpawned(int count);
	//////////////////
	void UpdateScoreHousesAttacked(int count);
	void UpdateScoreArmySpawnersAttacked(int count);
	void UpdateScoreCiviliansAttacked(int count);
	void UpdateScoreShortRangeArmyAttacked(int count);
	void UpdateScoreLongRangeArmyAttacked(int count);
	void UpdateScoreTownCenterAttacked(int count);
	///////////////////
	void UpdateScoreHousesDestroyed(int count);
	void UpdateScoreArmySpawnersDestroyed(int count);
	void UpdateScoreCiviliansKilled(int count);
	void UpdateScoreShortRangeArmyKilled(int count);
	void UpdateScoreLongRangeArmyKilled(int count);
	void UpdateScoreTownCenterDestroyed(int count);

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	ScoreCard();
	~ScoreCard();
	
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