#include "protoname.h"

void Protoname ::init_myMonitors()
{
	myMonitor  = -1;
	
}
nsaddr_t Protoname ::getMyMonitor()
{
	return myMonitor;
}
void Protoname ::configureMonitor(int Stage)
{
	if( forwarder ==  baseStation)
	{
	    printf(" Node %d (baseStation) is set as monitor of nodes %d and %d",baseStation,ra_addr(),forwarder);
		return;
	}
	if(forwarder == -1)
	{
		printf("No Forwarder for Node %d\n",ra_addr());
		return;
	}
	if(Stage == FIRST_STAGE)
	{
		if(hopDistance % 2 == 0 )
		{
     		printf("\n Node %d sending neigbor req message to its forwarder %d\n",ra_addr(),forwarder);
     		sendPacket(NULL,NEIGHBOR_REQ_MESG, forwarder);
		}
	}
	else if(Stage == SECOND_STAGE)
	{
		//Send message to  forwarder asking for its one hop neighbors.
		if(hopDistance % 2 != 0 )
		{
			sendPacket(NULL,NEIGHBOR_REQ_MESG, forwarder);
 		}
		
	}
	else
    {
    	  isFinalStage = true;
    	  sendPacket(NULL,NEIGHBOR_REQ_MESG, forwarder);
    }
}
nsaddr_t Protoname ::getInterimMonitor()
{
	return interimMonitor;
}
void Protoname ::setInterimMonitor(nsaddr_t srcAddr)
{ 
  	interimMonitor=srcAddr;
}
void Protoname ::GetMonitor(u_int8_t  forwarders_neighbors[])
{
	int count = 0;
	int randm = 0; 

	if(forwarders_neighbors== NULL)
	{
		printf(" no common neighbors for node %d and its forwarder %d\n", ra_addr(),forwarder);
		return;		
	}

	nsaddr_t commonNeighbors[ MAX_NEIGHBORS_COUNT];
	
   //This function should return the common neighbors
	for(int i = 0;i< MAX_NEIGHBORS_COUNT; i++)
	{
		for(int k=0; k<(int) rtable_.size();k++)
		{
			if(neighbors[k].isActive == false && forwarders_neighbors[i] == neighbors[k].nodeId )
			{
				commonNeighbors[count] = neighbors[k].nodeId;
				count ++;						
			}
		}
	}
   if(count == 0)
   {
	   	printf(" Node %d ( forwarder)neighbors are \n", forwarder);
   		return;
   }
   
   if(count <= 1 && commonNeighbors[count-1] ==  baseStation)
   {
   		printf(" BaseStation is the only common neighbor... returning\n");
   	 	return;
   }
   if (count > 1)
   {
	  	/*Choose the node with minimum active one hop neighbors
	   	* For this get the active one hop neighbor count for each node .
	   	* This is obtained by sending Active neighbor request message
	  	*/
		if(isFinalStage == true)
		{

			/*srand ( (unsigned)Scheduler :: instance().clock());
			randm =  (rand()%count);			
			if(commonNeighbors[randm] ==  baseStation)
			{
				printf(" BaseStation is randomly chosen common neighbor... returning\n");
			 	return;
			}*/
			
			printf("\n Active Neighbor mesg recvd value for node %d is %d\n",ra_addr(),active_neighbor_mesg_recvd);
			for(int i=0;i<active_neighbor_mesg_recvd;i++)
			{
				
				if((commonInfo[i].nodeId!= max) && (ra_addr() != commonInfo[i].forwarder) && (forwarder != commonInfo[i].forwarder))
				{
					printf("\n We are in final stage of configuring monitor for %d and monitor req message is sent to %d\n",ra_addr(),commonInfo[i].nodeId);
					sendPacket(NULL,MONITOR_REQ_MESG,commonInfo[i].nodeId);
					break;
				}	
				
			}	
			/*printf("\n We are in final stage of configuring monitor for %d and monitor req message is sent to %d\n",ra_addr(),commonNeighbors[randm]);
			sendPacket(NULL,MONITOR_REQ_MESG,commonNeighbors[randm]);*/
			 
		}
		else
		{
			printf("\n We are not in final stage of configuring monitors at %lf\n",CURRENT_TIME);
			for(int i=0;i< count;i++)
			{  
   	    		sendPacket(NULL,ACTIVE_NEIGHBOR_REQ_MESG,commonNeighbors[i]);
			}    
			//Start neighbor timer and make it expire to get node with minimum active one hop neigbors.
			printf("Started neighbor timer at %lf\n",CURRENT_TIME); 	   	 
			neighbor_timer.resched(count * 2 * (PACKET_TRANSMIT_TIME+JITTER));
		}
   }
}
void Protoname :: deleteMonInfo(nsaddr_t addr1,nsaddr_t addr2,nsaddr_t addr3)
{ 
	for(int i = 0; i < MAX_MONITORED_LINKS;i++)
	{
		if((monInfo[i].sender   == addr1 && monInfo[i].receiver == addr2 ) ||
		   (monInfo[i].sender   == addr2 && monInfo[i].receiver == addr3 ) )
	  	{
	  		
	  		monInfo[i].sender = -1;
	  		monInfo[i].receiver = -1;
  		
	  		monitoredLinks --;
	  		activeNodeCount +=2; 	  
	  		
	  		if(monitoredLinks < 1)
	  		{
	  			monitoredLinks = 0;
	  			isMonitor = false;
	  		}		
	  	}
	}

	
}
void Protoname ::setMyMonitor(nsaddr_t srcAddr)
{ 
  	myMonitor=srcAddr;
}
int Protoname :: checkMonInfo(nsaddr_t addr1,nsaddr_t addr2,nsaddr_t addr3)
{ 
	int count = 0;
	
	for(int i = 0; i < MAX_MONITORED_LINKS;i++)
	{
		if((monInfo[i].sender   == addr1 && monInfo[i].receiver == addr2 )) //||
		   //(monInfo[i].sender   == addr2 && monInfo[i].receiver == addr3 ) )
	  			 count ++;
	}

	return count;   
}
void Protoname ::AddMonitoredNodes(nsaddr_t srcAddr,nsaddr_t forwd)
{
  for(int i=0 ;i< MAX_MONITORED_NODES;i++)
  {
  	if(monitoringNodes[i] == -1)
  	{
  		monitoringNodes[i]=srcAddr;
  		monitoringNodes[i+1]=forwd;
  		break;
  	}
  }		
}
void Protoname ::init_MonitoringNodes()
{
	for(int i = 0; i < MAX_MONITORED_NODES; i++)
		monitoringNodes[i]  = -1;
	
}
