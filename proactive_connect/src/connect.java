
public class connect {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		System.out.println("hello world");
		try{
			System.load("/home/frank/swimExtended/swim/examples/proactive_connect/src/module.so");
		} catch(UnsatisfiedLinkError e){
			System.err.println("Native code library failed to load.\n" + e);
			System.exit(1);
		}
		//System.load("/home/frank/swimExtended/swim/examples/planDB_connect/src/module.so");
		System.out.println("loaded");
		
	}

}
