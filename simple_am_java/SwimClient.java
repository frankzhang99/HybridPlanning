package simple_am_java;

import java.io.*;
import java.net.*;
import java.util.*;


public class SwimClient{

	
	public enum ServerType {
		NONE, A, B, C
	};
	
	public enum TrafficLoad {
        ZERO, TWENTYFIVE, FIFTY, SEVENTYFIVE, HUNDRED, INVALID
    };
	
	static String cmdDecDimmer = "dec_dimmer";
	static String cmdIncDimmer = "inc_dimmer";

	static String cmdDivertTraffic_100_0_0 = "divert_100_0_0";
	static String cmdDivertTraffic_75_25_0 = "divert_75_25_0";
	static String cmdDivertTraffic_75_0_25 = "divert_75_0_25";
	static String cmdDivertTraffic_50_50_0 = "divert_50_50_0";
	static String cmdDivertTraffic_50_0_50 = "divert_50_0_50";
	static String cmdDivertTraffic_50_25_25 = "divert_50_25_25";
	static String cmdDivertTraffic_25_75_0 = "divert_25_75_0";
	static String cmdDivertTraffic_25_0_75 = "divert_25_0_75";
	static String cmdDivertTraffic_25_50_25 = "divert_25_50_25";
	static String cmdDivertTraffic_25_25_50 = "divert_25_25_50";
	static String cmdDivertTraffic_0_100_0 = "divert_0_100_0";
	static String cmdDivertTraffic_0_0_100 = "divert_0_0_100";
	static String cmdDivertTraffic_0_75_25 = "divert_0_75_25";
	static String cmdDivertTraffic_0_25_75 = "divert_0_25_75";
	static String cmdDivertTraffic_0_50_50 = "divert_0_50_50";
	
	private Socket mClientSocket = null;
	private boolean mDebug = false;
	private String mHost = "localhost";
	private int mPort = 4242;
	
	public SwimClient(String host, int port) {
		mHost = host;
		mPort = port;
	}
	
	public void connect() {
		try {
			mClientSocket = new Socket(mHost, mPort);
		} catch(UnknownHostException e) {
			e.printStackTrace();
		} catch(IOException e) {
			e.printStackTrace();
		}
	}
	
	public boolean isConnected() {
		return mClientSocket.isConnected();
	}
	
	public String sendCommand(String command) {
		if(!isConnected()) {
			throw new RuntimeException("socket is closed");
		}
		
		DataOutputStream outToServer = null;
		try {
			outToServer = new DataOutputStream(mClientSocket.getOutputStream());
		} catch(IOException e) {
			e.printStackTrace();
		}
		
		try {
			if(mDebug) {
				System.out.println("Sending Command: " + command);
			}
			outToServer.writeBytes(command + '\n');
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		BufferedReader inFromServer = null;
		try {
			inFromServer = new BufferedReader(new InputStreamReader(mClientSocket.getInputStream()));
		} catch(IOException e) {
			e.printStackTrace();
		}
		
		String jsonOutput = null;
		try {
			jsonOutput = inFromServer.readLine();
		} catch(IOException e) {
			e.printStackTrace();
		}
		
		if(mDebug) {
			System.out.println("FROM SERVER: " + jsonOutput);
		}
		
		return jsonOutput;
	}
	
	public double probeDouble(String command) {
		String reply = sendCommand(command);
		return Double.parseDouble(reply);
	}
	
	public int probeInt (String command) {
		String reply = sendCommand(command);
		return Integer.parseInt(reply);
	}
	
	public double getDimmer() {
		return probeDouble("get_dimmer\n");
	}
	
	public int getServers(ServerType serverType) {
		return probeInt("get_servers " + serverType + "\n");
	}
	
	public int getActiveServers(ServerType serverType) {
		return probeInt("get_active_servers " + serverType + "\n");
	}
	
	public int getMaxServers(ServerType serverType) {
		return probeInt("get_max_servers " + serverType + "\n");
	}
	
	public double getAverageResponseTime() {
		return probeDouble("get_avg_rt\n");
	}
	
	public double getArrivalRate() {
	    return probeDouble("get_arrival_rate\n");
	}
	
	public void addServer(ServerType serverType) {
	    sendCommand("add_server " + serverType + "\n");
	}
	
	public void removeServer(ServerType serverType) {
		sendCommand("remove_server " + serverType + "\n");
	}
	
	public void increaseDimmer() {
		sendCommand(cmdIncDimmer + " \n");
	}
	
	public void decreaseDimmer() {
		sendCommand(cmdDecDimmer + " \n");
	}
	
	public void divertTraffic(String divertTrafficCmd) {
		sendCommand("divert_traffic " + divertTrafficCmd + "\n");
	}
	
	public void disconnect() {
		try {
			mClientSocket.close();
		} catch(IOException e) {
			e.printStackTrace();
		}
	}
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}

}






