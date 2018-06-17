#include "Engine/AI/GA/Chromosome.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Chromosome::Chromosome(int geneCount,float mutationChance)
{
	m_geneCount		 = geneCount;
	m_mutationChance = mutationChance;
	for(int geneIndex = 0;geneIndex < geneCount;geneIndex++)
	{
		Gene *gene = new Gene(mutationChance);
		m_genes.push_back(gene);
	}
}

// DESTRUCTOR
Chromosome::~Chromosome()
{
	for(int index = static_cast<int>(m_genes.size()) - 1;index >=0; index--)
	{
		delete m_genes.at(index);
		m_genes.at(index) = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : returns the total fitness of the current offspring
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Chromosome::GetTotalFitness(Chromosome* taget)
{
	m_totalFitness = 0.01f;
	for (int geneIndex = 0; geneIndex < m_genes.size(); geneIndex++)
	{
		if (geneIndex < taget->m_genes.size())
		{
			if (m_genes.at(geneIndex)->IsAlmostEqual(taget->m_genes.at(geneIndex)))
			{
				m_totalFitness++;
			}
		}
	}
	return m_totalFitness;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Chromosome * Chromosome::CrossOver(Chromosome* ch1)
{
	Chromosome *chr = new Chromosome(m_geneCount,m_mutationChance);
	for (int index = 0; index < m_genes.size(); index++)
	{
		if(index < m_genes.size()/2.f)
		{
			Gene *gene = m_genes.at(index)->Clone();
			chr->m_genes.at(index) = gene;
		}
		else
		{
			if (index < ch1->m_genes.size())
			{
				Gene *gene = ch1->m_genes.at(index)->Clone();
				chr->m_genes.at(index) = gene;
			}
		}
	}
	return chr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : GA crossover part (for this part we take half from each chromosome interchange with other chromosome)
*@param   : 2 Chromosomes
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Chromosome::Mutate()
{
	for(int index = 0;index < m_genes.size();index++)
	{
		m_genes.at(index)->Mutate();
	}
}
