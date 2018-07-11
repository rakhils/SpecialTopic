#include "Engine/AI/NeuralNetwork/NeuralNetwork.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetworkConstants.h"
// CONSTRUCTOR
NeuralNetwork::NeuralNetwork()
{
	CreateNeuralNetwork(100,144,2);
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
/*DATE    : 2018/06/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::SetRandomWeight()
{
	m_inputs->SetRandomWeights();
	m_hiddenLayers->SetRandomWeights();
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
/*DATE    : 2018/07/09
*@purpose : Feeds the input forward
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::FeedForward(std::vector<float> &inputs)
{
	for (int inputIndex = 0; inputIndex < m_inputs->m_neurons.size(); inputIndex++)
	{
		if (inputIndex < inputs.size())
		{
			m_inputs->m_neurons.at(inputIndex).m_value = inputs.at(inputIndex);
		}
	}
	Update();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/06
*@purpose : Do the back prorogation algorithm to get the corrected weights
*@param   : Known outputs
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::DoBackPropogation(std::vector<float> &knownOutputs)
{
	std::vector<float> outputErrors;
	std::vector<float> hiddenLayerErrors;
	outputErrors.reserve(knownOutputs.size());
	hiddenLayerErrors.reserve(m_hiddenLayers->m_neurons.size());

	//CALCULATES THE OUTPUT ERRORS
	////////////////////////////////////////////////////////////////////////////
	for(size_t index = 0;index < m_outputs->m_neurons.size();index++)
	{
		float outputError = knownOutputs.at(index) - m_outputs->m_neurons.at(index).m_value;
		outputErrors.push_back(outputError);
	}
	////////////////////////////////////////////////////////////////////////////
	
	//CALCULATING HIDDEN ERRORS
	////////////////////////////////////////////////////////////////////////////
	std::vector<float> sumOfWeights;
	sumOfWeights.reserve(m_outputs->m_neurons.size());
	for(size_t outputIndex = 0;outputIndex < m_outputs->m_neurons.size();outputIndex++)
	{
		float sum = GetSumOfWeightsInHiddenLayer(outputIndex);
		sumOfWeights.push_back(sum);
	}
		
	for(size_t hiddenNeuronIndex = 0;hiddenNeuronIndex < m_hiddenLayers->m_neurons.size();hiddenNeuronIndex++)
	{
		float hiddenLayerError = 0.0f;
		for (size_t outputErrorIndex = 0; outputErrorIndex < m_outputs->m_neurons.size(); outputErrorIndex++)
		{
			float sumOfWeight      =  sumOfWeights.at(outputErrorIndex);
			float weight		   =  m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.at(outputErrorIndex);
			hiddenLayerError       += ( (weight * outputErrors.at(outputErrorIndex)) / sumOfWeight );
		}
		hiddenLayerErrors.push_back(hiddenLayerError);
	}
	TrainHiddenOutputLayer(outputErrors);
	TrainInputHiddenLayer(hiddenLayerErrors);
	/*////////////////////////////////////////////////////////////////////////////
	//  DELTA CHANGE IN ERROR
	//		: L.R				= Learning Rate
	//		: OE				= OUTPUT-ERRORS
	//		: O					= OUTPUT LAYER VALUES
	//		: H					= HIDDEN LAYER VALUES
	//		: ( O * (1 - O))	= SIGMOID DERIVATIVE OF OUTPUT
	//		: deltaW (for HiddenOutput) = L.R * OE * ( O * (1 - O)) * H
	// 
	////////////////////////////////////////////////////////////////////////////
	//CALCULATE SIGMOID DERIVATIVE FOR OUTPUT :- value * (1 - value)
	std::vector<float> outputSigmoidDerivative;
	outputSigmoidDerivative.reserve(m_outputs->m_neurons.size());
	for(size_t outputIndex = 0;outputIndex < m_outputs->m_neurons.size();outputIndex++)
	{
		float outputValue = m_outputs->m_neurons.at(outputIndex).m_value;
		outputSigmoidDerivative.push_back(GetFastSigmoidDerivative(outputValue));
	}

	////////////////////////////////////////////////////////////////////////////
	//CALCULATE L.R * OUPUT ERROR * SIGMOID DERIVATIVE OF OUTPUT :- L.R * OE * ( O * (1 - O))
	std::vector<float> outputGradient;
	
	for(size_t outputIndex = 0;outputIndex < m_outputs->m_neurons.size();outputIndex++)
	{
		float value = outputSigmoidDerivative.at(outputIndex) * outputErrors.at(outputIndex) * NEURALNET_LEARNING_RATE;
		outputGradient.push_back(value);
	}

	////////////////////////////////////////////////////////////////////////////
	//CALCULATE GRADIENT VALUE * HIDDEN INPUTS :- [[L.R * OE * ( O * (1 - O))]]  * H
	std::vector<float> deltaWeights;
	deltaWeights.reserve(static_cast<int>(outputGradient.size())*static_cast<int>(m_hiddenLayers->m_neurons.size()));
	for(size_t weightIndexHO = 0;weightIndexHO < m_outputs->m_neurons.size();weightIndexHO++)
	{
		for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_hiddenLayers->m_neurons.size(); hiddenLayerIndex++)
		{
			float deltaWeight = outputGradient.at(weightIndexHO) * m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_value;
			deltaWeights.push_back(deltaWeight);
		}
	}
	// ======================== DELTA WEIGHTS CALCULATED ========================
	// ADD ALL DELTA WEIGHTS TO CURRENT WEIGHTS
	for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_hiddenLayers->m_neurons.size(); hiddenLayerIndex++)
	{
		for (size_t weightIndex = 0; weightIndex < m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_weights.size(); weightIndex++)
		{
			size_t sum = (weightIndex + 1)*(hiddenLayerIndex + 1);
			m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_weights.at(weightIndex) += deltaWeights.at(sum);
		}
	}*/

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::TrainHiddenOutputLayer(std::vector<float> &outputErrors)
{
	////////////////////////////////////////////////////////////////////////////
	//  DELTA CHANGE IN ERROR
	//		: L.R				= Learning Rate
	//		: OE				= OUTPUT-ERRORS
	//		: O					= OUTPUT LAYER VALUES
	//		: H					= HIDDEN LAYER VALUES
	//		: ( O * (1 - O))	= SIGMOID DERIVATIVE OF OUTPUT
	//		: deltaW (for HiddenOutput) = L.R * OE * ( O * (1 - O)) * H
	// 
	////////////////////////////////////////////////////////////////////////////
	//CALCULATE SIGMOID DERIVATIVE FOR OUTPUT :- value * (1 - value)
	std::vector<float> outputSigmoidDerivative;
	outputSigmoidDerivative.reserve(m_outputs->m_neurons.size());
	for (size_t outputIndex = 0; outputIndex < m_outputs->m_neurons.size(); outputIndex++)
	{
		float outputValue = m_outputs->m_neurons.at(outputIndex).m_value;
		outputSigmoidDerivative.push_back(GetFastSigmoidDerivative(outputValue));
	}

	////////////////////////////////////////////////////////////////////////////
	//CALCULATE L.R * OUPUT ERROR * SIGMOID DERIVATIVE OF OUTPUT :- L.R * OE * ( O * (1 - O))
	std::vector<float> outputGradient;

	for (size_t outputIndex = 0; outputIndex < m_outputs->m_neurons.size(); outputIndex++)
	{
		float value = outputSigmoidDerivative.at(outputIndex) * outputErrors.at(outputIndex) * NEURALNET_LEARNING_RATE;
		outputGradient.push_back(value);
	}

	////////////////////////////////////////////////////////////////////////////
	//CALCULATE GRADIENT VALUE * HIDDEN INPUTS :- [[L.R * OE * ( O * (1 - O))]]  * H
	std::vector<float> deltaWeights;
	deltaWeights.reserve(static_cast<int>(outputGradient.size())*static_cast<int>(m_hiddenLayers->m_neurons.size()));
	for (size_t weightIndexHO = 0; weightIndexHO < m_outputs->m_neurons.size(); weightIndexHO++)
	{
		for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_hiddenLayers->m_neurons.size(); hiddenLayerIndex++)
		{
			float deltaWeight = outputGradient.at(weightIndexHO) * m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_value;
			deltaWeights.push_back(deltaWeight);
		}
	}
	// ======================== DELTA WEIGHTS CALCULATED ========================
	// ADD ALL DELTA WEIGHTS TO CURRENT WEIGHTS
	for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_hiddenLayers->m_neurons.size(); hiddenLayerIndex++)
	{
		for (size_t weightIndex = 0; weightIndex < m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_weights.size(); weightIndex++)
		{
			size_t sum = (weightIndex)+ (hiddenLayerIndex * m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_weights.size());
			m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_weights.at(weightIndex) += deltaWeights.at(sum);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::TrainInputHiddenLayer(std::vector<float> &outputErrors)
{
	////////////////////////////////////////////////////////////////////////////
	//  DELTA CHANGE IN ERROR
	//		: L.R				= Learning Rate
	//		: OE				= OUTPUT-ERRORS
	//		: O					= OUTPUT LAYER VALUES
	//		: H					= HIDDEN LAYER VALUES
	//		: ( O * (1 - O))	= SIGMOID DERIVATIVE OF OUTPUT
	//		: deltaW (for HiddenOutput) = L.R * OE * ( O * (1 - O)) * H
	// 
	////////////////////////////////////////////////////////////////////////////
	//CALCULATE SIGMOID DERIVATIVE FOR OUTPUT :- value * (1 - value)
	std::vector<float> outputSigmoidDerivative;
	outputSigmoidDerivative.reserve(m_hiddenLayers->m_neurons.size());
	for (size_t outputIndex = 0; outputIndex < m_hiddenLayers->m_neurons.size(); outputIndex++)
	{
		float outputValue = m_hiddenLayers->m_neurons.at(outputIndex).m_value;
		outputSigmoidDerivative.push_back(GetFastSigmoidDerivative(outputValue));
	}

	////////////////////////////////////////////////////////////////////////////
	//CALCULATE L.R * OUPUT ERROR * SIGMOID DERIVATIVE OF OUTPUT :- L.R * OE * ( O * (1 - O))
	std::vector<float> outputGradient;
	outputGradient.reserve(m_hiddenLayers->m_neurons.size());
	for (size_t outputIndex = 0; outputIndex < m_hiddenLayers->m_neurons.size(); outputIndex++)
	{
		float value = outputSigmoidDerivative.at(outputIndex) * outputErrors.at(outputIndex) * NEURALNET_LEARNING_RATE;
		outputGradient.push_back(value);
	}

	////////////////////////////////////////////////////////////////////////////
	//CALCULATE GRADIENT VALUE * HIDDEN INPUTS :- [[L.R * OE * ( O * (1 - O))]]  * H
	std::vector<float> deltaWeights;
	deltaWeights.reserve(static_cast<int>(outputGradient.size())*static_cast<int>(m_inputs->m_neurons.size()));
	for (size_t weightIndexHO = 0; weightIndexHO < m_hiddenLayers->m_neurons.size(); weightIndexHO++)
	{
		for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_inputs->m_neurons.size(); hiddenLayerIndex++)
		{
			float deltaWeight = outputGradient.at(weightIndexHO) * m_inputs->m_neurons.at(hiddenLayerIndex).m_value;
			deltaWeights.push_back(deltaWeight);
		}
	}
	// ======================== DELTA WEIGHTS CALCULATED ========================
	// ADD ALL DELTA WEIGHTS TO CURRENT WEIGHTS
	for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_inputs->m_neurons.size(); hiddenLayerIndex++)
	{
		for (size_t weightIndex = 0; weightIndex < m_inputs->m_neurons.at(hiddenLayerIndex).m_weights.size(); weightIndex++)
		{
			size_t sum = (weightIndex)+ (hiddenLayerIndex * m_inputs->m_neurons.at(hiddenLayerIndex).m_weights.size());

			m_inputs->m_neurons.at(hiddenLayerIndex).m_weights.at(weightIndex) += deltaWeights.at(sum);
		}
	}

	/*for (size_t weightIndex = 0; weightIndex < m_hiddenLayers->m_neurons.size(); weightIndex++)
	{
		for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_inputs->m_neurons.size(); hiddenLayerIndex++)
		{

			size_t sum = (weightIndex)+(hiddenLayerIndex * m_inputs->m_neurons.at(hiddenLayerIndex).m_weights.size());

			m_inputs->m_neurons.at(hiddenLayerIndex).m_weights.at(weightIndex) += deltaWeights.at(sum);

		}
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/06
*@purpose : Gets the hidden layer weight index
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float NeuralNetwork::GetSumOfWeightsInHiddenLayer(int outputIndex)
{
	float sum = 0.f;
	for(int hiddenLayerIndex = 0;hiddenLayerIndex < m_hiddenLayers->m_neurons.size();hiddenLayerIndex++)
	{
		sum += m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_weights.at(outputIndex);
	}
	return sum;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : Get all outputs as floats
*@param   : NIL
*@return  : Outputs as vector of floats
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::GetOutputs()
{
	std::vector<float> outputs;
	for(int index = 0;index < m_outputs->m_neurons.size() ;index++)
	{
		float value = m_outputs->m_neurons.at(index).m_value;
		outputs.push_back(value);
	}
	//return outputs;
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
	return GetSigmoidValue(value);
	/*if(value > 0)
	{
		return 1;
	}
	return -1;*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/06
*@purpose : Calculates the fast sigmoid function
*@param   : Value
*@return  : Corresponding sigmoid function
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float NeuralNetwork::GetFastSigmoidValue(float value)
{
	return (value / (1 + GetAbsolute(value)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/06
*@purpose : returns the derivative of the sigmoid function
*@param   : Value
*@return  : Derivative sigmoid function
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float NeuralNetwork::GetFastSigmoidDerivative(float value)
{
	return (value) * (1 - (value));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float NeuralNetwork::GetSigmoidValue(float value)
{
	float e = 2.7183;
	return 1.f / (1.f + pow(e, -value));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : Mutates all weights of NN
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::Mutate()
{

	for(int inputNeuronIndex = 0;inputNeuronIndex < m_inputs->m_neurons.size();inputNeuronIndex++)
	{
		for(int inputNeuronWeightIndex = 0;inputNeuronWeightIndex < m_inputs->m_neurons.at(inputNeuronIndex).m_weights.size();inputNeuronWeightIndex++)
		{
			if(GetRandomFloatZeroToOne() < 0.1)
			{
				float random = GetRandomFloatInRange(-0.5, 0.5);
				float weight = m_inputs->m_neurons.at(inputNeuronIndex).m_weights.at(inputNeuronWeightIndex) + random;
				weight = ClampFloat(weight, -1, 1);
				m_inputs->m_neurons.at(inputNeuronIndex).m_weights.at(inputNeuronWeightIndex) = weight;
			}
		}
	}
	for(int hiddenNeuronIndex = 0;hiddenNeuronIndex < m_hiddenLayers->m_neurons.size();hiddenNeuronIndex++)
	{
		for (int hiddenNeuronWeightIndex = 0; hiddenNeuronWeightIndex < m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.size(); hiddenNeuronWeightIndex++)
		{
			if (GetRandomFloatZeroToOne() < 0.1)
			{
				float random = GetRandomFloatInRange(-0.5, 0.5);
				float weight = m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.at(hiddenNeuronWeightIndex) + random;
				weight = ClampFloat(weight, -1, 1);
				m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.at(hiddenNeuronWeightIndex) = weight;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : Copy all weights of neural net
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NeuralNetwork::CopyWeights(NeuralNetwork &copy)
{
	if(m_hiddenLayers->m_neurons.size() != copy.m_hiddenLayers->m_neurons.size())
	{
		return false;
	}
	if(m_outputs->m_neurons.size()      != copy.m_outputs->m_neurons.size())
	{
		return false;
	}
	for (int inputNeuronIndex = 0; inputNeuronIndex < m_inputs->m_neurons.size(); inputNeuronIndex++)
	{
		for (int inputNeuronWeightIndex = 0; inputNeuronWeightIndex < m_inputs->m_neurons.at(inputNeuronIndex).m_weights.size(); inputNeuronWeightIndex++)
		{
			float myweight = m_inputs->m_neurons.at(inputNeuronIndex).m_weights.at(inputNeuronWeightIndex);
			copy.m_inputs->m_neurons.at(inputNeuronIndex).m_weights.at(inputNeuronWeightIndex) = myweight;
		}
		//Neuron neuron(m_inputs->m_neurons.at(inputNeuronIndex).m_numberOfWeights);
		//copy.m_inputs->m_neurons.push_back(neuron);
	}
	for (int hiddenNeuronIndex = 0; hiddenNeuronIndex < m_hiddenLayers->m_neurons.size(); hiddenNeuronIndex++)
	{
		for (int hiddenNeuronWeightIndex = 0; hiddenNeuronWeightIndex < m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.size(); hiddenNeuronWeightIndex++)
		{
			float myweight = m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.at(hiddenNeuronWeightIndex);
			copy.m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.at(hiddenNeuronWeightIndex) = myweight;
		}
		//Neuron neuron(m_inputs->m_neurons.at(hiddenNeuronIndex).m_numberOfWeights);
		//copy.m_hiddenLayers->m_neurons.push_back(neuron);
	}
	return true;
	//for(int outputIndex = 0;outputIndex < m_outputs->m_neurons.size();outputIndex++)
	//{
	//	Neuron neuron(0);
	//	copy.m_outputs->m_neurons.push_back(neuron);
	//}
}
