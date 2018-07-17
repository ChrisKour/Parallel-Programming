
public class Sort{

	private static final int ITERATIONS = 1000;//Arithmos stoixeiwn poy 8a kanoume sort
	
	public static void main(String[] args){
		
		int[] x= new int[ITERATIONS];
		int[] y= new int[ITERATIONS];
		int i,j,chunk = 250; //Symfwna me to chunk 8a dhmiourgi8oyn ta threads kai apo pou ews poy 8a trexoun
		
		for (i=0; i<ITERATIONS; i++) //arxikopoihsh pinaka
			x[i] = ITERATIONS - i;
		
		MyThread thr[] = new MyThread[ITERATIONS/chunk]; //Dhmiourgia pinaka gia ta threads, se ayth thn periptwsh 4
			
		for (i=0,j=0; i<(ITERATIONS/chunk); i++, j+=chunk) {
			thr[i] = new MyThread(j,(i*chunk)+chunk,ITERATIONS,x,y); //Dhmiougia threads kai ksekinhma
			thr[i].start();
	   }  
		
		for(i = 0; i < thr.length; i++) {//kanoume join ksana ta nhmata
			try {
				thr[i].join();
			}
			catch (InterruptedException e) {
				System.err.println("Error");
			}
		}
	   
	   for (i=0; i<ITERATIONS; i++) 
			System.out.println(y[i]);//typwnoume ta apotelesmata
		
	}
	
	
	
}
