#include "protoname.h"

bool Protoname ::checkDataSender(nsaddr_t srcAddr)
{
	for(int j=0;j<MAX_MONITORED_LINKS;j++)
	{
		if(monInfo[j].sender == srcAddr || monInfo[j].receiver == srcAddr )
		{
			return true;
		}
	}
	return false;
}
void Protoname ::monitorLinks(Packet *p,nsaddr_t srcAddr)
{
	struct hdr_cmn* ch	= HDR_CMN(p);		
	int count =0;
	for(int i=0 ;i< MAX_MONITORED_LINKS;i++)
	{
		if(monInfo[i].sender  == srcAddr )
	  	{
			if(monInfo[i].receiver  == baseStation )
	  		{
	  			//printf("  Monitor %d not buffering packet %d as it is sent to baseStation\n",ra_addr(),ch->uid());
	  			return;
	  		}
	 		
	 		if(monRecvQueue[i].has_packet(ch->uid()) == true)
	  		{
	  			//printf(" Monitor %d ignoring packet %d as its already been delivered to %d\n",ra_addr(),ch->uid(),monInfo[i].receiver);
	  			return;
	  		}
	  		if(monDataQueue.has_packet(ch->uid()) == true &&
	  		   HDR_CMN(monDataQueue.get_packet(ch->uid()))->prev_hop()== monInfo[i].sender)
	  		{
	  			//printf(" Monitor %d ignoring packet %d as its already been in queue\n",ra_addr(),ch->uid());
	  			return;	  			
	  		}
	 		if(monDataQueue.is_full() == true || monitorPacketCount == (QUEUEPACKET_SZ*100))
	 		{	
	 			//printf(" Monitor %d Queue is full at %ld \n", ra_addr(),monitorPacketCount);
	 			return;	 			
	 		}
	 		
	 		
	 	    prevHop[i] = srcAddr; //maintains the packet sender fo this link
	  		link[i] ++; //maintains the packets sent by this link sender	
	 		monDataQueue.insert_item(p, srcAddr);
	 		monitorPacketCount ++;
	 		//printf(" Adding the packet Id:%d to the monitor %d data queue at %lf and count %ld\n",ch->uid(), ra_addr(),CURRENT_TIME,monitorPacketCount);
	 		//start a timer,and after the timeout monitor has to take appropriate action here.
	 		//Starting a timer is not a good idea, as the monitor has to start MAX_MONITORED_LINKS timers here.
	 		// LOG_DBG(" printing the monitor data queue at node %d\n", ra_addr());
	 		#ifdef PDEBUG
 			//monDataQueue.print_the_queue("MONITOR DATA QUEUE",ra_addr());
 			#endif
	 		if(route_timer[ch->uid()]->status() == TIMER_IDLE)
	 		{
				float timeout = 0.8;
	 			LOG_DBG(" value of monitor timeout for %d is %lf\n",ra_addr(),timeout);
	 			route_timer[ch->uid()]->sentPacketId = ch->uid();
	 			route_timer[ch->uid()]->resched(timeout);//just for testing.
	 			
	 			//printf(" Monitor node %d starting timer for %d at %lf should timeout(%lf) at %lf\n ", ra_addr(),ch->uid(),CURRENT_TIME,timeout,CURRENT_TIME+timeout);
	 		 	 		 	
	 		} 
	 		else
	 		{
	 			//printf(" Monitor node %d not starting timer for packet %d at %lf\n",ra_addr(),ch->uid(),CURRENT_TIME);
	 		}
	 		count ++;
	  	}
	  	else if (monInfo[i].receiver == srcAddr)
	  	{
	  		LOG_DBG(" Monitor %d cross checking packet Id :%d sent by %d \n", ra_addr(),ch->uid(),srcAddr);
	  		if(monDataQueue.has_packet(ch->uid()) == true &&
	  		   HDR_CMN(monDataQueue.get_packet(ch->uid()))->prev_hop()== monInfo[i].sender)
	  		{
	  			monDataQueue.remove_packet(ch->uid());
	  			if(route_timer[ch->uid()]->sentPacketId == ch->uid() && route_timer[ch->uid()]->status() != TIMER_IDLE)
	  				{
	  					route_timer[ch->uid()]->cancel();
	 					route_timer[ch->uid()]->sentPacketId = -1;
	 					LOG_DBG(" Monitor %d stopping the timer for packet Id :%d \n", ra_addr(),ch->uid());
	  				}
	 		 	link[i] --;
	  			if(link[i] == 0)
	  				 prevHop[i] = -1;
	  				
	  			//printf(" Monitor %d verified the packet Id : %d sent  by %d is reliably transferred to %d\n", ra_addr(),ch->uid(),
	  										//monInfo[i].sender,srcAddr);
	  		 	#ifdef PDEBUG
 			 		monDataQueue.print_the_queue("MONITOR DATA QUEUE",ra_addr());
 			 	#endif
	  		
	  		}
	  		else
	  		{
	  			//printf(" Monitor %d received the packet Id : %d sent  by %d earlier than %d for link :%d\n", ra_addr(),ch->uid(),monInfo[i].receiver,monInfo[i].sender,i);
	  			if(monRecvQueue[i].is_full() == true)	
					monRecvQueue[i].remove_item();
	  			monRecvQueue[i].insert_item(p,srcAddr);
	  			#ifdef PDEBUG
	  				monRecvQueue[i].print_the_queue("MONITOR EARLY QUEUE",ra_addr());
	  			#endif
	  		}
	  		count ++;
	  	}
	  	 	
  }
  if(count ==0)
  {
  		//printf(" Monitor %d can not buffer/verify packet sent by %d \n", ra_addr(),srcAddr);
  }
}

void  Protoname ::forward_data_with_monitor(Packet* p,nsaddr_t type)
{
    struct hdr_cmn* ch = HDR_CMN(p);
    struct hdr_ip* ih = HDR_IP(p);

	if(isActive == true)
	{
		//Current node got  the Data packet. Forward the packet
		if(ih->saddr() != ra_addr()) 
		{
			//Case where the packet is not destined for current node
			//printf("\n Inside forward_data with monitor\n");
			if(ch->next_hop()!=	ra_addr())		
			{
				printf(" Dropping the packet %d,as not destined for node %d\n",ra_addr(),ch->uid());
				return;
			}
			
			//Case where the packet is destined for current node or has to be forwarded
			if(ch->next_hop() == ra_addr())
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
						//printf("Node %d should not receive packet %d from %d at % lf ",ra_addr() ,ch->uid(),ch->prev_hop(),CURRENT_TIME);
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
			   //printf("Node %d overheard packet %d from %d at % lf ",ra_addr() ,ch->uid(),ch->prev_hop(),CURRENT_TIME);
			   return;
			}
			

		}
		else //Current node originated the Data packet. Broadcast the packet.
		{
			//Control should go down for adding the received datapacket to queue.
			ih->prio() = -1;
		}
	}
	//Case for Inactive nodes.
	else
	{
		 //Handling the data in case of monitor nodes
		
		 if(isMonitor == true)
		 {
			if(type == (nsaddr_t)IP_BROADCAST)
			{
				if(checkDataSender(ch->prev_hop()) == false)
				{
					printf("\n Data sender and receiver are checked and it returned false\n");
					return;
				}
				else
				{
			  	  	printf(" Monitor Node %d received a DATA packet Id:%d sent by %d at %lf\n",ra_addr(),ch->uid(),ch->prev_hop(),CURRENT_TIME);
			      	monitorLinks(p,ch->prev_hop());
					return;
				}
		    }

		  }
		  else
		  {
				 printf("\n Inactive node %d received packet in forward_data_with_monitor.Hence discarding\n",ra_addr());
				return;
		  }		
	}
	//Add the packet to the queue
	if(recvDataQueue.is_full())
	{
	    recvDataQueue.print_the_queue("RECEIVE DATA QUEUE FULL",ra_addr());
		return;
	}					
	if(recvDataQueue.has_packet(ch->uid())== false)
	{		
		recvDataQueue.insert_item(p, ch->next_hop());
		#ifdef PDEBUG
		recvDataQueue.print_the_queue("RECEIVE DATA QUEUE:After Packet Added",ra_addr());
		#endif
		sendDataPacket_with_monitor();

	}
	return;
 }
void Protoname :: sendDataPacket_with_monitor()
 {
	if(isActive == false && isMonitor == false)
	{
		//printf(" Inactive Node Dropping at %d \n",ra_addr());
		return;
	}
	int siz =  recvDataQueue.queue_sz();
	int packetCount = 0;

	for(int i =0 ;i < siz;i++)
	{
	 	int dest;
	 	int randm = 0;
		nsaddr_t srcAddr = -1;
		/* get the data packet from the queue */
		Packet* dataPacket = allocpkt();
		
		dataPacket = recvDataQueue.get_item(&dest);// get data packet	
		assert(dataPacket != 0);
		struct hdr_cmn* dataPacket_ch = HDR_CMN(dataPacket); // common header of the data packet	
		struct hdr_ip* dataPacket_ih = HDR_IP(dataPacket); // common header of the data packet			
		dataPacket_ch->direction() = hdr_cmn::DOWN;
		dataPacket_ch->addr_type() = NS_AF_INET;
		dataPacket_ch->num_forwards() += 1;		
		srcAddr = dataPacket_ch->prev_hop();
		
		dataPacket_ch->prev_hop() = ra_addr();
		//!!!!!!!!!Need  to add this later. 
		//dataPacket_ch->xmit_failure_ = transmitFailedCallback; 
		dataPacket_ch->xmit_failure_data_ = (void *)this;
		if(isMonitor == true)
			dataPacket_ih->prio() = ra_addr();
	
		/*If a node is a monitor node then choose one of the forwarders from the forwarder set at random */
		
		if(isMonitor == true && dataPacket_ch->next_hop() != myMonitor)
		{	
			dataPacket_ch->next_hop() = forwarder;		
		}	
		
		if(isMonitor == false && dataPacket_ch->next_hop() != myMonitor)
			dataPacket_ch->next_hop() = forwarder;

		//check if this packet is already sent
		if(sentDataQueue.has_packet(dataPacket_ch->uid()) == true)
		{
			LOG_WARN(" Node %d already sent packet %d : at %lf\n",ra_addr(),dataPacket_ch->uid(),CURRENT_TIME);
			if(recvDataQueue.has_packet(dataPacket_ch->uid()) == true)
				recvDataQueue.remove_packet(dataPacket_ch->uid()) ;	
			continue;
		} 
		// send the data Packet	
		srand ( (unsigned)Random::seed_heuristically());
		float fran = (rand()%10)*(0.1)*(hopDistance*0.1)*(0.001);
		printf("Value of fran is %f for node %d before sending the packet %d\n",fran,ra_addr(),dataPacket_ch->uid());
		if(isMonitor == true && dataPacket_ih->prio()!=-1)
		{
			printf(" Packet %d forwarded by Monitor %d whose forwarder is %d :\n",dataPacket_ch->uid(),dataPacket_ih->prio(),dataPacket_ch->next_hop());
		}
		Scheduler::instance().schedule(target_,dataPacket,fran);
		printf("\n Node %d sent the packet %d to node %d at %lf\n",ra_addr(),dataPacket_ch->uid(),forwarder,CURRENT_TIME);
		recvDataQueue.remove_item() ;
				

		if(sentDataQueue.is_full() == true)	
			sentDataQueue.remove_item();
	
		sentDataQueue.insert_item(dataPacket,dataPacket_ch->next_hop());	
		packetCount ++;

	}

}
void Protoname::routePacket(int sentPacketId)
{
	Packet *p = allocpkt();
	p = monDataQueue.get_packet(sentPacketId);
	struct hdr_cmn* ch	= HDR_CMN(p);
	if(p == 0)
	{
		printf(" Packet not found in Monitor %d Queue\n",ra_addr());
	}
	else
	{
		//Add the packet to the queue
		if(recvDataQueue.is_full())
		{
			//printf(" Receive buffer full for Monitor node %d at %lf\n",ra_addr(),CURRENT_TIME);
			recvDataQueue.print_the_queue("RECEIVE DATA QUEUE FULL",ra_addr());
			return;
		}	

		if(recvDataQueue.has_packet(ch->uid())== false)
		{		
			if(sentDataQueue.has_packet(ch->uid() == true))
			{
				printf(" Monitor node %d already sent the Packet \n",ra_addr());
			}
			else
			{						
				//printf(" Monitor Node %d is adding the data packet:%d to recvDataQueue.Send from %d\n",ra_addr(),ch->uid(),ch->prev_hop());
				recvDataQueue.insert_item(p, ch->prev_hop());				
				sendDataPacket_with_monitor();
									 
			}	
			if(recvDataQueue.has_packet(ch->uid())== false)
			{    	
				monDataQueue.remove_packet(sentPacketId);
								
			}
		}
	}
}
 