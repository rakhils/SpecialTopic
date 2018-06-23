#include "Engine/AI/NeuralNetwork/NeuralNetworkGene.hpp"


// CONSTRUCTOR
NeuralNetworkGene::NeuralNetworkGene(float mutateChance) : Gene(mutateChance)
{
	m_neuralNet = new NeuralNetwork();
}

NeuralNetworkGene::NeuralNetworkGene(const NeuralNetworkGene& copy): Gene(copy)
{
	m_neuralNet = copy.m_neuralNet;
}

// DESTRUCTOR
NeuralNetworkGene::~NeuralNetworkGene()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/20
*@purpose : Mutates the nueral network gene(Doing backpropogation here. TODO:: might change later)
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetworkGene::Mutate()
{
	int a = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Gene* NeuralNetworkGene::Clone()
{
	Gene* gene = new NeuralNetworkGene(*this);
	return gene;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : Retireves the fitness value of the current gene
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float NeuralNetworkGene::GetFitnessValue(Gene *target)
{
	return 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NeuralNetworkGene::IsAlmostEqual(Gene *gene)
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : Gets the current neural net attached to gene
*@param   : NIL
*@return  : Neural network
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NeuralNetwork* NeuralNetworkGene::GetNeuralNetwork()
{
	return m_neuralNet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetworkGene::CreateNeuralNetwork()
{
	m_neuralNet->CreateNeuralNetwork(100, 36, 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : Sets all inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetworkGene::UpdateWithInputs(std::vector<float>& inputs)
{
	for(int inputIndex = 0;inputIndex < m_neuralNet->m_inputs->m_neurons.size();inputIndex++)
	{
		if(inputIndex < inputs.size())
		{
			m_neuralNet->m_inputs->m_neurons.at(inputIndex).m_value = inputs.at(inputIndex);
		}
	}
	m_neuralNet->Update();
}