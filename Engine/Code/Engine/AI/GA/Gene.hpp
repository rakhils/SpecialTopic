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
	Neuron m_neuron;
	char   m_char;
	float  m_fitnessValue;
	//Static_Member_Variables

	//Methods

	Gene();
	~Gene();
	
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