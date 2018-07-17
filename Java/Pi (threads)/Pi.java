import java.text.DecimalFormat;
import java.util.Scanner;


public class Pi {
	
	public static double sum =0.0; //Dhmiourgia ka8olikhs metablhths
	
	public static void main(String[] args) {
		
		int n,i;
		double PI25DT = 3.141592653589793238462643;
		double pi = 0, h;
		int done =0;
		
		Scanner in = new Scanner(System.in);
		
		n =0 ;
		while(done ==0)
		{
			System.out.println("Enter the number of intervals: (0 quits) ");
			n = in.nextInt(); //Pernoume to input apo ton xrhsth
			
			if(n == 0)
				done = 1;
			else
			{
				Threads thr[] = new Threads[4];	//Dhmiougia pinaka mege8ous 4 gia ta threads		
				
				pi = 0.0;
				h = 1.0 / (double) n;
				sum =0.0;
				for(i=0;i<4;i++)
				{
					thr[i] = new Threads(h,i*(n/4),i*(n/4)+n/4);//Dhmiourgia twn 4 thread
					thr[i].start();
				}
				
				
				for(i=0;i<thr.length;i++)
				{
					try {
						thr[i].join();
					}
					catch (InterruptedException e) {
						System.err.println("Error");
					}
				}
				
				pi += h*sum;
				
			}
			
			
			
			DecimalFormat myFormat = new DecimalFormat("##.################");
			//Ektypwsh apotelesmatwn
			System.out.println("pi is approximately "+ pi + " Error is " + myFormat.format(Math.abs(pi - PI25DT)));
		}
		
	}

	private static class Threads extends Thread
	{
		double x,partial_sum=0,h;
		int i,start,stop;

		public Threads(double h,int start,int stop)//Kataskeyasths toy thread
		{
			this.h = h;
			this.start = start;
			this.stop = stop;
		}

		public void run()
		{
			for(i=start;i<stop;i++){
				x = h * ((double)i - 0.5);
				partial_sum += (4.0 / (1.0 + x*x));
			}
			synchronized(this)//Sto sum grafoun ena ena ta nhmata
			{
				sum = sum + partial_sum;
			}
		}

	}
	
}
