#ifndef __protoname_rtable_h__
#define	__protoname_rtable_h__

#include<trace.h>
#include<map>
/*Changed the second paarmeter from nsaddr_t to int as we are storing the hop distance
 instead of the address */

#define MAX_NEIGHBOR_NODES	50
typedef std :: map <nsaddr_t, int> rtable_t;
typedef std :: map <int, nsaddr_t> nodesMap;

typedef struct ranks 
{
	nsaddr_t src;
	int rank;
	bool obtainedRank;
}temprank;	
 
class protoname_rtable
{
rtable_t rt_;

public:
protoname_rtable();
void print(Trace*);
void clear();
void rm_entry(nsaddr_t);
void add_entry(nsaddr_t,nsaddr_t);
int lookup(nsaddr_t);
u_int32_t size();
};
#endif
