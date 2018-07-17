
public class SqrtGroupThread extends Thread
{
	private double [] table;
	private int mystop;
	private int mystart,iterations,chunk,numofthreads;

	// Κατασκευαστής
	public SqrtGroupThread(double [] array, int start, int stop,int iter,int chunk,int numofthreads)
	{
		table = array;
		mystart = start;
		mystop = stop;
		iterations = iter;
		this.chunk = chunk;
		this.numofthreads = numofthreads;
	}

	// Κάθε νήμα υπολογίζει την τετραγωνική ρίζα των 100 στοιχείων
	// πίνακα σειριακά
	public void run()
	{
		while(mystart < iterations && mystop <= iterations){
			for(int i=mystart; i<mystop; i++)
				table[i] = Math.sqrt(table[i]);
			mystart = mystart + (chunk*numofthreads); //Threads * chunk
			mystop = mystop  + (chunk*numofthreads);
		}
	}
}
