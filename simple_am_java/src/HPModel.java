import java.net.*;
import java.util.Random;
import java.util.concurrent.*;

import pladapt.EnvironmentDTMCPartitioned;
import pladapt.PMCAdaptationManager;
import pladapt.TimeSeriesPredictor;

public class HPModel {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
	}

	double timeActiveServerCountLast;
    double evaluationPeriod;

//    HPConfiguration configuration;
//    Environment environment;
//    Observations observations;
//    EnvironmentVector environmentPrediction;
//    EnvRecord envRecord;

    double bootDelay;
    int horizon;
    int serverThreads;

//    ServerInfo serverA;
//    ServerInfo serverB;
//    ServerInfo serverC;

    int numberOfBrownoutLevels;

    TimeSeriesPredictor pArrivalMeanPredictor;
    
    
    //NEW STUFF
    double brownoutFactor;
    int max_server_A;
    int max_server_B;
    int max_server_C;
    int active_server_A;
    int active_server_B;
    int active_server_C;
    int traffic_A;
    int traffic_B;
    int traffic_C;
    
    
    
    
    public HPModel() {
    	this.numberOfBrownoutLevels = 3;
    	this.evaluationPeriod = 60;
    	this.bootDelay = 120;
    	this.max_server_A = 1;
    	this.max_server_B = 1;
    	this.max_server_C = 1;
    	int maxServers = max_server_A + max_server_B + max_server_C;
    	this.horizon = (int) Math.max(5,
                Math.ceil(bootDelay / evaluationPeriod) * (maxServers - 1) + 1);
    	pArrivalMeanPredictor = TimeSeriesPredictor.getInstance("LES 0.8 0.15", 15, 5);
    	pArrivalMeanPredictor.observe(0.00655949);
    	pArrivalMeanPredictor.observe(0.00707864);
    	pArrivalMeanPredictor.observe(0.00737987);
    	pArrivalMeanPredictor.observe(0.00752144);
    	pArrivalMeanPredictor.observe(0.00729423);
    	pArrivalMeanPredictor.observe(0.00740298);
    	pArrivalMeanPredictor.observe(0.00730431);
    	pArrivalMeanPredictor.observe(0.00727339);
    	pArrivalMeanPredictor.observe(0.00688481);
    	pArrivalMeanPredictor.observe(0.00631957);
    	pArrivalMeanPredictor.observe(0.00626385);
    	pArrivalMeanPredictor.observe(0.00600893);
    	pArrivalMeanPredictor.observe(0.0060793);
    	pArrivalMeanPredictor.observe(0.00603236);
    	pArrivalMeanPredictor.observe(0.00612817);
    	
    	//testing
    	this.active_server_A = 1;
    	this.traffic_A = 4;
    	System.out.println("DONE INIT");
    }
    
    public String getDTMC() {
    	int branchingDepth = horizon - 1;
    	EnvironmentDTMCPartitioned envDTMC = 
    			pArrivalMeanPredictor.generateEnvironmentDTMC(branchingDepth, Math.max(branchingDepth, horizon));
    	return PMCAdaptationManager.generateEnvironmentDTMC(envDTMC);
    }
    
    
    
	
	
}
