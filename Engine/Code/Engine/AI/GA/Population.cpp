#include "Engine/AI/GA/Population.hpp"
#include "Engine/Math/MathUtil.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Population::Population(int chromosomeCount, int genecount, float crossOverChance, float mutationChance, GA_TYPE type)
{
	CreatesNewPopulation(chromosomeCount, genecount,crossOverChance, mutationChance, type);
}

Population::Population(int chromosomeCount, int genecount, float crossOverChance, float mutationChance, GA_TYPE type,std::vector<Gene*>& inputs)
{
	CreatesNewPopulation(chromosomeCount, genecount, crossOverChance, mutationChance, type, inputs);
}

// DESTRUCTOR
Population::~Population()
{
	for(int index = static_cast<int>(m_chromosomes.size());index >=0 ;index--)
	{
		delete m_chromosomes.at(index);
		m_chromosomes.at(index) = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/20
*@purpose : Creates new populations
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Population::CreatesNewPopulation(int chromosomeCount, int genecount,float crossOverChance, float mutationChance, GA_TYPE type)
{
	m_chromosomeCount = chromosomeCount;
	m_crossOverChance = crossOverChance;
	for (int chIndex = 0; chIndex < chromosomeCount; chIndex++)
	{
		Chromosome *chromosome = new Chromosome(genecount, mutationChance,type);
		m_chromosomes.push_back(chromosome);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Population::CreatesNewPopulation(int chromosomeCount, int genecount, float crossOverChance, float mutationChance, GA_TYPE type, std::vector<Gene*>& inputs)
{
	m_chromosomeCount = chromosomeCount;
	m_crossOverChance = crossOverChance;
	int index = 0;
	for (int chIndex = 0; chIndex < chromosomeCount; chIndex++)
	{
		std::vector<Gene*> input;
		for(;index < inputs.size();index++)
		{
			input.push_back(inputs.at(index));
		}
		Chromosome *chromosome = new Chromosome(genecount, mutationChance, type,input);
		m_chromosomes.push_back(chromosome);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Build chromosome as you go in the list of chromosomes 
Chromosome* Population::PickRandomChromosomeByWeight(int maxFitness)
{
	int maxIteration     = 1000;
	int currentIteration = 0;
	while(true)
	{
		int index			= GetRandomIntLessThan(static_cast<int>(m_chromosomes.size()));
		Chromosome *partner = m_chromosomes.at(index);
		int r			    = GetRandomIntLessThan(maxFitness);

		if(static_cast<float>(r) < partner->m_totalFitness)
		{
			return partner;
		}
		currentIteration++;
		if(currentIteration > maxIteration)
		{
			break;
		}
	}
	int index			= GetRandomIntLessThan(static_cast<int>(m_chromosomes.size()));
	Chromosome *partner = m_chromosomes.at(index);
	return partner;
}
