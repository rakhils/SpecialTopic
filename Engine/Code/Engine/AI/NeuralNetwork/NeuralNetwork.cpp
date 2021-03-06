#include "Engine/AI/NeuralNetwork/NeuralNetwork.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetworkConstants.h"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/FileUtil/File.h"
#include "Engine/Core/StringUtils.hpp"

// CONSTRUCTOR
NeuralNetwork::NeuralNetwork()
{
	//CreateNeuralNetwork(100,144,2);
}

NeuralNetwork::NeuralNetwork(int numberOfInputNeurons, int numberOfHiidenNeurons, int numberOfOutputNeuron)
{
	CreateNeuralNetwork(numberOfInputNeurons, numberOfHiidenNeurons, numberOfOutputNeuron);
}

// DESTRUCTOR
NeuralNetwork::~NeuralNetwork()
{
	if(m_inputs!=nullptr)
	{
		delete m_inputs;
		m_inputs = nullptr;
	}
	if(m_hiddenLayers != nullptr)
	{
		delete m_hiddenLayers;
		m_hiddenLayers = nullptr;
	}
	if(m_outputs != nullptr)
	{
		delete m_outputs;
		m_outputs = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/13
*@purpose : Retrieves number of inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NeuralNetwork::GetNumberOfInputs()
{
	return static_cast<int>(m_inputs->m_neurons.size());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/13
*@purpose : Gets number of hiiden layer count
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NeuralNetwork::GetNumberOfHiddenLayerNeuron()
{
	return static_cast<int>(m_hiddenLayers->m_neurons.size());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/13
*@purpose : gets number of outputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NeuralNetwork::GetNumberOfOutputs()
{
	return static_cast<int>(m_outputs->m_neurons.size());
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
void NeuralNetwork::FeedForward(std::vector<double> &inputs)
{
	// FEED INPUTS
	for (int inputIndex = 0; inputIndex < m_inputs->m_neurons.size(); inputIndex++)
	{
		if (inputIndex < inputs.size())
		{
			m_inputs->m_neurons.at(inputIndex).m_value = (inputs.at(inputIndex));
		}
	}
	
	//FEED FORWARDS FROM INPUT TO HIDDEN
	for (int hiddenIndex = 0; hiddenIndex < m_hiddenLayers->m_neurons.size(); hiddenIndex++)
	{
		double sum = 0;
		for (int inputIndex = 0; inputIndex < m_inputs->m_neurons.size(); inputIndex++)
		{
			sum += (m_inputs->m_neurons.at(inputIndex).m_value) * (m_inputs->m_neurons.at(inputIndex).m_weights.at(hiddenIndex));
		}
		// ADD BIAS
		if(m_inputs->m_biasEnabled)
		{
			sum += m_inputs->m_bias.m_value * m_inputs->m_bias.m_weights.at(hiddenIndex);
		}

		m_hiddenLayers->m_neurons.at(hiddenIndex).m_sumOfPreviousLayer = sum;
		m_hiddenLayers->m_neurons.at(hiddenIndex).m_value = GetActivationValue(sum);
	}

	//FEED FORWARDS FROM HIDDEN TO OUTPUT
	for (int outputIndex = 0; outputIndex < m_outputs->m_neurons.size(); outputIndex++)
	{
		double sum = 0;
		for (int hiddenIndex = 0; hiddenIndex < m_hiddenLayers->m_neurons.size(); hiddenIndex++)
		{
			sum += (m_hiddenLayers->m_neurons.at(hiddenIndex).m_value) * (m_hiddenLayers->m_neurons.at(hiddenIndex).m_weights.at(outputIndex));
		}
		// ADD BIAS
		if(m_hiddenLayers->m_biasEnabled)
		{
			sum += m_hiddenLayers->m_bias.m_value * m_hiddenLayers->m_bias.m_weights.at(outputIndex);
		}
		m_outputs->m_neurons.at(outputIndex).m_sumOfPreviousLayer = sum;
		m_outputs->m_neurons.at(outputIndex).m_value = GetActivationValue(sum);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : Feed forward inpus from 2 sources
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::FeedForward(std::vector<double> &inputs, std::vector<double> &inputs1)
{
	int inputIndex = 0;
	for (; inputIndex < inputs.size(); inputIndex++)
	{
		if(inputIndex < m_inputs->m_neurons.size())
		{
			//m_inputs->m_neurons.at(inputIndex).m_value = GetSigmoidValue(inputs.at(inputIndex));
			m_inputs->m_neurons.at(inputIndex).m_value = (inputs.at(inputIndex));
		}
	}
	for(;inputIndex < inputs.size() + inputs1.size(); inputIndex++)
	{
		if(inputIndex < m_inputs->m_neurons.size())
		{
			//m_inputs->m_neurons.at(inputIndex).m_value = GetSigmoidValue(inputs1.at(inputIndex - inputs.size()));
			m_inputs->m_neurons.at(inputIndex).m_value = (inputs1.at(inputIndex - inputs.size()));
		}
	}
	FeedForwardNN();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/06
*@purpose : Do the back prorogation algorithm to get the corrected weights
*@param   : Known outputs
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::DoBackPropogation(std::vector<double> &knownOutputs)
{
	BackPropogateOuputToHiddenLayer(knownOutputs);
	BackPropogateHiddenToInputLayer(knownOutputs);
	/*std::vector<float> outputErrorDerivatives;
	std::vector<float> hiddenLayerErrors;
	outputErrorDerivatives.reserve(knownOutputs.size());
	hiddenLayerErrors.reserve(m_hiddenLayers->m_neurons.size());

	// OUTPUT ERROR
	////////////////////////////////////////////////////////////////////////////
	for(size_t index = 0;index < m_outputs->m_neurons.size();index++)
	{
		float outputError = knownOutputs.at(index) - m_outputs->m_neurons.at(index).m_value;
		outputErrorDerivatives.push_back(outputError);
	}
	////////////////////////////////////////////////////////////////////////////
	





	//CALCULATING HIDDEN ERRORS
	////////////////////////////////////////////////////////////////////////////
	std::vector<float> sumOfWeights;
	sumOfWeights.reserve(m_outputs->m_neurons.size());
	for (size_t outputIndex = 0; outputIndex < m_outputs->m_neurons.size(); outputIndex++)
	{
		float sum = GetSumOfWeightsInHiddenLayer(static_cast<int>(outputIndex));
		sumOfWeights.push_back(sum);
	}
		
	for(size_t hiddenNeuronIndex = 0;hiddenNeuronIndex < m_hiddenLayers->m_neurons.size();hiddenNeuronIndex++)
	{
		float hiddenLayerError = 0.0f;
		for (size_t outputErrorIndex = 0; outputErrorIndex < m_outputs->m_neurons.size(); outputErrorIndex++)
		{
			float sumOfWeight      =  sumOfWeights.at(outputErrorIndex);
			float weight		   =  m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.at(outputErrorIndex);
			hiddenLayerError       += ( (weight * outputErrorDerivatives.at(outputErrorIndex)) / sumOfWeight );
		}
		hiddenLayerErrors.push_back(hiddenLayerError);
	}*/
	
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
	// Net -> sum of weights * hidden inputs 
	// Out -> Activation(net)
	// 
	// 
	// PARTIAL DERIVATIVES
	//	d Cost Function	  	 d Cost Function  		 d Out O1		d Net 01
	//	--------------	  =	--------------	    *   ----------  *  ----------
	//		d W n		  	  d Out O1		         d Net O1        d W n
	//  
	//  
	//						      1									 1		
	//	Cost Function     =	     --- *  (target O1 - Out1) ^ 2   + 	--- * (target O2 - Out2) ^ 2			// summation		
	//						      2									 2
	//
	//   d Cost Function		  1		
	//	 -----------      =      --- * 2 * (target O1 - Out1) * -1
	// 	  d Out O1			      2
	//
	// 
	//     d Out 01 
	//   ------------     =   Out 01 * ( 1- Out 01)               // Derivative of Sigmoid
	//	   d Net 01
	//
	//
	//	   d Net 01
	//	 ------------	  =   Out H1								// (Net 01 = W1 * OutH1  +  W2 * OutH2 ) 
	//		d W n
	// 
	// 
	//  d Cost Function	
	// --------------	   = (Out1 - targetO1)  *  Out 01 * ( 1- Out 01)  *  Out H1
	// 	    d W n		
void NeuralNetwork::BackPropogateOuputToHiddenLayer(std::vector<double> &knownOutputs)
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

	// CALCULATE ETotal
	double Etotal = 0;
	for (size_t index = 0; index < m_outputs->m_neurons.size(); index++)
	{
		double outputError = m_outputs->m_neurons.at(index).m_value - static_cast<double>(knownOutputs.at(index));
		Etotal += outputError * outputError * 0.5;
	}


	//					d Cost Function    1		 
	//	Calculating		 ----------    =  --- * 2 * (target O1 - Out1) * -1 
	//					  d Net 01         2
	////////////////////////////////////////////////////////////////////////////
	std::vector<double> outputErrorDerivatives;
	outputErrorDerivatives.reserve(knownOutputs.size());

	for (size_t index = 0; index < m_outputs->m_neurons.size(); index++)
	{
		double outputError = m_outputs->m_neurons.at(index).m_value - static_cast<double>(knownOutputs.at(index));
		outputErrorDerivatives.push_back(outputError);
	}
	////////////////////////////////////////////////////////////////////////////


	//					  d Out 01       	 
	//	Calculating		 ----------  =  Out 01 * ( 1- Out 01)
	//					  d Out O1       
	std::vector<double> outputSigmoidDerivative;  
	outputSigmoidDerivative.reserve(m_outputs->m_neurons.size());
	for (size_t outputIndex = 0; outputIndex < m_outputs->m_neurons.size(); outputIndex++)
	{
		double outputValue = m_outputs->m_neurons.at(outputIndex).m_value;
		outputSigmoidDerivative.push_back(GetFastSigmoidDerivative(outputValue));
	}


	//					   d Net 01
	//	Calculating		 ------------	=   Out H1
	//					 	d W n

	////////////////////////////////////////////////////////////////////////////

	// CALCULATING TOTAL DERIVATIVE
	// OUPUT ERROR * SIGMOID DERIVATIVE OF OUTPUT * HIDDENLAYER OUTPUT  :-  OE * ( O * (1 - O)) * OutHn

	for (size_t outputIndex = 0; outputIndex < m_outputs->m_neurons.size(); outputIndex++)
	{
		for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_hiddenLayers->m_neurons.size(); hiddenLayerIndex++)
		{
			double outputGradient = outputErrorDerivatives.at(outputIndex) * outputSigmoidDerivative.at(outputIndex) 
							* m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_value;
			m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_oldWeights.at(outputIndex) = m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_weights.at(outputIndex);
			m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_weights.at(outputIndex) -= NEURALNET_LEARNING_RATE * outputGradient;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Net -> sum of weights * hidden inputs 
	// Out -> Activation(net)
	// 
	// 
	// PARTIAL DERIVATIVES
	//	 d ErrorTotal		 d ErrorTotal		 d Out Hn		d Net Hn
	//	--------------	=	--------------	*   ----------  *  ----------
	//		d W n			  d Out Hn		     d Net Hn        d W n
	//  
	//  
	//						 1									 1		
	//	 ErrorTotal     =	--- *  (target O1 - Out1) ^ 2   + 	--- * (target O2 - Out2) ^ 2					
	//						 2									 2
	//
	//1  d ErrorTotal		 d Error On		 d Error O(n+1)
	//	 -----------    =   ------------  * ----------  
	// 	  d Out H			 d Out Hn		 d Out H(n+1)
	//
	// 
	//1.a	 d Error On			 d Error On		 d Error Net Hn
	//	 	------------ = 		------------  * ----------------
	//	      d Out Hn			 d Net Hn		 d out Hn
	// 
	//2    d Out Hn 
	//   ------------   =   Out Hn * ( 1 - Out Hn)               // Derivative of Sigmoid
	//	   d Net Hn
	//
	//
	//3   d Net 01
	//	 ------------	=   Out H1								// (Net 01 = W1 * OutH1  +  W2 * OutH2 ) 
	//		d W n
	// 
	// 
	//  d ErrorTotal	
	// --------------	= (Out1 - targetO1)  *  Out 01 * ( 1- Out 01)  *  Out H1
	// 	    d W n		

void NeuralNetwork::BackPropogateHiddenToInputLayer(std::vector<double> &knownOutputs)
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
	//					d ErrorTotal    d Error On		 d Error O(n+1)
	//	Calculating		 ----------  =  -----------  +  ---------------         ...
	//					  d Out Hn       d Out Hn		   d Out Hn
	//					  
	//					d Error On     d Error On		d Net On
	//					----------  = ------------   * ----------
	//					 d Out Hn		d Net On		d Out Hn
	//					 
	//					 d Error On
	//					------------  =  (On - Tn) // Calculated on BackProp of Output to Hidden
	//					  d Net On
	//					  
	//					  d Net On
	//					 ----------	  =  Whn
	//					  d Out Hn
	////////////////////////////////////////////////////////////////////////////

	std::vector<double> outputHiddenErrorDerivatives;
	outputHiddenErrorDerivatives.reserve(m_hiddenLayers->m_neurons.size() * m_outputs->m_neurons.size());

	for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_hiddenLayers->m_neurons.size(); hiddenLayerIndex++)
	{
		double outputErrorHiddenDerivative = 0;
		for (size_t outputIndex = 0; outputIndex < m_outputs->m_neurons.size(); outputIndex++)
		{
			double outputErrorDerivative = m_outputs->m_neurons.at(outputIndex).m_value - static_cast<double>(knownOutputs.at(outputIndex));
			double outputErrorSigmodidDerivative = GetFastSigmoidDerivative(m_outputs->m_neurons.at(outputIndex).m_value);
			double netOnHn = m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_oldWeights.at(outputIndex);
			outputErrorHiddenDerivative += outputErrorDerivative * outputErrorSigmodidDerivative * netOnHn;
		}
		outputHiddenErrorDerivatives.push_back(outputErrorHiddenDerivative);
	}

	std::vector<double> hiddenLayerSigmoidDerivative;
	hiddenLayerSigmoidDerivative.reserve(m_hiddenLayers->m_neurons.size());
	// HIDDEN SIGMOID
	for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_hiddenLayers->m_neurons.size(); hiddenLayerIndex++)
	{
		hiddenLayerSigmoidDerivative.push_back(GetFastSigmoidDerivative(m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_value));
	}

	/*std::vector<float> hiddentGradiant;
	hiddentGradiant.reserve(m_hiddenLayers->m_neurons.size() * m_inputs->m_neurons.size());*/
	for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_hiddenLayers->m_neurons.size(); hiddenLayerIndex++)
	{
		for (size_t inputIndex = 0; inputIndex < m_inputs->m_neurons.size(); inputIndex++)
		{
			double gradientValue = outputHiddenErrorDerivatives.at(hiddenLayerIndex) * hiddenLayerSigmoidDerivative.at(hiddenLayerIndex);
			gradientValue = gradientValue * m_inputs->m_neurons.at(inputIndex).m_value;
			//hiddentGradiant.push_back(gradientValue);

			m_inputs->m_neurons.at(inputIndex).m_oldWeights.at(hiddenLayerIndex) = m_inputs->m_neurons.at(inputIndex).m_weights.at(hiddenLayerIndex);
			m_inputs->m_neurons.at(inputIndex).m_weights.at(hiddenLayerIndex) -= NEURALNET_LEARNING_RATE * gradientValue;
		}
	}


	/*size_t gradientIndex = 0;
	for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_hiddenLayers->m_neurons.size(); hiddenLayerIndex++)
	{
		for (size_t inputIndex = 0; inputIndex < m_inputs->m_neurons.size(); inputIndex++)
		{
			m_inputs->m_neurons.at(inputIndex).m_oldWeights.at(hiddenLayerIndex) = m_inputs->m_neurons.at(inputIndex).m_weights.at(hiddenLayerIndex);
			m_inputs->m_neurons.at(inputIndex).m_weights.at(hiddenLayerIndex) -= NEURALNET_LEARNING_RATE * hiddentGradiant.at(gradientIndex);
			gradientIndex++;
		}
	}*/
	

	/*std::vector<float> hiddenErrorDerivatives;
	hiddenErrorDerivatives.reserve(m_hiddenLayers->m_neurons.size() * m_inputs->m_neurons.size());

	for (size_t inputWeightIndex = 0; inputWeightIndex < m_hiddenLayers->m_neurons.size() * m_inputs->m_neurons.size(); inputWeightIndex++)
	{
		float outputError = 0;
		for (size_t outputIndex = 0; outputIndex < m_outputs->m_neurons.size(); outputIndex++)
		{
			for (size_t hiddenIndex = 0; hiddenIndex < m_hiddenLayers->m_neurons.size(); hiddenIndex++)
			{
				outputError += m_outputs->m_neurons.at(outputIndex).m_value - knownOutputs.at(outputIndex);
			}
		}
		hiddenErrorDerivatives.push_back(outputError);
	}
	////////////////////////////////////////////////////////////////////////////


	//					  d Out 01       	 
	//	Calculating		 ----------  =  Out 01 * ( 1- Out 01)
	//					  d Out O1       
	std::vector<float> outputSigmoidDerivative;
	outputSigmoidDerivative.reserve(m_outputs->m_neurons.size());
	for (size_t outputIndex = 0; outputIndex < m_outputs->m_neurons.size(); outputIndex++)
	{
		float outputValue = m_outputs->m_neurons.at(outputIndex).m_value;
		outputSigmoidDerivative.push_back(GetFastSigmoidDerivative(outputValue));
	}


	//					   d Net 01
	//	Calculating		 ------------	=   Out H1
	//					 	d W n

	////////////////////////////////////////////////////////////////////////////

	// CALCULATING TOTAL DERIVATIVE
	// OUPUT ERROR * SIGMOID DERIVATIVE OF OUTPUT * HIDDENLAYER OUTPUT  :-  OE * ( O * (1 - O)) * OutHn
	std::vector<float> outputGradient;
	outputGradient.reserve(m_hiddenLayers->m_neurons.size() * m_outputs->m_neurons.size());

	/ *for (size_t outputIndex = 0; outputIndex < m_outputs->m_neurons.size(); outputIndex++)
	{
		for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_hiddenLayers->m_neurons.size(); hiddenLayerIndex++)
		{
			float value = outputHiddenErrorDerivatives.at(outputIndex) * outputSigmoidDerivative.at(outputIndex)
				* m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_value;
			outputGradient.push_back(value);
		}
	}* /

	// ======================== DELTA WEIGHTS CALCULATED ========================
	// ADD ALL DELTA WEIGHTS TO CURRENT WEIGHTS
	// 
	size_t gradientIndex = 0;
	for (size_t outputIndex = 0; outputIndex < m_outputs->m_neurons.size(); outputIndex++)
	{
		for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_hiddenLayers->m_neurons.size(); hiddenLayerIndex++)
		{
			m_hiddenLayers->m_neurons.at(hiddenLayerIndex).m_weights.at(outputIndex) -= NEURALNET_LEARNING_RATE * outputGradient.at(gradientIndex);
			gradientIndex++;
		}
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::BackPropogateHiddenToInputLayer1(std::vector<double> &outputErrors)
{
	// OBSOLETE (NOT USED)
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
	std::vector<double> outputSigmoidDerivative;
	outputSigmoidDerivative.reserve(m_hiddenLayers->m_neurons.size());
	for (size_t outputIndex = 0; outputIndex < m_hiddenLayers->m_neurons.size(); outputIndex++)
	{
		double outputValue = m_hiddenLayers->m_neurons.at(outputIndex).m_value;
		outputSigmoidDerivative.push_back(GetFastSigmoidDerivative(outputValue));
	}

	////////////////////////////////////////////////////////////////////////////
	//CALCULATE L.R * OUPUT ERROR * SIGMOID DERIVATIVE OF OUTPUT :- L.R * OE * ( O * (1 - O))
	std::vector<double> outputGradient;
	outputGradient.reserve(m_hiddenLayers->m_neurons.size());
	for (size_t outputIndex = 0; outputIndex < m_hiddenLayers->m_neurons.size(); outputIndex++)
	{
		double value = outputSigmoidDerivative.at(outputIndex) * outputErrors.at(outputIndex) * NEURALNET_LEARNING_RATE;
		outputGradient.push_back(value);
	}

	////////////////////////////////////////////////////////////////////////////
	//CALCULATE GRADIENT VALUE * HIDDEN INPUTS :- [[L.R * OE * ( O * (1 - O))]]  * H
	std::vector<double> deltaWeights;
	deltaWeights.reserve(static_cast<int>(outputGradient.size())*static_cast<int>(m_inputs->m_neurons.size()));
	for (size_t weightIndexHO = 0; weightIndexHO < m_hiddenLayers->m_neurons.size(); weightIndexHO++)
	{
		for (size_t hiddenLayerIndex = 0; hiddenLayerIndex < m_inputs->m_neurons.size(); hiddenLayerIndex++)
		{
			double deltaWeight = outputGradient.at(weightIndexHO) * m_inputs->m_neurons.at(hiddenLayerIndex).m_value;
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
double NeuralNetwork::GetSumOfWeightsInHiddenLayer(int outputIndex)
{
	double sum = 0.f;
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
	std::vector<double> outputs;
	for(int index = 0;index < m_outputs->m_neurons.size() ;index++)
	{
		double value = m_outputs->m_neurons.at(index).m_value;
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
void NeuralNetwork::FeedForwardNN()
{
	for (int hiddenIndex = 0; hiddenIndex < m_hiddenLayers->m_neurons.size(); hiddenIndex++)
	{
		double sum = 0;
		for (int index = 0; index < m_inputs->m_neurons.size(); index++)
		{
			sum += (m_inputs->m_neurons.at(index).m_value)*(m_inputs->m_neurons.at(index).m_weights.at(hiddenIndex));		
		}
		//sum += m_inputs->m_bias.m_value * m_inputs->m_bias.m_weights.at(hiddenIndex);

		m_hiddenLayers->m_neurons.at(hiddenIndex).m_sumOfPreviousLayer = sum;
		m_hiddenLayers->m_neurons.at(hiddenIndex).m_value = GetActivationValue(sum);
	}

	for (int outputIndex = 0; outputIndex < m_outputs->m_neurons.size(); outputIndex++)
	{
		double sum = 0;
		for (int hiddenIndex = 0; hiddenIndex < m_hiddenLayers->m_neurons.size(); hiddenIndex++)
		{
			sum += (m_hiddenLayers->m_neurons.at(hiddenIndex).m_value)*(m_hiddenLayers->m_neurons.at(hiddenIndex).m_weights.at(outputIndex));
		}

		//sum += m_hiddenLayers->m_bias.m_value * m_hiddenLayers->m_bias.m_weights.at(outputIndex);

		m_outputs->m_neurons.at(outputIndex).m_sumOfPreviousLayer = sum;
		m_outputs->m_neurons.at(outputIndex).m_value = GetActivationValue(sum);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double NeuralNetwork::GetActivationValue(double value)
{
	return GetSigmoidValue(value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/07
*@purpose : Calculates Rectified linear units activation value
*@param   : Neuron value
*@return  : RELU activated value
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double NeuralNetwork::GetRELUActivationValue(double value)
{
	return ClampDouble(value, 0, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/06
*@purpose : Calculates the fast sigmoid function
*@param   : Value
*@return  : Corresponding sigmoid function
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double NeuralNetwork::GetFastSigmoidValue(double value)
{
	return (value / (1 - (value)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/06
*@purpose : returns the derivative of the sigmoid function
*@param   : Value
*@return  : Derivative sigmoid function
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double NeuralNetwork::GetFastSigmoidDerivative(double value)
{
	return (value) * (1 - (value));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double NeuralNetwork::GetSigmoidValue(double value)
{
	double e = 2.7183;
	return 1 / (1 + pow(e, -value));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double NeuralNetwork::GetSigmoidDerivative(double value)
{
	return value * (1 - value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : Mutates all weights of NN
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::Mutate(float mutationRate,float mutationScale)
{
	if(mutationScale <= 0)
	{
		return;
	}
	for(int inputNeuronIndex = 0;inputNeuronIndex < m_inputs->m_neurons.size();inputNeuronIndex++)
	{
		for(int inputNeuronWeightIndex = 0;inputNeuronWeightIndex < m_inputs->m_neurons.at(inputNeuronIndex).m_weights.size();inputNeuronWeightIndex++)
		{
			if(GetRandomFloatZeroToOne() < mutationRate)
			{
				double random = static_cast<double>(GetRandomFloatInRange(-1, 1) / mutationScale);
				double weight = m_inputs->m_neurons.at(inputNeuronIndex).m_weights.at(inputNeuronWeightIndex) + random;
				weight = ClampDouble(weight, -1, 1);
				m_inputs->m_neurons.at(inputNeuronIndex).m_weights.at(inputNeuronWeightIndex) = weight;
			}
		}
	}
	for(int hiddenNeuronIndex = 0;hiddenNeuronIndex < m_hiddenLayers->m_neurons.size();hiddenNeuronIndex++)
	{
		for (int hiddenNeuronWeightIndex = 0; hiddenNeuronWeightIndex < m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.size(); hiddenNeuronWeightIndex++)
		{
			if (GetRandomFloatZeroToOne() < mutationRate)
			{
				double random = static_cast<double>(GetRandomFloatInRange(-1, 1) / mutationScale);
				double weight = m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.at(hiddenNeuronWeightIndex) + random;
				weight		  = ClampDouble(weight, -1, 1);
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
bool NeuralNetwork::CopyWeightsTo(NeuralNetwork &copy)
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
			double myweight = m_inputs->m_neurons.at(inputNeuronIndex).m_weights.at(inputNeuronWeightIndex);
			copy.m_inputs->m_neurons.at(inputNeuronIndex).m_weights.at(inputNeuronWeightIndex) = myweight;
		}
		//Neuron neuron(m_inputs->m_neurons.at(inputNeuronIndex).m_numberOfWeights);
		//copy.m_inputs->m_neurons.push_back(neuron);
	}
	for (int hiddenNeuronIndex = 0; hiddenNeuronIndex < m_hiddenLayers->m_neurons.size(); hiddenNeuronIndex++)
	{
		for (int hiddenNeuronWeightIndex = 0; hiddenNeuronWeightIndex < m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.size(); hiddenNeuronWeightIndex++)
		{
			double myweight = m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.at(hiddenNeuronWeightIndex);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/11
*@purpose : Do a cross over of all weights
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::CrossOver(NeuralNetwork &second)
{
	UNUSED(second);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetwork::StoreToFile(const char* filePathname)
{
	FILE *fp = FileOpenForWrite(filePathname);
	//Data\NN\BestGame\2
	for (int inputNeuronIndex = 0; inputNeuronIndex < m_inputs->m_neurons.size(); inputNeuronIndex++)
	{
		for (int inputNeuronWeightIndex = 0; inputNeuronWeightIndex < m_inputs->m_neurons.at(inputNeuronIndex).m_weights.size(); inputNeuronWeightIndex++)
		{
			double myweight = m_inputs->m_neurons.at(inputNeuronIndex).m_weights.at(inputNeuronWeightIndex);
			FileAppendString(fp, ToString(myweight));
			FileAppendString(fp, ":");
		}
	}
	for (int inputNeuronWeightIndex = 0; inputNeuronWeightIndex < m_inputs->m_bias.m_weights.size(); inputNeuronWeightIndex++)
	{
		double myweight = m_inputs->m_bias.m_weights.at(inputNeuronWeightIndex);
		FileAppendString(fp, ToString(myweight));
		FileAppendString(fp, ":");
	}

	for (int hiddenNeuronIndex = 0; hiddenNeuronIndex < m_hiddenLayers->m_neurons.size(); hiddenNeuronIndex++)
	{
		for (int hiddenNeuronWeightIndex = 0; hiddenNeuronWeightIndex < m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.size(); hiddenNeuronWeightIndex++)
		{
			double myweight = m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.at(hiddenNeuronWeightIndex);
			FileAppendString(fp, ToString(myweight));
			FileAppendString(fp, ":");
		}
	}
	for (int hiddenNeuronWeightIndex = 0; hiddenNeuronWeightIndex < m_hiddenLayers->m_bias.m_weights.size(); hiddenNeuronWeightIndex++)
	{
		double myweight = m_hiddenLayers->m_bias.m_weights.at(hiddenNeuronWeightIndex);
		FileAppendString(fp, ToString(myweight));
		FileAppendString(fp, ":");
	}
	FileClose(fp);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/15
*@purpose : Loads NN from file
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NeuralNetwork::LoadFromFile(const char* filePathname)
{
	if(!DoFileExist(std::string(filePathname)))
	{
		return false;
	}
	std::string fileContent = GetFileContentAsString(filePathname);
	std::vector<std::string> values;
	Split(values,fileContent, ':');
	int index = 0;
	for (int inputNeuronIndex = 0; inputNeuronIndex < m_inputs->m_neurons.size(); inputNeuronIndex++)
	{
		for (int inputNeuronWeightIndex = 0; inputNeuronWeightIndex < m_inputs->m_neurons.at(inputNeuronIndex).m_weights.size(); inputNeuronWeightIndex++)
		{
			double weight = 0;
			ToDouble(values.at(index++),&weight);
			//values.at(index).clear();
			m_inputs->m_neurons.at(inputNeuronIndex).m_weights.at(inputNeuronWeightIndex) = weight;
		}
	}
	for (int inputNeuronWeightIndex = 0; inputNeuronWeightIndex < m_inputs->m_bias.m_weights.size(); inputNeuronWeightIndex++)
	{
		double weight = 0;
		ToDouble(values.at(index++), &weight);
		//values.at(index).clear();
		m_inputs->m_bias.m_weights.at(inputNeuronWeightIndex) = weight;
	}
	for (int hiddenNeuronIndex = 0; hiddenNeuronIndex < m_hiddenLayers->m_neurons.size(); hiddenNeuronIndex++)
	{
		for (int hiddenNeuronWeightIndex = 0; hiddenNeuronWeightIndex < m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.size(); hiddenNeuronWeightIndex++)
		{
			double weight = 0;
			ToDouble(values.at(index++), &weight);
			//values.at(index).clear();
			m_hiddenLayers->m_neurons.at(hiddenNeuronIndex).m_weights.at(hiddenNeuronWeightIndex) = weight;
		}
	}
	for (int hiddenNeuronWeightIndex = 0; hiddenNeuronWeightIndex < m_hiddenLayers->m_bias.m_weights.size(); hiddenNeuronWeightIndex++)
	{
		double weight = 0;
		ToDouble(values.at(index++), &weight);
		//values.at(index).clear();
		m_hiddenLayers->m_bias.m_weights.at(hiddenNeuronWeightIndex) = weight;
	}
	//fileContent.clear();
	
	return true;
}
