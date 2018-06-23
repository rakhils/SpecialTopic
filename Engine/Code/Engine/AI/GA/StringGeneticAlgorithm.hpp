#pragma once
#include "Engine/AI/GA/Chromosome.hpp"
#include "Engine/AI/GA/Population.hpp"
#include "Engine/AI/GA/GA_Constants.h"
#include "Engine/AI/GA/Abstract/GeneticAlgorithm.hpp"
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

class StringGeneticAlgorithm : public GeneticAlgorithm
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	StringGeneticAlgorithm(int initialPopulation, int geneLength, float crossOverChance, float mutationRate);
	~StringGeneticAlgorithm();
	     
	void		 DoCrossOverAndSetNewPopulation();
	void		 DoMutation();
	void		 Evaluate();

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