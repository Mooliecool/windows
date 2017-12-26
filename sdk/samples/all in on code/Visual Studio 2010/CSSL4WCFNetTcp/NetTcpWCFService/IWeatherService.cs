/****************************** Module Header ******************************\
* Module Name:                IWeatherService.cs
* Project:                    NetTcpWCFService
* Copyright (c) Microsoft Corporation.
* 
* Weather Service ServiceContract.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System.ServiceModel;

namespace NetTcpWCFService
{
    [ServiceContract(CallbackContract=typeof(IWeatherServiceCallback))]
    public interface IWeatherService
    {
        [OperationContract(IsOneWay = true)]
        void Subscribe();

        [OperationContract(IsOneWay = true)]
        void UnSubscribe();
    }

    public interface IWeatherServiceCallback
    {
        [OperationContract(IsOneWay=true)]
        void WeatherReport(string report);
    }
}
