#pragma once
#include <vector>
/*\class  : NetObjectConnectionView	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 12/5/2018 2:35:23 AM
* \contact: srsrakhil@gmail.com
*/
class NetObjectView;
class NetObjectConnectionView
{

public:
	//Member_Variables
	std::vector<NetObjectView*> m_netObjectViews;
	//Static_Member_Variables

	//Methods

	NetObjectConnectionView();
	~NetObjectConnectionView();

	void PushNetObjectView(NetObjectView *netObjView);
	void RemoveNetObjectView(uint8_t objectID);
	void UpdateAge(uint8_t objectID);
	NetObjectView * GetNetObjectView(uint8_t objectID);
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