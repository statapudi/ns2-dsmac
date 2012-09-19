/**
 * 
 */
import java.io.*;
import java.util.*;
/**
 * @author vnandana
 *
 */

public class genSrc{
	
 //  static int baseStation = 28;
 //  static String srcNodes ="4 9 10 11 12 13 15 16 18 19 21 23 29 33 35 38 39 45 48 49";
  //static String activeNodes ="0 1 2 3 4 5 8 9 10 11 12 13 16 17 18 19 21 23 25 27 28 29 31 32 33 35 36 38 39 40 41 42 43 46 47 48 49";
   //static String inactiveNodes ="6 7 14 15 20 22 24 26 30 34 37 44 45";	
   static int baseStation = -1;
   static String srcNodes ="";
   static String activeNodes ="";
   static String inactiveNodes ="";
   static int [] aNodes;
   
   static int activeCount = 0;
   static int inactiveCount = 0;
   
   public static  void genTrafficFile(int nodeCount,int srcCount,int pktSize,double pktIntrvl,int dutyCycle) throws IOException
	{
		PrintWriter pw = null;
		String trafficFile_name = nodeCount + "_Node_Traffic.tcl";
		double simulation_start = 240.0 ;
		double simulation_stop =  250.0;
		
		String[] linesToWrite = {"#set the baseStation id here","set BaseStation "+baseStation,"#Set the Source Id's here",
								 "set Source {"+srcNodes+"}","#set the parameters here",
								 "set packetSize  "+pktSize,"set intrval     "+pktIntrvl,"set randm       1","set maxPkts     10000",
								 "set srcCount [llength $Source]","set indx 0","foreach x $Source {","#Setting up the UDP Agent",
								 "set udp_($indx) [new Agent/UDP]","$udp_($indx) set fid_ [expr $indx \\+ 1]","$ns_ attach-agent $node_($x) $udp_($indx)",
								 "incr indx","}","set null_(0) [new Agent/Null]" ,"$ns_ attach-agent $node_($BaseStation) $null_(0)",
								 "for {set i 0} {$i < $srcCount} {incr i} {","set cbr_($i) [new Application/Traffic/CBR]",
								 "$cbr_($i) set packetSize_ $packetSize","$cbr_($i) set interval_ $intrval",
								 "$cbr_($i) set random_ $randm","$cbr_($i) set maxpkts_ $maxPkts","$cbr_($i) attach-agent $udp_($i)",
								 "$ns_ connect $udp_($i) $null_(0)", "$ns_ at "+simulation_start+" \"$cbr_($i) start\"","$ns_ at "+simulation_stop+" \"$cbr_($i) stop\" }"
									};
		
		pw = new PrintWriter(new FileWriter(trafficFile_name));
		for (int i = 0; i < linesToWrite.length; i++)
		{
			if(i == linesToWrite.length-1 )
				pw.print(linesToWrite[i]);
			else
				pw.println(linesToWrite[i]);
		}
		pw.flush();
	
		//Close the PrintWriter
		if (pw != null)
			pw.close();
	}		

	public static void genSrcSinkFile(int nodeCount,int srcCount,int pktSize,double pktIntrvl,int dutyCycle,int MonStatus) throws IOException
	{
		PrintWriter pw = null;
		String srcSinkFile_name = nodeCount + "_Node_SourceSink.txt";
		String[] linesToWrite = {"BaseStation: "+baseStation,"srcCount: "+srcCount,
								 "Source: "+srcNodes,"dutyCycle: "+dutyCycle,"Monitors: "+MonStatus,
								 "Active: "+activeNodes,"Inactive: "+inactiveNodes};
				
		pw = new PrintWriter(new FileWriter(srcSinkFile_name));
		for (int i = 0; i < linesToWrite.length; i++)
		{
			if(i == linesToWrite.length-1 )
				pw.print(linesToWrite[i]);
			else
				pw.println(linesToWrite[i]);
		}
		pw.flush();
	
		//Close the PrintWriter
		if (pw != null)
			pw.close();
	}	

	public static void genNodeStatus(int nodeCount)
	{
		 //Generate Active/inactive nodes count
		  Random randm = new Random();
		  int count = 0;
		  int[] NodeList = new int[nodeCount];
	      aNodes  = new int[nodeCount];
	           
		  for(int i=0;i<nodeCount;i++)
			  NodeList[i] = i;

		  while(count < nodeCount)
		  {
			  		  
			  double source = (randm.nextInt(100000));
			  
			  if(source < 60000 && NodeList[count] != -1)
			  {
				  activeNodes+=count+" ";
				  aNodes[activeCount] = count;
				  activeCount++;
				  NodeList[count] = -1;
			  }
			  count ++;
		  }  
		  for(int i=0;i<nodeCount;i++)
		  {
			  if(NodeList[i] == i)
			  {
				  inactiveNodes+=i+" ";
				  inactiveCount++;
			  }
		  }
	
	}
	public static void genSources(int nodeCount,int srcCount)
	{
		int count = 0;
		int[] NodeList = new int[nodeCount];
		Random randm = new Random();	
		if(baseStation < 0)
			baseStation = aNodes[(int) (Math.random() * (activeCount) + 0)];
			
		  
		  for(int i=0;i<nodeCount;i++)
			  NodeList[i] = i;
		  
		  NodeList[baseStation] = -1;
		  
		  count = 0;
		  if(activeCount < srcCount)
		  {
			  System.out.println("Not enough Active nodes to chose given "+srcCount+" Sources");
			  System.exit(0);
		  }
		  while(count != srcCount)
		  {
			  
			  int source = aNodes[randm.nextInt(activeCount)];
			 
			  
			  if(source != baseStation && NodeList[source] != -1)
			  {
				  srcNodes +=source+" ";
				  NodeList[source] = -1;
				  count ++;
			  }
		  }		
	}
	public static void assignNodeStatus(int nodeCount)
	{
		aNodes  = new int[nodeCount];
		StringTokenizer strtkn = new StringTokenizer(activeNodes," ");
		int i = strtkn.countTokens();
		System.out.println("Token Count :"+strtkn.countTokens());
	    for(int k=0;k<i;k++)
	    {
	    	 aNodes[k] = Integer.parseInt(strtkn.nextToken());
	    	 activeCount++;
	    }
	     inactiveCount = nodeCount-activeCount;
	}
    public static void main(String[] args) throws IOException
    {

		if(args.length != 6)
		{
			System.out.println("Usage <No of Nodes> <Source-Node Count> <Packet-size> <Packet-Interval> <dutyCycle> <MonStatus>");
			System.exit(0);
		}
	  int nodeCount = Integer.parseInt(args[0]);
	  int srcCount  = Integer.parseInt(args[1]);
	  int pktSize   = Integer.parseInt(args[2]);
	  double pktIntrvl   = Double.parseDouble(args[3]);
	  int dutyCycle = Integer.parseInt(args[4]);
	  int MonStatus = Integer.parseInt(args[5]);
      
	  if(activeNodes.length()==0)
		  genNodeStatus(nodeCount);
	  else
		  assignNodeStatus(nodeCount);  
	  
	  if(srcNodes.length()==0)
		  genSources(nodeCount,srcCount);		  
	  
	  

	  System.out.println(activeCount+" :Active Nodes:"+activeNodes);
	  System.out.println(inactiveCount+" :Inactive Nodes:"+inactiveNodes);
	 
	  genTrafficFile(nodeCount,srcCount,pktSize,pktIntrvl,dutyCycle);
	  genSrcSinkFile(nodeCount,srcCount,pktSize,pktIntrvl,dutyCycle,MonStatus);
	  System.out.println("BaseStation :"+baseStation);
	  System.out.println("Done!");
     }

  }



