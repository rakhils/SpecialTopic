#pragma once
#include "Engine/AI/GA/Chromosome.hpp"
#include "Engine/AI/GA/Population.hpp"
/*\class  : GeneticAlgorithm   
* \group  : <GroupName>   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/13/2018 5:52:14 PM
* \contact: srsrakhil@gmail.com
*/
 
class GeneticAlgorithm
{

public:
	//Member_Variables
	Population*				 m_population				 = nullptr;
	Population*				 m_newPopulation			 = nullptr;
	Chromosome*				 m_target					 = nullptr;
	Chromosome*				 m_best						 = nullptr;
	int						 m_currentGenerationCount  	 = 0;
	int 					 m_initPopulation;
	int 					 m_geneLength;
	float					 m_mutationRate;
	bool					 m_isFinished				 = false;
	float					 m_averageFitnessValue		 = 0.f;
	//Static_Member_Variables

	//Methods

	GeneticAlgorithm(int initialPopulation,int geneLength,float mutationRate);
	~GeneticAlgorithm();
	
	void  SetTarget(Chromosome* chromosome);
	void  CreateInitialPopulation();
	void  UpdateFitnessValue();
	bool  Epoch();
	void  GenerateNewPopulation();
	void  Evaluate();

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