#include "dgtree_pkt.h"
#include "dgtree.h"
#include <agent.h>
#include <packet.h>
#include <address.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmu-trace.h>

int hdr_dgtree::offset_;


DGTree::DGTree(nsaddr_t id) :
	Agent(PT_DGTREE){
	int i=0;
	bind_bool("accessible_var_", &accessible_var_);
	ra_addr_ = id;
	/*Initially we assume a node can accommodate the desired number of forwarders.
	Once all forwarders are determined, this is adjusted accordingly*/
	num_forwarders = MAX_FORWARDERS;
	/* Forwarders are set to -1 initially. Will be changed to the actual
	addresses after determining them*/
	for(i=0;i<num_forwarders;i++)
		forwarder[i] = -1;

}

int DGTree::command(int argc, const char* const * argv) {
	if (argc == 2) {
		if (strcasecmp(argv[1], "start") == 0) {

			return TCL_OK;
		}
	}
	return Agent::command(argc, argv);
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
		// else if this is a packet I am originating, must add IP header
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


	// Release resources
	Packet::free(p);
}



void  DGTree::forward_data(Packet* p){



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
		TclClass("Agent/DGTree") {}
	TclObject* create(int argc, const char* const * argv) {
		assert(argc == 5);
		return (new DGTree((nsaddr_t)Address::instance().str2addr(argv[4])));
	}
} class_rtDGTree;

/********** ENDOF TCL Hooks************/
