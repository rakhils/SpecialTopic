#pragma once
#include <vector>
#include "Engine/AI/GA/Chromosome.hpp"
#include "Engine/AI/GA/GA_Constants.h"
/*\class  : Population		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/13/2018 2:27:47 PM
* \contact: srsrakhil@gmail.com
*/
 
class Population
{

public:
	//Member_Variables
	int   m_chromosomeCount = 0;
	float m_crossOverChance = 0.f;
	std::vector<Chromosome*> m_chromosomes;
	//Static_Member_Variables

	//Methods

	Population(int chromosomeCount, int genecount, float crossOverChance, float mutationChance, GA_TYPE type);
	Population(int chromosomeCount, int genecount, float crossOverChance, float mutationChance, GA_TYPE type, std::vector<Gene*>& inputs);

	~Population();

	void		 CreatesNewPopulation(int chromosomeCount, int genecount, float crossOverChance, float mutationChance, GA_TYPE type);
	void		 CreatesNewPopulation(int chromosomeCount, int genecount, float crossOverChance, float mutationChance, GA_TYPE type,std::vector<Gene*>& inputs);

	bool		 IsFinished();
	Chromosome*  GetBestOffspring();
	Chromosome*  PickRandomChromosomeByWeight(int m_maxFitness);
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