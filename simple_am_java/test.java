
public class test {
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		System.out.println("hello world");
		
		//testing SWIG
		{
			try {
				System.load("/home/frank/swimExtended/swim/examples/simple_am_java/module.so");
			} catch(UnsatisfiedLinkError e) {
				System.err.println("Native code library failed to load.\n" + e);
				System.exit(1);
			}
		}
		
		System.out.println(example.fact(5));
	}
}
