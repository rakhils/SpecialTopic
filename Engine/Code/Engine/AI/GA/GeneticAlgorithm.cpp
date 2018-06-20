#include "Engine/AI/GA/GeneticAlgorithm.hpp"
#include "Engine/Math/MathUtil.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
GeneticAlgorithm::GeneticAlgorithm(int initialPopulation, int geneLength, float mutationRate)
{
	m_initPopulation = initialPopulation;
	m_geneLength     = geneLength;
	m_mutationRate   = mutationRate;
	CreateInitialPopulation();
}

// DESTRUCTOR
GeneticAlgorithm::~GeneticAlgorithm()
{
	delete m_population;
	m_population = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : Sets the target for the offspring
*@param   : target chromosome
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GeneticAlgorithm::SetTarget(Chromosome* chromosome)
{
	m_target = chromosome;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : Creates initial population
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GeneticAlgorithm::CreateInitialPopulation()
{
	m_population	= new Population(m_initPopulation, m_geneLength, m_mutationRate);
	m_newPopulation = new Population(m_initPopulation, m_geneLength, m_mutationRate);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : Update fitness value for each offspring
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GeneticAlgorithm::UpdateFitnessValue()
{
	m_averageFitnessValue = 0.f;
	for(int index = 0;index < m_population->m_chromosomes.size();index++)
	{
		Chromosome *ch = m_population->m_chromosomes.at(index);
		ch->GetTotalFitness(m_target);
		m_averageFitnessValue += ch->m_totalFitness;
		if(ch->m_totalFitness == 0.f)
		{
			ch->m_totalFitness = 0.01f;
		}
	}
	m_averageFitnessValue /= m_population->m_chromosomes.size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : Runs one iteration of evolution
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GeneticAlgorithm::Epoch()
{
	if(m_isFinished)
	{
		return true;
	}
	UpdateFitnessValue();
	m_currentGenerationCount++;
	GenerateNewPopulation();
	Evaluate();
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : Generates new population
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GeneticAlgorithm::GenerateNewPopulation()
{
	float maxFitness   = 0.f;
	int populationSize = m_population->m_chromosomes.size();
	for (int index = 0; index < populationSize; index++)
	{
		maxFitness += m_population->m_chromosomes.at(index)->GetTotalFitness(m_target);
	}
	
	for( int index = 0; index < populationSize; index++)
	{
		Chromosome *ch1 = m_population->PickRandomChromosomeByWeight(static_cast<int>(maxFitness));
		Chromosome *ch2 = m_population->PickRandomChromosomeByWeight(static_cast<int>(maxFitness));
		Chromosome *ch3 = ch1->CreateAndSetCrossOverToNewPopulation(ch2,m_newPopulation,index);
		ch3->Mutate();
		ch3->GetTotalFitness(m_target);
	}

	Population* tempPopulation = m_population;
	m_population = m_newPopulation;
	m_newPopulation = tempPopulation;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : Evaluate the correctness of the best offspring
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GeneticAlgorithm::Evaluate()
{
	float maxFitness = -1;
	for (int index = 0; index < m_population->m_chromosomes.size(); index++)
	{
		Chromosome *ch = m_population->m_chromosomes.at(index);
		float fitness = ch->GetTotalFitness(m_target);
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
}
