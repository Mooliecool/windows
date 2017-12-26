//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Runtime.Serialization;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.Workflow.Activities;
using System.Workflow.ComponentModel;

namespace Microsoft.WorkflowServices.Samples
{
    // Static methods for managing context.
	public static class ContextManager
	{
        static readonly string contextHeaderName = "Context";
        static readonly string contextHeaderNamespace = "http://schemas.microsoft.com/ws/2006/05/context";

        // Apply Context to a SendActivity
        static public void ApplyContext(SendActivity activity, IDictionary<string, string> context)
        {
            if (activity.ExecutionStatus == ActivityExecutionStatus.Initialized)
            {
                activity.Context = context;
            }
        }

        // Apply EndpointAddress to a SendActivity
        static public void ApplyEndpointAddress(SendActivity activity, EndpointAddress epr)
        {
            if (activity.ExecutionStatus == ActivityExecutionStatus.Initialized)
            {
                if (epr.Uri != null)
                {
                    activity.CustomAddress = epr.Uri.ToString();
                }
                if (epr.Headers != null && epr.Headers.Count > 0)
                {
                    AddressHeader contextHeader = epr.Headers.FindHeader(contextHeaderName, contextHeaderNamespace);
                    IDictionary<string, string> context = contextHeader.GetValue<Dictionary<string, string>>();                    
                    activity.Context = context;
                }
            }
        }

        // Create EndpointAddress from Uri and Context
        static public EndpointAddress CreateEndpointAddress(string uri, IDictionary<string, string> context)
        {
            EndpointAddress epr = null;
            if (context != null && context.Count > 0)
            {
                AddressHeader contextHeader = AddressHeader.CreateAddressHeader(contextHeaderName, contextHeaderNamespace, new Dictionary<string, string>(context));
                epr = new EndpointAddress(new Uri(uri), contextHeader);
            }
            else
            {
                epr = new EndpointAddress(uri);
            }
            return epr;
        }

        // Create EndpointAddress from Uri and ReceiveActivity
        static public EndpointAddress CreateEndpointAddress(string uri, ReceiveActivity receiveActivity)
        {
            return CreateEndpointAddress(uri, receiveActivity.Context);
        }

        // Apply Context to an IClientChannel
        static public bool ApplyContextToChannel(IDictionary<string, string> context, IClientChannel channel)
        {
            if (context != null)
            {
                IContextManager cm = channel.GetProperty<IContextManager>();
                if ((cm != null) && (cm.Enabled))
                {   // apply context to ContextChannel
                    cm.SetContext(context);
                    return true;
                }
            }
            return false;
        }

        // Extract context from an IClientChannel
        const string WscContextKey = "WscContext";
        static public IDictionary<string, string> ExtractContextFromChannel(IClientChannel channel)
        {   // extract context from channel
            IContextManager cm = channel.GetProperty<IContextManager>();
            if ((cm != null) && (cm.Enabled))
            {   // attempt to extract context from channel
                return cm.GetContext();
            }
            return null;
        }

        // Deserialize context from file
        static public IDictionary<string, string> DepersistContext(string fileName)
        {   // retrieve context from file
            if (File.Exists(fileName))
            {
                DataContractSerializer serializer = new DataContractSerializer(typeof(Dictionary<string, string>));
                bool opDone = true;
                while (opDone)
                {
                    try
                    {
                        FileStream stream = new FileStream(fileName, FileMode.Open);
                        return (Dictionary<string, string>)serializer.ReadObject(stream);
                    }
                    catch (IOException)
                    {
                    }
                    finally
                    {
                        opDone = false;
                    }
                }
            }
            return null;
        }

        // Serialize context into file
        static public void PersistContext(IDictionary<string, string> context, string fileName)
        {   // persist context to file
            if (context != null)
            {
                DataContractSerializer serializer = new DataContractSerializer(typeof(Dictionary<string, string>));
                bool opDone = true;
                while (opDone)
                {
                    try
                    {
                        FileStream stream = new FileStream(fileName, FileMode.Create);
                        serializer.WriteObject(stream, context);
                    }
                    catch (IOException)
                    {
                    }
                    finally
                    {
                        opDone = false;
                    }
                }
                
            }
        }

        // Delete context file
        static public void DeleteContext(string fileName)
        {
            bool opDone = true;
            while (opDone)
            {
                try
                {
                    if (File.Exists(fileName))
                        File.Delete(fileName);
                }
                catch (IOException)
                {

                }
                finally
                {
                    opDone = false;
                }
            }

        }

    }
}
