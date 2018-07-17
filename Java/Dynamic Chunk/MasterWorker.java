
public class MasterWorker {

	private static int totalTasks; // ������� ��������
	private static int nThreads; // ������� �������
	private static int tasksAssigned = -10; // �������������� ��������� ������� ��������
	private static int chunk; // mege8os chunk

	public static void main(String[] args)
	{
		totalTasks = 201; 
		nThreads = 10; 
		chunk = 10;
		
		// ���������� ��� ������������ ������
		double[] a = new double[totalTasks]; 
		for(int i=0; i<totalTasks; i++)
		{
			a[i] = i + 3;
			System.out.println(a[i]);   
		} 

		// ���������� ������� �����������
		Thread[] threads = new Thread[nThreads];
		for (int i = 0; i < threads.length; ++i)
		{
			threads[i] = new Thread(new Worker(a,i));
		}		

		// �������� ������� �����������
		for (int i = 0; i < threads.length; ++i)
		{
			threads[i].start();
		}

		// ������
		for (int i = 0; i < threads.length; ++i)
		{
			try {
				threads[i].join();
			}
			catch (InterruptedException e) {
				System.err.println("this should not happen");
			}
		}
		
		// �������� ��������� ������
		for(int i = 0; i < totalTasks; i++)
			System.out.println(a[i]);
	}

	//  ������� ������� ��� ��� ������� ��������
	private static synchronized int getTask()
	{
		// �������� ��� ������� (� �������� ������) �� �������� 
		if ((tasksAssigned += chunk) < totalTasks) //to tasksAssigned anebainei kata chunk
			return tasksAssigned;
		else
			return -1;
	}

	// ������� ��� ������� �� ���� ���� - �����������
	private static class Worker implements Runnable
	{

		private int myID,i;
		private double[] table;

		// �������������
		public Worker(double[] array, int myID)
		{
			this.myID = myID;
			table = array;
		}

		public void run()
		{
			int element;
			// �������� ������� (� �������� ������) �� �������
			while ((element = getTask()) >= 0)
			{
				i = element;
				while (i < (element+chunk) && i < totalTasks){ //Elegxos wste to i na mhn yperbainei oute to ka8e chunk oute to totalTasks se periptwsh pou den ginetai teleia diairesh me totalTasks kai chunk
					System.out.println("worker " + myID + " received " + i);
					// ���������� ��� ����������� ���� ��� ��������� ��� �����
					table[i]= Math.sqrt(table[i]);   
					i++;
				}
			}
			System.out.println("worker " + myID + " done");
		}
	}
}

