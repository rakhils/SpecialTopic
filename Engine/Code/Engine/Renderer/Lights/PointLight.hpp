#pragma once
#include "Engine/Renderer/Lights/Light.hpp"
#include "Engine/Math/Vector3.hpp"
/*\class  : PointLight
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
* \date   : 4/8/2018 2:48:06 PM
*
* \contact: srsrakhil@gmail.com
*/
 
class PointLight : public Light
{

public:
	//Member_Variables
	float   m_specFactor;
	float   m_specPower;
	bool    m_dot3Enabled = false;
	bool    m_phongEnabled = false;
	bool    m_BlinnphongEnabled = false;
	Vector3 m_position;

	//Static_Member_Variables

	//Methods
	void SetPosition(Vector3 position)   { m_position          = position;    }
	void SetSpecPower(float specPower)   { m_specPower         = specPower;   }
	void SetSpecFactor(float specFactor) { m_specFactor        = specFactor;  }
	void SetDot3(bool value)			 { m_dot3Enabled       = value;       }
	void SetPhong(bool value)			 { m_phongEnabled      = value;       }
	void SetBilnnPong(bool value)		 { m_BlinnphongEnabled = value;       }

	void SetUniformsInShader();

	void ResetValues();

	float GetSpecFactor()                { return m_specFactor; }
	float GetSpecPower()                 { return m_specPower;  }
	Vector3 GetPosition()                { return m_position;   }

	PointLight(Renderer *renderer);
	~PointLight();
	
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