#include "dgtree_pkt.h"
#include "dgtree.h"
#include <agent.h>
#include <packet.h>
#include <address.h>

int hdr_dgtree::offset_;

DGTree::DGTree(nsaddr_t id) :
	Agent(PT_DGTREE){
	bind_bool("accessible_var_", &accessible_var_);
	ra_addr_ = id;
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
DGTree::~DGTree(){}

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
