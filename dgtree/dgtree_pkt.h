#ifndef __dgtree_pkt_h__
#define __dgtree_pkt_h__

#include <packet.h>

#define PARENT_HELLO 0
#define CHILD_ACK 1
#define PARENT_SUPPORT_NUM 2

#define HDR_DGTREE(p) hdr_dgtree::access(p)

struct hdr_dgtree {

	nsaddr_t pkt_src_;
	inline nsaddr_t& pkt_src() {
		return pkt_src_;
	}
	static int offset_;
	inline static int& offset() {
		return offset_;
	}
	inline static hdr_dgtree* access(const Packet* p) { //used to access the packet header
		return (hdr_dgtree*) p->access(offset_);
	}


};

#endif
