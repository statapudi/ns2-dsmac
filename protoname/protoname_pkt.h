#ifndef __protoname_pkt_h__
#define __protoname_pkt_h__

#include <packet.h>
#define HDR_PROTONAME_PKT(p) ((struct hdr_protoname_pkt*) hdr_protoname_pkt::access(p))
#define MAX_NEIGHBORS_COUNT 50

struct hdr_protoname_pkt
{
		nsaddr_t        pkt_dest_;       //node which originated this packet           //4
       // nsaddr_t        pkt_src_;        //node which originated this packet		
        nsaddr_t        pkt_monitor_;												//4	
        nsaddr_t        pkt_forwarder_;												//4
        nsaddr_t		pkt_forwarders_forwarder_;//forwarders forwarder			//4
        u_int16_t       pkt_len_;        //packet length in bytes					//2
        //int             pkt_rank_;
        u_int8_t		pkt_seq_num_;    //packet sequence number					//1
        u_int8_t		pkt_monitored_links_;										//1
        u_int8_t		pkt_neighbors_[MAX_NEIGHBORS_COUNT]	;   //packet sequence number //50
        u_int8_t		nodeStatus_; //0: InActive   1:Active							//1
        u_int8_t		pkt_hopCount_; //0: InActive   1:Active	
        u_int8_t		pkt_activeNeighborCount_; 
        u_int8_t		pkt_prioritySources_[20];
        u_int8_t 		pkt_prioritySourceCount_;
        u_int8_t		pkt_reduceReporting_[20];
        u_int8_t 		pkt_reduceReportingCount_;
        u_int8_t		pkt_processInterval_;
        int				uid;														//4
		
	static int offset_;
	inline		nsaddr_t&	pkt_uid() {return uid; }
	inline		nsaddr_t&	pkt_monitor() {return pkt_monitor_; }
	inline		nsaddr_t&	pkt_forwarder() {return pkt_forwarder_;}
	inline		nsaddr_t&	pkt_forwarders_forwarder() {return pkt_forwarders_forwarder_;}
	inline		nsaddr_t&	pkt_dest() {return pkt_dest_; }
	inline		u_int16_t&	pkt_len() {return pkt_len_; }
	//inline 		u_int8_t&   pkt_neighbors() {return pkt_neighbors_; }
	//inline		int&	pkt_rank(){return pkt_rank_;}
	inline		u_int8_t&	pkt_seq_num(){return pkt_seq_num_;}
	inline		u_int8_t&	pkt_nodeStatus(){return nodeStatus_;}
	inline		u_int8_t&	pkt_monitored_links(){return pkt_monitored_links_;}
	inline  	u_int8_t&	pkt_hopCount(){return pkt_hopCount_;}
	inline 		u_int8_t& 	pkt_activeNeighborCount(){return pkt_activeNeighborCount_;}
	inline 		u_int8_t& 	pkt_prioritySourceCount(){return pkt_prioritySourceCount_;}
	inline 		u_int8_t& 	pkt_reduceReportingCount(){return pkt_reduceReportingCount_;}
	inline 		u_int8_t& 	pkt_processInterval(){return pkt_processInterval_;}
	inline static hdr_protoname_pkt * access(const Packet* p)
	{
		return (hdr_protoname_pkt *)p->access(offset_);
	}
};

#endif
