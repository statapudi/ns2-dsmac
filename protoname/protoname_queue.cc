/* constructor */
#include <protoname/protoname_queue.h>
#include <protoname/protoname_pkt.h>

ProtonameQueue::ProtonameQueue(int QueueSize ) {
	/* initialize elements of queue to null */
	int i;
	for(i = 0; i < QUEUEPACKET_SZ; i++) {
		pkt_queue_[i] = 0;
	}
	next_hop_ = -1;
	/* queue is empty */
	head_ = 0;
	size_ = 0;
}	
ProtonameQueue::ProtonameQueue(){
		/* initialize elements of queue to null */
	int i;
	for(i = 0; i < QUEUEPACKET_SZ; i++) {
		pkt_queue_[i] = 0;
	}
	next_hop_ = -1;
	/* queue is empty */
	head_ = 0;
	size_ = 0;
}

/* add an element to the queue */
bool ProtonameQueue::insert_item(Packet* p, int next_hop) {
	int insertindex;
	
	/* assume that the packet is not null */
	assert(p != 0);
	
	/* make sure there's enough room in the queue */
	if(size_ + 1 >= QUEUEPACKET_SZ)
		return false;
	else {
		/* find out where to store the packet */
		insertindex = (head_ + size_) % QUEUEPACKET_SZ;
		assert(pkt_queue_[insertindex] == 0);
		/* need to do a special copy which'll increment
		   the reference count */
		pkt_queue_[insertindex] = p->refcopy();
		next_hop_ = next_hop;
		/* increment the size */
		size_++;
		return true;
	}
}

/* check if the queue is full */
bool ProtonameQueue::is_full() {
	if(size_ + 1 >= QUEUEPACKET_SZ)
		return true;
	else
		return false;
}

/* check if the queue contains a packet with the given id */
bool ProtonameQueue::has_packet(int id) {
	int i;
	struct hdr_cmn* ch;

	for(i = 0; i < size_; i++) {
		/* get the common header of the packet */
		ch = HDR_CMN(pkt_queue_[(head_ + i) % QUEUEPACKET_SZ]);
		/* check if the IDs match */
		if(ch->uid() == id)
			return true;
	}
	/* packet was not found */
	return false;
}

/* check if the queue contains a protoname packet with the given id */
bool ProtonameQueue::has_protoname_packet(int id) {
	int i;
	//struct hdr_cmn* ch;
	struct hdr_protoname_pkt* ph ;
	for(i = 0; i < size_; i++) {
		/* get the common header of the packet */
		ph = HDR_PROTONAME_PKT(pkt_queue_[(head_ + i) % QUEUEPACKET_SZ]);
		//ch = HDR_CMN(pkt_queue_[(head_ + i) % QUEUEPACKET_SZ]);
		/* check if the IDs match */
		
		//printf("\t Comparing Datapacket id %d with %d present in the queue\n",id,ph->pkt_uid());		
		if(ph->pkt_uid() == id)
			return true;
		
	}
	/* packet was not found */
	return false;
}
/* remove packet with given id from queue */
bool ProtonameQueue::remove_protoname_packet(int id) {
	int i, j;
	//struct hdr_cmn* ch;
	struct hdr_protoname_pkt* ph ;
	if(has_protoname_packet(id)) {
		for(i = 0; i < size_; i++) {
			/* get the common header of the packet */
			ph = HDR_PROTONAME_PKT(pkt_queue_[(head_ + i) % QUEUEPACKET_SZ]);
			if(ph->pkt_uid()== id) {
				/* remove the packet */
				Packet::free(pkt_queue_[(head_ + i) % QUEUEPACKET_SZ]);
				//printf("\tRemoved packet %d at position %d\n", id, (head_ + i) % QUEUEPACKET_SZ);
				/* shift the other packets over */
				for(j = 1; i+j < size_; j++) {
					pkt_queue_[(head_ + i + j - 1) % QUEUEPACKET_SZ] = pkt_queue_[(head_ + i + j) % QUEUEPACKET_SZ];
				}
				/* decrement size */
				size_--;
				/* exit the function */
				return true;
			}
		}		
	}
	return false;
}
/* remove packet with given id from queue */
bool ProtonameQueue::remove_packet(int id) {
	int i, j;
	struct hdr_cmn* ch;

	if(has_packet(id)) {
		for(i = 0; i < size_; i++) {
			/* get the common header of the packet */
			ch = HDR_CMN(pkt_queue_[(head_ + i) % QUEUEPACKET_SZ]);
			if(ch->uid() == id) {
				/* remove the packet */
				Packet::free(pkt_queue_[(head_ + i) % QUEUEPACKET_SZ]);
				printf("\tRemoved packet %d at position %d\n", id, (head_ + i) % QUEUEPACKET_SZ);
				/* shift the other packets over */
				for(j = 1; i+j < size_; j++) {
					pkt_queue_[(head_ + i + j - 1) % QUEUEPACKET_SZ] = pkt_queue_[(head_ + i + j) % QUEUEPACKET_SZ];
				}
				/* decrement size */
				size_--;
				/* exit the function */
				return true;
			}
		}		
	}
	return false;
}

/* gets the first item in the queue without removing it */
Packet* ProtonameQueue::get_item(int* next_hop) {
	Packet* p;

	if(size_ != 0) {
		/* get the first element in the queue */
		p = pkt_queue_[head_]->copy();
		*next_hop = next_hop_;
		/* return the packet */
		return p;
	}
	else
		return 0;
}
/* check if the queue contains a packet with the given id and returns the packet */
Packet* ProtonameQueue::get_packet(int id) {
	int i;
	Packet* p;
	struct hdr_cmn* ch;

    //printf("Size in get_packet is %d\n",size_);
	for(i = 0; i < size_; i++) {
		/* get the common header of the packet */
		
		ch = HDR_CMN(pkt_queue_[(head_ + i) % QUEUEPACKET_SZ]);
		/* check if the IDs match */
		if(ch->uid() == id)
		{
			p = pkt_queue_[(head_ + i) % QUEUEPACKET_SZ]->copy();
			return p;
		}
	}
	/* packet was not found */
	return 0;
}

//Added by ramya to get packets based on their priority
Packet* ProtonameQueue::get_priority_packet(int priority) {
	
	int i;
	Packet* p;
	struct hdr_cmn* ch;
	for(i = 0; i < size_; i++) {
		/* get the common header of the packet */
		ch = HDR_CMN(pkt_queue_[(head_ + i) % QUEUEPACKET_SZ]);
		/* check if the priority matches */
		if(ch->priority() == priority)
		{
			p = pkt_queue_[(head_ + i) % QUEUEPACKET_SZ]->copy();
			return p;
		}
	}
	/* packet was not found */
	return 0;
}
/* removes the first item in the queue */
void ProtonameQueue::remove_item() {
	if(size_ > 0) {
		/* release resources */
		Packet::free(pkt_queue_[head_]);
		/* clear the position in the queue */
		pkt_queue_[head_] = 0;
		/* move the head of the queue */
		head_ = (head_ + 1) % QUEUEPACKET_SZ;
		/* decrement the size of the queue */
		size_--;
	}
}
/* returns the next hop expected to forward packet to */
int ProtonameQueue::next_hop() {
	if(size_ != 0)
		return next_hop_;
	else
		return -1;
}

/* change next hop of the first item */
void ProtonameQueue::change_next_hop(int nh) {
	next_hop_ = nh;
}

/* returns the size of the queue */
int ProtonameQueue::queue_sz() {
	return size_;
	
}

/* print the contents of the queue */
void ProtonameQueue::print_the_queue(char *s,int srcAddr) {
	int index, i;
	struct hdr_cmn* ch;
	
	printf("\t Node %d :%s QUEUE CONTENTS (size: %d):\t[",srcAddr, s,size_);
	for(index = head_, i = 0; i < size_; i++) {
		index = (head_ + i) % QUEUEPACKET_SZ;
		ch = HDR_CMN(pkt_queue_[index]);
		printf("%d--->%d", ch->uid(),ch->priority());
	}
	printf("]; Next Hop: %d\n", next_hop_);
}

/* print the contents of the queue */
void ProtonameQueue::print_the_protoname_queue(char *s) {
	int index, i;
	//struct hdr_cmn* ch;
	struct hdr_protoname_pkt* ph ;
	printf("\t %s QUEUE CONTENTS (size: %d):\t[", s,size_);
	for(index = head_, i = 0; i < size_; i++) {
		index = (head_ + i) % QUEUEPACKET_SZ;
		ph = HDR_PROTONAME_PKT(pkt_queue_[index]);
		printf("%d ", ph->pkt_uid());
	}
	printf("]; Next Hop: %d\n", next_hop_);
}
/* get the head packet ID # */
int ProtonameQueue::get_head_id() {
	struct hdr_cmn* ch = HDR_CMN(pkt_queue_[head_]);
	return ch->uid();
}
int ProtonameQueue::get_rear_id(){
	int index;
	if(size_ == 0)
	{
		//printf("Size is 0 initially \n");
		return -1;
	}
	else
	{
		//printf("\n Size of queue is %d\n",size_);
		index = (head_ + size_ -1) % QUEUEPACKET_SZ;
		struct hdr_cmn* ch = HDR_CMN(pkt_queue_[index]);
		return ch->uid();
	}
	
}
