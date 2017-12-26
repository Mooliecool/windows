// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==


using System;
using System.Threading;

class Mainy {

public static void DoWork() {
    int k=0;
    while(k<3) {
        Console.WriteLine("{0}: Restarting run {1}",Thread.CurrentThread.Name,k);
        int[] largeArray = new int[1000000];
        for (int i=0;i<=100;i++){
            int[] saveArray = largeArray;
            largeArray = new int[largeArray.Length + 100000];
            saveArray = null;
            //Console.WriteLine("{0} at size {1}",Thread.CurrentThread.Name,largeArray.Length.ToString());
        }
        k++;
   }
}

public static void Main(String[] args) {
    long Threads = 1;

    if(args.Length>1)
    {
        Console.WriteLine("usage: LargeObjectAlloc <number of threads>");
        return;
    }
    else if(args.Length==1)
    {
        Threads = Int64.Parse(args[0]);
    }

    Console.WriteLine("LargeObjectAlloc started with {0} threads. Control-C to exit",Threads.ToString());

    Thread myThread = null;
    for(long i = 0; i<Threads; i++)
    {
        myThread = new Thread(new ThreadStart(Mainy.DoWork));
        myThread.Name = i.ToString();
        myThread.Start();
    }

    Console.WriteLine("All threads started");
	myThread.Join();

    Environment.ExitCode=0;
    Console.WriteLine("Test Passed");
}

}

