/****************************** Module Header ******************************\
* Project Name:   CSAzureWebRoleBackendProcessing
* Module Name:    Common
* File Name:      BackendProcessor.cs
* Copyright (c) Microsoft Corporation
*
* This class runs a backend task in a new thread.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using System.Diagnostics;
using System.Threading;
using Microsoft.WindowsAzure.StorageClient;

namespace CSAzureWebRoleBackendProcessing.Common
{
    public class BackendProcessor
    {
        /// <summary>
        /// Start the running in backend.
        /// </summary>
        public void Start()
        {
            new Thread(Run).Start();

            Trace.TraceInformation("BackendProcessor is started.");
        }

        /// <summary>
        /// This method serves as the main thread of backend processing.
        /// The Run method should implement a long-running thread that carries
        //  out operations
        /// </summary>
        public void Run()
        {
            while (true)
            {
                try
                {
                    DataSources ds = new DataSources();

                    // Retrieve a new message from the queue.
                    CloudQueueMessage msg = ds.WordQuque.GetMessage(TimeSpan.FromSeconds(1));
                    
                    if (msg != null)
                    {
                        // Process the task.
                        var messageParts = msg.AsString.Split(new char[] { ',' });
                        WordEntry entry = ds.GetWordEntry(messageParts[0], messageParts[1]);

                        entry.Content = entry.Content.ToUpper();
                        entry.IsProcessed = true;
                        ds.UpdateWordEntry(entry);

                        // Delete message only when the task is completed.
                        ds.WordQuque.DeleteMessage(msg);
                        Trace.TraceInformation("Process result \"{0}\".", entry.Content);
                    }
                    else
                    {
                        Thread.Sleep(1000);
                    }
                }
                catch (Exception ex)
                {
                    Trace.TraceError("Exception when processing queue item. Message: '{0}'", ex.Message);
                    System.Threading.Thread.Sleep(5000);
                }
            }
        }
    }
}