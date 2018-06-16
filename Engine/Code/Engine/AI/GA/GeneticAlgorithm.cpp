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
	/*for(int index = m_currentPopulation.size();index >=0 ;index--)
	{
		delete m_currentPopulation.at(index);
		m_currentPopulation.at(index) = nullptr;
	}*/
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
	m_population = new Population(m_initPopulation, m_geneLength, m_mutationRate);
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
			ch->m_totalFitness = 0.01;
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
	int maxFitness = 0;
	for (int index = 0; index < m_population->m_chromosomes.size(); index++)
	{
		maxFitness += m_population->m_chromosomes.at(index)->GetTotalFitness(m_target);
	}
	std::vector<Chromosome*> newpopulation;
	newpopulation.reserve(m_initPopulation);
	for(int index = 0;index < m_population->m_chromosomes.size();index++)
	{
		Chromosome *ch1 = m_population->AcceptReject(maxFitness);
		Chromosome *ch2 = m_population->AcceptReject(maxFitness);
		Chromosome *ch3 = ch1->CrossOver(ch2);
		ch3->Mutate();
		ch3->GetTotalFitness(m_target);
		newpopulation.push_back(ch3);
	}
	for(int index = m_population->m_chromosomes.size() -1;index >=0 ;index--)
	{
		delete m_population->m_chromosomes.at(index);
	}
	m_population->m_chromosomes = newpopulation;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : Evaluate the correctness of the best offspring
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GeneticAlgorithm::Evaluate()
{
	int maxFitness = -1;
	Chromosome *best = nullptr;
	for (int index = 0; index < m_population->m_chromosomes.size(); index++)
	{
		Chromosome *ch = m_population->m_chromosomes.at(index);
		int fitness = ch->GetTotalFitness(m_target);
		if(fitness > maxFitness)
		{
			maxFitness = fitness;
			m_best = ch;
		}
		if(fitness == ch->m_genes.size())
		{
			m_isFinished = true;
			m_best = ch;
		}
		/*bool isDone = true;
		for (int geneIndex = 0; geneIndex < ch->m_genes.size(); geneIndex++)
		{
			if (geneIndex < m_taget->m_genes.size())
			{
				if (ch->m_genes.at(geneIndex) != m_taget->m_genes.at(geneIndex))
				{
					isDone = false;
				}
			}
		}
		if(isDone)
		{
			m_isFinished = true;
			m_best = ch;
		}*/
	}
}
