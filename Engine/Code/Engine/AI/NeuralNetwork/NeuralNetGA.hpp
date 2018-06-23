#pragma once
#include "Engine/AI/GA/Abstract/GeneticAlgorithm.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetwork.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetworkGene.hpp"
/*\class  : NeuralNetGA		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :	   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0	   
* \date   : 6/22/2018 11:06:31 PM
* \contact: srsrakhil@gmail.com
*/
 
class NeuralNetGA : public GeneticAlgorithm
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods
	
	NeuralNetGA(int chromosomeCount, int geneLength, float crossOverChance, float mutationRate,std::vector<Gene*>& inputs);
	~NeuralNetGA();

	void				CreatePopulation(int chromosomeCount,int geneLength,std::vector<Gene*>& inputs);
	void				DoCrossOverAndSetNewPopulation();
	void				DoMutation();
	void				Evaluate();
	void				UpdateWithInputs(std::vector<float>& inputs);
	std::vector<float>  GetOutputs();
	void				Update();
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