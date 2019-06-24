public class main {
     public static void main(String argv[]) {
       
       {
             try {
                    System.load("/home/frank/swimExtended/swim/examples/simple_am_java/module.so");
                        } catch (UnsatisfiedLinkError e) {
                                System.err.println("Native code library failed to load.\n" + e);
                                      System.exit(1);
                                          }
               }
       
       
       System.out.println(example.fact(5));

     }
}
