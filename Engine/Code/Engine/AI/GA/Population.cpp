#include "Engine/AI/GA/Population.hpp"
#include "Engine/Math/MathUtil.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Population::Population(int chromosomeCount,int genecount,float mutationChance)
{
	m_chromosomeCount = chromosomeCount;
	for(int chIndex = 0;chIndex < chromosomeCount;chIndex++)
	{
		Chromosome *chromosome = new Chromosome(genecount,mutationChance);
		m_chromosomes.push_back(chromosome);
	}
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
/*DATE    : 2018/06/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Chromosome* Population::AcceptReject(int m_maxFitness)
{
	int maxIteration     = 1000;
	int currentIteration = 0;
	while(true)
	{
		int index			= GetRandomIntLessThan(static_cast<int>(m_chromosomes.size()));
		Chromosome *partner = m_chromosomes.at(index);
		int r			    = GetRandomIntLessThan(m_maxFitness);

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
