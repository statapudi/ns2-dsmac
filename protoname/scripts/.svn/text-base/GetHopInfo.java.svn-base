/*
 * GetHopInfo.java
 *
 *  Created on: May 21, 2009
 *      Author: rkavalap
 */

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.StringTokenizer;
import java.util.Vector;


public class GetHopInfo {
	
	double x_coord;		// X Coordinate of Node
	double y_coord;		// Y Coordinate of Node
	
	static final int MAX_NO_OF_NODES = 300; // Maximum No Of Nodes
		
	/**
	 * @param args
	 */
	public static void main(String[] args) 
	{
		// TODO Auto-generated method stub
		String inp = "";
		int hopDist;
		
		
		int i = 0;
		int j = 0;
		
		if(args.length != 3)
		{
			System.out.println("Usage <Topology File> <No of Nodes> <Hop Distance>");
			System.exit(0);
		}
		int noOfNodes = Integer.parseInt(args[1]);
		if(noOfNodes > MAX_NO_OF_NODES)
		{
			System.out.println("Nodes greater than 500 not supported");
			System.exit(0);
		}
		
		
		try 
		{
			BufferedReader br = new BufferedReader(new FileReader(args[0]));
			hopDist = Integer.parseInt(args[2]);
								
			Vector<GetHopInfo> nodes = new Vector<GetHopInfo>(MAX_NO_OF_NODES);
			
			for (i = 0; i < MAX_NO_OF_NODES; i++)
				nodes.add(new GetHopInfo());
			
			int index = 0;
			String [] splitStr = new String[5];
			
		
			while ((inp = br.readLine())!= null)
			{
				splitStr = inp.split(" ");
				
				if(splitStr[0].contains("#"))
					continue;
				else if(splitStr[0].contains("$node_"))
				{
					StringTokenizer tok = new StringTokenizer(splitStr[0],"()");
					tok.nextToken();
					index = Integer.parseInt(tok.nextToken());
								
					if(splitStr[2].equals("X_"))
						nodes.get(index).x_coord = Double.parseDouble(splitStr[3]);
					
					if(splitStr[2].equals("Y_"))
						nodes.get(index).y_coord = Double.parseDouble(splitStr[3]);
				}
						
			}
			
			br.close();
			
			String outFile = noOfNodes + "_Node_hopCount_" + hopDist + ".tcl";
			
			BufferedWriter bw = new BufferedWriter(new FileWriter(outFile));
			
			bw.newLine();
			bw.newLine();
			
			bw.write("#***************************************************************************");
			bw.newLine();
			bw.write("# Hop Distances of All nodes in Topology ");
			bw.newLine();
			String hD = "# One Hop Distance is " + hopDist;
			bw.write(hD);
			bw.newLine();
			bw.write("#***************************************************************************");
			bw.newLine();
			
			double tempDist = 0;
			String tstr = "$god_ set-dist ";
			String str ="";
			
			for (i = 0; i < noOfNodes; i++)
			{
				for (j = i; j < noOfNodes; j++)
				{
					if(i != j)
					{
						tempDist = Math.sqrt(Math.pow((nodes.get(i).x_coord - nodes.get(j).x_coord),2) + Math.pow((nodes.get(i).y_coord - nodes.get(j).y_coord),2));
						str = tstr + " " +i +" "+ j +" "+ (int)Math.ceil(tempDist/hopDist);
						int dist = (int)Math.ceil(tempDist/hopDist);
						str = tstr + " " +i +" "+ j +" "+ dist;
						
						if(dist == 1)
							System.out.print(" "+j+",");
						if(j>=i)
						{
							bw.write(str);
							bw.newLine();
						}
					}
				}
				
				
			}
			
			bw.close();
			
		}catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
	}

}
