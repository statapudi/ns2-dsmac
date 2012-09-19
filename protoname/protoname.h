#ifndef __protoname_h__
#define __protoname_h__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <cmu-trace.h>
#include <address.h>
#include <agent.h>
#include <packet.h>
#include <trace.h>
#include <timer-handler.h>
#include <random.h>
#include <classifier-port.h>
#include <god.h>
#include <random.h>
#include <vector>
#include <syslog.h>
#include "protoname_energy.h"	
#include "protoname_pkt.h"
#include "protoname_rtable.h"
#include "mac-simple.h"	
#include "protoname_queue.h"
#include "mac-802_11.h"


#define SRC_SINK_FILE 	"150_Node_SourceSink.txt"
#define CURRENT_TIME	Scheduler :: instance().clock()
#define JITTER			(Random :: uniform()*0.05)

#define FORWARDER_REQ_MESG 		1
#define FORWARDER_REPLY_MESG 	2
#define FORWARDER_REQ_MESG_RESEND_COUNT   3
#define MISSING_INFO_MESG		4
#define MISSING_INFO_MESG_RESEND_COUNT   1
#define NEIGHBOR_REQ_MESG		6
#define NEIGHBOR_REPLY_MESG		7
#define ACTIVE_NEIGHBOR_REQ_MESG 8
#define ACTIVE_NEIGHBOR_REPLY_MESG 9
#define	FIRST_STAGE				10
#define	MONITOR_REQ_MESG 11
#define MONITOR_AGREE_MESG 12
#define MONITOR_NOTIFICATION_MESG 13
#define	SECOND_STAGE 14
#define ACK_MESG        		15
#define ACK_TIMER				16
#define ALL					    17
#define	SET_FORWARDER_MESG		18
#define MONITOR_REQ_RESEND_COUNT          3
#define MONITOR_AGREE_RESEND_COUNT        3
#define MONITOR_NOTIFICATION_RESEND_COUNT 3

#define PACKET_TRANSMIT_TIME  (75*8)/ (2457.6)
#define DATA_PACKET_TRANSMIT_TIME  (516*8) / M_BANDWIDTH

#define MAX_MONITORED_LINKS     1
#define MAX_MONITORED_NODES		2*MAX_MONITORED_LINKS //setting the max value. need to change this.

#define MAX_HOP_COUNT      7
#define LOG_LVL_ALM     LOG_CRIT
#define LOG_LVL_ERR     LOG_ERR
#define LOG_LVL_WARN    LOG_WARNING
#define LOG_LVL_INFO	LOG_NOTICE
#define LOG_LVL_DBG     LOG_INFO

//#define LOGGER(iLogLevel, cFmt, cMsg...)    if(iSeverityLevel >=  iLogLevel)syslog( iLogLevel, "%s ""%d " cFmt,__FILE__, __LINE__,  ##cMsg )
#define LOGGER(iLogLevel, cFmt, cMsg...)    if(iSeverityLevel >=  iLogLevel)fprintf( stdout,cFmt,##cMsg )
#define LOG_OPEN(cProcName,iLogLevel)        OpenLogFile(cProcName, iLogLevel)
#define LOG_ALM(cFmt, cMsg...)               LOGGER(LOG_CRIT, cFmt,##cMsg)
#define LOG_ERROR(cFmt,cMsg...)              LOGGER(LOG_ERR, cFmt,##cMsg)
#define LOG_WARN(cFmt, cMsg...)              LOGGER(LOG_WARNING, cFmt,##cMsg)
#define LOG_INF(cFmt,cMsg...)                LOGGER(LOG_NOTICE, cFmt,##cMsg)
#define LOG_DBG(cFmt, cMsg...)               LOGGER(LOG_INFO, cFmt,##cMsg)
#define LOG_CLOSE()                          closelog()


#define TOTAL_DATA_TIME (RTS_TIME + CTS_TIME + DATA_TIME + ACK_TIME + JIT_TIME)
#define LOG_MORE 1
class Protoname;	//forward declration

struct monitorInfo
{
	nsaddr_t  sender;
	nsaddr_t  receiver;
	int		  link;		 
};
struct neighborNodeStatus
{ 
	nsaddr_t  nodeId;   //node id
	bool	  isActive; //true or false
	int		  dist;		//hop distance of this node from the base station 
	bool 	  isForwarder;
};
struct commonNeighborInfo
{
	nsaddr_t nodeId;
	int 	activeCnt;
	nsaddr_t forwarder;
	int 	hopdistance;
};

struct recoveryIntervalInfo
{
	nsaddr_t sourceId;
	int		 interval;
	int 	 startInterval;
	bool	 hasRecovered;
	
};		
/*******************************************************************
* Class			:Init_Timer()
*
* Description	:main class for the Initial Setup Timer.Its base class is TimerHandler
*
* Prototype		:class Init_Timer : public TimerHandler
*
* Inputs		:none
*
* Outputs		:none
*
* Created		:01/22/2010
* 
* Created By	:Ramya
*
*********************************************************************/
class Init_Timer : public TimerHandler
{
	public:
		Init_Timer(Protoname* agent) : TimerHandler ()
		{
			agent_ = agent;/* save reference to routing agent */
		}
	
	protected:
		Protoname* agent_;
		virtual void expire(Event* e);
		

};
class Recv_Timer : public TimerHandler
{
	public:
		Recv_Timer(Protoname* agent) : TimerHandler ()
		{
			agent_ = agent;/* save reference to routing agent */
		}
	
	protected:
		Protoname* agent_;
		virtual void expire(Event* e);
		
};

/*******************************************************************
* Class			:StartUp_Timer()
* 
* Description	:main class for the Startup Timer.Its base class is TimerHandler
* 				 This timer will be called when "start" command is issued to NS2
* 				  
* Prototype		:class StartUp_Timer : public TimerHandler 
* 
* Inputs		:none
* 
* Outputs		:none 
* 				  
* Created		:01/22/2010
* 
* Created By	:Ramya
* 
*********************************************************************/
class StartUp_Timer : public TimerHandler 
{
	public:
		StartUp_Timer(Protoname* agent) : TimerHandler ()
		{
			agent_ = agent;/* save reference to routing agent */
		}
	
	protected:
		Protoname* agent_;
		virtual void expire(Event* e);
		
};

//Ramya
class Process_Timer : public TimerHandler 
{
	public:
		Process_Timer(Protoname* agent) : TimerHandler ()
		{
			agent_ = agent;/* save reference to routing agent */
		}
	
	protected:
		Protoname* agent_;
		virtual void expire(Event* e);
		
};
//Ramya
class Data_Timer : public TimerHandler 
{
	public:
		Data_Timer(Protoname* agent) : TimerHandler ()
		{
			agent_ = agent;/* save reference to routing agent */
		}
	
	protected:
		Protoname* agent_;
		virtual void expire(Event* e);
		
};


//Ramya
class Neighbor_Timer : public TimerHandler 
{
	public:
		Neighbor_Timer(Protoname* agent) : TimerHandler ()
		{
			agent_ = agent;/* save reference to routing agent */
		}
	
	protected:
		Protoname* agent_;
		virtual void expire(Event* e);
		
};

//Ramya
class Monitor_Timer : public TimerHandler 
{
	public:
		Monitor_Timer(Protoname* agent) : TimerHandler ()
		{
			agent_ = agent;/* save reference to routing agent */
		}
	
	protected:
		Protoname* agent_;
		virtual void expire(Event* e);
		
};
class Ack_Timer : public TimerHandler 
{
	public:
		Ack_Timer(Protoname* agent) : TimerHandler ()
		{
			agent_ = agent;
		}
	    int sentPacketType;
	    Packet* sentPacket;
	protected:
		Protoname* agent_;
		virtual void expire(Event* e);
		
};
class RouteData_Timer : public TimerHandler 
{
	public:
		RouteData_Timer(Protoname* agent) : TimerHandler ()
		{
			agent_ = agent;
		}
		int sentPacketId;
	protected:
		Protoname* agent_;
		virtual void expire(Event* e);
		
};
/* Callback helper */

/*******************************************************************
* Class			:Protoname()
*
* Description	:main class for the protoname.Its base class is Agent
*
* Prototype		:class Protoname : public Agent
*
* Inputs		:none
*
* Outputs		:none
*
* Created		:01/22/2010
* 
* Created By	:Ramya
*
*********************************************************************/

class Protoname : public Agent
{
	/*Friends*/
	friend class Init_Timer;
	friend class StartUp_Timer;
	friend class Process_Timer;
	friend class Data_Timer;
	friend class Neighbor_Timer;
	friend class Monitor_Timer;
	friend class Ack_Timer;
	friend class RouteData_Timer;
	friend class Recv_Timer;
	friend class MacSimple;
	friend class Mac802_11;
	friend class Protoname_EnergyModel;
	
	NodeStatus	*nodeStatus;
	u_int8_t			seq_num_;
	God*		    	godinstance_;
	int8_t	 			hopDistance;
	int 				srcNode;
	int					baseStation;
	bool 				isMissingInfoRecvd;
	bool				sentNotification;
	bool				isMonitor;
	bool 				isFinalStage;
	nsaddr_t    		interimMonitor;
	bool				isForwarderSource;
	int					monitorPacketCount;

	/*Private members*/
	nsaddr_t			ra_addr_;
	protoname_rtable	rtable_;
	nodesMap			nodesmap;
	int			accesible_var_;
	int         iSeverityLevel ;
	int			processInterval;
	int			lastProcessInterval;
	int 		srcCount;
	bool		isActive; //Is the current node in active data path.
	bool 		isForwarder;
	nsaddr_t 	activeNodes[2*MAX_NEIGHBOR_NODES];
	nsaddr_t    monitoringNodes[MAX_MONITORED_NODES]; // Nodes being monitored by me. 	
	nsaddr_t 	prioritySources[20];
	int			prioritySourceCount;
	nsaddr_t 	prevPrioritySources[20];
	int			prevPrioritySourceCount;
	nsaddr_t	reduceReporting[20];
	int			reduceReportingCount;
	int 		recoveredSourcesCount;
	int 		recoveredIntervals;
	int			notRecoveredSourcesCount;
	int			notRecoveredIntervals;
	/*Declare three arrays each for priority sources */
	nsaddr_t 	prioritySources1[20];
	nsaddr_t 	prioritySources2[20];
	nsaddr_t 	prioritySources3[20];
	int 		prioritySourceCount1;
	int 		prioritySourceCount2;
	int 		prioritySourceCount3;
	bool		increaseReportingRate;
	bool		decreaseReportingRate;
	int	 		increaseReportingCount;
	int			decreaseReportingCount;
	bool		hasIncreasedReportingRate;
	bool		overallReduceReporting;
	//bool		setPriority;
	//nsaddr_t 	min;
	nsaddr_t 	max;
	int 	    actNodeCount;
	int         monitoredLinks;
	nsaddr_t	srcNodes[20];
	struct monitorInfo monInfo[MAX_MONITORED_LINKS];
	struct neighborNodeStatus neighbors[MAX_NEIGHBOR_NODES];
	struct commonNeighborInfo commonInfo[MAX_NEIGHBOR_NODES];
	struct recoveryIntervalInfo recoveryInterval[20];
	int	   recoveryIntervalSources;
	int activeNodeCount;
	//Active Node ofr one node will be only one.
	//Inactive nodes for one node will all its remaining one hop neighbors.
	nsaddr_t forwarder; // the only active node in my neigbor hood. all others are inactive
	nsaddr_t forwarders_forwarder;
	nsaddr_t forwarders_monitor;
	Protoname_EnergyModel energymodel;/* object of energy model class */
	nsaddr_t 	myMonitor;
	bool		MONITORS_ENABLED;
	
	
protected:
	PortClassifier*     dmux_;		// For passing packets up to agents
	Trace*		    	logtarget_;	// For logging.
	Init_Timer  		init_timer_;     // Timer for initializing the stats
	Data_Timer			data_timer_;  //Timer for sending data only at fixed intervals
	StartUp_Timer       startup_timer;   // Timer for Startup
	Process_Timer		process_timer; //Timer for processing at BS
	Neighbor_Timer		neighbor_timer;
	Monitor_Timer		monitor_timer;
	Recv_Timer			recv_timer;
	Ack_Timer*			ack_timer[MAX_MONITORED_LINKS];//used at monitor
	RouteData_Timer*  	route_timer[QUEUEPACKET_SZ*100]; //Timer used by monitor node to route the packet to when a link fails  
	
    Mac802_11*			mac;
    LL*					ll;  //Ramya added to access the link layer
	u_int8_t 			forwarder_req_mesg_sent;
	u_int8_t 			missing_info_mesg_sent;
	u_int8_t			active_neighbor_mesg_recvd;
	u_int8_t 			monitor_notification_mesg_sent;
	u_int8_t 			monitor_agree_mesg_sent;
	u_int8_t 			monitor_req_mesg_sent;
	ProtonameQueue		DataQueue; //Queue for sending packets only at fixed interval of time
	ProtonameQueue		monQueue;/* Queue for storing MONITOR_REQ_MESG packets */
	ProtonameQueue		ackQueue;
	ProtonameQueue		monDataQueue;	/* Queue for storing data packets overheard by monitor */
	ProtonameQueue      monRecvQueue[MAX_MONITORED_LINKS];
	int 		link[MAX_MONITORED_LINKS];
	int 		prevHop[MAX_MONITORED_LINKS];
	ProtonameQueue		recvDataQueue;  	/* Queue for storing received data packets */
	ProtonameQueue		sentDataQueue;
	
	 
	inline nsaddr_t&		ra_addr()		{	return ra_addr_;}
	inline int &			accesible_var() {	return accesible_var_; }
	void recv_protoname_pkt(Packet*);
	void send_protoname_pkt();
	void end_init_timer();
	void end_data_timer();
	void end_startup_timer();
	void end_process_timer();
	void end_neighbor_timer();
	void end_monitor_timer();
	void end_recv_timer();
	void end_ack_timer(int timerType,Packet *p);

public:
	int getHopDistancefromBS();
	void sendPacket(Packet* receivedPacket,int packetType,nsaddr_t dest);
	void setUpDataPath();
	void sendMissingInfoMesg(Packet *p);
	void forward_data(Packet*,nsaddr_t dest);
	void configureFinalForwarder();
	Protoname(nsaddr_t Address);
	int	command(int,const char*const*);
	void recv(Packet*, Handler*);
	void getOneHopInfo();
	nsaddr_t getInterimMonitor();
	void init_MonitoringNodes();
    void setInterimMonitor(nsaddr_t srcAddr);
	void process_pkt(Packet* p);
	void startTimer(int timerType,Packet *p,int packetType,double delay);
	void transmitFailed(Packet *pkt);
	void AddMonitoredNodes(nsaddr_t srcAddr,nsaddr_t destAddr);
	int  checkMonInfo(nsaddr_t addr1,nsaddr_t addr2,nsaddr_t addr3);
	void getBaseStationID();
	void init_neighborStatus();
	void setMyMonitor(nsaddr_t srcAddr);
	void init_myMonitors();
	void GetMonitor(u_int8_t * forwarders_neigbors);
	void deleteMonInfo(nsaddr_t addr1,nsaddr_t addr2,nsaddr_t addr3);
	nsaddr_t getMyMonitor();
	void configureMonitor(int);
	bool checkDataSender(nsaddr_t srcAddr);
	void monitorLinks(Packet *p,nsaddr_t srcAddr);
	void forward_data_with_monitor(Packet* p,nsaddr_t type);
	void routePacket(int sentPacketId);
	void sendDataPacket_with_monitor();
	friend void transmitFailedCallback(Packet *pkt, void *data);
	bool isSourceNode(nsaddr_t neighbor);
	void setInactiveNodes();
	bool		setPriority;
	bool		noSendingState;
	int			noSendingCount;
	//bool		prevSendingState;
	//Packet*		prevPacketSent;

	
};
/*******************************************************************
* Class			:ProtonameHeaderClass()
*
* Description	:Static class for the protoname packet header
*
* Prototype		:static class ProtonameHeaderClass : public PacketHeaderClass
*
* Inputs		:none
*
* Outputs		:none
* 
* Created		:01/22/2010
* 
* Created By	:Ramya
*
*********************************************************************/
static class ProtonameHeaderClass : public PacketHeaderClass
{
	public:
		ProtonameHeaderClass() : PacketHeaderClass("PacketHeader/Protoname",sizeof(struct hdr_protoname_pkt))
		{

			bind_offset(&hdr_protoname_pkt :: offset_);
		}
}class_rtProtoProtoname_hdr;


/*******************************************************************
* Class			:ProtonameClass()
*
* Description	:Tcl Hooks for Protoname class
*
* Prototype		: static class ProtonameClass : public TclClass
*
* Inputs		:none
*
* Outputs		:none
*
* Created		:01/22/2010
* 
* Created By	:Ramya
*
*********************************************************************/

 static class ProtonameClass : public TclClass
 {
 public:
     ProtonameClass() : TclClass("Agent/Protoname") {}
     TclObject* create(int argc, const char*const* argv)
     {
         assert(argc == 5);
         return (new Protoname((nsaddr_t) Address::instance().str2addr(argv[4])));

     }
 } class_rtProtoProtoname;




#endif

