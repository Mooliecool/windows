
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
using System;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Channels;
using System.Collections;
using System.Collections.Generic;
using System.Configuration;
using System.Security;
using System.IdentityModel.Claims; 
using System.IdentityModel.Policy; 
using System.IdentityModel.Tokens; 
using System.IdentityModel.Selectors;
using System.Security.Cryptography.X509Certificates;
using System.ServiceModel;
using System.ServiceModel.Security;
using System.ServiceModel.Security.Tokens;

// M:N broadcast application that enables senders to send announcements to multiple receivers
// using Peer Channel (a multi-party channel). The sender is implemented by a different program.

// If you are unfamiliar with new concepts used in this sample, refer to the WCF Basic\GettingStarted sample.

namespace Microsoft.ServiceModel.Samples
{
    // Define a service contract.
    [ServiceContract(Namespace="http://Microsoft.ServiceModel.Samples")]
    public interface IBroadcast
    {
        [OperationContract(IsOneWay = true)]
        void Announce(string msg);
    }

    [ServiceContract(Namespace="http://Microsoft.ServiceModel.Samples.PeerChannel")]
    public interface IQuoteChange
    {
        [OperationContract(IsOneWay = true)]
        void PriceChange(string item, double change, double price);
    }

    class PublisherValidator : X509CertificateValidator
    {
        string senderThumbprint;
        public PublisherValidator(X509Certificate2 sender)
        {
            if (sender == null)
                throw new ArgumentException("sender");
            this.senderThumbprint = sender.Thumbprint;
        }
        public override void Validate(X509Certificate2 certificate)
        {
            if (0 != String.CompareOrdinal(certificate.Thumbprint, senderThumbprint))
                throw new SecurityTokenValidationException(String.Format("Certificate '{0}' failed validation", certificate));
        }
    }

    [ServiceBehavior(InstanceContextMode = InstanceContextMode.Single)]
    public class BroadcastReceiver : IQuoteChange
    {
        // PeerNode event handlers
        static void OnOnline(object sender, EventArgs e)
        {
            Console.WriteLine("** Online");
        }

        static void OnOffline(object sender, EventArgs e)
        {
            Console.WriteLine("** Offline");
        }

        Claim FindClaim(ServiceSecurityContext context)
        {
            Claim result = null;
            foreach (ClaimSet set in context.AuthorizationContext.ClaimSets)
            {
                IEnumerator<Claim> claims = set.FindClaims(ClaimTypes.Name, null).GetEnumerator();
                if (claims.MoveNext())
                {
                    result = claims.Current;
                    break;
                }
            }
            return result;
        }

        public void PriceChange(string item, double change, double price)
        {
            ServiceSecurityContext context = ServiceSecurityContext.Current;
            Claim claim = FindClaim(context);
            string source = "unknown";
            if (claim != null)
            {
                source = claim.Resource as string;
            }            
            Console.WriteLine("{0}=>(item: {1}, change: {2}, price: {3})", source,item, change.ToString("C"), price.ToString("C"));
        }                                                           

        // Host the receiver within this EXE console application.
        public static void Main()
        {
            string recognizedPublisherName = ConfigurationManager.AppSettings["publisherQName"];

            ServiceHost receiver = new ServiceHost(new BroadcastReceiver());

            //this settings specifies that only messages signed with above cert should be accepted.
            NetPeerTcpBinding binding = new NetPeerTcpBinding("Binding1");
            
            // set peer credentials
            X509Certificate2 certificate = GetCertificate(StoreName.TrustedPeople, StoreLocation.CurrentUser, recognizedPublisherName, X509FindType.FindBySubjectDistinguishedName);
            receiver.Credentials.Peer.MessageSenderAuthentication.CertificateValidationMode = X509CertificateValidationMode.Custom;
            receiver.Credentials.Peer.MessageSenderAuthentication.CustomCertificateValidator = new PublisherValidator(certificate);

            // Open the ServiceHostBase to create listeners and start listening for messages.
            receiver.Open();

            // Retrieve the PeerNode associated with the receiver and register for online/offline events
            // PeerNode represents a node in the mesh. Mesh is the named collection of connected nodes.

            // use the first base address.
            string baseAddress = receiver.BaseAddresses[0].ToString();

            EndpointAddress lookFor = new EndpointAddress(baseAddress + "announcements");
            for (int i=0; i<receiver.ChannelDispatchers.Count; ++i)
            {
                ChannelDispatcher channelDispatcher = receiver.ChannelDispatchers[i] as ChannelDispatcher;
                if (channelDispatcher != null)
                {
                    for (int j=0; j<channelDispatcher.Endpoints.Count; ++j)
                    {
                        EndpointDispatcher endpointDispatcher = channelDispatcher.Endpoints[j];
                        if (endpointDispatcher.EndpointAddress == lookFor)
                        {
                            IOnlineStatus ostat = (IOnlineStatus)channelDispatcher.Listener.GetProperty<IOnlineStatus>();
                            if (ostat != null)
                            {
                                ostat.Online += OnOnline;
                                ostat.Offline += OnOffline;
                                if (ostat.IsOnline)
                                {
                                    Console.WriteLine("**  Online");
                                }
                            }
                        }
                    }
                }
            }
            
            // The receiver can now receive broadcast announcements
            Console.WriteLine("**  The receiver is ready");
            Console.WriteLine("Press <ENTER> to terminate receiver.");
            Console.ReadLine();

            // Close the ServiceHostBase to shutdown the receiver
            receiver.Close();
        }

        static internal X509Certificate2 GetCertificate(StoreName storeName, StoreLocation storeLocation, string key, X509FindType findType)
        {
            X509Certificate2 result;

            X509Store store = new X509Store(storeName, storeLocation);
            store.Open(OpenFlags.ReadOnly);
            try
            {
                X509Certificate2Collection matches;
                matches = store.Certificates.Find(findType, key, false);
                if (matches.Count > 1)
                    throw new InvalidOperationException(String.Format("More than one certificate with key '{0}' found in the store.", key));
                if (matches.Count == 0)
                    throw new InvalidOperationException(String.Format("No certificates with key '{0}' found in the store.", key));
                result = matches[0];
            }
            finally
            {
                store.Close();
            }
            return result;
        }
    }
}

