#include "Engine/AI/GA/Chromosome.hpp"
#include "Engine/AI/GA/Population.hpp"
#include "Engine/AI/GA/SimpleCharGene.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetworkGene.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Chromosome::Chromosome(int geneCount,float mutationChance, GA_TYPE type)
{
	CreateInitialRandomChromosome(geneCount, mutationChance, type);
}

Chromosome::Chromosome(int geneCount, float mutationChance, GA_TYPE type, std::vector<Gene*>& inputs)
{
	CreateInitialRandomChromosome(geneCount, mutationChance, type,inputs);
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
/*DATE    : 2018/06/20
*@purpose : Creates a random chromosome(consist of genes with random value)
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Chromosome::CreateInitialRandomChromosome(int geneCount,float mutationChance,GA_TYPE type)
{
	m_geneCount		  = geneCount;
	m_mutationChance  = mutationChance;
	for (int geneIndex = 0; geneIndex < geneCount; geneIndex++)
	{
		Gene *gene;
		switch (type)
		{
		case SIMPLE_CHAR:
			gene = new SimpleCharGene(mutationChance);
			break;
		case NEURAL_NET:
			gene = new NeuralNetworkGene(mutationChance);
			break;
		default:
			break;
		}
		m_genes.push_back(gene);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Chromosome::CreateInitialRandomChromosome(int geneCount, float mutationChance, GA_TYPE type, std::vector<Gene*>& inputs)
{
	m_geneCount = geneCount;
	m_mutationChance = mutationChance;
	for (int geneIndex = 0; geneIndex < geneCount; geneIndex++)
	{
		m_genes.push_back(inputs.at(geneIndex));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : returns the total fitness of the current offspring
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Chromosome::GetTotalFitness(Chromosome* target)
{
	m_totalFitness = 0.01f;
	for (int geneIndex = 0; geneIndex < m_genes.size(); geneIndex++)
	{
		if (geneIndex < target->m_genes.size())
		{
			m_totalFitness += m_genes.at(geneIndex)->GetFitnessValue(target->m_genes.at(geneIndex));
		}
	}
	return m_totalFitness;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/15
*@purpose : Do the crossover and set it to the next generation of population
*@param   : 2ndchromosome , new population pointer, new population's chromosome index
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Chromosome::CrossOverAndSet(Chromosome* ch1,Chromosome *chnewPopulation)
{
	for (int index = 0; index < m_genes.size(); index++)
	{
		if(index < m_genes.size()/2.f)
		{
			Gene *gene = m_genes.at(index)->Clone();
			chnewPopulation->m_genes.at(index) = gene;
		}
		else
		{
			if (index < ch1->m_genes.size())
			{
				Gene *gene = ch1->m_genes.at(index)->Clone();
				chnewPopulation->m_genes.at(index) = gene;
			}
		}
	}
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
