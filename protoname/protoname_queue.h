#ifndef PROTONAME_QUEUE_H_
#define PROTONAME_QUEUE_H_
#include <packet.h>

#define QUEUEPACKET_SZ 			500
/* A queue to hold packets */
class ProtonameQueue {

public:
	/* constructor */
	ProtonameQueue(int QueueSize);
	
	ProtonameQueue();

	/* add an element to the queue */
	bool 	insert_item(Packet*, int);

	/* check if the queue is full */
	bool	is_full();

	/* check if a packet with the given id exists in the queue */
	bool	has_packet(int id);

	/* remove packet with given id from queue */
	bool	remove_packet(int id);

	/* get first item */
	Packet* get_item(int*);

	/* removes the first item in the queue */
	void	remove_item();

	/* returns the next hop expected to forward packet to */
	int		next_hop();

	/* change next hop of the first item */
	void	change_next_hop(int);

	/* returns the size of the queue */
	int 	queue_sz();

	/* print the queue */
	void 	print_the_queue(char *s);

	/* get the ID number of the first packet in the queue */
	int 	get_head_id();
	
	int     get_rear_id();
	
	Packet*  get_packet(int id); 
	
	Packet* get_index(int id);
	
	bool has_protoname_packet(int id); 
	
	bool remove_protoname_packet(int id) ;
	void print_the_queue(char *s,int srcAddr) ; 
	void print_the_protoname_queue(char *s);
	Packet* get_priority_packet(int priority);
	
	
private:
	Packet*	pkt_queue_[QUEUEPACKET_SZ];
	int		next_hop_;
	int		head_;
	int		size_;
	//int maxSize ;
	
};
#endif /*PROTONAME_QUEUE_H_*/
