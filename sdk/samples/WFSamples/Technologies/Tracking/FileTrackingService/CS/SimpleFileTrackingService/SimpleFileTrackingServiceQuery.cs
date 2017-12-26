//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.IO;
using System.Collections.ObjectModel;

namespace Microsoft.Samples.Workflow.FileTrackingServiceAndQuerySample
{
    public static class QueryLayer
    {
    // Represents the query API layer that reads tracked Workflow Events from tracking file
        public static Collection<String> GetTrackedWorkflowEvents(String trackingFile)
        {
            // Creates a collection of workflow events
            Collection<String> workflowEvents = new Collection<String>();
            // Opens the tracking file
            FileStream fileStream = File.Open(trackingFile, FileMode.Open, FileAccess.Read);
            if (fileStream != null)
            {
                StreamReader reader = new StreamReader(fileStream);
                string line;
                // read from the tracking file 
                while (!reader.EndOfStream)
                {
                    line = reader.ReadLine();
                    if (line.Contains("Workflow:"))
                    {
                        int nSeparatorIndex = line.IndexOf(":", StringComparison.OrdinalIgnoreCase);
                        if (0 <= nSeparatorIndex)
                        {
                            // add workflow events found to the collection of workflow events
                            workflowEvents.Add(line.Substring(nSeparatorIndex + 1).Trim());
                        }
                    }
                }
                reader.Close();
                fileStream.Close();
            }
            // return workflow events collection
            return workflowEvents;
        }
    }
}