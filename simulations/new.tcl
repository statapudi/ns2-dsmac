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
set val(x) 		250		 ;# X dimension of topology
set val(y) 		250		 ;# Y dimension of topology
#set val(cp)		""	         ;# node movement model file
#set val(sc)		""		 ;# traffic model file
set val(ifqlen) 50                       ;# max packet in ifq
set val(nn)     6            	         ;# number of nodes
set val(seed)	0.0
set val(stop)	60.0			 ;# simulation time
#set val(dcset)	0.2			 ;# Duty Cycle offset
set val(tr)	vamroute.tr		 ;# trace file name
set val(rp)     DGTree                ;# routing protocol
#set val(energymodel) EnergyModel
#set val(initialenergy) 30000
set AgentTrace  OFF
set RouterTrace OFF
set MacTrace    OFF

# change transmission range (15 meters: 8.5457e-07)
#Phy/WirelessPhy set RXThresh_ 0.5457e-09
#Phy/WirelessPhy set CSThresh_ 0.5457e-09

# change bandwidth
#Mac set bandwidth_ 2.4Kb

# Remove useless packet headers
remove-all-packet-headers
add-packet-header IP DGTree Mac ARP LL

#Create a simulator object
set ns_ [new Simulator]

# Open trace file 
$ns_ use-newtrace        ;# Use new trace format
set tracefd     [open $val(tr) w]
$ns_ trace-all $tracefd

# set up topography object
set topo [new Topography]
$topo load_flatgrid $val(x) $val(y)

# create channel 
set chan [new $val(chan)]

# Create God
create-god $val(nn)
set god_ [God instance]

#  Create the specified number of mobile nodes [$val(nn)] and "attach" them
#  to the channel.
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
$node_(0) set X_ 110.0
$node_(0) set Y_ 230.0
$node_(0) set Z_ 0.0

$node_(2) set X_ 130.0
$node_(2) set Y_ 210.0
$node_(2) set Z_ 0.0

$node_(4) set X_ 90.0
$node_(4) set Y_ 200.0
$node_(4) set Z_ 0.0

$node_(3) set X_ 133.0
$node_(3) set Y_ 161.0
$node_(3) set Z_ 0.0

$node_(5) set X_ 212.0
$node_(5) set Y_ 212.0
$node_(5) set Z_ 0.0

$node_(1) set X_ 106.0
$node_(1) set Y_ 232.0
$node_(1) set Z_ 0.0


# Load the god object with shortest hop information
$god_ set-dist 0 1 1
$god_ set-dist 0 2 1
$god_ set-dist 0 3 1
$god_ set-dist 0 4 2
$god_ set-dist 0 5 2
$god_ set-dist 1 2 1
$god_ set-dist 1 3 2
$god_ set-dist 1 4 1
$god_ set-dist 1 5 2
$god_ set-dist 2 3 1
$god_ set-dist 2 4 1
$god_ set-dist 2 5 1
$god_ set-dist 3 4 2
$god_ set-dist 3 5 1
$god_ set-dist 4 5 1



# Tell nodes when the simulation ends
for {set i 0} {$i < $val(nn) } {incr i} {
    $ns_ at $val(stop)  "$node_($i) reset";
}

$ns_ at $val(stop)  "stop"
$ns_ at $val(stop)  "puts \"NS EXITING...\" ; $ns_ halt"
proc stop {} {
    global ns_ tracefd 
    $ns_ flush-trace
    close $tracefd
}

puts "Starting Simulation..."
$ns_ run
