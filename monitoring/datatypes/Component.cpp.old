#include "Component.h"

namespace monitoring {

Component::Component()
{
}


Component::Component(const uint64_t cid, const uint64_t hwid, 
		     const uint64_t swid, const uint64_t iid)
   : cid_(cid), hwid_(hwid), swid_(swid), iid_(iid)
{
}


Component::Component(const Component &c)
{
	cid_  = c.cid();
	hwid_ = c.hwid();
	swid_ = c.swid();
	iid_  = c.iid();
}


uint64_t Component::cid() const 
{ 
	return cid_; 
}


uint64_t Component::swid() const 
{ 
	return swid_; 
}


uint64_t Component::hwid() const 
{ 
	return hwid_; 
}


uint64_t Component::iid() const 
{ 
	return iid_; 
}

}
