/****************************** Module Header ******************************\
* Module Name:  CheckinEventService.cs
* Project:	    CSTFSEventListener
* Copyright (c) Microsoft Corporation.
* 
* This class implement the IEventService interface, and it is used to subscribe 
* a TFS Check-in Event. If a user checked in a changeset which met the filters
* of the subscription, TFS will call the Notify method of this WCF service with 
* the parameters. The Notify method display the message if the Changeset has
* policy failures. 
* 
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
using System.IO;
using System.Text;
using System.Xml.Serialization;
using Microsoft.TeamFoundation.VersionControl.Common;

namespace CSTFSEventListener
{
    
    public class CheckinEventService : IEventService
    {

        /// <summary>
        /// This method will send out an email if the Changeset has policy failures. 
        /// </summary>
        /// <param name="eventXml">
        /// This parameter is a Xml format string which is serialized from a CheckinEvent
        /// object.
        /// </param>
        /// <param name="tfsIdentityXml">
        /// The TFS url, like 
        /// "<TeamFoundationServer url="http://server:8080/tfs/VSTS/Services/v3.0/LocationService.asmx" />"
        /// </param>
        public void Notify(string eventXml, string tfsIdentityXml)
        {

            // Initialize a CheckinEvent XmlSerializer.
            XmlSerializer serializer = new XmlSerializer(typeof(CheckinEvent));

            // Deserialize the eventXml to a CheckinEvent object.
            CheckinEvent cievent = null;
            using (var reader = new StringReader(eventXml))
            {
                cievent = serializer.Deserialize(reader) as CheckinEvent;
            }
           

            if (cievent == null)
            {
                return;
            }

            // Check whether the Changeset has policy failures. 
            if (cievent.PolicyFailures.Count > 0)
            {
                StringBuilder message = new StringBuilder();

                message.AppendFormat("ChangeSet{0} Check In Policy Policy Failed. \n",
                    cievent.ContentTitle);


                message.AppendFormat("Committer : {0}\n", cievent.Committer);
                message.AppendFormat("Override Comment : {0}\n", cievent.PolicyOverrideComment);
                message.Append("Check in policy failures:\n");
                foreach (NameValuePair p in cievent.PolicyFailures)
                {
                    message.AppendFormat("\t{0} : {1}", p.Name, p.Value);

                }
                Console.WriteLine(message.ToString());
            }
        }
    }
}
