//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities.Tracking;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Configuration;
using System.Linq;
using System.ServiceModel;
using System.ServiceModel.Activities;
using System.ServiceModel.Activities.Tracking.Configuration;
using System.ServiceModel.Channels;
using System.ServiceModel.Description;
using System.Web.Configuration;

namespace Microsoft.Samples.TextFileTracking
{

    public class TextFileTrackingBehavior : IServiceBehavior
    {
        string profileName;
        string path;
        public TextFileTrackingBehavior(string path, string profileName)
        {
            this.path = path;
            this.profileName = profileName;
        }

        public virtual void ApplyDispatchBehavior(ServiceDescription serviceDescription, ServiceHostBase serviceHostBase)
        {
             WorkflowServiceHost workflowServiceHost = serviceHostBase as WorkflowServiceHost;
             if (null != workflowServiceHost)
             {
                 string workflowDisplayName = workflowServiceHost.Activity.DisplayName;
                 TrackingProfile trackingProfile = GetProfile(this.profileName, workflowDisplayName);

                 TextFileTrackingParticipant participant = new TextFileTrackingParticipant()
                 {
                     TrackingProfile = trackingProfile,
                     Path = path
                 };

                 workflowServiceHost.WorkflowExtensions.Add(participant);
             }
        }

        TrackingProfile GetProfile(string profileName, string displayName)
        {
            TrackingProfile trackingProfile = null;
            TrackingSection trackingSection = (TrackingSection)WebConfigurationManager.GetSection("system.serviceModel/tracking");
            if (trackingSection == null)
            { 
                return null; 
            }

            //Find the profile with the specified profile name in the list of profile found in config
            var match = from p in new List<TrackingProfile>(trackingSection.TrackingProfiles)
                        where p.Name == profileName
                        select p;

            if (match.Count() == 0)
            {
                throw new ConfigurationErrorsException(string.Format("Could not find a profile with name '{0}'", profileName));
            }
            else
            {
                trackingProfile = match.First();
            }

            //No matching profile with the specified name was found
            if (trackingProfile == null)
            {
                //return an empty profile
                trackingProfile = new TrackingProfile()
                {
                    ActivityDefinitionId = displayName
                };
            }

            return trackingProfile;
        }     

        public virtual void AddBindingParameters(ServiceDescription serviceDescription, ServiceHostBase serviceHostBase, Collection<ServiceEndpoint> endpoints, BindingParameterCollection bindingParameters) { }
        public virtual void Validate(ServiceDescription serviceDescription, ServiceHostBase serviceHostBase) { }
    }
}
