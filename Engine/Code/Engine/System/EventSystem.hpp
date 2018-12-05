#pragma once

/*\class  : EventSystem	   
* \group  : <GroupName>	   
* \brief  :		   
* \TODO:  :		   
* \note   :	   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 12/3/2018 1:22:21 AM
* \contact: srsrakhil@gmail.com
*/
 
class EventSystem
{

public:
	//Member_Variables
	void *classPtr	  = nullptr;
	void *functionPtr = nullptr;
	//Static_Member_Variables

	//Methods

	EventSystem();
	~EventSystem();
	
	void Subscribe(void * ptr, void *functionPtr);
	void Trigger();
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