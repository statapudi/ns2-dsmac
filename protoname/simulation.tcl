# simulation.tcl
# Tests Protoname routing agent

# Define options
# ======================================================================
set val(chan)	Channel/WirelessChannel	 ;# channel type
set val(prop)	Propagation/TwoRayGround ;# radio-propagation model
set val(netif)	Phy/WirelessPhy          ;# network interface type
#set val(mac)	Mac/802_11               ;# MAC type
set val(mac)	Mac/Simple               ;# MAC type
set val(ifq) 	Queue/DropTail/PriQueue  ;# interface queue type
#set val(ifq) 	CMUPriQueue				 ;# interface queue type (for DSR)
set val(ll)		LL                       ;# link layer type
set val(ant)	Antenna/OmniAntenna      ;# antenna model
set val(x) 		500		 ;# X dimension of topology
set val(y) 		500		 ;# Y dimension of topology
set val(cp)		""	         ;# node movement model file
set val(sc)		""		 ;# traffic model file
set val(ifqlen) 50                       ;# max packet in ifq
set val(nn)     11            	         ;# number of nodes
set val(seed)	0.0
set val(stop)	150.0			 ;# simulation time
set val(dcset)	0.2			 ;# Duty Cycle offset
set val(tr)	vamroute.tr		 ;# trace file name
set val(rp)     Protoname                ;# routing protocol
set AgentTrace  ON
set RouterTrace ON
set MacTrace    ON

# Initialize Global Variables
add-packet-header Protoname
set ns_		[new Simulator]
$ns_ color 1	Blue
$ns_ color 2    Red

# Open trace file 
$ns_ use-newtrace        ;# Use new trace format
set namfd 	[open nam-exp.tr w]
$ns_ namtrace-all-wireless $namfd $val(x) $val(y)
set tracefd     [open $val(tr) w]
$ns_ trace-all $tracefd

# set up topography object
set topo       [new Topography]
$topo load_flatgrid $val(x) $val(y)

# create channel 
set chan [new $val(chan)]

# Create God
set god_ [create-god $val(nn)]
#  Create the specified number of mobile nodes [$val(nn)] and "attach" them
#  to the channel. Five nodes are created : node(0), node(1),node(2),node(4) and node(5)
	$ns_ node-config -adhocRouting $val(rp) \
			 -llType $val(ll) \
			 -macType $val(mac) \
			 -ifqType $val(ifq) \
			 -ifqLen $val(ifqlen) \
			 -antType $val(ant) \
			 -propType $val(prop) \
			 -phyType $val(netif) \
			 -channel $chan \
			 -topoInstance $topo \
			 -agentTrace $AgentTrace \
			 -routerTrace $RouterTrace \
			 -macTrace $MacTrace \
			 -movementTrace OFF			
#Disable random motion of the nodes 
	for {set i 0} {$i < $val(nn) } {incr i} {
		set node_($i) [$ns_ node]	
		$node_($i) random-motion 0		;# disable random motion
	}

# Provide initial (X,Y, for now Z=0) co-ordinates for mobilenodes
# default transmission 250m (should change it later...)

# Node 0 is the base station
$node_(0) set X_ 430.0
$node_(0) set Y_ 110.0
$node_(0) set Z_ 0.0

$node_(1) set X_ 230.0
$node_(1) set Y_ 140.0
$node_(1) set Z_ 0.0

$node_(2) set X_ 240.0
$node_(2) set Y_ 80.0
$node_(2) set Z_ 0.0

$node_(3) set X_ 100.0
$node_(3) set Y_ 100.0
$node_(3) set Z_ 0.0

$node_(4) set X_ 0.0
$node_(4) set Y_ 0.0
$node_(4) set Z_ 0.0

# Load the god object with shortest hop information
$god_ set-dist 0 1 1
$god_ set-dist 0 2 1
$god_ set-dist 0 3 1
$god_ set-dist 0 4 2
$god_ set-dist 0 5 2
$god_ set-dist 0 6 3
$god_ set-dist 0 7 3
$god_ set-dist 0 8 4
$god_ set-dist 0 9 4
$god_ set-dist 0 10 5
$god_ set-dist 1 2 2
$god_ set-dist 1 3 1
$god_ set-dist 1 4 2
$god_ set-dist 1 5 2
$god_ set-dist 1 6 3
$god_ set-dist 1 7 3
$god_ set-dist 1 8 4
$god_ set-dist 1 9 4
$god_ set-dist 1 10 5
$god_ set-dist 2 3 1
$god_ set-dist 2 4 2
$god_ set-dist 2 5 2
$god_ set-dist 2 6 3
$god_ set-dist 2 7 3
$god_ set-dist 2 8 4
$god_ set-dist 2 9 4
$god_ set-dist 2 10 5
$god_ set-dist 3 4 1
$god_ set-dist 3 5 1
$god_ set-dist 3 6 2
$god_ set-dist 3 7 2
$god_ set-dist 3 8 3
$god_ set-dist 3 9 3
$god_ set-dist 3 10 4
$god_ set-dist 4 5 1
$god_ set-dist 4 6 2
$god_ set-dist 4 7 2
$god_ set-dist 4 8 3
$god_ set-dist 4 9 3
$god_ set-dist 4 10 4
$god_ set-dist 5 6 1
$god_ set-dist 5 7 1
$god_ set-dist 5 8 2
$god_ set-dist 5 9 2
$god_ set-dist 5 10 3
$god_ set-dist 6 7 1
$god_ set-dist 6 8 2
$god_ set-dist 6 9 2
$god_ set-dist 6 10 3
$god_ set-dist 7 8 1
$god_ set-dist 7 9 1
$god_ set-dist 7 10 2
$god_ set-dist 8 9 1
$god_ set-dist 8 10 2
$god_ set-dist 9 10 1

# Setup UDP traffic flow between nodes 3 connecting to 0 at time 100.0
set udp_(0) [new Agent/UDP]
$udp_(0) set fid_ 1
$ns_ attach-agent $node_(4) $udp_(0)

set null_(0) [new Agent/Null]
$ns_ attach-agent $node_(0) $null_(0)

set cbr_(0) [new Application/Traffic/CBR]
$cbr_(0) set packetSize_ 512
$cbr_(0) set interval_ 15.0
$cbr_(0) set random_ 1
$cbr_(0) set maxpkts_ 10000
$cbr_(0) attach-agent $udp_(0)
$ns_ connect $udp_(0) $null_(0)
$ns_ at 30.0 "$cbr_(0) start"

#Define node initial position in nam, only for nam
for {set i 0} {$i < $val(nn)} {incr i} {
	# The function must be called after mobility model is defined.
	$ns_ initial_node_pos $node_($i) 60
}
# Set the duty cycle of each node
#for {set i 0} {$i < $val(nn) } {incr i} {
#        $ns_ "[$node_($i) agent 255] base-station 0";
#        $ns_ "[$node_($i) agent 255] source-node  4";
#  }

# Tell nodes when the simulation ends
for {set i 0} {$i < $val(nn) } {incr i} {
    $ns_ at $val(stop)  "$node_($i) reset";
}

# Display routing table
$ns_ at 70.0 "[$node_(1) agent 255] print_rtable"
$ns_ at 70.0 "[$node_(2) agent 255] print_rtable"
$ns_ at 70.0 "[$node_(3) agent 255] print_rtable"
$ns_ at 70.0 "[$node_(4) agent 255] print_rtable"

$ns_ at $val(stop)  "stop"
$ns_ at $val(stop)  "puts \"NS EXITING...\" ; $ns_ halt"
proc stop {} {
    global ns_ tracefd namfd
    $ns_ flush-trace
    close $tracefd
    close $namfd
}
puts $tracefd "M 0.0 nn $val(nn) x  $val(x) y $val(y)  rp $val(rp)"
puts $tracefd "M 0.0 sc $val(sc) cp  $val(cp) seed $val(seed)"
puts $tracefd "M 0.0 prop $val(prop) ant $val(ant)"
puts "Starting Simulation..."
$ns_ run
