#pragma once
#include "Engine/AI/GA/Gene.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetwork.hpp"
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
	NeuralNetwork *m_neuralNet;
	//Static_Member_Variables
	
	//Methods

	//bool		    operator	== (const Vector2& compare) const;

	NeuralNetworkGene(float mutateChance);
	NeuralNetworkGene(const NeuralNetworkGene& copy);
	~NeuralNetworkGene();

	void				Mutate();
	Gene*			    Clone();
	float				GetFitnessValue(Gene *target);
	bool				IsAlmostEqual(Gene *gene);
	NeuralNetwork*		GetNeuralNetwork();
	void				CreateNeuralNetwork();
	void				UpdateWithInputs(std::vector<float>& inputs);
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