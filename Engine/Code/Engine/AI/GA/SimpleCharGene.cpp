#include "Engine/AI/GA/SimpleCharGene.hpp"
#include "Engine/Math/MathUtil.hpp"
// CONSTRUCTOR
SimpleCharGene::SimpleCharGene(float mutationChance) : Gene(mutationChance)
{
	m_char = GetRandomCharacter();
}

SimpleCharGene::SimpleCharGene(const SimpleCharGene &copy) : Gene(copy)
{
	m_char = copy.m_char;
}

// DESTRUCTOR
SimpleCharGene::~SimpleCharGene()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : Mutate the current gene
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SimpleCharGene::Mutate()
{
	if (GetRandomFloatZeroToOne() < m_mutateChance)
	{
		m_char = GetRandomCharacter();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : Clones(Creates copy)
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Gene* SimpleCharGene::Clone()
{
	Gene* gene = new SimpleCharGene(*this);
	return gene;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : Returns the fitness value of the current gene
*@param   : Expected result
*@return  : Fitness value
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float SimpleCharGene::GetFitnessValue(Gene *target)
{
	if(((SimpleCharGene*)target)->m_char == m_char)
	{
		return 1.f;
	}
	return 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/22
*@purpose : Is almost equal to the this gene
*@param   : Target gene
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SimpleCharGene::IsAlmostEqual(Gene *gene)
{
	if(((SimpleCharGene*)gene)->m_char == m_char)
	{
		return true;
	}
	return false;
}
