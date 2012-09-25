#ifndef __dgtree_pkt_h__
#define __dgtree_pkt_h__

#include <packet.h>

#define PARENT_HELLO 0
#define CHILD_ACK 1
#define PARENT_SUPPORT_NUM 2

#define HDR_DGTREE(p) hdr_dgtree::access(p)

struct hdr_dgtree {

	nsaddr_t pkt_src_;
	u_int16_t pkt_len_;
	u_int8_t pkt_seq_num_;
	u_int16_t flags_;
	uint16_t hopcount_;

	inline nsaddr_t& pkt_src() {
		return pkt_src_;
	}
	inline u_int16_t& pkt_len() {
		return pkt_len_;
	}
	inline u_int16_t& hopcount() {
			return hopcount_;
		}
	inline u_int8_t& pkt_seq_num() {
		return pkt_seq_num_;
	}
	inline u_int16_t& flags() {
			return flags_;
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
