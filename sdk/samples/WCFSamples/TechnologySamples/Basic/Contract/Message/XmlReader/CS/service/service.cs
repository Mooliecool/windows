
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Globalization;
using System.ServiceModel.Channels;
using System.IO;
using System.ServiceModel;
using System.Xml;

namespace Microsoft.ServiceModel.Samples
{
    // Define a service contract.
    [ServiceContract(Namespace="http://Microsoft.ServiceModel.Samples")]
    public interface ICalculator
    {
        [OperationContract]
        double Add(double n1, double n2);
        [OperationContract]
        double Subtract(double n1, double n2);
        [OperationContract]
        double Multiply(double n1, double n2);
        [OperationContract]
        double Divide(double n1, double n2);
		[OperationContract]
		Message Sum(Message request);
    }

    // Service class which implements the service contract.
    public class CalculatorService : ICalculator
    {
        public double Add(double n1, double n2)
        {
            return n1 + n2;
        }

        public double Subtract(double n1, double n2)
        {
            return n1 - n2;
        }

        public double Multiply(double n1, double n2)
        {
            return n1 * n2;
        }

        public double Divide(double n1, double n2)
        {
            return n1 / n2;
        }


        //The Sum operation operates on the WCF Message object directly
        public Message Sum(Message request)
		{
			int sum = 0;
			string text = "";

            //The body of the message contains a list of numbers which will be read directly using an XmlReader
            XmlReader body = request.GetReaderAtBodyContents();

			while (body.Read())
			{
				text = body.ReadString().Trim();
				if (text.Length > 0)
				{
					sum += Convert.ToInt32(text,CultureInfo.InvariantCulture);
				}
			}
			body.Close();

            Message response = Message.CreateMessage(request.Version, "http://Microsoft.ServiceModel.Samples/ICalculator/SumResponse", sum);
			return response;
		
		}
        
    }

}
