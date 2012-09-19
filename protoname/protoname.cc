#include <stdio.h>
#include <stdlib.h>
#include <cmu-trace.h>
#include <address.h>
#include <protoname/protoname.h>
#include <protoname/protoname_pkt.h>
#include <protoname/protoname_rtable.h>
#include <protoname/protoname_queue.h>

/*static variables*/
int hdr_protoname_pkt:: offset_;
void transmitFailedCallback(Packet *pkt, void *data)
{
//	Protoname *agent = (Protoname *)data;  // cast of trust
//  	agent->transmitFailed(pkt);
  
}

/*******************************************************************
* Function		:Protoname()
* 
* Description	:Constructor for Protoname Class
* 				  
* Prototype		:Protoname :: 
* 			     Protoname (nsaddr_t id) : Agent(PT_PROTONAME), pkt_timer_(this)
* 
* Inputs		:nsaddr_t id: Unique Node Id
*                pkt_timer_(this) : Protoname Packet timer 
* 
* Outputs		:void 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/
Protoname :: Protoname (nsaddr_t id) : Agent(PT_PROTONAME),init_timer_(this), 
									  startup_timer(this),process_timer(this),
									  data_timer_(this),neighbor_timer(this),
									  monitor_timer(this),recv_timer(this)
                                    
                                
{
	bind_bool("accesible_var_",&accesible_var_);

	/* get the god object */
	godinstance_ = God::instance();
	assert(godinstance_ != 0);
	iSeverityLevel = 0;
	/* save the given identifier as the routing agent's address */
	ra_addr_ = id;
	//Set the forwarder for this node as -1 implies no forwarder for this node
	forwarder = -1;
    actNodeCount = 0;
	getBaseStationID();
	//min = -1;
	max = -1;
	monitoredLinks = 0;
	//Base Station will be in active-datapath by default.
	if(ra_addr() == baseStation || ra_addr() == srcNode)
		isActive = true;  
	
	if(isActive == true)
	{
		printf("Node %d is Active\n",ra_addr());
	}
	else
	{
		printf("Node %d is Inactive\n",ra_addr());
	}	
	for(int i=0;i<MAX_MONITORED_LINKS;i++)
	{
		ack_timer[i] = new Ack_Timer(this) ;
	}
	for(int i=0;i<QUEUEPACKET_SZ*100;i++)
	{
		route_timer[i] = new RouteData_Timer(this) ;
	}
	for(int i=0;i<20;i++)
	{
		prioritySources[i] = -1;
	}	
	prioritySourceCount = 0;
	
	for(int i=0;i<20;i++)
	{
		prevPrioritySources[i] = -1;
	}	
	prevPrioritySourceCount = 0;
	
	for(int i=0;i<20;i++)
	{
		reduceReporting[i] = -1;
	}	
	reduceReportingCount = 0;
	overallReduceReporting = false;
	processInterval = -1;
	setPriority = false;
	hasIncreasedReportingRate = false;
	//currentSendingState = false;
	//prevSendingState = false;
	noSendingState = true;
	//noSendingState = 0;
	isFinalStage = false;
	forwarder_req_mesg_sent = 0;
	isMissingInfoRecvd = false;
	missing_info_mesg_sent = 0;
	active_neighbor_mesg_recvd = 0;
	recoveryIntervalSources = 0;
	hopDistance = -1;
	activeNodeCount = 0;
	init_neighborStatus();
	init_MonitoringNodes();
	init_myMonitors();//Setting myMonintor to -1 initially.
	init_timer_.resched(0.0);
	monitorPacketCount = 0;
	isForwarderSource = false;
	isForwarder = false;
	increaseReportingRate = false;
	decreaseReportingRate = false;
	increaseReportingCount = 0;
	decreaseReportingCount = 0;
	recoveredSourcesCount = 0;
	notRecoveredSourcesCount = 0;
	recoveredIntervals = 0;
	notRecoveredIntervals = 0;
	//prevPacketSent = NULL;
} 



/*******************************************************************
* Function		:command()
* 
* Description	:This function processes the commands given from TCL script
* 				  
* Prototype		:int
*				 Protoname :: command (int argc, const char*const* argv)
* 
* Inputs		:int argc : number of arguments passed
* 				 const char*const* argv : arguments passed
* 
* Outputs		:TCL_OK on Success
* 				 TCL_ERROR on Failure 	 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/
int Protoname :: command (int argc, const char*const* argv)
{
     if (argc == 2 )
	 {
		if(strcasecmp(argv[1], "start") == 0 )
		{
			startup_timer.resched(2.0);
			process_timer.resched(205.0);
			//data_timer_.resched(206.0);
			//recv_timer.resched(207.0);
			return TCL_OK;
			
		}
		else if (strcasecmp(argv[1],"print_rtable") == 0)
		{
			if(logtarget_!=0)
			{
				sprintf(logtarget_->pt_->buffer(),"P %f _%d_ Routing Table",CURRENT_TIME,
				ra_addr());
				logtarget_->pt_->dump();
				rtable_.print(logtarget_);
			}
			else
			{
				fprintf(stdout,"%f _%d_ If you want to print this routing table you must create a trace file in  your tcl script",
						CURRENT_TIME,
						ra_addr());
			}
			return TCL_OK;
		}
		else if(strcasecmp(argv[1], "first_set_monitors") == 0)
		{ 
		
			if(isActive == true && ra_addr()!= baseStation && getMyMonitor() == -1)
			{
			    printf("********* CONFIGURING THE MONITORS in FIRST STAGE********\n");
				configureMonitor(FIRST_STAGE);
				
			}
			return TCL_OK;
		}
		else if(strcasecmp(argv[1], "second_set_monitors") == 0)
		{
			if(isActive == true && ra_addr()!= baseStation && getMyMonitor() == -1)
			{
				printf(" ********* CONFIGURING THE MONITORS IN SECOND STAGE********\n");
				configureMonitor(SECOND_STAGE);
				
			}
			return TCL_OK;
		}	
		else if(strcasecmp(argv[1], "final_set_monitors") == 0)
		{
			if(isActive == true && ra_addr()!= baseStation && getMyMonitor() == -1)
			{
				printf(" ********* CONFIGURING THE MONITORS IN FINAL STAGE********\n");
				configureMonitor(ALL);
				
			}
			return TCL_OK;
		}	
		
		else if(strcasecmp(argv[1], "check_forwarder") == 0)
		{
			configureFinalForwarder();
			return TCL_OK;
		}
		else if(strcasecmp(argv[1], "set_inactivenodes") == 0)
		{
			setInactiveNodes();
			return TCL_OK;
		}		
		
		else if(strcasecmp(argv[1], "check_monitors") == 0)
		{
			monitor_req_mesg_sent = MONITOR_REQ_RESEND_COUNT +1 ;
			monitor_agree_mesg_sent = MONITOR_AGREE_RESEND_COUNT;
            monitor_notification_mesg_sent = MONITOR_NOTIFICATION_RESEND_COUNT;
			forwarder_req_mesg_sent = FORWARDER_REQ_MESG_RESEND_COUNT;
			
			if(isActive == true && ra_addr()!= baseStation && forwarder !=baseStation )
			{
				printf(" Node %d forwarder is (ActiveNode) %d and hopDistance :%d\n",ra_addr(),forwarder,hopDistance);
				
				mac->setMyMonitor(getMyMonitor());
				
                if(getMyMonitor() == -1)
					printf(" No monitor for link between %d and %d \n",ra_addr(),forwarder);
				else
					printf(" Monitor for node %d and %d is %d \n",ra_addr(),forwarder,getMyMonitor());
				
			}
			if(isActive == false && isMonitor == true)
			{

				for(int i = 0; i < MAX_MONITORED_LINKS;i++)
				{
				 	monInfo[i].sender = -1;
				  	monInfo[i].receiver = -1;
  		
				}
				int k=0;
				for(int i=0 ;i< MAX_MONITORED_NODES;i+=2)
				  {
					  	if(monitoringNodes[i] != -1)
					  	{
					  		printf(" Monitor for node %d and %d is %d\n",monitoringNodes[i],monitoringNodes[i+1],ra_addr());
					  		monInfo[k].sender = monitoringNodes[i];
				  		    monInfo[k].receiver = monitoringNodes[i+1];
							k++;
					  	}				  	
				  	
				  }	
				  mac->setMonitorStatus(monitoringNodes,2);
		
				printf(" Node %d forwarder is (MonitorNode)%d and hopDistance :%d\n",ra_addr(),forwarder,hopDistance);
			}
			if(isActive == false && isMonitor == false)
			{
				printf(" Node %d forwarder is (InactiveNode)%d and hopDistance :%d\n",ra_addr(),forwarder,hopDistance);
			}		
			if(forwarder == baseStation && isActive == true)
			{
				 printf(" Node %d forwarder is (baseStation) %d and hopDistance :%d\n",ra_addr(),forwarder,hopDistance);
				 printf(" Monitor not required for node %d as its forwarder is BaseStation \n",ra_addr());
			}
			mac->setMyForwarder(forwarder);
			return TCL_OK;
		}		
		
	}
	else if (argc == 3)
	{
		//Obtains corresponding dmux to carry packets to upper layers
		if (strcmp (argv[1], "port-dmux") == 0)
		{
			dmux_ = (PortClassifier*)TclObject :: lookup(argv[2]);
			if(dmux_ == 0)
			{
				fprintf(stderr, "%s: %s lookup of %s failed \n", __FILE__,argv[1],
																argv[2]);
				return TCL_ERROR;
			}
			return TCL_OK;
		}
		//obtains corresponding tracer
		else if (strcmp(argv[1],"log-target") == 0 ||
				 strcmp(argv[1],"tracetarget") == 0 )
		{
				logtarget_ = (Trace*)TclObject :: lookup(argv[2]);
				if(logtarget_ == 0)
					return TCL_ERROR;
				
				return TCL_OK;
				 
		}

		else if(strcasecmp(argv[1], "install-tap") == 0)
		{
		    mac = (Mac802_11*) TclObject::lookup(argv[2]);	
			return TCL_OK;
		}
		//Ramya added
		else if(strcasecmp(argv[1], "down-target") == 0)
		{
			ll = (LL*) TclObject::lookup(argv[2]);	
			return TCL_OK;
		}	
	}
	
	//pass the command to the base class
	return Agent :: command(argc,argv); 
}

/*******************************************************************
* Function		:recv()
* 
* Description	:This function processes the packets received
* 				  
* Prototype		:void
*				 Protoname :: recv(Packet* p, Handler* h)
* 
* Inputs		:Packet* p : Packet received
* 				 Handler* h : Handler to the packet
* 
* Outputs		:void	 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
* *********************************************************************/

void Protoname :: recv(Packet* p, Handler* h)
{
		struct hdr_cmn* ch	= HDR_CMN(p);
		struct hdr_ip*  ih 	= HDR_IP(p);

		switch(ch->ptype())
		{		
			case PT_PROTONAME:

					
					if(ih->saddr() == ra_addr())
					{  
						LOG_INF(" Node :%d received a Protoname from itself destined to %d.I hate this \n",ra_addr(),ih->daddr());
					}
					else
					{
							recv_protoname_pkt(p);
						
					}
					return;
			case PT_CBR:
				    
					//current node is not Active and it is not monitor. 
					if(isActive == false && isMonitor == false)
						 return;
					//LOG_INF(" Node %d received data packet %d next hop is %d at %lf\n",ra_addr(),ch->uid(),ch->next_hop(),CURRENT_TIME);
					if(ih->saddr() == ra_addr())
					{
						//if the source address is same as the current node address, we received our own packet
						//also the number of times the packet is forwarded will be zero	
						//Drop the packet if it is going in loop
						if(ch->num_forwards() > 0)
						{
							drop(p,DROP_RTR_ROUTE_LOOP);
							return;
						}
						else if(ch->num_forwards() < 1)
						{

							printf(" Node %d created a DATA packet:%d at %lf and Node is ",ra_addr(),ch->uid(),CURRENT_TIME);
							//Add the timestamp to the packet
							ch->timestamp() = CURRENT_TIME;				
							//Add the Ip header
							ch->size() += IP_HDR_LEN;
							ch->genid() = ra_addr();
							//Set the revious hop as the current node address
							ch->prev_hop() = ra_addr();
							//Set the priority of the packets based on MissingInfoMesg from BaseStation
							
							//Set the number of times this packet was forwarded as zero.
							ch->num_forwards() = 0;
							
							/* Code to drop alternate packets to make the source nodes to send the
							 * packets at the correct interval*/			 
							 if((noSendingState == true) && (increaseReportingRate == false) && (decreaseReportingRate == false))
							 {
							 		printf("\n Node %d dropping the alternate packet %d  with priority %d at %lf\n",ra_addr(),ch->uid(),ch->priority(),CURRENT_TIME);
							 		drop(p); /*drop the first one and send the second*/
							 		noSendingState = false;
							 		return;
							 }	
							
							if(setPriority == true)
							{
								printf("\n Node %d setting priority to 1 for packet %d at %lf\n",ra_addr(),ch->uid(),CURRENT_TIME);
								ch->priority() = 1;
								ll->setPriority(true);

							}
							else
							{
								printf("\n Node %d setting priority to 0 for packet %d at %lf\n",ra_addr(),ch->uid(),CURRENT_TIME);
								ch->priority() = 0;
								ll->setPriority(false);							
								
							}
							
							
							if((increaseReportingRate == true) && (decreaseReportingRate == false))
							{	
								printf("\n Node %d increasing the reporting rate and reportingCount is %d at %lf\n",ra_addr(),increaseReportingCount,CURRENT_TIME);
								noSendingState = false;
								increaseReportingCount++;
								hasIncreasedReportingRate = true;
								if(increaseReportingCount > 70)
								{
									noSendingState = true;
									increaseReportingCount = 0;
									increaseReportingRate = false;
								}

							}
							else
							if((increaseReportingRate == false) && (decreaseReportingRate == true))	
							{
								printf("\n Node %d decreasing the reporting rate by dropping the packet %d  with priority %d and decreasereportingCount is %d at %lf\n",ra_addr(),ch->uid(),ch->priority(),decreaseReportingCount,CURRENT_TIME);
								drop(p);
								decreaseReportingCount++;
								
								if(decreaseReportingCount > 50)
								{
									noSendingState = true;
									decreaseReportingCount = 0;
									decreaseReportingRate = false;
								}
								return;
								
							}
						}
						
					}
					else
					{						
						printf(" Node %d received DataPacket %d from %d at %lf\n",ra_addr(),ch->uid(), ch->prev_hop(),CURRENT_TIME);
						//recevied data packet forwarded by someone else. we should forward it.
						//noSendingState = false;
							//printf("\n reason for being dropped is this\n");
						ih->ttl_ --;
						if(ih->ttl_ == 0)
						{
							printf(" Node %d dropping packet %d as ttl became zero\n",ra_addr(),ch->uid());
							drop(p,DROP_RTR_TTL);
							return ;	
						}
						//random function to drop packets
						//ch->prev_hop() = ra_addr(); //Setting the prev hop to be the current node address
		 				srand ( (unsigned)Random::seed_heuristically());
						int randm =  (rand()%10000)+1;
						//if(isActive == true && ih->prio() == ra_addr())
								
						
						/*if(isActive == true && ch->next_hop() == ra_addr())
							{
						  		if(randm <= 1000)
						   		{
						            printf(" Forcing Node %d to drop packet %d at %lf\n",ra_addr(),ch->uid(),CURRENT_TIME);
									drop(p,DROP_RTR_TTL);
									return;				
						   		}
							}*/
						/*
						if((ra_addr() == 0) && ((ch->uid()% 2) == 0))
						{
							printf(" Forcing Node %d to drop packet %d at %lf\n",ra_addr(),ch->uid(),CURRENT_TIME);
							drop(p,DROP_RTR_TTL);
							return;
						}*/	
					}
					break;
		}
    double rxtime;
    rxtime = ch->size()*8/M_BANDWIDTH;
    /* Ramya: Modify code to push the packets into stack and pop the last one when timer ends
    */
    
    /*if(ih->saddr() == ra_addr()) //Current node is source
    {
    	//Push the packets into the queue till then
    	DataQueue.insert_item(p, ch->prev_hop());
    }
    else*/
    
    //Ends
	//forward_data(p,IP_BROADCAST);
	
	if((noSendingState == false) && (ih->saddr() == ra_addr())) 
	{
		printf("\n SourceNode %d forwarding packet at %lf\n",ra_addr(),CURRENT_TIME); 
		forward_data_with_monitor(p,IP_BROADCAST);
		noSendingState = true;
	}
	else
	if(ih->saddr() != ra_addr())
	{
		printf("\n Node %d forwarding packet at %lf\n",ra_addr(),CURRENT_TIME); 
		forward_data_with_monitor(p,IP_BROADCAST);
	}
	else
	printf("\nNode %d not forwarding packet %d\n",ra_addr(),ch->uid());
		
	
	
	return;
}
/*******************************************************************
* Function		:process_pkt()
* 
* Description	:This function processes the data packets received
* 				  
* Prototype		:void
				 Protoname :: process_pkt(Packet* p)
* 
* Inputs		:Packet* p : Packet received
* 				  
* Outputs		:void	 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/
void Protoname :: process_pkt(Packet* p)
{
	struct hdr_cmn* ch	= HDR_CMN(p);
	struct hdr_ip*  ih 	= HDR_IP(p);
	//LOG_DBG(" into process_pkt for node :%d\n",ra_addr());
	//If the current node is base station, no need to forward the packet.
	if(ra_addr()== baseStation)
	{
		LOG_DBG(" Base station:%d received the packet from %d of size %d and prev-hop %d \n",ra_addr(),
		ih->daddr(),ch->size(),ch->prev_hop());
	}
	// As the node received the packet, Rx Energy should be decremented here*/
	dmux_->recv(p, (Handler*)0);
	
	return;	

}

/*******************************************************************
* Function		:forward_data()
* 
* Description	:This function forwards a protoname packet
* 				  
* Prototype		:void  Protoname::forward_data(Packet* p)
* 
* Inputs		:Packet* p : packet to be forwarded
* 				  
* Outputs		:void	 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/
void  Protoname ::forward_data(Packet* p,nsaddr_t type)
 {
	    
    struct hdr_cmn* ch = HDR_CMN(p);
    struct hdr_ip* ih = HDR_IP(p);
	
	double txtime;
	txtime = ch->size()*8/M_BANDWIDTH;

	if(isActive == true)
	{
		//Current node got the Data packet. Forward the packet
		if(ih->saddr() != ra_addr()) 
		{
			//Case where the packet is not destined for current node
			if(ih->prio()!= ra_addr())
			{
				LOG_WARN(" Dropping the packet %d,as not destined for node %d\n",ra_addr(),ch->uid());
				return;
			}
			
			//Case where the packet is destined for current node or has to be forwarded
			if(ih->prio() == ra_addr())
			{
				//Case where the packet's final destination is the current node
				if(ih->daddr() == ra_addr())
				{
					if(ra_addr() == baseStation)
					{
						//printf("Base Station received packet %d from %d at % lf ",ch->uid(),ch->prev_hop(),CURRENT_TIME);
						dmux_->recv(p, (Handler*)0);
						return;
					}
					else
						printf("Node %d should not receive packet %d from %d at % lf ",ra_addr() ,ch->uid(),ch->prev_hop(),CURRENT_TIME);
						return;					
				}
				//Case where the packet's final destination is not the current node.Data has to be forwarded.
				else
				{

				}

			}
			else
			{
			   //Data packet is not for this node. 
			   printf("Node %d overheard packet %d from %d at % lf ",ra_addr() ,ch->uid(),ch->prev_hop(),CURRENT_TIME);
			   return;
			}
			

		}
		else //Current node originated the Data packet. Broadcast the packet.
		{
			if(ra_addr() == baseStation)
			{
				//printf("Base Station received packet\n");
				//dmux_->recv(p, (Handler*)0);
				return;
			}
			//Control should go down for adding the received datapacket to queue.
		}
	}
	//Case for Inactive nodes.
	else
	{
		 return;		
	}

	printf("\n forwarder of node %d is %d\n",ra_addr(),forwarder);
	
	ch->next_hop() = forwarder;
	ch->prev_hop() = ra_addr();
	ch->direction() = hdr_cmn::DOWN;
	ch->addr_type() = NS_AF_INET;
	ch->num_forwards() += 1;
	ch->xmit_failure_ = transmitFailedCallback;
	ch->xmit_failure_data_ = (void *)this;
	ih->prio() = forwarder;
    txtime = ch->size()*8/M_BANDWIDTH;	
    // send the data Packet		
	srand ( (unsigned)Random::seed_heuristically());
	float fran = (rand()%10)*(0.1)*(hopDistance*0.1)*(0.001);
    
	//Handling the data in case of inactive nodes
	if(isActive == false )
	{			
	  	return;	
	}	
	printf("Node %d forwarding the data packet to %d\n",ra_addr(), ch->next_hop()); 
	Scheduler::instance().schedule(target_,p,fran);
	
	return;

 }

/*******************************************************************
* Function		:getOneHopInfo()
* 
* Description	:This function gets one hop neighborhood info 
* 				 using the god object and duty cycle offset table.
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
void Protoname::getOneHopInfo()
 {
	/* get the number of nodes in the network */
	int nodeCount = godinstance_->nodes();
	int i;
    int j =0;
    
    //LOG_DBG(" **********node count is %d and my address is %d************\n",nodeCount,ra_addr());
	for(i = 0; i < nodeCount; i++)
	{
		if((ra_addr() != (nsaddr_t)i) && (godinstance_->hops(ra_addr(), i)==1)) // 1 hop neighbors
		{		
			if(i == baseStation)
				rtable_.add_entry(i,0);
			else
				rtable_.add_entry(i,godinstance_->hops(i, baseStation));
			
			nodesmap[j] = i ;
			//fill the neighbor node structure
			neighbors[j].nodeId = i;
			neighbors[j].isActive = false;
			neighbors[j].dist = godinstance_->hops(i, baseStation);	
			neighbors[j].isForwarder = false;
			j ++;
		}
	}

}

/*******************************************************************
* Function		:getHopDistancefromBS
* 
* Description	:This function gets the hop distance of the node from the base 
* 				 station
* 				  
* Prototype		:void  Protoname::getHopDistancefromBS()
* 
* Inputs		:void
* 				  
* Outputs		:int	 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/
int Protoname :: getHopDistancefromBS()
{
	//ra_addr() is the current nodes address.
	int dist;
	//returns the no of hops of the current node to the base station.
	dist = godinstance_->hops(ra_addr(), baseStation);
	return dist;
	
}	
void Protoname :: sendPacket(Packet* receivedPacket,int packetType,nsaddr_t dest)
{
	//Accessing the data in the received packet
	struct hdr_cmn* rp_ch	= NULL;
	struct hdr_protoname_pkt * rp_ph = NULL;
	int cnt =0;
	if(receivedPacket != NULL)
	{
		rp_ph = HDR_PROTONAME_PKT(receivedPacket);
		rp_ch = HDR_CMN(receivedPacket);
	}
	//Creating a new packet to be sent out
	// allocate the packet 
	Packet* p           = allocpkt();
	struct hdr_cmn* ch	= HDR_CMN(p);						// Common header
	struct hdr_ip*  ih 	= HDR_IP(p);						// Ip header
	struct hdr_protoname_pkt* ph = HDR_PROTONAME_PKT(p);	// Protoname header
		
	//fill the headers of the packet to be send		
	//Protoname header
	ph->pkt_hopCount()  = hopDistance;  
    ph->pkt_len()          = 74 + MAX_NEIGHBORS_COUNT;
    ph->pkt_seq_num()      = seq_num_ ++;
   	ph->pkt_nodeStatus()   = (isActive == true)?1:0;	
   	ph->pkt_processInterval()  = processInterval;
	
	//Commnon header
	ch->ptype()            = PT_PROTONAME;
	ch->direction()        = hdr_cmn::DOWN;
	ch->size()             = IP_HDR_LEN + ph->pkt_len();
	ch->error()            = 0;
	ch->next_hop()         = dest;
	ch->addr_type()        = NS_AF_INET;
	ch->timestamp()        = CURRENT_TIME;
	ch->xmit_failure_ = transmitFailedCallback;
	ch->xmit_failure_data_ = (void *)this;
	//Ip header
	ih->saddr()            = ra_addr();
	ih->daddr()            = dest;
	ih->sport()            = RT_PORT;
	ih->dport()            = RT_PORT;
	ih->ttl()              = IP_DEF_TTL;	
	ih->flowid() 		   = packetType;
	//printf("\n flow id in send packet is %d\n",ih->flowid());
	double txtime;
	txtime = ch->size()*8/M_BANDWIDTH;
	
	//Calculate the Transmit energy for messages used while data forwarding.
	//Do not decerement energy for initialization messages.
	
	switch(packetType)
	{
		case ACK_MESG :
				break;
		case FORWARDER_REQ_MESG:
		
				//printf("\nSending forwarder req mesg\n");
				ph->pkt_hopCount() = hopDistance;  
				break;

		case FORWARDER_REPLY_MESG:
				break;	
				
		//Base Station informing sources of lack of data
		case MISSING_INFO_MESG:
				if(ra_addr() == baseStation)
				{
					printf("\n BaseStation: Number of sources to be given priority is %d\n",prioritySourceCount);
					
					for(int n=0;n < prioritySourceCount; n++)
					{
						ph->pkt_prioritySources_[n]= prioritySources[n];
					}
					ph->pkt_prioritySourceCount() = prioritySourceCount;
					
					printf("\n BaseStation: Number of sources to reduce reporting rate is %d\n",reduceReportingCount);
					for(int n=0;n < reduceReportingCount; n++)
					{
						ph->pkt_reduceReporting_[n]= reduceReporting[n];
					}	
					ph->pkt_reduceReportingCount() = reduceReportingCount;
						
				}
				else
				{
					printf("\n OtherNode: Number of sources to be given priority is %d\n",rp_ph->pkt_prioritySourceCount());
			
					for(int n=0;n< rp_ph->pkt_prioritySourceCount(); n++)
					{
						ph->pkt_prioritySources_[n]= rp_ph->pkt_prioritySources_[n];
					}
					ph->pkt_prioritySourceCount() = rp_ph->pkt_prioritySourceCount();
					
					printf("\n OtherNode: Number of sources to reduce reporting rate is %d\n",rp_ph->pkt_reduceReportingCount());
					for(int n=0;n < rp_ph->pkt_reduceReportingCount(); n++)
					{
						ph->pkt_reduceReporting_[n]= rp_ph->pkt_reduceReporting_[n];
					}	
					ph->pkt_reduceReportingCount() = rp_ph->pkt_reduceReportingCount();
					
				}	
				break;	
				
		case NEIGHBOR_REQ_MESG:
				//printf(" Sending NEIGHBOR_REQ_MESG ");
				break;	
				
		case SET_FORWARDER_MESG:
				break;
		
		case NEIGHBOR_REPLY_MESG:
				//Fill the one hop neighbors and the forwarder
				//printf("\n Node %d sending neighbor reply message to %d\n",ra_addr(),dest);
				ph->pkt_forwarder()          = forwarder;
				ph->pkt_monitor() 			 = getMyMonitor();
				for(int n =0;n<(int)rtable_.size();n++)
				{
					if(neighbors[n].isActive == false)
					{
						ph->pkt_neighbors_[n]=neighbors[n].nodeId;
					}
				}
				break;	
				
		case ACTIVE_NEIGHBOR_REQ_MESG:
				break;
			
		case ACTIVE_NEIGHBOR_REPLY_MESG:
				//printf("Node %d active neighbor count is %d\n",ra_addr(),activeNodeCount);
				ph->pkt_activeNeighborCount() = activeNodeCount;
				/*Using this field to know if the monitors forwarder is same as the nodes forwarder */
				ph->pkt_forwarders_forwarder()= forwarder; 
				
				break;	
				
		case MONITOR_REQ_MESG: 
				ph->pkt_forwarder()          = forwarder;
				/* If the forwarder has a monitor don't configure monitor for it.*/
				/*if(forwarders_monitor == -1)
				ph->pkt_forwarders_forwarder() = forwarders_forwarder;
				else*/
				ph->pkt_forwarders_forwarder() = -1;
				monitor_req_mesg_sent ++;		
				break;	
		
		case MONITOR_AGREE_MESG:
				if(receivedPacket !=NULL)
					ph->pkt_monitored_links() = checkMonInfo(dest,rp_ph->pkt_forwarder(),rp_ph->pkt_forwarders_forwarder());
				//These details will be used while removing the data from moninfo structure
				ph->pkt_forwarder()          = rp_ph->pkt_forwarder();
				ph->pkt_forwarders_forwarder() = rp_ph->pkt_forwarders_forwarder();	   
				//printf("\n Node %d sending Monitor agree message\n",ra_addr());
				monitor_agree_mesg_sent ++ ;
				break;
		case MONITOR_NOTIFICATION_MESG:
				ph->pkt_forwarder()           = forwarder;
				ph->pkt_monitored_links()     = monitoredLinks;
				ph->pkt_monitor()= getInterimMonitor();
				ph->pkt_forwarders_forwarder() = forwarders_forwarder;
				monitor_notification_mesg_sent ++;
				monitoredLinks = 0;
				break;
				
	}	
	ih->daddr()     = dest;
	ph->pkt_dest()  = dest;
	
	if((packetType == FORWARDER_REQ_MESG))//|| (packetType == MISSING_INFO_MESG))
	  Scheduler::instance().schedule(target_, p,(ra_addr()*0.01)+JITTER);
	else
	   //printf("\n Jitter value is %lf\n",JITTER);
	   Scheduler::instance().schedule(target_, p,JITTER);	
		
}

/*******************************************************************
* Function		:recv_protoname_pkt()
* 
* Description	:This function processes the protoname packets received
* 				  
* Prototype		:void
				 Protoname :: recv_protoname_pkt(Packet* p)
* 
* Inputs		:Packet* p : Packet received
* 				  
* Outputs		:void	 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
*********************************************************************/

void Protoname :: recv_protoname_pkt(Packet* p)
{
	struct hdr_ip* ih	= HDR_IP(p);
    struct hdr_cmn* ch	= HDR_CMN(p);
	struct hdr_protoname_pkt * ph = HDR_PROTONAME_PKT(p);
	nsaddr_t srcAddr = ih->saddr();
	int seq = 0;
	bool isSource = false;
	//all routing messages are sent from and to port RT_PORT,so we check it
	assert(ih->sport() == RT_PORT);
	assert(ih->dport() == RT_PORT);
	/*Processing of protoname packet*/
	switch(ih->flowid())
	{
		
		case SET_FORWARDER_MESG:
				printf("\n Node %d received set_forwarder_mesg from %d and setting itself as forwarder\n",ra_addr(), ih->saddr());
				isForwarder = true;
				break;
				
		case FORWARDER_REQ_MESG:
				//Node received forwarder message. send the forwarder reply message here.
				//printf("\n Node %d received forwarder request message \n", ra_addr());
				if(ra_addr() == baseStation)
				{	
					return;
				}

				if(ih->saddr()== baseStation )
				{
					forwarder = baseStation;
					printf(" Forwarder of node %d is %d \n",ra_addr(),forwarder);
					hopDistance  = 1;
					if(isActive == true)
						setUpDataPath();
					return;
				}	
				if(forwarder == -1)
				{
					/*This code change is done to make sure the forwarder is not a source node.
					 * By looking at the trace file and output file, it appeared that packest 
					 * were getting delayed at nodes which are both forwader and source */
					
						forwarder = ih->saddr();
						hopDistance = ph->pkt_hopCount()+1;
						printf(" Forwarder of node %d is %d \n",ra_addr(),forwarder);
						if(isActive == true)
							setUpDataPath();
							
				}	
				else
				{
					
					if(hopDistance >  (ph->pkt_hopCount()+1)) //&& isSource == false)
					{
						
						forwarder = ih->saddr();
						printf(" Resend Forwarder of node %d is %d \n",ra_addr(),forwarder);
						hopDistance = ph->pkt_hopCount()+1;
						//The below code is to send the forwarder req again to make sure the hopdistance 
						//is set correctly.
						if(forwarder_req_mesg_sent >2)
							forwarder_req_mesg_sent--;
					}
					if(isActive == true)
					  setUpDataPath();
				}	
				
				break;		
				
		//Once a node receives this message, it passes the message down the network,
		//Also if the nodes acting as monitors receive it, they will take further action
		case MISSING_INFO_MESG:
				//printf("\nprocessInterval of node %d is %d at %lf\n",ra_addr(),processInterval,CURRENT_TIME);
				printf("Node %d received missing info message at %lf from node %d\n",ra_addr(),CURRENT_TIME,srcAddr);
				//if(ph->pkt_processInterval() == processInterval)
				{
				
					if(ra_addr() == baseStation)
					{
						return;
					}
					if((isActive == true ))//&& (isMissingInfoRecvd == false))
					{
						printf("\n Active Node %d received MISSING INFO message at %lf from node %d\n", ra_addr(),CURRENT_TIME,srcAddr);					
						
						if(isMissingInfoRecvd == false)
						{
								setPriority = false;
								increaseReportingRate = false;
								decreaseReportingRate = false;
								for(int j=0;j < ph->pkt_prioritySourceCount();j++)
								{
									if(ra_addr() == ph->pkt_prioritySources_[j])
									{
										printf("\n SourceNode %d received MISSING INFO message at %lf from node %d \n", ra_addr(),CURRENT_TIME,srcAddr);
										printf("\n Source node %d setting increase reporting rate to true\n",ra_addr()); 
										setPriority = true;
										if(hasIncreasedReportingRate == true)
										{	
											increaseReportingRate = false;
											hasIncreasedReportingRate = false;
										}	
										else
											increaseReportingRate = true;
									}	
								
								}
		
								for(int j=0;j < ph->pkt_reduceReportingCount();j++)
								{
									if(ra_addr() == ph->pkt_reduceReporting_[j])
									{
										printf("\n SourceNode %d received MISSING INFO message at %lf from node %d \n", ra_addr(),CURRENT_TIME,srcAddr);
										printf("\n Source node %d setting reduce reporting to true\n",ra_addr()); 
										decreaseReportingRate = true;	
									}	
								
								}
								isMissingInfoRecvd = true;
								
						}
							
						sendMissingInfoMesg(p);
					}
						
					if((isActive == false ))//&& (isMissingInfoRecvd == false))
					{
						//take further action here
						isMissingInfoRecvd = true;
						printf("\n Inactive node %d received missing info message at %lf\n",ra_addr(),CURRENT_TIME); 
					}	
				}
				else
					printf("\n Node %d received Missing info message too late at %lf\n",ra_addr(),CURRENT_TIME);

				break;	
				
		case NEIGHBOR_REQ_MESG:
				//printf("\n Node %d received neighbor req message from %d\n",ra_addr(),srcAddr);
				if(ih->daddr() == ra_addr() && ra_addr() != baseStation)
					sendPacket(p,NEIGHBOR_REPLY_MESG,srcAddr);
				break;
		
		case NEIGHBOR_REPLY_MESG:
				//printf("\n Node %d received neighbor reply message\n",ra_addr());
				if(ih->daddr() == ra_addr() && ra_addr() != baseStation)
			    {
					forwarders_forwarder = ph->pkt_forwarder();
					forwarders_monitor = ph->pkt_monitor(); 	 
					GetMonitor(ph->pkt_neighbors_);
			    }		
				break;
		case ACTIVE_NEIGHBOR_REQ_MESG:
				if(ih->daddr() == ra_addr() && ra_addr() != baseStation)
					sendPacket(p,ACTIVE_NEIGHBOR_REPLY_MESG,srcAddr);
				break;
		
		case ACTIVE_NEIGHBOR_REPLY_MESG:
				if(ih->daddr() == ra_addr() && ra_addr() != baseStation)
			    {
					commonInfo[active_neighbor_mesg_recvd].nodeId = ih->saddr();
					commonInfo[active_neighbor_mesg_recvd].activeCnt =  ph->pkt_activeNeighborCount();
					commonInfo[active_neighbor_mesg_recvd].forwarder = ph->pkt_forwarders_forwarder();
					commonInfo[active_neighbor_mesg_recvd].hopdistance = ph->pkt_hopCount();
					active_neighbor_mesg_recvd++;
			    }		
				break;	
		case ACK_MESG:
			//printf(" Received ACK_MESG  from %d to %d \n",ih->saddr(),ra_addr());
			if(ih->daddr() == ra_addr() && ih->saddr() == getInterimMonitor())
			{
					setMyMonitor(ih->saddr());
					printf(" Node %d setting its monitor as %d\n",ra_addr(),myMonitor);
					monitoredLinks = 0;
			}
			else
			{
				
			}
			
			break;	
		case MONITOR_REQ_MESG:
				if(CURRENT_TIME < ((6*(PACKET_TRANSMIT_TIME+JITTER)) +ch->timestamp()))
				{
					 if(ih->daddr() != ra_addr() )
					 {
						//LOG_DBG(" Node %d ignoring MONITOR_REQ_MESG destined for %d sent from %d\n",ra_addr(),ih->daddr(),ih->saddr()); 
						break;
					 }
					 else if (isActive == true && ih->daddr() == ra_addr() )
					 {
					 	//LOG_DBG(" node %d ignoring monitor request message from %d :Active\n",ra_addr(),ih->saddr());
					 	break;
					 }
					 if(isActive == false && activeNodeCount > 2)
				     {
					    	if((isMonitor == true && monitoredLinks < MAX_MONITORED_LINKS) ||
					    	   (isMonitor == false))
					    	{ 
		   						printf("Node %d received MONITOR_REQ_MESG\n",ra_addr());
					    		monQueue.insert_item(p, ih->daddr());
								monitor_timer.resched((activeNodeCount*(25 + MAX_NEIGHBORS_COUNT)*8)/M_BANDWIDTH);
					    	}
					    	//else
					    		//printf("Node %d received MONITOR_REQ_MESG but ignores as it is already a monitor\n",ra_addr());
				     }
				}
				else
				{
					if(ih->daddr() == ra_addr())
					{
				 		//printf(" Node %d received MONITOR_REQ_MESG at %lf which is tooooo late.Ignoring \n",ra_addr(),CURRENT_TIME);					
				 	}
					 
				}
				break;	
		case MONITOR_AGREE_MESG:
				if(ra_addr() == baseStation)
					return;
				if(CURRENT_TIME < ((6*(PACKET_TRANSMIT_TIME+JITTER)) +ch->timestamp()))
				{
					if(sentNotification == false || myMonitor == -1)
					{
						if(ih->daddr() != ra_addr() || ra_addr()== baseStation || isActive == false)
						{
							return;
						}
						//printf("received MONITOR_AGREE_MESG from node %d to %d \n",ih->saddr(),ra_addr());
						if(myMonitor == -1)
						{
							monitoredLinks = ph->pkt_monitored_links();
							setInterimMonitor(srcAddr); 
						}
						sendPacket(NULL,MONITOR_NOTIFICATION_MESG,IP_BROADCAST);
						sentNotification = true;
					}		
				}
				else
				{
					if(ih->daddr() == ra_addr())
					{
						//printf(" Node %d received MONITOR_AGREE_MESG at %lf which is tooooo late.Ignoring \n",ra_addr(),CURRENT_TIME);
					}
				}
				break;	
				
		case MONITOR_NOTIFICATION_MESG:
				if(CURRENT_TIME < ((6*(PACKET_TRANSMIT_TIME+JITTER)) +ch->timestamp()))
				{
					if(isActive == false)
					{
						if(isMonitor == false)
						{
						  	if(ph->pkt_monitor() == ra_addr())
							{
								isMonitor = true;
								AddMonitoredNodes(srcAddr,ph->pkt_forwarder());		
								for(int i = 0;i< MAX_MONITORED_LINKS; i++)
								{
									if(monInfo[i].sender == ih->saddr())
										seq = i;
								} 
								sendPacket(NULL,ACK_MESG,srcAddr);	  
							}
	
						}
						else
						{
							//printf("\n Inactive node which is already a monitor will ignore this message.\n");
						}	
					}
				}
				break;			
									
					
	}
	//Release resources
	Packet :: free(p);
}

/*******************************************************************
* Function		:forward_data()
* 
* Description	:This function forwards a protoname packet
* 				  
* Prototype		:void  Protoname::forward_data(Packet* p)
* 
* Inputs		:Packet* p : packet to be forwarded
* 				  
* Outputs		:void	 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/

void Protoname :: setUpDataPath()
{
	 nsaddr_t  neighbor = 0;
	//establish the datapath.
	if(forwarder_req_mesg_sent >= FORWARDER_REQ_MESG_RESEND_COUNT)
	{
		return;
	}

   for(int i = 0; i < (int)rtable_.size(); i++)
   {
    	neighbor = nodesmap[i];
    	if(ra_addr() == baseStation)
    	{
	    	//printf("\n Base Station sending forwarder req mesg to its neighbor %d at %lf\n",neighbor,CURRENT_TIME);
	    	sendPacket(NULL,FORWARDER_REQ_MESG,neighbor);
    	}
    	else 
    	{	
    		
    		
    		if(godinstance_->hops(neighbor, baseStation) >= godinstance_->hops(ra_addr(), baseStation))
    		{
    			sendPacket(NULL,FORWARDER_REQ_MESG,neighbor);
    		}
			
    	}

    }
   // #endif
	forwarder_req_mesg_sent ++;
}

//This message is sent to all the nodes in the network. Thos ndoes which are monitors
//will take action.Others will ignore it.
void Protoname::sendMissingInfoMesg(Packet *p)
{
	
   nsaddr_t  neighbor = 0;
 
   if(missing_info_mesg_sent >= MISSING_INFO_MESG_RESEND_COUNT)
	{
		printf("Node %d not sending missing info as max count increased\n",ra_addr());
		return;
	}
	/*
   for(int i = 0; i < (int)rtable_.size(); i++)
   {
    	neighbor = nodesmap[i];
    	
   	   	if(ra_addr() == baseStation)
    	{
    		printf("Node %d sending missing info to node %d at %lf\n",ra_addr(),neighbor,CURRENT_TIME);
    		sendPacket(NULL,MISSING_INFO_MESG,neighbor);
    	}
    	else 
    	{	
    		if(godinstance_->hops(neighbor, baseStation) >= godinstance_->hops(ra_addr(), baseStation))
    		{
    			printf("Node %d sending missing info to node %dat %lf\n",ra_addr(),neighbor,CURRENT_TIME);
    			sendPacket(p,MISSING_INFO_MESG,neighbor);
    		}
    					
    	}

    }*/
  
   
    
    	    
    
    if(ra_addr() == baseStation)
    	{
    		printf("Node %d sending missing info to node %d at %lf\n",ra_addr(),neighbor,CURRENT_TIME);
    		sendPacket(NULL,MISSING_INFO_MESG,IP_BROADCAST);
    	}
    	else 
    	{	
    			printf("Node %d sending missing info to node %dat %lf\n",ra_addr(),neighbor,CURRENT_TIME);
    			sendPacket(p,MISSING_INFO_MESG,IP_BROADCAST);   					
    	}
     missing_info_mesg_sent++;
    //sendPacket(NULL,MISSING_INFO_MESG,IP_BROADCAST);
}	

/***************************************
 * getBaseStationID
 * Description: Finds and sets the
 *   base station.
 ***************************************/
void Protoname::getBaseStationID()
{
	FILE* ssFile = NULL;
	char tmpStr[200];
	int MonitorStatus = 1;
	/* open the file for read */
	ssFile = fopen(SRC_SINK_FILE, "r");
	/* check that the file was opened properly */
	if(ssFile == NULL) {
		LOG_ALM("Error: Unable to open the source-sink file\n");
		exit(0);
	}
	
	/*Read the word*/
	fscanf(ssFile, "%s", tmpStr);
	/* read the value */
	fscanf(ssFile, "%s", tmpStr);
	/* store in static variable */
	baseStation = atoi(tmpStr);
	
	/* read the srcCount word */
	fscanf(ssFile, "%s", tmpStr);
	/* read the srcCount value */
	fscanf(ssFile, "%s", tmpStr);	
	/* store in static variable */
	srcCount = atoi(tmpStr);
		
	LOG_DBG(" the Source is %d\n",srcCount);
	/* skip the first word */
	fscanf(ssFile, "%s", tmpStr);
	
	for(int k = 0; k< srcCount; k++) //Change here to change the number of sources
	{
		/* read in the ID */
		fscanf(ssFile, "%s", tmpStr);
		srcNodes[k] = atoi(tmpStr);
		srcNode = atoi(tmpStr);
		if(srcNode == ra_addr())
			isActive = true;
		//printf(" srcNode is %d \n",srcNode);
	}
	//LOG_DBG(" base station set as %d and src is %d\n:",baseStation,srcNode	);
	/* close the file */
	/* skip the first word */
	fscanf(ssFile, "%s", tmpStr);
	/* read in the ID */
	fscanf(ssFile, "%s", tmpStr);

	//Set the duty Cycle here.
	if(ra_addr()!= baseStation)
	{
		//dutyCycle = 10/(double) 100;
		//dutyCycle = (double)atoi(tmpStr);
	}
    if(ra_addr() == baseStation)
		LOG_DBG(" Duty Cycle is set to %f \n",(double)atoi(tmpStr));	
	/* skip the first word */
	fscanf(ssFile, "%s", tmpStr);
	/* read in the ID */
	fscanf(ssFile, "%s", tmpStr);
	
	MonitorStatus = atoi(tmpStr); 

	if(MonitorStatus == 0)
	{ 
		LOG_DBG(" Monitors Disabled\n");
	}
	else
	{
		LOG_DBG(" Monitors Enabled\n");
	}
	
	//Reading the active /inactive node sources
	/* skip the first word */
	memset(tmpStr,0,200);
	fscanf(ssFile, "%s", tmpStr);
	bool inact = false;

	if(strcmp(tmpStr,"Active:")==0)
	{
		while(fscanf(ssFile, "%s", tmpStr)!= EOF)
		{
			//LOG_DBG(" Strlen : %d of %d \n",strlen(tmpStr),atoi(tmpStr));
			if(strcmp(tmpStr,"Inactive:")==0)
			{
				//LOG_DBG(" Inactive set to true \n");
				inact = true;
			}
			else
			{
				
				if(inact == false)
				{
					activeNodes[actNodeCount] = atoi(tmpStr);
					actNodeCount ++;
				}
				
				if(atoi(tmpStr) == ra_addr())
				{
					if(inact == false)
						isActive = true;
					else
						isActive = false;
					
				}
			}
			
		}
		
	}
	/* read in the ID */
	
	//LOG_DBG(" Active Node count :%d as in File for Node %d \n",actNodeCount	
		
	fclose(ssFile);
	if(ra_addr() == baseStation)
	{
		printf("\n Active Nodes(%d) are : ",actNodeCount);
		for(int j=0;j<actNodeCount;j++)
		{	
			printf(" %d ",activeNodes[j]);
		}
	}
}

void Protoname :: init_neighborStatus()
{
	for(int i =0; i< MAX_NEIGHBOR_NODES; i++)
	{
		neighbors[i].nodeId = -1;   //node id
		neighbors[i].isActive = false; //true or false
		neighbors[i].dist = -1;		//hop distance of this node from the base station
		neighbors[i].isForwarder = false;
	} 
}
void Protoname::configureFinalForwarder()
{
	if(ra_addr() == baseStation)
	{
		printf("\n Current node is base Station. Hence no need of forwarder for it\n");
		return;
	}
	
	if((isSourceNode(forwarder) == true)|| (forwarder == -1))
	{
		//Now choose one of the active nodes which are closer to the base station as forwarder
		printf("\n Forwarder %d of node %d is a source node\n",forwarder,ra_addr());
		nsaddr_t neighbor = 0;
		for(int i = 0; i < (int)rtable_.size(); i++)
		{
		  if((isSourceNode(neighbors[i].nodeId ) == false)&&(neighbors[i].isActive == true) && (godinstance_->hops(neighbors[i].nodeId, baseStation) < godinstance_->hops(ra_addr(), baseStation)))
		  {
	  	  		forwarder = neighbors[i].nodeId;
	  	  		neighbors[i].isForwarder = true;
				hopDistance = godinstance_->hops(neighbors[i].nodeId, baseStation) + 1;
				printf(" Forwarder of node %d in final stage is %d \n",ra_addr(),forwarder);
				break;
						  	  	
		  }
		  else
		  if((isSourceNode(neighbors[i].nodeId ) == false)&&(neighbors[i].isActive == true) && (godinstance_->hops(neighbors[i].nodeId, baseStation) == godinstance_->hops(ra_addr(), baseStation)))
		  {
	  	  		forwarder = neighbors[i].nodeId;
	  	  		neighbors[i].isForwarder = true;
				hopDistance = godinstance_->hops(neighbors[i].nodeId, baseStation) + 1;
				printf(" Forwarder of node %d in final stage is %d \n",ra_addr(),forwarder);
				break;
						  	  	
		  }	 
				  	
		}
		printf("\n Node %d sending set_forwarder_mesg to its forwarder %d\n",ra_addr(),forwarder);
		sendPacket(NULL,SET_FORWARDER_MESG,forwarder);
						
	}	
	
}	

bool Protoname::isSourceNode(nsaddr_t neighbor)
{
	isForwarderSource = false;
	for(int k=0;k<srcCount;k++)
	{
		
		if(neighbor == srcNodes[k])
		{
			isForwarderSource = true;
			break;
		}	
			
	}	
	return isForwarderSource;
}	

void Protoname::setInactiveNodes()
{
	if((isForwarder == false) && (isSourceNode(ra_addr()) == false))
	{
		isActive = false;
		printf("\n Node %d is not a forwarder or a source node.Hence it is inactive\n");
		
	}	
}	