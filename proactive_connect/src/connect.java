import java.util.Random;
import pladapt.TimeSeriesPredictor;
import pladapt.EnvironmentDTMCPartitioned;
import pladapt.PMCAdaptationManager;
//import pladapt.*;


class connect {

	static {
		try{
			System.load("/home/frank/swimExtended/swim/examples/proactive_connect/src/libpladapt_wrap.so");
		} catch(UnsatisfiedLinkError e){
			System.err.println("Native code library failed to load.\n" + e);
			System.exit(1);
		}
	}
	
	public static void log(String msg) {
		System.out.println(msg);
	}
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		System.out.println("hello world");
		try{
			//System.load("/home/frank/swimExtended/swim/examples/proactive_connect/src/libpladapt_wrap.so");
		} catch(UnsatisfiedLinkError e){
			System.err.println("Native code library failed to load.\n" + e);
			System.exit(1);
		}
		System.out.println("loaded");
		
		TimeSeriesPredictor ts = TimeSeriesPredictor.getInstance("LES 0.8 0.15", 15, 5);
		//PMCAdaptationManager am = new PMCAdaptationManager();
		Random rnd = new Random(3);
		for(int i = 0; i < 30; ++i) {
			ts.observe(2 + i + 2 * rnd.nextDouble());
		}
		EnvironmentDTMCPartitioned envDTMC = ts.generateEnvironmentDTMC(2, 4);
		String test = PMCAdaptationManager.generateEnvironmentDTMC(envDTMC);
		//System.out.println(envDTMC);
		//System.out.println(test);
		
		
//		log("Starting PLA Adaptation Manager init");
//		JavaSDPAdaptationManager adaptMgr = new JavaSDPAdaptationManager();
//		final int MAX_SERVERS = 3;
//		final int LATENCY_PERIODS = 1;
//		final int DIMMER_LEVELS = 5;
//		GenericConfigurationManager configMgr = new GenericConfigurationManager();
//		GenericConfiguration configTemplate = configMgr.getConfigurationTemplate();
//		configTemplate.setInt("s", 0);
//      configTemplate.setInt("d", 0);
//      configTemplate.setInt("addServerProgress", 0);
		
	}

}
