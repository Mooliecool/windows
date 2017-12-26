//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Globalization;
using System.IO;
using System.ServiceModel;
using System.Text;
using System.Threading;

namespace Microsoft.ServiceModel.Samples
{
    // Define a service contract.
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples")]
    public interface ICalculator
    {
        [OperationContract]
        double Add(double n1, double n2);

        [OperationContract]
        double Subtract(double n1, double n2);

        //Multiply involves some file I/O so we'll make it Async.
        [OperationContract(AsyncPattern = true)]
        IAsyncResult BeginMultiply(double n1, double n2, AsyncCallback callback, object state);
        double EndMultiply(IAsyncResult ar);

        //Divide involves some file I/O so we'll make it Async.
        [OperationContract(AsyncPattern = true)]
        IAsyncResult BeginDivide(double n1, double n2, AsyncCallback callback, object state);
        double EndDivide(IAsyncResult ar);
    }

    // Service class which implements the service contract.
    [ServiceBehavior(InstanceContextMode=InstanceContextMode.PerCall, ConcurrencyMode=ConcurrencyMode.Multiple)]
    public class CalculatorService : ICalculator
    {
        public double Add(double n1, double n2)
        {
            Console.WriteLine("Received Add Synchronously on ThreadID {0}:  Sleeping for 3 seconds", Thread.CurrentThread.ManagedThreadId);
            Thread.Sleep(3000);
            Console.WriteLine("Returning Add Result on ThreadID {0}", Thread.CurrentThread.ManagedThreadId);
            return n1 + n2;
        }

        public double Subtract(double n1, double n2)
        {
            Console.WriteLine("Received Subtract Synchronously on ThreadID {0}:  Sleeping for 3 seconds", Thread.CurrentThread.ManagedThreadId);
            Thread.Sleep(3000);
            Console.WriteLine("Returning Subtract Result on ThreadID {0}", Thread.CurrentThread.ManagedThreadId); 
            return n1 - n2;
        }

        public IAsyncResult BeginMultiply(double n1, double n2, AsyncCallback callback, object state)
        {
            Console.WriteLine("Asynchronous call: BeginMultiply on ThreadID {0}", Thread.CurrentThread.ManagedThreadId); 
            //return an AsyncResult
            return new MathAsyncResult(new MathExpression(n1, n2, "*"), callback, state);
        }

        public double EndMultiply(IAsyncResult ar)
        {
            Console.WriteLine("EndMultiply called on ThreadID {0}", Thread.CurrentThread.ManagedThreadId); 
            //use the AsyncResult to complete that async operation
            return MathAsyncResult.End(ar);
        }

        public IAsyncResult BeginDivide(double n1, double n2, AsyncCallback callback, object state)
        {
            Console.WriteLine("Asynchronous call: BeginDivide on ThreadID {0}", Thread.CurrentThread.ManagedThreadId); 
            //return an AsyncResult
            return new MathAsyncResult(new MathExpression(n1, n2, "/"), callback, state);
        }

        public double EndDivide(IAsyncResult ar)
        {
            Console.WriteLine("EndDivide called on ThreadID {0}", Thread.CurrentThread.ManagedThreadId); 
            //use the AsyncResult to complete that async operation
            return MathAsyncResult.End(ar);
        }
    
        // Host the service within this EXE console application.
        public static void Main()
        {
            // Create a ServiceHost for the CalculatorService type.
            using (ServiceHost serviceHost = new ServiceHost(typeof(CalculatorService)))
            {
                // Open the ServiceHost to create listeners and start listening for messages.
                serviceHost.Open();

                // The service can now be accessed.
                Console.WriteLine("The service is ready.");
                Console.WriteLine("Press <ENTER> to terminate service.");
                Console.WriteLine();
                Console.ReadLine();
            }
        }
    }

    public class MathExpression
    {
        private double n1;
        private double n2;
        private string operation;

        public MathExpression(double n1, double n2, string operation)
        {
            this.n1 = n1;
            this.n2 = n2;
            this.operation = operation;
        }

        public double N1
        {
            get { return n1; }
        }

        public double N2
        {
            get { return n2; }
        }

        public string Operation
        {
            get { return operation; }
        }

        public double Result
        {
            get
            {
                switch (Operation)
                {
                    case "+":
                        return N1 + N2;
                    case "-":
                        return N1 - N2;
                    case "*":
                        return N1 * N2;
                    case "/":
                        return N1 / N2;
                    default:
                        throw new InvalidOperationException("could not handle " + Operation + " operation.");
                }
            }
        }

        public byte[] ToBytes()
        {
            return Encoding.Unicode.GetBytes(string.Format(CultureInfo.InvariantCulture, "{0} {1} {2} = {3}", N1, Operation, N2, Result));
        }
    }

    /// <summary>
    /// Implementation of async Math invocation 
    /// </summary>
    class MathAsyncResult : Microsoft.ServiceModel.Samples.TypedAsyncResult<double>
    {
        FileStream fs;
        MathExpression expr;

        public MathAsyncResult(MathExpression mathExpression, AsyncCallback callback, object state)
            : base(callback, state)
        {
            expr = mathExpression;
            //Turn the expression into an array of bytes
            byte[] bytes = expr.ToBytes();

            //open a file for writing
            fs = File.OpenWrite(Path.GetRandomFileName() + ".txt");

            //begin writing asynchronously
            IAsyncResult result = fs.BeginWrite(bytes, 0, bytes.Length, new AsyncCallback(OnWrite), this);

            //if the write did not complete synchronously, we are done setting up this AsyncResult
            if (!result.CompletedSynchronously)
                return;

            //If the write did complete synchronously, then we'll complete the AsyncResult
            CompleteWrite(result, true);
        }

        //Completes asynchronous work.
        //cleans up any resources managed by this AsyncResult
        //Signals the base class that all work is finished
        void CompleteWrite(IAsyncResult result, bool synchronous)
        {
            try
            {
                //Complete the asynchronous file write
                fs.EndWrite(result);
            }
            finally
            {
                //Clean up the file resources
                fs.Close();
            }

            //Calling Complete on the base AsyncResult signals the WaitHandle
            //And makes the callback if necessary
            base.Complete(expr.Result, synchronous);
        }

        void OnWrite(IAsyncResult result)
        {
            //if we returned synchronously, then CompletWrite will be called directly
            if (result.CompletedSynchronously)
                return;

            Console.WriteLine("IO thread for {0} operation on ThreadID {1}", expr.Operation, Thread.CurrentThread.ManagedThreadId);

            try
            {
                //Call CompleteWrite to cleanup resources and complete the AsyncResult
                CompleteWrite(result, false);
            }
            catch (Exception e)
            {
                //if something bad happend, then call the exception overload
                //on the base class.  This will serve up the exception on the
                //AsyncResult
                base.Complete(false, e);
            }
        }
    }
}
