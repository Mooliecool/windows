/****************************** Module Header ******************************\
* Module Name:                WeatherService.cs
* Project:                    NetTcpWCFService
* Copyright (c) Microsoft Corporation.
* 
* Weather Service implementation.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.ServiceModel;
using System.Threading;

namespace NetTcpWCFService
{
    [ServiceBehavior(InstanceContextMode=InstanceContextMode.PerSession)]
    public class WeatherService : IWeatherService
    {
        static WeatherService()
        {
            // Creating a separate thread to generate fake
            // weather report periodically.
            ThreadPool.QueueUserWorkItem(
                new WaitCallback(delegate
                {
                    string[] weatherArray = { "Sunny", "Windy", "Snow", "Rainy" };
                    Random rand = new Random();

                    while (true)
                    {
                        Thread.Sleep(1000);
                        if (WeatherReporting != null)
                        {
                            WeatherReporting(
                                null,
                                new WeatherEventArgs
                                {
                                    WeatherReport =
                                    weatherArray[rand.Next(weatherArray.Length)]
                                });
                        }
                    }
                }));
        }

        static event EventHandler<WeatherEventArgs> WeatherReporting;

        IWeatherServiceCallback _callback;

        public void Subscribe()
        {
            _callback = OperationContext.Current.GetCallbackChannel<IWeatherServiceCallback>();
            WeatherReporting += new EventHandler<WeatherEventArgs>(WeatherService_WeatherReporting);
        }

        public void UnSubscribe()
        {
            WeatherReporting -= new EventHandler<WeatherEventArgs>(WeatherService_WeatherReporting);
        }

        void WeatherService_WeatherReporting(object sender, WeatherEventArgs e)
        {
            // Remember check the callback channel's status before using it.
            if (((ICommunicationObject)_callback).State == CommunicationState.Opened)
            {
                try
                {
                    _callback.WeatherReport(e.WeatherReport);
                }
                catch
                {
                    UnSubscribe();
                }
            }
            else
            {
                UnSubscribe();
            }
        }
    }

    class WeatherEventArgs:EventArgs
    {
        public string WeatherReport{set;get;}
    }
}
