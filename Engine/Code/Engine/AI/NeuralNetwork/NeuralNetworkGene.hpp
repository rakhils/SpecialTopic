#pragma once
#include "Engine/AI/GA/Gene.hpp"
/*\class  : NeuralNetworkGene		   
* \group  : <GroupName>	   
* \brief  :	   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/15/2018 3:38:38 PM
* \contact: srsrakhil@gmail.com
*/
 
class NeuralNetworkGene : public Gene
{

public:
	//Member_Variables

	//Static_Member_Variables

	//Methods
	bool operator==(const NeuralNetworkGene& obj) const;
	//bool		    operator	== (const Vector2& compare) const;

	NeuralNetworkGene(float mutateChance);
	NeuralNetworkGene(const NeuralNetworkGene& copy);
	~NeuralNetworkGene();

	void				Mutate();
	NeuralNetworkGene*  Clone();
	bool				IsAlmostEqual(NeuralNetworkGene *gene);
	
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