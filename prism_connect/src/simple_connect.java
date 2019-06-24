
public class simple_connect {

	
	public static void main(String[] args) {
		System.out.println("hello world");
		//testing SWIG
		try{
			System.load("/home/frank/swimExtended/swim/examples/prism_connect/src/module.so");
		} catch(UnsatisfiedLinkError e){
			System.err.println("Native code library failed to load.\n" + e);
			System.exit(1);
		}
		System.out.println("loaded");
		AdaptationPlanner ap = new AdaptationPlanner();
		ap.test();
		ap.plan("", "");
		
		System.out.println("DONE");
	}
	
}
