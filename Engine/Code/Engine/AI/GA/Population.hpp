#pragma once
#include <vector>
#include "Engine/AI/GA/Chromosome.hpp"
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
	int m_chromosomeCount = 0;
	std::vector<Chromosome*> m_chromosomes;
	//Static_Member_Variables

	//Methods

	Population(int chromosomeCount,int genecount,float mutationChance);
	~Population();

	bool		 IsFinished();
	Chromosome*  GetBestOffspring();
	Chromosome*  AcceptReject(int m_maxFitness);
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