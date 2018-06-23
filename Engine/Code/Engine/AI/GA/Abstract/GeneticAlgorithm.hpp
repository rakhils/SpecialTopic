#pragma once
#include "Engine/AI/GA/Chromosome.hpp"
#include "Engine/AI/GA/Population.hpp"
#include "Engine/AI/GA/GA_Constants.h"
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
	Population*				 m_population					 = nullptr;
	Population*				 m_newPopulation				 = nullptr;
	Chromosome*				 m_target					     = nullptr;
	Chromosome*				 m_best						     = nullptr;
	int						 m_currentGenerationCount		 = 0;
	int 					 m_chromosomeCount				 = 1;
	int 					 m_geneLength					 = 1;
	float					 m_mutationChance			     = 0.0f;
	float					 m_crossOverChance			     = 0.f;
	bool					 m_isFinished					 = false;
	float					 m_averageFitnessValue			 = 0.f;
	//Static_Member_Variables

	//Methods

	GeneticAlgorithm(int initialPopulation, int geneLength, float crossOverChance, float mutationRate, GA_TYPE type);
	GeneticAlgorithm(int initialPopulation, int geneLength, float crossOverChance, float mutationRate, GA_TYPE type,std::vector<Gene*>& inputs);

	GeneticAlgorithm();
	virtual ~GeneticAlgorithm();

	void				 SetTarget				(Chromosome* chromosome);
	void	     		 CreateInitialPopulation(GA_TYPE type);
	void				 CreateInitialPopulation(GA_TYPE type, std::vector<Gene*>& genes);
	void				 UpdateFitnessValue();
	bool				 Epoch();

	virtual void	     DoCrossOverAndSetNewPopulation()					= 0;
	virtual void		 DoMutation ()										= 0;
	virtual void		 Evaluate()											= 0;

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