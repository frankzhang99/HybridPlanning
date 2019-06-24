
public class connect {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		System.out.println("hello world");
		//testing SWIG
		try{
			System.load("/home/frank/swimExtended/swim/examples/prism_fix_path/src/module.so");
		} catch(UnsatisfiedLinkError e){
			System.err.println("Native code library failed to load.\n" + e);
			System.exit(1);
		}
		try{
			System.load("/home/frank/swimExtended/swim/examples/prism_fix_path/Plan/module.so");
		} catch(UnsatisfiedLinkError e){
			System.err.println("Native code library failed to load.\n" + e);
			System.exit(1);
		}
		System.out.println("loaded");
		
		AdaptationPlanner ap = new AdaptationPlanner();
		
		//ap.test();
		SWIGTYPE_p_std__string s = new SWIGTYPE_p_std__string();
		//ap.plan("", "", s, false);
		//ap.plan("", "");
		
		
		System.out.println("DONE");
	}

}
