#include "dgtree_pkt.h"
#include "dgtree.h"
#include <agent.h>
#include <packet.h>
#include <address.h>

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




}

void  DGTree::forward_data(Packet* p,nsaddr_t type){



}
DGTree::~DGTree(){} // Empty destructor

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
