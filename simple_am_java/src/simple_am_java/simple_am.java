package simple_am_java;

import java.net.*;

public class simple_am{

	final static int NUMBER_OF_DIMMER_LEVELS = 3;
	final static double DIMMER_STEP = 1.0 / (NUMBER_OF_DIMMER_LEVELS - 1);
	final static double RT_THRESHOLD = 1.00;
	final static int PERIOD = 60;

	
	
	public String getDivertTrafficCommand(int activeServersA, int activeServersB, int activeServersC) {
	    String cmd = "";

	    if ((activeServersA != 0) && (activeServersB != 0) && (activeServersC != 0)) {
	        cmd = SwimClient.cmdDivertTraffic_50_25_25;
	    } else if (!(activeServersA != 0) && (activeServersB != 0) && (activeServersC != 0)) {
	        cmd = SwimClient.cmdDivertTraffic_0_50_50;
	    } else if ((activeServersA != 0) && !(activeServersB != 0) && (activeServersC != 0)) {
	        cmd = SwimClient.cmdDivertTraffic_50_0_50;
	    } else if ((activeServersA != 0) && (activeServersB != 0) && !(activeServersC != 0)) {
	        cmd = SwimClient.cmdDivertTraffic_50_50_0;
	    } else if (!(activeServersA != 0) && !(activeServersB != 0) && (activeServersC != 0)) {
	        cmd = SwimClient.cmdDivertTraffic_0_0_100;
	    } else if (!(activeServersA != 0) && (activeServersB != 0) && !(activeServersC != 0)) {
	        cmd = SwimClient.cmdDivertTraffic_0_100_0;
	    } else if ((activeServersA != 0) && !(activeServersB != 0) && !(activeServersC != 0)) {
	        cmd = SwimClient.cmdDivertTraffic_100_0_0;
	    }

	    return cmd;
	}
	
	void handleHighResponseTime(SwimClient swim) {
	    int serversA = swim.getServers(SwimClient.ServerType.A);
	    int serversB = swim.getServers(SwimClient.ServerType.B);
	    int serversC = swim.getServers(SwimClient.ServerType.C);
	    double dimmerFactor = swim.getDimmer();

	    int totalServers = serversA + serversB + serversC;

	    int activeServersA = swim.getActiveServers(SwimClient.ServerType.A);
	    int activeServersB = swim.getActiveServers(SwimClient.ServerType.B);
	    int activeServersC = swim.getActiveServers(SwimClient.ServerType.C);

	    int totalActiveServers = activeServersA + activeServersB + activeServersC;
	    boolean isServerBooting = (totalServers > totalActiveServers);
	    String divertTrafficCmd = getDivertTrafficCommand(activeServersA, activeServersB, activeServersC);
	    swim.divertTraffic(divertTrafficCmd);

	    if (!isServerBooting) {
	        if (serversC < swim.getMaxServers(SwimClient.ServerType.C)) {
	            swim.addServer(SwimClient.ServerType.C);
	        } else if (serversB < swim.getMaxServers(SwimClient.ServerType.B)) {
	            swim.addServer(SwimClient.ServerType.B);
	        } else if (serversA < swim.getMaxServers(SwimClient.ServerType.A)) {
	            swim.addServer(SwimClient.ServerType.A);
	        }
	    }

	    // Compare probability for a request having an optional content
	    // Less dimmer factor implies lesser request are served with optional content
	    // Therefore, if probability is greater than 0 then decrease it.
	    if (dimmerFactor > 0.0) {
	        swim.decreaseDimmer();
	    }
	}
	
	void handleLowResponseTime(SwimClient swim) {
	    int serversA = swim.getServers(SwimClient.ServerType.A);
	    int serversB = swim.getServers(SwimClient.ServerType.B);
	    int serversC = swim.getServers(SwimClient.ServerType.C);
	    double dimmerFactor = swim.getDimmer();

	    int totalServers = serversA + serversB + serversC;

	    int activeServersA = swim.getActiveServers(SwimClient.ServerType.A);
	    int activeServersB = swim.getActiveServers(SwimClient.ServerType.B);
	    int activeServersC = swim.getActiveServers(SwimClient.ServerType.C);

	    int totalActiveServers = activeServersA + activeServersB + activeServersC;
	    boolean isServerBooting = (totalServers > totalActiveServers);

	    if (!isServerBooting) {
	        if (activeServersA > 0) {
	            activeServersA = 0;

	            String divertTrafficCmd = getDivertTrafficCommand(activeServersA, activeServersB, activeServersC);
	            swim.divertTraffic(divertTrafficCmd);
	            swim.removeServer(SwimClient.ServerType.A);
	        } else if(activeServersB > 0) {
	            activeServersB = 0;

	            String divertTrafficCmd = getDivertTrafficCommand(activeServersA, activeServersB, activeServersC);
	            swim.divertTraffic(divertTrafficCmd);
	            swim.removeServer(SwimClient.ServerType.B);
	        } else if (activeServersC > 0) {
	            activeServersC = 0;

	            String divertTrafficCmd = getDivertTrafficCommand(activeServersA, activeServersB, activeServersC);
	            swim.divertTraffic(divertTrafficCmd);
	            swim.removeServer(SwimClient.ServerType.C);
	        }

	    }

	    // Compare probability for a request having an optional content
	    // More dimmer factor implies more request are served with optional content
	    // Therefore, if probability is less than 1 then increase it.
	    if (dimmerFactor < 1.0) {
	        swim.increaseDimmer();
	    }
	}
	
	void simpleAdaptationManager(SwimClient swim) {
	    boolean test = true;

	    while (swim.isConnected()) {
	        if (test) {
	            swim.addServer(SwimClient.ServerType.C);
	            swim.decreaseDimmer();
	            test = false;
	        }

	        double responseTime = swim.getAverageResponseTime();

	        if (responseTime > RT_THRESHOLD) {
	            handleHighResponseTime(swim);
	        } else if (responseTime < RT_THRESHOLD) {
	            handleLowResponseTime(swim);
	        }

	        try {
				Thread.sleep(PERIOD);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
	    }
	}
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		System.out.println("hello world\n");
		
		//testing SWIG
		{
			try {
				System.load("/home/frank/swimExtended/swim/examples/simple_am_java/"
						+ "module.so");
			} catch(UnsatisfiedLinkError e) {
				System.err.println("Native code library failed to load.\n" + e);
				System.exit(1);
			}
		}
		
		System.out.println(example.fact(5));
		
		
		
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
		
		/*
		do {
			swim.connect();
		} while(!swim.isConnected());
		*/
		
		
		System.out.println("Connected to SWIM");

		
	}

}
