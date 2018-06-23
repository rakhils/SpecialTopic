#pragma once
#include "Engine/AI/GA/Gene.hpp"
/*\class  : SimpleCharGene		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :	   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/22/2018 7:17:41 PM
* \contact: srsrakhil@gmail.com
*/
 
class SimpleCharGene : public Gene
{

public:
	//Member_Variables
	char   m_char;
	//Static_Member_Variables

	//Methods

	SimpleCharGene(float mutationChance);
	SimpleCharGene(const SimpleCharGene &copy);
	~SimpleCharGene();

	void  Mutate();
	Gene* Clone();
	float GetFitnessValue(Gene *target);
	bool  IsAlmostEqual  (Gene *gene);

	
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