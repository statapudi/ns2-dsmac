#include <protoname/protoname.h>
#include <protoname/protoname_pkt.h>
#include <protoname/protoname_rtable.h>

/*******************************************************************
* Function		:protoname_rtable()
* 
* Description	:Constructor for protoname_rtable class
* 				  
* Prototype		:void  Protoname::getOneHopInfo()
* 
* Inputs		:void
* 				  
* Outputs		:void	 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya

* 
*********************************************************************/
protoname_rtable::protoname_rtable() { }

/*******************************************************************
* Function		:print()
* 
* Description	:Function to print the rtable info
* 				  
* Prototype		:void protoname_rtable :: print(Trace * out)
* 
* Inputs		:Trace * out : logfile to which the output has to be
* 				 written
* 				  
* Outputs		:void	 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/
void protoname_rtable :: print(Trace * out)
{
	printf("\n\tNode\tHops");
	//sprintf(out->pt_->buffer(),"P\tNode\tHops");
	//out->pt_->dump();
	
	for(rtable_t :: iterator it = rt_.begin(); it != rt_.end(); it++)
	{
		printf("\n\t%d\t%d",(*it).first,(*it).second);
		//sprintf(out->pt_->buffer(),"P\t%d\t%d",(*it).first,(*it).second);
		//out->pt_->dump();
	}
}

/*******************************************************************
* Function		:clear()
* 
* Description	:Function to remove all entries in routing table
* 				  
* Prototype		:void protoname_rtable :: clear()
* 
* Inputs		:void
* 				  
* Outputs		:void	 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/
void protoname_rtable :: clear()
{
	rt_.clear();
}


/*******************************************************************
* Function		:rm_entry()
* 
* Description	:Function to remove an entry given an destination address
* 				  
* Prototype		:void protoname_rtable :: rm_entry(nsaddr_t dest)
* 
* Inputs		:nsaddr_t dest : address to be removed from routing table
* 				  
* Outputs		:void	 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/
void protoname_rtable :: rm_entry(nsaddr_t dest)
{
	rt_.erase(dest);
}

/*******************************************************************
* Function		:add_entry()
* 
* Description	:Function to add a new entry in the routing table
* 				 given its destination and hop address
* 				  
* Prototype		:void protoname_rtable :: add_entry(nsaddr_t dest,nsaddr_t next)
* 
* Inputs		:nsaddr_t dest : Destination address
* 				 nsaddr_t next : Hop distance
* 				  
* Outputs		:void	 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/
void protoname_rtable :: add_entry(nsaddr_t dest,int next)
{
	rt_[dest] = next ;
} 

/*******************************************************************
* Function		:lookup()
* 
* Description	:Function to lookup the next hop address of an entry 
* 				 given its destination address
* 				  
* Prototype		:nsaddr_t protoname_rtable :: lookup(nsaddr_t dest)
* 
* Inputs		:nsaddr_t dest : Destination address
* 				  				  
* Outputs		:nsaddr_t : returns the next hop address	 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/
int protoname_rtable :: lookup(nsaddr_t dest)
{
	rtable_t :: iterator it = rt_.find(dest);
	if(it == rt_.end())
		return IP_BROADCAST;
	else
		return (*it).second;
}

/*******************************************************************
* Function		:size()
* 
* Description	:Function to return the size of routing table
* 				  
* Prototype		:u_int32_t protoname_rtable :: size()
* 
* Inputs		:void
* 				  				  
* Outputs		:u_int32_t : returns the size of routing table	 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/
u_int32_t protoname_rtable :: size()
{
	return rt_.size();
}


