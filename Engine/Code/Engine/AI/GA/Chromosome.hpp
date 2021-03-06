#pragma once
#include <vector>
#include "Engine/AI/GA/Gene.hpp"
#include "Engine/AI/GA/GA_Constants.h"
/*\class  : Chromosome		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/13/2018 2:26:21 PM
* \contact: srsrakhil@gmail.com
*/
class Population;
class Chromosome
{

public:
	//Member_Variables
	std::vector<Gene*> m_genes;
	float			   m_totalFitness    = 0.01f;
	int				   m_geneCount		 = 0;
	float			   m_mutationChance  = 0.f;
	//Static_Member_Variables

	//Methods

	Chromosome(int geneCount, float mutationChance, GA_TYPE type);
	Chromosome(int geneCount, float mutationChance, GA_TYPE type,std::vector<Gene*>& inputs);
	~Chromosome();

	void		 CreateInitialRandomChromosome(int geneCount, float mutationChance, GA_TYPE type);
	void		 CreateInitialRandomChromosome(int geneCount, float mutationChance, GA_TYPE type,std::vector<Gene*>& inputs);
	float		 GetTotalFitness(Chromosome* target);
	void		 CrossOverAndSet(Chromosome* ch1,Chromosome *chnewPopulation);
	void		 Mutate();
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