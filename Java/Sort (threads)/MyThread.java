
public class MyThread extends Thread{

	private int my_num,my_place,i,j,start,stop,iterations;
	private int [] x ;
	private int[] y;
	
	public MyThread(int start,int stop,int iter, int[] primary, int [] results)//Kataskeyasths nhmatwn
	{
		this.start = start;	//Apo pou ews pou 
		this.stop = stop;	//8a trexei to thread
		iterations = iter;	//Synoliko mege8os pinaka
		x = primary;
		y = results;
	}
	
	public void run()
	{
		for (j=start; j<stop; j++)//To sorting toy pinaka gia to mege8os pou dw8ike sto ka8e thread
		{
			my_num = x[j];
			my_place = 0;
			for (i=0; i<iterations; i++)
			{
				if ( my_num > x[i] )
				{
					my_place++;
				}
			}
			y[my_place] = my_num;
		}
	}
	
}
