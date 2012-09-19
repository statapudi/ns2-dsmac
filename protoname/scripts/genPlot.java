/**
 * 
 */
import java.io.*;
import java.util.*;
/**
 * @author vnandana
 *
 */

public class genPlot{
	

	public static  void genPlotFile(String xlabel,String ylabel,String dataFile,String title,String outputFile) throws IOException
	{
		PrintWriter pw = null;
		String plotFile = "gnuPlotFile.txt";

		String[] linesToWrite = {"set data style linespoints","set terminal postscript color",
								 "set title font \"Helvetica,20\"","set xlabel \""+xlabel+"\"","set ylabel \""+ylabel+"\"",
								 "set output \""+outputFile+"\"","plot \""+dataFile+"\""+" using 2:1 title \""+title+"\""
								 };
		
		pw = new PrintWriter(new FileWriter(plotFile));
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



    public static void main(String[] args) throws IOException
    {

		if(args.length != 5)
		{
			System.out.println("Usage <xlabel> <ylabel> <dataFile> <title> <outputFile> "+args.length );
			System.exit(0);
		}
		
		genPlotFile(args[0],args[1],args[2],args[3],args[4]);

      
	  System.out.println("Done!");
     }

  }



