/****************************** Module Header ******************************\
* Project Name:   CSAzureServiceBusProtocolBridging
* Module Name:    Service
* File Name:      MyService.cs
* Copyright (c) Microsoft Corporation
*
* This service contains a method that returns the sum of two numbers.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using System.ServiceModel;
using Common;

namespace Service
{
    [ServiceBehavior(InstanceContextMode = InstanceContextMode.Single,
        ConcurrencyMode = ConcurrencyMode.Multiple)]
    public class MyService : IMyService
    {
        public int Add(int number1, int number2)
        {
            Console.WriteLine("Add({0}, {1}) is called.", number1, number2);
            return number1 + number2;
        }
    }
}
