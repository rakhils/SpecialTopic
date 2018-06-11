#include "MapGenSteps.hpp"
#include "Game/GamePlay/Maps/Map.hpp"

MapGenSteps::MapGenSteps(const XMLElement& genStepXmlElement)
{
	const char* name = genStepXmlElement.GetText();
}

MapGenSteps::MapGenSteps()
{

}

MapGenSteps::~MapGenSteps()
{

}

MapGenSteps* MapGenSteps::CreateMapGenStep(XMLElement& genStepXmlElement)
{
	const char *name = genStepXmlElement.Name();
	std::string genStepName; genStepName.append(name);
	if (genStepName == "FromFile")
		return new MapGenSteps_FromFile(genStepXmlElement);
	if (genStepName == "SpawnActor")
		return new MapGenSteps_SpawnActor(genStepXmlElement);
	return nullptr;
}

void MapGenSteps_FromFile::Run(Map &newMap)
{
	//int mapWidth = newMap.m_def->m_width;
	//int mapHeight = newMap.m_def->m_heigth;

	/*for (int y = 0; y < m_image->texture->getDimensions().y; y++)
	{
		for (int x = 0; x < m_image->texture->getDimensions().x; x++)
		{
			int index = y * mapWidth + x;
			Rgba rgba = m_image->GetTexel(x, y);
			std::string rgbstring = rgba.getAsString();
			if (rgbstring == "255,255,255,0")
			{
				continue;
			}
			EntityDefinition* entityDef = EntityDefinition::getDefinitionByRGB(rgbstring);
			if (entityDef == nullptr)
			{
				continue;
			}
			newMap.SpawnEntities(entityDef->m_type,Vector2(x/32.f,y/32.f),Vector2(0,1));

			//newMap.m_tiles.at(index)->m_tileDef = entityDef;
			//newMap.m_tiles.at(index)->m_type = entityDef->m_name;
		}
	}*/
	int a = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MapGenSteps_SpawnActor::Run(Map &newMap)
{
	if(m_name == "Brick")
	{
		newMap.CreateBricks(m_position,m_dimensions,m_hidden);
	}
	if(m_name == "Pipe")
	{
		newMap.CreatePipes(m_position, m_dimensions);
	}
}
