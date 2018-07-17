
public class ThreadParGroupSqrt {

	public static void main(String[] args)
	{
		// Δημιουργία και αρχικοποίηση πίνακα
		int iterations = 1000;
		double[] a = new double[iterations];
		int numofthreads = 10;
		int chunk = 10;
		
		for(int i = 0; i < iterations; i++)
			a[i] = i; 

		// Δημιουργία πίνακα νημάτων
		SqrtGroupThread threads[] = new SqrtGroupThread[numofthreads];
		
		// Δημιουργία και εκκίνηση νημάτων
		for(int i = 0, j = 0; i < numofthreads; i++,j += chunk) 
		{
			threads[i] = new SqrtGroupThread(a,j,(i*chunk)+chunk,iterations,chunk,numofthreads);
			threads[i].start();
		}

		// Φράγμα
		for (int i = 0; i < threads.length; ++i)
		{
			try {
				threads[i].join();
			}
			catch (InterruptedException e) {
				System.err.println("this should not happen");
			}
		}
		
		// Εμφάνιση στοιχείων πίνακα
		for(int i = 0; i < 1000; i++) 
		{
			System.out.println(a[i]);
		}
	}
	
}
