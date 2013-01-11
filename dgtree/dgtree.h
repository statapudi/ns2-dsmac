#ifndef __dgtree_h__
#define __dgtree_h__
#include "dgtree_pkt.h"
#include <agent.h>
#include <packet.h>
#include <trace.h>
#include <timer-handler.h>
#include <random.h>
#include <classifier-port.h>
#include <cmu-trace.h>
#include <god.h>
#include <mac-802_11.h>

#define CURRENT_TIME Scheduler::instance().clock()
#define JITTER (Random::uniform()*0.5)
#define MAX_NEIGHBOURS 50
#define MAXWAITCHAIN 15
#define MAX_BACKLOG 10
#define MAX_FORWARDERS 10
#define PARENT_HELLO 0
#define CHILD_ACK 1
#define CHILDREN_COUNT 2

class DGTree;// forward declaration

/* Timers */
struct forwarder{
	nsaddr_t addr_;
	int childCount_;
};

class DGTree_PktTimer: public TimerHandler {
public:
	DGTree_PktTimer(DGTree* agent) :
		TimerHandler() {
		agent_ = agent;
	}
protected:
	DGTree* agent_;
	virtual void expire(Event* e);
};

/* Agent */

class DGTree: public Agent {

	/* Friends */
	friend class DGTree_PktTimer;

	/* Private members */
	nsaddr_t ra_addr_;
	Mac802_11 *mymac;
	Packet* backlog[MAX_BACKLOG];
	int hop_;
	int neighbourcount_;
	bool forwarderSetupDone;
	int currbacklog;
	God* godinstance_;
	nsaddr_t baseStation_;
	int accessible_var_;
	int potential_forwarders_;
	int childcountsrecvd;
	int num_desired_forwarders_;
	int num_acks_recvd_;
	int numhellosrecvd_;
	u_int8_t seq_num_;
	nsaddr_t downStreamNeighbors[MAX_NEIGHBOURS];
	nsaddr_t potential_forwarder_set[MAX_NEIGHBOURS];
	int roundrobin;


protected:

	PortClassifier* dmux_; // For passing packets up to agents.
	Trace* logtarget_; // For logging.
	DGTree_PktTimer pkt_timer_; // Timer for sending packets.


	inline nsaddr_t& ra_addr() {
		return ra_addr_;
	}

	inline int& accessible_var() {
		return accessible_var_;
	}

	inline int& num_acks_recvd() {
		return num_acks_recvd_;
	}

	inline int& neighbourCount() {
		return neighbourcount_;
	}

	inline int& hop() {
			return hop_;
		}

	inline int& num_forwarders() {
		return num_desired_forwarders_;
	}
	inline nsaddr_t& baseStation() {
		return baseStation_;
	}

	void forward_data(Packet*);
	//void* PrintHello(void*);
	int buildNeighbourInfo();
	void printdownStreamNeighbours();
	void printForwarderSet();
	void recv_dgtree_pkt(Packet*);
	void send_dgtree_pkt(nsaddr_t,int,int);
	void reset_dgtree_pkt_timer();
	void clearBacklog();
	void addBacklog(Packet*);

public:

	DGTree(nsaddr_t);
	void test();
	int command(int, const char* const *);
	void recv(Packet*, Handler*);
	forwarder forwarderset[MAX_FORWARDERS];

};

#endif

