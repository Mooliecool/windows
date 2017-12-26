using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel;

namespace Microsoft.ServiceModel.Samples
{
  [ServiceContract(Namespace="http://Microsoft.ServiceModel.Samples")]
    interface IEchoAble
    {
        [OperationContract]
        string Echo(string theString);
    }

    class EchoableService : IEchoAble
    {
        public string Echo(string theString)
        {
            Console.WriteLine("WCF service received {0}", theString);

            return theString;
        }

        static void Main(string[] args)
        {
            using (ServiceHost host = new ServiceHost(typeof(EchoableService)))
            {
                host.Open();
                Console.WriteLine("The service is ready.");
                Console.WriteLine("Press <ENTER> to terminate service.");

                Console.ReadLine();
            }
        }
    }

  
}
