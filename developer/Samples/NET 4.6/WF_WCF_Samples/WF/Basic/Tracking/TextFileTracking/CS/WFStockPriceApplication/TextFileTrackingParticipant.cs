//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities.Tracking;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Runtime.Serialization;
using System.Text;
using System.Xml;

namespace Microsoft.Samples.TextFileTracking
{

    public class TextFileTrackingParticipant : TrackingParticipant
    {

        public String Path { get;set;}

        //  The track method is called whenever the workflow runtime emits a tracking record
        protected override void Track(TrackingRecord record, TimeSpan timeout)
        {
            try
            {

                using (FileStream fs = new FileStream(Environment.ExpandEnvironmentVariables(Path), FileMode.Append))
                {
                    XmlTextWriter writer = new XmlTextWriter(fs, ASCIIEncoding.ASCII) { Formatting = Formatting.Indented };
                    DataContractSerializer serializer = new DataContractSerializer(record.GetType());
                    serializer.WriteObject(writer, record);
                    writer.WriteRaw(Environment.NewLine);                    
                    writer.Flush();
                    fs.Flush();
                }
            }
            catch (Exception e)
            {
                Trace.WriteLine(String.Format(CultureInfo.InvariantCulture, "Exception in track method {0}", e.Message));
            }
        }
    }
}
