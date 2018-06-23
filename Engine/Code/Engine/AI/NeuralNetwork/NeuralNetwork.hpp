#pragma once
#include <vector>
#include "Engine/Math/MathUtil.hpp"
/*\class  : NeuralNetworl		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 6/20/2018 2:10:47 PM
* \contact: srsrakhil@gmail.com
*/
struct Neuron
{
	float				m_value;
	int					m_numberOfWeights;
	std::vector<float>  m_weights;
	Neuron(int numberOfWeights)
	{
		m_numberOfWeights = numberOfWeights; 
		m_weights.reserve(numberOfWeights);
		for(int weightIndex = 0;weightIndex < m_numberOfWeights;weightIndex++)
		{
			m_weights.push_back(GetRandomFloatInRange(-0.5,0.5));
		}
	}
	
};
struct NeuronLayer
{
	int m_numberOfNeurons;
	std::vector<Neuron> m_neurons;
	NeuronLayer(int numberOfNeurons,int numberOfWeights)
	{
		m_numberOfNeurons = numberOfNeurons;
		m_neurons.reserve(numberOfNeurons);
		for(int neuronIndex = 0;neuronIndex < m_numberOfNeurons;neuronIndex++)
		{
			Neuron neuron(numberOfWeights);
			m_neurons.push_back(neuron);
		}
	}
};
class NeuralNetwork
{

public:
	//Member_Variables
	bool		 m_init			 = false;
	NeuronLayer *m_inputs		 = nullptr;
	NeuronLayer *m_hiddenLayers	 = nullptr;
	NeuronLayer *m_outputs		 = nullptr;
	//Static_Member_Variables

	//Methods
	NeuralNetwork();
	NeuralNetwork(int numberOfInputNeuron,int numberOfHiidenNeurons,int numberOfOutputNeuron);
	~NeuralNetwork();
	
	void				 CreateNeuralNetwork(int numberOfInputNeuron, int numberOfHiidenNeurons, int numberOfOutputNeuron);
	std::vector<float>&  GetOutputs();
	void				 Update();
	float				 GetActivationValue(float value);
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