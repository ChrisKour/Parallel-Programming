
public class MasterWorker {

	private static int totalTasks; // αριθμός εργασιών
	private static int nThreads; // αριθμός νημάτων
	private static int tasksAssigned = -10; // διαμοιραζόμενη μεταβλητή μετρητή εργασιών
	private static int chunk; // mege8os chunk

	public static void main(String[] args)
	{
		totalTasks = 201; 
		nThreads = 10; 
		chunk = 10;
		
		// Δημιουργία και αρχικοποίηση πίνακα
		double[] a = new double[totalTasks]; 
		for(int i=0; i<totalTasks; i++)
		{
			a[i] = i + 3;
			System.out.println(a[i]);   
		} 

		// Δημιουργία νημάτων εργαζομένων
		Thread[] threads = new Thread[nThreads];
		for (int i = 0; i < threads.length; ++i)
		{
			threads[i] = new Thread(new Worker(a,i));
		}		

		// Εκκίνηση νημάτων εργαζομένων
		for (int i = 0; i < threads.length; ++i)
		{
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
		for(int i = 0; i < totalTasks; i++)
			System.out.println(a[i]);
	}

	//  Κρίσιμη μέθοδος για την διανομή εργασιών
	private static synchronized int getTask()
	{
		// Διανέμει μια εργασία (ή στοιχείο πίνακα) αν υπάρχουν 
		if ((tasksAssigned += chunk) < totalTasks) //to tasksAssigned anebainei kata chunk
			return tasksAssigned;
		else
			return -1;
	}

	// Κώδικας που εκτελεί το κάθε νήμα - εργαζόμενος
	private static class Worker implements Runnable
	{

		private int myID,i;
		private double[] table;

		// Κατασκευαστής
		public Worker(double[] array, int myID)
		{
			this.myID = myID;
			table = array;
		}

		public void run()
		{
			int element;
			// Λαμβάνει εργασία (ή στοιχείο πίνακα) αν υπάρχει
			while ((element = getTask()) >= 0)
			{
				i = element;
				while (i < (element+chunk) && i < totalTasks){ //Elegxos wste to i na mhn yperbainei oute to ka8e chunk oute to totalTasks se periptwsh pou den ginetai teleia diairesh me totalTasks kai chunk
					System.out.println("worker " + myID + " received " + i);
					// Υπολογίζει την τετραγωνική ρίζα του στοιχείου που έλαβε
					table[i]= Math.sqrt(table[i]);   
					i++;
				}
			}
			System.out.println("worker " + myID + " done");
		}
	}
}

