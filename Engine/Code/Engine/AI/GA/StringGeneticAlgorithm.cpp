#include "Engine/AI/GA/StringGeneticAlgorithm.hpp"
#include "Engine/Math/MathUtil.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
StringGeneticAlgorithm::StringGeneticAlgorithm(int chromosomeCount, int geneLength,float crossOverChance, float mutationChance)
	: GeneticAlgorithm(chromosomeCount,geneLength,crossOverChance,mutationChance,SIMPLE_CHAR)
{

}
// DESTRUCTOR
StringGeneticAlgorithm::~StringGeneticAlgorithm()
{

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : Does crossover and set the chromosome to next population
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void StringGeneticAlgorithm::DoCrossOverAndSetNewPopulation()
{
	float maxFitness = 0.f;
	int populationSize = static_cast<int>(m_population->m_chromosomes.size());
	for (int index = 0; index < populationSize; index++)
	{
		maxFitness += m_population->m_chromosomes.at(index)->GetTotalFitness(m_target);
	}
	for (int index = 0; index < populationSize; index++)
	{
		if (GetRandomFloatZeroToOne() < m_population->m_crossOverChance)
		{
			Chromosome *ch1   = m_population->PickRandomChromosomeByWeight(static_cast<int>(maxFitness));
			Chromosome *ch2   = m_population->PickRandomChromosomeByWeight(static_cast<int>(maxFitness));
			Chromosome *chnew = m_newPopulation->m_chromosomes.at(index);
			ch1->CrossOverAndSet(ch2, chnew);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : Does mutation part of GA
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void StringGeneticAlgorithm::DoMutation()
{
	int populationSize = static_cast<int>(m_newPopulation->m_chromosomes.size());
	for (int index = 0; index < populationSize; index++)
	{
		Chromosome *ch = m_newPopulation->m_chromosomes.at(index);
		ch->Mutate();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : Evaluate the correctness of the best offspring
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void StringGeneticAlgorithm::Evaluate()
{
	Population* tempPopulation   = m_population;
	m_population				 = m_newPopulation;
	m_newPopulation				 = tempPopulation;

	float maxFitness      = -1;
	m_averageFitnessValue = 0.f;
	for (int index = 0; index < m_population->m_chromosomes.size(); index++)
	{
		Chromosome *ch = m_population->m_chromosomes.at(index);
		float fitness  = ch->GetTotalFitness(m_target);
		m_averageFitnessValue += fitness;
		if(fitness > maxFitness)
		{
			maxFitness = fitness;
			m_best = ch;
		}
		if(static_cast<int>(fitness) == static_cast<int>(ch->m_genes.size()))
		{
			m_isFinished = true;
			m_best = ch;
		}
	}
	m_averageFitnessValue /= m_population->m_chromosomes.size();
}
