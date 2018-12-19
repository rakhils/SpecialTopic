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
	double				 m_value = 0;
	double				 m_sumOfPreviousLayer = 0;
	int					 m_numberOfWeights;
	std::vector<double>  m_weights;
	std::vector<double>  m_oldWeights;
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
			m_weights.push_back   (static_cast<double>(GetRandomFloatInRange(-1, 1)));
			m_oldWeights.push_back(static_cast<double>(GetRandomFloatInRange(-1, 1)));
		}
	}

	void SetRandomWeights()
	{
		for (int weightIndex = 0; weightIndex < m_numberOfWeights; weightIndex++)
		{
			m_weights.at(weightIndex)    = static_cast<double>(GetRandomFloatInRange(-1, 1));
			m_oldWeights.at(weightIndex) = static_cast<double>(GetRandomFloatInRange(-1, 1));
		}
	}
};
struct NeuronLayer
{
	int					m_numberOfNeurons = 0;
	std::vector<Neuron> m_neurons;
	Neuron				m_bias;
	bool				m_biasEnabled	  = true;

	NeuronLayer(int numberOfNeurons,int numberOfWeights)
	{
		InitNeuronsLayer(numberOfNeurons, numberOfWeights, false);
	}

	NeuronLayer(int numberOfNeurons, int numberOfWeights,bool enableBias)
	{
		InitNeuronsLayer(numberOfNeurons, numberOfWeights, enableBias);
	}

	~NeuronLayer()
	{

	}

	void InitNeuronsLayer(int numberOfNeurons, int numberOfWeights, bool enableBias)
	{
		m_numberOfNeurons = numberOfNeurons;
		m_biasEnabled = enableBias;
		m_neurons.reserve(numberOfNeurons);
		for (int neuronIndex = 0; neuronIndex < m_numberOfNeurons; neuronIndex++)
		{
			Neuron neuron(numberOfWeights);
			m_neurons.push_back(neuron);
		}
		if (m_biasEnabled)
		{
			m_bias.m_numberOfWeights = numberOfWeights;
			m_bias.m_value = static_cast<double>(GetRandomFloatInRange(0, 1));
			m_bias.Init();
		}
	}
	void SetRandomWeights()
	{
		for (int neuronIndex = 0; neuronIndex < m_numberOfNeurons; neuronIndex++)
		{
			m_neurons.at(neuronIndex).SetRandomWeights();
		}
		if(m_biasEnabled)
		{
			m_bias.SetRandomWeights();
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
	
	int					 GetNumberOfInputs();
	int					 GetNumberOfHiddenLayerNeuron();
	int					 GetNumberOfOutputs();

	void				 SetRandomWeight();
	void				 CreateNeuralNetwork(int numberOfInputNeuron, int numberOfHiidenNeurons, int numberOfOutputNeuron);
	void				 FeedForward(std::vector<double> &inputs);
	void				 FeedForward(std::vector<double> &inputs,std::vector<double> &inputs1);

	void				 DoBackPropogation(std::vector<double> &knownOutputs);
	void				 BackPropogateOuputToHiddenLayer(std::vector <double> &knownOutputs);
	void				 BackPropogateHiddenToInputLayer(std::vector <double> &knownOutputs);
	void				 BackPropogateHiddenToInputLayer1(std::vector<double> &knownOutputs);

	void				 GetOutputs();
	void				 FeedForwardNN();
	double				 GetSumOfWeightsInHiddenLayer(int outputIndex);
	double				 GetActivationValue(double value);
	double				 GetFastSigmoidValue(double value);
	double				 GetFastSigmoidDerivative(double value);
	double				 GetSigmoidDerivative(double value);
	double				 GetSigmoidValue(double value);
	void				 Mutate(float mutationRate,float mutationScale);
	bool				 CopyWeightsTo(NeuralNetwork &copy);
	void				 CrossOver(NeuralNetwork &second);
	void				 StoreToFile(const char* filePathname);
	bool				 LoadFromFile(const char* filePath);
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