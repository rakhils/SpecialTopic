#include "Engine/AI/NeuralNetwork/NeuralNetGA.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetworkGene.hpp"
// CONSTRUCTOR
NeuralNetGA::NeuralNetGA(int chromosomeCount, int geneLength, float crossOverChance, float mutationRate,std::vector<Gene*>& inputs) 
	: GeneticAlgorithm(chromosomeCount,geneLength,crossOverChance,mutationRate,NEURAL_NET,inputs)
{

}

// DESTRUCTOR
NeuralNetGA::~NeuralNetGA()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetGA::DoCrossOverAndSetNewPopulation()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetGA::DoMutation()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetGA::Evaluate()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : Sets inputs for neural network
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetGA::UpdateWithInputs(std::vector<float>& inputs)
{
	for(int chIndex = 0;chIndex < m_population->m_chromosomes.size();chIndex++)
	{
		Chromosome *ch = m_population->m_chromosomes.at(chIndex);
		for(int geneIndex = 0;geneIndex < ch->m_genes.size();geneIndex++)
		{
			((NeuralNetworkGene*)ch->m_genes.at(geneIndex))->UpdateWithInputs(inputs);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : Updates all GA with inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NeuralNetGA::Update()
{

}
