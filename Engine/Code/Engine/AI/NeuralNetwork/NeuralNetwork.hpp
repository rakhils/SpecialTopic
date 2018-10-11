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
	float				m_value = 0;
	float				m_sumOfPreviousLayer = 0;
	int					m_numberOfWeights;
	std::vector<float>  m_weights;
	std::vector<float>  m_oldWeights;
	Neuron(){}
	Neuron(int numberOfWeights)
	{
		m_numberOfWeights = numberOfWeights;
		Init();
	}

	void Init()
	{
		m_weights.reserve(m_numberOfWeights);
		m_oldWeights.reserve(m_numberOfWeights);
		for (int weightIndex = 0; weightIndex < m_numberOfWeights; weightIndex++)
		{
			m_weights.push_back(GetRandomFloatInRange(-1, 1));
			m_oldWeights.push_back(GetRandomFloatInRange(-1, 1));
		}
	}

	void SetRandomWeights()
	{
		for (int weightIndex = 0; weightIndex < m_numberOfWeights; weightIndex++)
		{
			m_weights.at(weightIndex) = GetRandomFloatInRange(-1, 1);
			m_oldWeights.at(weightIndex) = GetRandomFloatInRange(-1, 1);
		}
	}
	
};
struct NeuronLayer
{
	int m_numberOfNeurons;
	std::vector<Neuron> m_neurons;
	Neuron				m_bias;
	NeuronLayer(int numberOfNeurons,int numberOfWeights)
	{
		m_numberOfNeurons = numberOfNeurons;
		m_neurons.reserve(numberOfNeurons);
		for(int neuronIndex = 0;neuronIndex < m_numberOfNeurons;neuronIndex++)
		{
			Neuron neuron(numberOfWeights);
			m_neurons.push_back(neuron);
		}
		m_bias.m_numberOfWeights = numberOfWeights;
		m_bias.m_value = GetRandomFloatInRange(0, 1);
		m_bias.Init();
	}
	~NeuronLayer()
	{

	}
	void SetRandomWeights()
	{
		for (int neuronIndex = 0; neuronIndex < m_numberOfNeurons; neuronIndex++)
		{
			m_neurons.at(neuronIndex).SetRandomWeights();
		}
		m_bias.SetRandomWeights();
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
	
	int					 GetNumberOfInputs();
	int					 GetNumberOfHiddenLayerNeuron();
	int					 GetNumberOfOutputs();

	void				 SetRandomWeight();
	void				 CreateNeuralNetwork(int numberOfInputNeuron, int numberOfHiidenNeurons, int numberOfOutputNeuron);
	void				 FeedForward(std::vector<float> &inputs);
	void				 FeedForward(std::vector<float> &inputs,std::vector<float> &inputs1);

	void				 DoBackPropogation(std::vector<float> &knownOutputs);
	void				 BackPropogateOuputToHiddenLayer(std::vector<float> &outputErrors);
	void				 BackPropogateHiddenToInputLayer(std::vector<float> &outputErrors);
	void				 BackPropogateHiddenToInputLayer1(std::vector<float> &outputErrors);

	float				 GetSumOfWeightsInHiddenLayer(int outputIndex);
	void				 GetOutputs();
	void				 FeedForwardNN();
	float				 GetActivationValue(float value);
	float				 GetFastSigmoidValue(float value);
	float				 GetFastSigmoidDerivative(float value);
	float				 GetSigmoidDerivative(float value);
	float				 GetSigmoidValue(float value);
	void				 Mutate();
	bool				 CopyWeightsTo(NeuralNetwork &copy);
	void				 CrossOver(NeuralNetwork &second);
	void				 StoreToFile(const char* filePathname);
	void				 LoadFromFile(const char* filePath);
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