#pragma once
#include "Engine/AI/NeuralNetwork/Neuron.hpp"
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
	char   m_char;
	float  m_fitnessValue;
	float  m_mutateChance;
	//Static_Member_Variables

	//Methods
	bool operator==(const Gene& obj) const;
	//bool		    operator	== (const Vector2& compare) const;			// vec2 == vec2

	Gene(float mutateChance);
	Gene(const Gene& copy);
	~Gene();
	
	void  Mutate();
	Gene* Clone();
	bool  IsAlmostEqual(Gene *gene);
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