#include "Engine/AI/GA/Abstract/GeneticAlgorithm.hpp"
#include "Engine/Math/MathUtil.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
GeneticAlgorithm::GeneticAlgorithm(int initialPopulation, int geneLength, float crossOverChance, float mutationRate, GA_TYPE type)
{
	m_chromosomeCount   = initialPopulation;
	m_geneLength		= geneLength;
	m_mutationChance	= mutationRate;
	m_crossOverChance   = crossOverChance;
	CreateInitialPopulation(type);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GeneticAlgorithm::GeneticAlgorithm(int initialPopulation, int geneLength, float crossOverChance, float mutationRate, GA_TYPE type, std::vector<Gene*>& inputs)
{
	m_chromosomeCount = initialPopulation;
	m_geneLength = geneLength;
	m_mutationChance = mutationRate;
	m_crossOverChance = crossOverChance;
	CreateInitialPopulation(type,inputs);
}

// DESTRUCTOR
GeneticAlgorithm::~GeneticAlgorithm()
{
	delete m_population;
	m_population = nullptr;
	delete m_newPopulation;
	m_newPopulation = nullptr;
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
/*DATE    : 2018/06/20
*@purpose : Creates initial population of type
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GeneticAlgorithm::CreateInitialPopulation(GA_TYPE type)
{
	m_population    = new Population(m_chromosomeCount, m_geneLength, m_crossOverChance, m_mutationChance, type);
	m_newPopulation = new Population(m_chromosomeCount, m_geneLength, m_crossOverChance, m_mutationChance, type);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GeneticAlgorithm::CreateInitialPopulation(GA_TYPE type, std::vector<Gene*>& genes)
{
	m_population    = new Population(m_chromosomeCount, m_geneLength, m_crossOverChance, m_mutationChance, type,genes);
	m_newPopulation = new Population(m_chromosomeCount, m_geneLength, m_crossOverChance, m_mutationChance, type);
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
	for (int index = 0; index < m_population->m_chromosomes.size(); index++)
	{
		Chromosome *ch = m_population->m_chromosomes.at(index);
		ch->GetTotalFitness(m_target);
		m_averageFitnessValue += ch->m_totalFitness;
		if (ch->m_totalFitness == 0.f)
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
	if (m_isFinished)
	{
		return true;
	}
	m_currentGenerationCount++;
	//UpdateFitnessValue();
	DoCrossOverAndSetNewPopulation();
	DoMutation();
	Evaluate();
	return false;
}
