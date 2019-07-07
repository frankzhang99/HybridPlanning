import java.net.*;
import java.io.*;
import java.util.Random;
import java.util.concurrent.*;

import pladapt.EnvironmentDTMCPartitioned;
import pladapt.PMCAdaptationManager;
import pladapt.TimeSeriesPredictor;

public class HPAdaptationManager{

	//final static int NUMBER_OF_DIMMER_LEVELS = 3;
	//final static double DIMMER_STEP = 1.0 / (NUMBER_OF_DIMMER_LEVELS - 1);
	final static double RT_THRESHOLD = 1.00;
	final static int PERIOD = 5000;
	final static double SERVERA_COST_SEC = 1;
	final static double SERVERB_COST_SEC = 0.7;
	final static double SERVERC_COST_SEC = 0.5;
	final static int MAX_ARRIVAL_CAPACITY_A = 200;
	final static int MAX_ARRIVAL_CAPACITY_LOW_A = 400;
	final static int MAX_ARRIVAL_CAPACITY_B = 140;
	final static int MAX_ARRIVAL_CAPACITY_LOW_B = 280;
	final static int MAX_ARRIVAL_CAPACITY_C = 100;
	final static int MAX_ARRIVAL_CAPACITY_LOW_C = 200;
	final static double PENALTY = -0.25;
	
	
	
	public static String get_initial_state_str(HPModel hpModel) {
		String res = "";
		
		res += "mdp\n\n";
		
		res += "const double addServer_LATENCY = ";
		res += hpModel.bootDelay;
		res += ";\n";
		
		res += "const int HORIZON = ";
		res += hpModel.horizon;
		res += ";\n";
		
		res += "const double PERIOD = ";
		res += hpModel.evaluationPeriod;
		res += ";\n";
		
		res += "const int DIMMER_LEVELS = ";
		res += hpModel.numberOfBrownoutLevels;
		res += ";\n";
		
		res += "const int ini_dimmer = ";
		int discretizedBrownoutFactor = 1 + (hpModel.numberOfBrownoutLevels - 1) * (int)(hpModel.brownoutFactor);
		res += discretizedBrownoutFactor;
		res += ";\n";
		
		res += "const int MAX_SERVERS_A = ";
		res += hpModel.max_server_A;
		res += ";\n";
		
		res += "const int MAX_SERVERS_B = ";
		res += hpModel.max_server_B;
		res += ";\n";
		
		res += "const int MAX_SERVERS_C = ";
		res += hpModel.max_server_C;
		res += ";\n";
		
		res += "const int ini_servers_A = ";
		res += hpModel.active_server_A;
		res += ";\n";
		
		res += "const int ini_servers_B = ";
		res += hpModel.active_server_B;
		res += ";\n";
		
		res += "const int ini_servers_C = ";
		res += hpModel.active_server_C;
		res += ";\n";
		
		int addServerAState = 0;
	    int addServerBState = 0;
	    int addServerCState = 0;

	    int addServerState = 0;
		
		res += "const int ini_addServerA_state = ";
		res += addServerAState;
		res += ";\n";
		
		res += "const int ini_addServerB_state = ";
		res += addServerBState;
		res += ";\n";
		
		res += "const int ini_addServerC_state = ";
		res += addServerCState;
		res += ";\n";
		
		res += "const double SERVERA_COST_SEC = ";
		res += SERVERA_COST_SEC;
		res += ";\n";
		
		res += "const double SERVERB_COST_SEC = ";
		res += SERVERB_COST_SEC;
		res += ";\n";
		
		res += "const double SERVERC_COST_SEC = ";
		res += SERVERC_COST_SEC;
		res += ";\n";
		
		res += "const double MAX_ARRIVALA_CAPACITY = ";
		res += MAX_ARRIVAL_CAPACITY_A;
		res += ";\n";
		
		res += "const double MAX_ARRIVALA_CAPACITY_LOW = ";
		res += MAX_ARRIVAL_CAPACITY_LOW_A;
		res += ";\n";
		
		res += "const double MAX_ARRIVALB_CAPACITY = ";
		res += MAX_ARRIVAL_CAPACITY_B;
		res += ";\n";
		
		res += "const double MAX_ARRIVALB_CAPACITY_LOW = ";
		res += MAX_ARRIVAL_CAPACITY_LOW_B;
		res += ";\n";
		
		res += "const double MAX_ARRIVALC_CAPACITY = ";
		res += MAX_ARRIVAL_CAPACITY_C;
		res += ";\n";
		
		res += "const double MAX_ARRIVALC_CAPACITY_LOW = ";
		res += MAX_ARRIVAL_CAPACITY_LOW_C;
		res += ";\n";
		
		res += "const double penalty = ";
		res += PENALTY;
		res += ";\n";
		
		res += "const int ini_traffic_A = ";
		res += hpModel.traffic_A;
		res += ";\n";
		
		res += "const int ini_traffic_B = ";
		res += hpModel.traffic_B;
		res += ";\n";
		
		res += "const int ini_traffic_C = ";
		res += hpModel.traffic_C;
		res += ";\n";
		
		res += "const double interArrivalScaleFactorForDecision = 1;\n";
		
		return res;
	}
	
	
	
	public static void main(String[] args) throws InterruptedException{
		// TODO Auto-generated method stub
		System.out.println("hello world");
		
		{
			try {
				System.load("/home/frank/swimExtended/swim/examples/simple_am_java/src/"
						+ "module.so");
				System.load("/home/frank/swimExtended/swim/examples/proactive_connect/src/libpladapt_wrap.so");
			} catch(UnsatisfiedLinkError e) {
				System.err.println("Native code library failed to load.\n" + e);
				System.exit(1);
			}
		}
		System.out.println("libraries loaded");
		//System.out.println(example.fact(5));
		
		
		
		String host = "localhost";
		int port = 4242;
		
		if(args.length > 2) {
			System.out.println("error: invalid options");
        	System.out.println("valid options: [host [port]]");
        	return;
		} else if (args.length > 0) {
			host = args[0];
        	if (args.length > 1) {
            	port = Integer.parseInt(args[1]);
        	}
		}
	
		
		
		SwimClient swim = new SwimClient(host, port);
		simple_am am = new simple_am();
		HPModel hpModel = new HPModel();
		String initString = get_initial_state_str(hpModel);
		//System.out.println(initString);
		String dtmc = hpModel.getDTMC();
		//System.out.println(dtmc);
		
		
		String fileName = "modelTemplate/temp.txt";
		String fileOutput = "modelTest/ttimemodel.prism";
		String line = null;
		try {
			FileReader fileReader = new FileReader(fileName);
			BufferedReader bufferedReader = new BufferedReader(fileReader);
			FileWriter fileWriter = new FileWriter(fileOutput);
			BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);
			bufferedWriter.write(initString);
			bufferedWriter.newLine();
			bufferedWriter.newLine();
			bufferedWriter.write("module environment\n");
			bufferedWriter.write(dtmc);
			while((line = bufferedReader.readLine()) != null) {
				//System.out.println(line);
				bufferedWriter.write(line);
				bufferedWriter.newLine();
			}
			bufferedReader.close();
			bufferedWriter.close();
		} catch(FileNotFoundException ex) {
			System.err.println("Unable to open file");
		} catch(IOException ex) {
			System.err.println("Error reading/writing file");
		}
		
		
		do {
			try {
				//swim.connect();
				Thread.sleep(500);
				System.out.println("Waiting for SWIM...");
			} catch (Exception e) {
				//System.out.println("THERE");
			}
		} while(!swim.isConnected());
		System.out.println("Connected to SWIM");
		
		
		boolean flag = true;
		int min = 0;
		do {
			double ar = swim.getArrivalRate();
			System.out.println("Min: " + min++);
			System.out.println(ar);
			TimeUnit.SECONDS.sleep(60);
		} while (flag);
		
		
		
		
		
		
	}

}
