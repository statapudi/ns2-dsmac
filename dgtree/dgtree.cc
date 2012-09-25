#include "dgtree_pkt.h"
#include "dgtree.h"
#include <agent.h>
#include <packet.h>
#include <address.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmu-trace.h>


int hdr_dgtree::offset_;

DGTree::DGTree(nsaddr_t id) :Agent(PT_DGTREE), pkt_timer_(this){
	int i = 0;
	bind_bool("accessible_var_", &accessible_var_);
	godinstance_ = God::instance();
	assert(godinstance_ != 0);
	ra_addr_ = id;
	printf("Hello address: %d\n", ra_addr_);
	baseStation_ = 0;
	/*Initially we assume a node can accommodate the desired number of forwarders.
	 Once all forwarders are determined, this is adjusted accordingly*/
	num_forwarders_ = MAX_FORWARDERS;
	/* Forwarders are set to -1 initially. Will be changed to the actual
	 addresses after determining them*/
	for (i = 0; i < num_forwarders_; i++)
		forwarderset[i] = -1;

}

int DGTree::command(int argc, const char* const * argv) {
	if (argc == 2) {
		if (strcasecmp(argv[1], "start") == 0) {
			/*
			 * Building neighborhood information
			 */
			neighbourcount_ = buildNeighbourInfo();
			printf("Count:%d" , neighbourcount_ );
			printdownStreamNeighbours();
			/*
			 * If node is base station, initiate the PARENT_HELLO message
			 */

			if(ra_addr_ == baseStation_)
			{
				hop_ = 0;
				send_dgtree_pkt(PARENT_HELLO,-1);
			}
			return TCL_OK;
		}

		else if (strcasecmp(argv[1], "print_rtable") == 0) {
			if (logtarget_ != 0) {
				sprintf(logtarget_->pt_->buffer(), "P %f _%d_ Routing Table",
						CURRENT_TIME, ra_addr());
				logtarget_->pt_->dump();
				//TODO: rtable_.print(logtarget_);
			} else {
				fprintf(stdout,
						"%f _%d_ If you want to print this routing table "
							"you must create a trace file in your tcl script",
						CURRENT_TIME, ra_addr());
			}
			return TCL_OK;
		}
	} else if (argc == 3) {
		//Obtains corresponding dmux to carry packets to upper layers
		if (strcmp(argv[1], "port-dmux") == 0) {
			dmux_ = (PortClassifier*) TclObject::lookup(argv[2]);
			if (dmux_ == 0) {
				fprintf(stderr, "%s: %s lookup of %s failed \n", __FILE__,
						argv[1], argv[2]);
				return TCL_ERROR;
			}
			return TCL_OK;
		} else if (strcmp(argv[1], "log-target") == 0 || strcmp(argv[1],
				"tracetarget") == 0) {
			logtarget_ = (Trace*) TclObject::lookup(argv[2]);
			if (logtarget_ == 0)
				return TCL_ERROR;
			return TCL_OK;

		}

	}

	return Agent::command(argc, argv);
}

int DGTree::buildNeighbourInfo()
{
	int nodeCount = godinstance_->nodes();
	int i;
	int j =0;
	int count=0;
	for(i = 0; i < nodeCount; i++){
		if((ra_addr() != (nsaddr_t)i) && (godinstance_->hops(ra_addr(),(nsaddr_t)i) == 1) && (godinstance_->hops((nsaddr_t)i, baseStation_)- (godinstance_->hops(ra_addr(), baseStation_)) ==1)){
			downStreamNeighbors[j++] = i;
			count++;
		}

	}
	return count;
}

void DGTree::printdownStreamNeighbours(){
	int i;
	printf("**Node %d: ", ra_addr());
	for(i=0;i<neighbourcount_;i++){
		printf("%d , ", downStreamNeighbors[i]);
	}
	printf("\n");

}

void DGTree::recv(Packet* p, Handler* h) {

	struct hdr_cmn* ch = HDR_CMN(p);
	struct hdr_ip* ih = HDR_IP(p);
	if (ih->saddr() == ra_addr()) {
		// If there exists a loop, must drop the packet
		if (ch->num_forwards() > 0) {
			drop(p, DROP_RTR_ROUTE_LOOP);
			return;
		}
		// else if this is a packet I am originating, must add IP header length
		else if (ch->num_forwards() == 0)
			ch->size() += IP_HDR_LEN;
	}

	// If it is a DGTree packet, must process it
	if (ch->ptype() == PT_DGTREE)
		recv_dgtree_pkt(p);
	// Otherwise, must forward the packet (unless TTL has reached zero)
	else {
		ih->ttl_--;
		if (ih->ttl_ == 0) {
			drop(p, DROP_RTR_TTL);
			return;
		}
		forward_data(p);
	}
}

void DGTree::recv_dgtree_pkt(Packet *p) {
	struct hdr_ip* ih = HDR_IP(p);
	struct hdr_dgtree* ph = HDR_DGTREE(p);

	/* All routing messages are sent from and to port RT_PORT,
	 so we check it */
	assert(ih->sport() == RT_PORT);
	assert(ih->dport() == RT_PORT);

	/* ... TODO: processing of dgtree packet ... */
	switch(ih->flowid()){
	case PARENT_HELLO:
		// Updating hop distance of current node from base station
			hop_ = ph->hopcount_ + 1;
			printf("***********Hello packet!***********\n");

			break;


	}


	// Release resources
	Packet::free(p);
}

void DGTree::send_dgtree_pkt(int type, int flags)
{
	Packet* p = allocpkt();
	struct hdr_cmn* ch = HDR_CMN(p);
	struct hdr_ip* ih = HDR_IP(p);
	struct hdr_dgtree* ph = HDR_DGTREE(p);
	ph->pkt_src() = ra_addr();
	ph->pkt_len() = 7;
	ph->pkt_seq_num() = seq_num_++;
	// TODO: switch case to determine value of flags
	ph->flags() = flags;
	ph->hopcount() = hop_;
	ch->ptype() = PT_DGTREE;
	ch->direction() = hdr_cmn::DOWN;
	ch->size() = IP_HDR_LEN + ph->pkt_len();
	ch->error() = 0;
	ch->next_hop() = IP_BROADCAST;
	ch->addr_type() = NS_AF_INET;
	ih->saddr() = ra_addr();
	ih->daddr() = IP_BROADCAST;
	ih->sport() = RT_PORT;
	ih->dport() = RT_PORT;
	ih->ttl() = IP_DEF_TTL;
	ih->flowid() = type;
	Scheduler::instance().schedule(target_, p, JITTER);

}

void DGTree::forward_data(Packet* p) {


}

void DGTree::reset_dgtree_pkt_timer() {
	pkt_timer_.resched((double)3.0);
}


void DGTree_PktTimer::expire(Event* e) {
	agent_->send_dgtree_pkt(0,-1);
	agent_->reset_dgtree_pkt_timer();
}
/********** TCL Hooks************/

static class DGTreeHeaderClass: PacketHeaderClass {
public:
	DGTreeHeaderClass() :
		PacketHeaderClass("PacketHeader/DGTree", sizeof(hdr_dgtree)) {
		bind_offset(&hdr_dgtree::offset_);

	}

} class_hdrDGTree;

static class DGTreeClass: TclClass {
public:
	DGTreeClass() :
		TclClass("Agent/DGTree") {
	}
	TclObject* create(int argc, const char* const * argv) {
		assert(argc == 5);
		return (new DGTree((nsaddr_t) Address::instance().str2addr(argv[4])));
	}
} class_rtDGTree;

/********** ENDOF TCL Hooks************/
