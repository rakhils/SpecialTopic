#pragma once

/*\class  : Goomba
*		   
* \group  : <GroupName>
*		   
* \brief  :
*		   
* \TODO:  :
*		   
* \note   :
*		    
* \author : Rakhil Soman
*		   
* \version: 1.0
*		   
* \date   : 3/1/2018 1:56:55 AM
*
* \contact: srsrakhil@gmail.com
*/
enum Enemy_Type
{
	GOOMBA,
	DUCK
};
class Enemy
{

public:
	//Member_Variables
	Enemy_Type m_type;
	//Static_Member_Variables

	//Methods

	Enemy();
	~Enemy();
	
	//Static_Methods

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};