#pragma once
#include <vector>
#include "Engine/AI/GA/Gene.hpp"
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
class Chromosome
{

public:
	//Member_Variables
	std::vector<Gene> m_genes;
	//Static_Member_Variables

	//Methods

	Chromosome();
	~Chromosome();
	
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