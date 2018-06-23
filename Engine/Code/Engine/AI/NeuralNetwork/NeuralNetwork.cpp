#include "Engine/AI/NeuralNetwork/NeuralNetwork.hpp"

// CONSTRUCTOR
NeuralNetwork::NeuralNetwork()
{
	CreateNeuralNetwork(100,36,2);
}

NeuralNetwork::NeuralNetwork(int numberOfInputNeurons, int numberOfHiidenNeurons, int numberOfOutputNeuron)
{
	CreateNeuralNetwork(numberOfInputNeurons, numberOfHiidenNeurons, numberOfOutputNeuron);
}

// DESTRUCTOR
NeuralNetwork::~NeuralNetwork()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/20
*@purpose : Creating neural network give inputs hidden layers and output neurons
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::CreateNeuralNetwork(int numberOfInputNeurons, int numberOfHiidenNeurons, int numberOfOutputNeuron)
{
	if(m_init)
	{
		return;
	}
	m_init			= true;
	//Current Neurons and weights for the  next layer
	m_inputs        = new NeuronLayer(numberOfInputNeurons , numberOfHiidenNeurons);
	m_hiddenLayers  = new NeuronLayer(numberOfHiidenNeurons, numberOfOutputNeuron);
	m_outputs		= new NeuronLayer(numberOfOutputNeuron , 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : Get all outputs as floats
*@param   : NIL
*@return  : Outputs as vector of floats
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<float>& NeuralNetwork::GetOutputs()
{
	std::vector<float> outputs;
	for(int index = 0;index < m_outputs->m_neurons.size() ;index++)
	{
		float value = m_outputs->m_neurons.at(index).m_value;
		outputs.push_back(value);
	}
	return outputs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : Updates the NN each time
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::Update()
{
	for (int hiddenIndex = 0; hiddenIndex < m_hiddenLayers->m_neurons.size(); hiddenIndex++)
	{
		float sum = 0;
		for (int index = 0; index < m_inputs->m_neurons.size(); index++)
		{
			sum += (m_inputs->m_neurons.at(index).m_value)*(m_inputs->m_neurons.at(index).m_weights.at(hiddenIndex));		
		}
		m_hiddenLayers->m_neurons.at(hiddenIndex).m_value = GetActivationValue(sum);
	}

	for (int outputIndex = 0; outputIndex < m_outputs->m_neurons.size(); outputIndex++)
	{
		float sum = 0;
		for (int hiddenIndex = 0; hiddenIndex < m_hiddenLayers->m_neurons.size(); hiddenIndex++)
		{
			sum += (m_hiddenLayers->m_neurons.at(hiddenIndex).m_value)*(m_hiddenLayers->m_neurons.at(hiddenIndex).m_weights.at(outputIndex));
		}
		m_outputs->m_neurons.at(outputIndex).m_value = GetActivationValue(sum);
	}


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float NeuralNetwork::GetActivationValue(float value)
{
	if(value > 0)
	{
		return 1;
	}
	return -1;
}
