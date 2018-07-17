
public class ThreadParGroupSqrt {

	public static void main(String[] args)
	{
		// ���������� ��� ������������ ������
		int iterations = 1000;
		double[] a = new double[iterations];
		int numofthreads = 10;
		int chunk = 10;
		
		for(int i = 0; i < iterations; i++)
			a[i] = i; 

		// ���������� ������ �������
		SqrtGroupThread threads[] = new SqrtGroupThread[numofthreads];
		
		// ���������� ��� �������� �������
		for(int i = 0, j = 0; i < numofthreads; i++,j += chunk) 
		{
			threads[i] = new SqrtGroupThread(a,j,(i*chunk)+chunk,iterations,chunk,numofthreads);
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
		for(int i = 0; i < 1000; i++) 
		{
			System.out.println(a[i]);
		}
	}
	
}
