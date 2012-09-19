#include "protoname.h"

/*******************************************************************
* Function		:expire() 
* 
* Description	:Timeout Handler for Initialization Timeout
* 				 This function is declared in Init_Timer class
* 
* Prototype		:void Init_Timer::expire(Event* e) 
* 
* Inputs		:Event* e : event occured
* 
* Outputs		:void 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/
void Init_Timer::expire(Event* e) 
{
	agent_->end_init_timer();
}
void RouteData_Timer::expire(Event* e) 
{
	agent_->routePacket(sentPacketId);
}

/*******************************************************************
* Function		:expire() 
* 
* Description	:Timeout Handler for Startup Timeout
* 				 This function is declared in Init_Timer class
* 
* Prototype		:void StartUp_Timer::expire(Event* e) 
* 
* Inputs		:Event* e : event occured
* 
* Outputs		:void 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/
void StartUp_Timer::expire(Event* e) 
{
	agent_->end_startup_timer();
}
void Neighbor_Timer::expire(Event* e) 
{
	agent_->end_neighbor_timer();
}

void Process_Timer::expire(Event* e) 
{
	agent_->end_process_timer();
}
void Data_Timer::expire(Event* e) 
{
	agent_->end_data_timer();
}
void Ack_Timer::expire(Event* e) 
{
	agent_->end_ack_timer(sentPacketType,sentPacket);
}
void Monitor_Timer::expire(Event* e) 
{
	agent_->end_monitor_timer();
}
void Recv_Timer::expire(Event* e) 
{
	agent_->end_recv_timer();
}


/*******************************************************************
* Function		:end_init_timer()
* 
* Description	:Function to gather the initial node info like
* 				 duty cycle,offsets and one hop distance.
* 				 This function is declared in Protoname class
* 
* Prototype		:void Protoname::end_init_timer()
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

void Protoname::end_process_timer()
{
	
	
	isMissingInfoRecvd = false;
	setPriority = false;
	noSendingState = true;
	noSendingCount = 0;
	missing_info_mesg_sent = 0;
	increaseReportingRate = false;
	decreaseReportingRate = false;
	increaseReportingCount = 0;
	decreaseReportingCount = 0;
	
	if(CURRENT_TIME >= 205.0 && CURRENT_TIME < 210.0) 
		processInterval = 1;
	else
	if(CURRENT_TIME >= 210.0 && CURRENT_TIME < 215.0) 
		processInterval = 2;
	else
	if(CURRENT_TIME >= 215.0 && CURRENT_TIME < 220.0) 
		processInterval = 3;
	else
	if(CURRENT_TIME >= 220.0 && CURRENT_TIME < 225.0) 
		processInterval = 4;
	else
	if(CURRENT_TIME >= 225.0 && CURRENT_TIME < 230.0) 
		processInterval = 5;
	else
	if(CURRENT_TIME >= 230.0 && CURRENT_TIME < 235.0) 
		processInterval = 6;
	else
	if(CURRENT_TIME >= 235.0 && CURRENT_TIME < 240.0) 
		processInterval = 7;
	else
	if(CURRENT_TIME >= 240.0 && CURRENT_TIME < 245.0) 
		processInterval = 8;
	else
	if(CURRENT_TIME >= 245.0 && CURRENT_TIME < 250.0) 
		processInterval = 9;
	else
	{	
		processInterval = 10;
		lastProcessInterval = 10;	
	}
	if(ra_addr() == baseStation)
	{		
		printf("\n Process Timer expired for BS at %lf\n",CURRENT_TIME);
		prioritySourceCount=0;
		reduceReportingCount = 0;
		
		for(int i=0;i<20;i++)
		{
			prioritySources[i] = -1;
			
		}
		for(int i=0;i<20;i++)
		{
			reduceReporting[i] = -1;
			
		}
		printf("\n Resetting the priority Sources and the priority Source Count completed at %lf\n",CURRENT_TIME);
		
		for(int k = 0; k< srcCount; k++) 
		{
			
			printf("\n Value of bsReceived at source %d is %d at %lf\n",srcNodes[k],mac->bsReceived(srcNodes[k]),CURRENT_TIME);
			if((mac->bsReceived(srcNodes[k]) == 35) && (overallReduceReporting == false))
			{
					printf("\nBase Station received enough packets from source %d at %lf \n",srcNodes[k],CURRENT_TIME);
			}
			else if(mac->bsReceived(srcNodes[k]) > 35)
			{
					
					printf("\nBase Station received more than enough packets from source %d at %lf \n",srcNodes[k],CURRENT_TIME);
					reduceReporting[reduceReportingCount] = srcNodes[k];
			  		reduceReportingCount++;
			}	 
			else
			{
			  		printf("\n Base Station did not receive atleast one packet from source %d at %lf\n", srcNodes[k],CURRENT_TIME);
			  		prioritySources[prioritySourceCount] = srcNodes[k];
			  		prioritySourceCount++;
			  		
			}
			
		}
		if(prioritySourceCount > 0)
		{
			printf("\nNumber of sources to be given priority is %d\n",prioritySourceCount);
			printf("\n They are \n");
			for(int j=0;j<prioritySourceCount;j++)
			{
				printf("%d    ,",prioritySources[j]); 
			}	
			printf("\n");
			
			printf("\nNumber of sources to reduce reporting rate is %d\n",reduceReportingCount);
			printf("\n They are \n");
			for(int j=0;j<reduceReportingCount;j++)
			{
				printf("%d    ,",reduceReporting[j]); 
			}	
			printf("\n");
			/* copying the prev priority sources */
			overallReduceReporting = false;
			for(int i=0;i<prevPrioritySourceCount;i++)
			{
				
				for(int j=0;j<prioritySourceCount;j++)
				{
					if(prevPrioritySources[i] == prioritySources[j])
					{
						overallReduceReporting = true;
						break;
					}	
				}
			}	
			for(int k = 0; k< srcCount; k++) 
			{		
				if((mac->bsReceived(srcNodes[k]) == 35) && (overallReduceReporting == true)) 
				{
					reduceReporting[reduceReportingCount] = srcNodes[k];
					reduceReportingCount++;
				}
			}	
			
			prevPrioritySourceCount = 0;
			for(int i=0;i<20;i++)
			{
				prevPrioritySources[i] = -1;
				
			}
			for(int j=0;j<prioritySourceCount;j++)
			{
				prevPrioritySources[j] = prioritySources[j];
			  	prevPrioritySourceCount++;
			  		
			}	
			
			/* Changes made for count of intervals
			 * strategy to find count of intervals in which we could recover missing information
			 * Take a structure in which we keep adding node seen in each interval among priority sources
			 * The structure has source id, the start interval, count of intervals to recover, and
			 * the hasRecovered varibale indicating if it has been recovered or not.*/
			 
			 
			
			 if(processInterval != lastProcessInterval)
				{
					
					/*First update the hasRecoveredStatus of the entries in the structure
					 */
					for(int k=0;k<recoveryIntervalSources;k++)
					{		
						bool isPresentInStructure = false;
						for(int i=0;i<prioritySourceCount;i++)
						{
							if((prioritySources[i] == recoveryInterval[k].sourceId))
								isPresentInStructure = true;
						}
						if(isPresentInStructure ==  false)
						{
							
							if(recoveryInterval[k].startInterval != -1)
							{
								recoveryInterval[k].hasRecovered = true;
								/*Once a node has been recovered , update the count and intervals variable */
								recoveredSourcesCount++;
								recoveredIntervals += recoveryInterval[k].interval;
								printf("\nrecoveredSourcesCount is %d and recoveredIntervals is %d for source %d\n", recoveredSourcesCount,recoveredIntervals,recoveryInterval[k].sourceId);
								/*Reset the start interval to -1 indicating it has been recovered and removed from structure */
								recoveryInterval[k].startInterval = -1;
								recoveryInterval[k].interval = 0;
							}
						}
						
					}	
							
					/* Here we update the interval count*/
					
					for(int i=0;i<prioritySourceCount;i++)
					{		
						bool isPresentInStructure = false;
						for(int k=0;k<recoveryIntervalSources;k++)
						{
							if((recoveryInterval[k].sourceId == prioritySources[i]))
							{
								isPresentInStructure = true;
								if((recoveryInterval[k].hasRecovered == false) && (recoveryInterval[k].startInterval != -1))  
								{
									recoveryInterval[k].interval++;
								}	
								else
								if(recoveryInterval[k].startInterval == -1)
								{
									recoveryInterval[k].interval = 1;
									recoveryInterval[k].startInterval = processInterval;
									recoveryInterval[k].hasRecovered = false;
								}	
								
							}
								
						}
						if(isPresentInStructure == false)
						{
							recoveryInterval[recoveryIntervalSources].sourceId = prioritySources[i];
							recoveryInterval[recoveryIntervalSources].interval = 1;
							recoveryInterval[recoveryIntervalSources].startInterval = processInterval;
							recoveryInterval[recoveryIntervalSources].hasRecovered = false;
							recoveryIntervalSources++;
						}
					} 					
					printf("\n RecoveryInterval Sources Count after interval2 is %d\n",recoveryIntervalSources);
				}	
				
				/*Processing at the last interval */								
				if(processInterval == lastProcessInterval)
				{
					for(int i=0;i<prioritySourceCount;i++)
					{		
						bool isPresentInStructure = false;
						for(int k=0;k<recoveryIntervalSources;k++)
						{
							if((recoveryInterval[k].sourceId == prioritySources[i])&&(recoveryInterval[k].hasRecovered == false))
							{
								//if((recoveryInterval[k].startInterval  == processInterval - 1) ||
								//(recoveryInterval[k].interval == processInterval - 1))
								{
									notRecoveredIntervals += recoveryInterval[k].interval;
									recoveryInterval[k].interval = 0;
									notRecoveredSourcesCount++;
									
								}	 
									
							}
								
						}
						
					} 
					
					for(int k=0;k<recoveryIntervalSources;k++)
					{
						if(recoveryInterval[k].interval > 0)
						{
								recoveredSourcesCount++;
								recoveredIntervals+= recoveryInterval[k].interval;	
								printf("\n recoveredSourcesCount is %d and recoveredIntervals is %d for source %d\n",recoveredSourcesCount,recoveredIntervals,recoveryInterval[k].sourceId);
								
						}
							
					}
					float averageIntervals;
					float averageRecoveredIntervals;
					float percentageRecoveredSources;
					int   totalsources;
					int   totalIntervals;
					totalsources =  recoveredSourcesCount + notRecoveredSourcesCount;
					totalIntervals = recoveredIntervals + notRecoveredIntervals;
					
					printf("\nTotalSources is %d ,TotalIntervals is %d ,recoveredSourcesCount is %d and recoveredIntervals is %d\n",totalsources,totalIntervals,recoveredSourcesCount,recoveredIntervals);
					if(totalsources > 0 && totalIntervals > 0) 
						averageIntervals = (float)totalIntervals/(float)totalsources;
					else
					averageIntervals = 0;
					
					if(recoveredSourcesCount > 0 && recoveredIntervals > 0) 
						averageRecoveredIntervals = (float)recoveredIntervals/(float)recoveredSourcesCount;
					else
					averageRecoveredIntervals = 0;
					
					if(recoveredSourcesCount > 0 && totalsources > 0) 			
						percentageRecoveredSources = ((float)(recoveredSourcesCount)/(float)(totalsources)) * 100;
					else
						percentageRecoveredSources = 0;
					
						 					
					
					/* Writing this info to a file for stats*/
					
					ofstream outputFile;
					outputFile.open("NovemberStatslatest/IntervalData_Priq-drop_10intervals_0.1.txt",ios::app);
					outputFile << averageIntervals << "\t\t\t\t" << percentageRecoveredSources << "\n";
					outputFile.close();
				
				}
				/* printing the structure to see if the structure is updated properly*/
				printf("\n Printing the structure for processing interval %d\n",processInterval);
			
				for(int k=0;k<recoveryIntervalSources;k++)
				{
					printf("\n %d\t%d\t%d\t%d",recoveryInterval[k].sourceId,recoveryInterval[k].startInterval,recoveryInterval[k].interval,recoveryInterval[k].hasRecovered);
				}
				printf("\n");
				
				
					
			printf("\n Base Station sending Missing Info Mesg at %lf\n",CURRENT_TIME);
			sendMissingInfoMesg(NULL);	
		}
		for(int k = 0; k< srcCount; k++) 
		{
			mac->setBSReceived(0,srcNodes[k]);
		}
		/* Resetting the prioritySourceNodes and the bsReceived for every processing interval*/
	}
	//printf("\n Current time before rescheduling timer is %lf\n",CURRENT_TIME);
	if(CURRENT_TIME < 250.0)
	process_timer.resched(5.0);
	
}

void Protoname::end_recv_timer()
{
	if(isMonitor == true)
	{
		printf("\n Recv_timer expired for the node %d at %lf\n",ra_addr(),CURRENT_TIME);
		sendDataPacket_with_monitor();
		
	}
	recv_timer.resched(5.0);
	
	
}
void Protoname::end_neighbor_timer()
{
	//nsaddr_t minforwarder;
	nsaddr_t maxforwarder;
	
	//int minValue = 0;
	int maxValue  = 0;
	//printf(" neighbor timer expired at %lf and active_neighbor_mesg_recvd value is %d\n",CURRENT_TIME,active_neighbor_mesg_recvd); 
	int i =0;
	//Code for testing
	printf("\n The neighbor timer expired for node %d\n",ra_addr());
	printf("-----------The inactive neighbor information------------");
	for(int j=0;j< active_neighbor_mesg_recvd;j++)
	{
		printf("\n The active node count of node %d is %d\n", commonInfo[j].nodeId,commonInfo[j].activeCnt);
		printf("\n Forwarder of this node is %d and my forwarder is %d\n",commonInfo[j].forwarder,forwarder);
	}	
	printf("\n ---------------Ends--------------\n");
	
	for(i=0 ;i< active_neighbor_mesg_recvd;i++)
	{
		/*if(i==0)
		{ 
			if(commonInfo[i].activeCnt > 2 && commonInfo[i].forwarder != forwarder && ra_addr() != commonInfo[i].forwarder) 
			{
				maxValue = commonInfo[i].activeCnt;
				max = commonInfo[i].nodeId;
				maxforwarder = commonInfo[i].forwarder;
			
			}
			else
			{
				maxValue = -1;
				
			}	
		}	
		
		else
		{
			if(maxValue == -1 && commonInfo[i].activeCnt > 2 && commonInfo[i].forwarder != forwarder && ra_addr() != commonInfo[i].forwarder)
			{
				maxValue = commonInfo[i].activeCnt;
				max = commonInfo[i].nodeId;
				maxforwarder = commonInfo[i].forwarder;
			}
			else
			if(maxValue!= -1 && commonInfo[i].activeCnt  > maxValue && commonInfo[i].activeCnt > 2 && commonInfo[i].forwarder != forwarder && ra_addr() != commonInfo[i].forwarder)
			{
				maxValue = commonInfo[i].activeCnt;
				max = commonInfo[i].nodeId;
				maxforwarder = commonInfo[i].forwarder;	
			}
			
		}*/
		
		// Selecting the monitor based on its hop distance to the base station
		//Starts here
		
		if(i==0)
		{ 
			if(commonInfo[i].activeCnt > 2 && commonInfo[i].forwarder != forwarder && ra_addr() != commonInfo[i].forwarder) 
			{
				maxValue = commonInfo[i].activeCnt;
				max = commonInfo[i].nodeId;
				maxforwarder = commonInfo[i].forwarder;
			
			}
			else
			{
				maxValue = -1;
				
			}	
		}	
		
		else
		{
			if(maxValue == -1 && commonInfo[i].activeCnt > 2 && commonInfo[i].forwarder != forwarder && ra_addr() != commonInfo[i].forwarder)
			{
				maxValue = commonInfo[i].activeCnt;
				max = commonInfo[i].nodeId;
				maxforwarder = commonInfo[i].forwarder;
			}
			else
			if(maxValue!= -1 && commonInfo[i].activeCnt  > maxValue && commonInfo[i].activeCnt > 2 && commonInfo[i].forwarder != forwarder && ra_addr() != commonInfo[i].forwarder)
			{
				maxValue = commonInfo[i].activeCnt;
				max = commonInfo[i].nodeId;
				maxforwarder = commonInfo[i].forwarder;	
			}
			
		}
		//Ends Here	
	}	
	
	if(maxValue == 0 || maxValue == -1)
	{
		printf("\n There is no common neighbor with more than 2 active nodes in its on ehop neighborhood\n");
		return;
	}	
	printf("Node %d sending monitor req message to node %d and my forwarder is %d \
	 and monitors forwarder is %d\n",ra_addr(),max,forwarder,maxforwarder);
	//Now send Monitor Request message to this node;
 	sendPacket(NULL,MONITOR_REQ_MESG,max);
 	
}	

void Protoname::end_data_timer()
{
	
	
	int headid;
	int rearid;
	printf("\n Data Timer expired at  %lf\n", CURRENT_TIME);
	
	DataQueue.print_the_queue("DATA QUEUE:After Packet Added",ra_addr());
	rearid = DataQueue.get_rear_id();
	
	//Packet* p = DataQueue.get_item(&headid);
	Packet* p = DataQueue.get_packet(rearid);
	if(p != 0)
	{
		struct hdr_cmn* ch = HDR_CMN(p);
		printf("\n Packet in queue being forwarded is %d\n",ch->uid());
		printf("\n Priority of the packets from node %d is %d\n",ra_addr(),hdr_cmn::access(p)->priority());
		forward_data_with_monitor(p,IP_BROADCAST);
	
		//Clear the queue
	    int size;
		size = DataQueue.queue_sz();
		//printf("\n The size of the Data Queue is %d\n",size);
		while(size !=0)
		{
			DataQueue.remove_item();
			size --;
		}
		//printf("\n The size of the Data Queue after deleting all elements is %d\n",size);
	}
	if(CURRENT_TIME < 216.0)
		data_timer_.resched(2.0);
		
}	


void Protoname::end_init_timer()
 {
 	/* If i'm the base station, start the hops process */
	if(ra_addr() == baseStation)
	{
		hopDistance = 0;
	}
	/* get neighbor information for both active and inactive nodes */
	getOneHopInfo();

	for(int n =0;n<(int)rtable_.size();n++)
	{
		//neighbors[n].isActive = false;
		for(int k =0;k<actNodeCount;k++)
		{
			if(neighbors[n].nodeId == activeNodes[k])
			{
				neighbors[n].isActive = true;
				activeNodeCount ++;
			}
		}
		
	}
	printf(" Node %d has activeNodeCount :%d\n",ra_addr(),activeNodeCount);
 }
 void Protoname::end_startup_timer()
 {
 	//Setup the datapath
 	if(forwarder_req_mesg_sent == 0)
 	{
		if(ra_addr() == baseStation)
		{
			//(" Node %d Setting up Data path for time %d\n",ra_addr(),(forwarder_req_mesg_sent+1));
			printf("\n Starting setting up of data path by base station at %lf\n",CURRENT_TIME);
			setUpDataPath();
		} 		
 	}
 	else
 	{
 			//LOG_DBG(" Node %d Setting up Data path for time %d\n",ra_addr(),(forwarder_req_mesg_sent+1));
			setUpDataPath();
 		
 	}

	if(forwarder_req_mesg_sent < FORWARDER_REQ_MESG_RESEND_COUNT)
	{
		startup_timer.resched(0.5);
	}
 }
 void Protoname::end_ack_timer(int packetType,Packet *p)
 {
	//Ignore for now;
 }	
 void Protoname::startTimer(int timerType,Packet *p,int packetType,double delay)
{
	//Ignore for now	
}
 void Protoname::end_monitor_timer()
 {
			
		for(int i =0; i< monQueue.queue_sz();i++)
		{
			//flip a coin if the corresponding node can be monitored or not 
			int nextHop; 
			Packet *p = monQueue.get_item(&nextHop);
			int randm = Random::seed_heuristically();
			
			struct hdr_ip* ih	= HDR_IP(p);
			struct hdr_protoname_pkt * ph = HDR_PROTONAME_PKT(p);
			nsaddr_t srcAddr = ih->saddr();
			
			//Make sure this inactive node does not act as MONITOR for its forwarder
			if((srcAddr == forwarder )|| (ph->pkt_forwarder() == forwarder ))
			{
				if(isFinalStage == true)
				{
					printf(" Node %d not monitoring the link between %d and %d as %d is its forwarder and it is final stage\n",ra_addr(),
							srcAddr,ph->pkt_forwarder(),forwarder); 
				}
				else
					printf(" Node %d not monitoring the link between %d and %d as %d is its forwarder and it is not final stage\n",ra_addr(),
							srcAddr,ph->pkt_forwarder(),forwarder); 				
				randm = -1;
			}
			if(((randm > 0 && monQueue.queue_sz()>1) &&
			 monitoredLinks < MAX_MONITORED_LINKS))
			{
				
				if(ph->pkt_hopCount() >= hopDistance && 
				  (ph->pkt_hopCount()-hopDistance) <= 1 )
				{

					monInfo[monitoredLinks].sender   = srcAddr;
					monInfo[monitoredLinks].receiver = ph->pkt_forwarder();
					monInfo[monitoredLinks].link = 1;
					monitoredLinks ++;
					activeNodeCount-=2;
					
	
				}
				printf("Node %d sending MONITOR_AGREE_MESG to %d\n",ra_addr(),srcAddr);
				sendPacket(p,MONITOR_AGREE_MESG,srcAddr);

			}
			else 
			if(randm != -1 && (monQueue.queue_sz()==1) &&
			 (monitoredLinks < MAX_MONITORED_LINKS))
			 {
			 	if(ph->pkt_hopCount() >= hopDistance && 
				  (ph->pkt_hopCount()-hopDistance) <= 1 )
				{

					monInfo[monitoredLinks].sender   = srcAddr;
					monInfo[monitoredLinks].receiver = ph->pkt_forwarder();
					monInfo[monitoredLinks].link = 1;
					monitoredLinks ++;
					activeNodeCount-=2;
					
	
				}
				printf("Node %d sending MONITOR_AGREE_MESG to %d\n",ra_addr(),srcAddr);
				monQueue.remove_item();
				sendPacket(p,MONITOR_AGREE_MESG,srcAddr);
			 	
			 }
			else	
			{
				printf("node %d ignoring monitor request message from %d as monitored links is %d, randm is %d \ 
				and monqueue size is %d\n",ra_addr(),srcAddr,monitoredLinks,randm,monQueue.queue_sz());
					
			}	
			monQueue.remove_item();
			i--;
		}
	
 } 
 