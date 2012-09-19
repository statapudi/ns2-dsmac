#ifndef __dgtree_h__
#define __dgtree_h__
#include "dgtree_pkt.h"
#include <agent.h>
#include <packet.h>
#include <trace.h>
#include <timer-handler.h>
#include <random.h>
#include <classifier-port.h>

#define CURRENT_TIME Scheduler::instance().clock()
#define JITTER (Random::uniform()*0.5)
#define MAX_FORWARDERS 1

class DGTree;// forward declaration

/* Timers */

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
	int accessible_var_;
	int num_forwarders;
	u_int8_t seq_num_;
	nsaddr_t forwarder[MAX_FORWARDERS];

protected:

	PortClassifier* dmux_; // For passing packets up to agents.
	Trace* logtarget_; // For logging.
	/*DGtree_PktTimer pkt_timer_; // Timer for sending packets.

	inline nsaddr_t& ra_addr() {
		return ra_addr_;
	}
	inline dgtree_state& state() {
		return state_;
	}*/
	inline int& accessible_var() {
		return accessible_var_;
	}

	//void forward_data(Packet*);
	void recv_dgtree_pkt(Packet*);
	void send_dgtree_pkt();

	void reset_dgtree_pkt_timer();

public:

	DGTree( nsaddr_t);
	~DGTree();
	int command(int, const char* const *);
	void recv(Packet*, Handler*);
	void forward_data(Packet*,nsaddr_t dest);

};

#endif
