#pragma once
/*\class  : Gene		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/13/2018 2:28:19 PM
* \contact: srsrakhil@gmail.com
*/
 
class Gene
{

public:
	//Member_Variables
	float  m_fitnessValue;
	float  m_mutateChance;
	//Static_Member_Variables

	//Methods
	
	Gene(float mutateChance);
	Gene(const Gene& copy);
	~Gene();
	
	virtual	void  Mutate()					    = 0;
	virtual	Gene* Clone()					    = 0;
	virtual float GetFitnessValue(Gene *target) = 0;
	virtual bool  IsAlmostEqual  (Gene *gene)   = 0;
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