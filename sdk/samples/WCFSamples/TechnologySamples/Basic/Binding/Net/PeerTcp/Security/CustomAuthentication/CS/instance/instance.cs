
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Configuration;
using System.IdentityModel.Selectors;
using System.IdentityModel.Tokens;
using System.Security;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Security;


// Multi-party chat application using Peer Channel (a multi-party channel)
// If you are unfamiliar with new concepts used in this sample, refer to the WCF Basic\GettingStarted sample.

namespace Microsoft.ServiceModel.Samples
{
    // Chat service contract
    // Applying [PeerBehavior] attribute on the service contract enables retrieval of PeerNode from IClientChannel.
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples", CallbackContract = typeof(IChat))]
    public interface IChat
    {
        [OperationContract(IsOneWay = true)]
        void Join(string member);

        [OperationContract(IsOneWay = true)]
        void Chat(string member, string msg);

        [OperationContract(IsOneWay = true)]
        void Leave(string member);
    }

    public interface IChatChannel : IChat, IClientChannel
    {
    }

    class IssuerBasedValidator : X509CertificateValidator
    {
        string issuerThumbprint;
        X509ChainPolicy policy = new X509ChainPolicy();
        public IssuerBasedValidator(X509Certificate2 issuer)
        {
            if (issuer == null)
                throw new ArgumentException("issuer");
            this.issuerThumbprint = issuer.Thumbprint;
            policy.ExtraStore.Add(issuer);
        }
        public override void Validate(X509Certificate2 certificate)
        {
            X509Chain chain = new X509Chain();
            chain.ChainPolicy = policy;
            chain.Build(certificate);
            foreach (X509ChainElement element in chain.ChainElements)
            {
                if (element.Certificate.Thumbprint == issuerThumbprint)
                    return;
            }
            throw new SecurityTokenValidationException(String.Format("The certificate '{0}' failed validation", certificate));
        }
    }

    public class ChatApp : IChat
    {
        // Host the chat instance within this EXE console application.
        public static void Main()
        {
            // Get the memberId from configuration
            string member = ConfigurationManager.AppSettings["member"];
            string issuerName = ConfigurationManager.AppSettings["issuer"];

            // Construct InstanceContext to handle messages on callback interface. 
            // An instance of ChatApp is created and passed to the InstanceContext.
            InstanceContext site = new InstanceContext(new ChatApp());

            // Create the participant with the given endpoint configuration
            // Each participant opens a duplex channel to the mesh
            // participant is an instance of the chat application that has opened a channel to the mesh

            DuplexChannelFactory<IChatChannel> cf = new DuplexChannelFactory<IChatChannel>(site, "ChatEndpoint");

            X509Certificate2 issuer = GetCertificate(StoreName.TrustedPeople, StoreLocation.CurrentUser, "CN=" + issuerName, X509FindType.FindBySubjectDistinguishedName);
            cf.Credentials.Peer.Certificate = GetCertificate(StoreName.My, StoreLocation.CurrentUser, "CN=" + member, X509FindType.FindBySubjectDistinguishedName);
            cf.Credentials.Peer.PeerAuthentication.CertificateValidationMode = X509CertificateValidationMode.Custom;
            cf.Credentials.Peer.PeerAuthentication.CustomCertificateValidator = new IssuerBasedValidator(issuer);

            IChatChannel participant = cf.CreateChannel();

            // Retrieve the PeerNode associated with the participant and register for online/offline events
            // PeerNode represents a node in the mesh. Mesh is the named collection of connected nodes.
            IOnlineStatus ostat = participant.GetProperty<IOnlineStatus>();
            ostat.Online += new EventHandler(OnOnline);
            ostat.Offline += new EventHandler(OnOffline);

            // Print instructions to user
            Console.WriteLine("{0} is ready", member);
            Console.WriteLine("Type chat messages after going Online");
            Console.WriteLine("Press q<ENTER> to terminate the application.");

            // Announce self to other participants
            participant.Join(member);

            while (true)
            {
                string message = Console.ReadLine();
                if (message == "q") break;
                participant.Chat(member, message);
            }

            // Leave the mesh and close the proxy
            participant.Leave(member);

            ((IChannel)participant).Close();

            cf.Close();
        }

        // IChat implementation
        public void Join(string member)
        {
            Console.WriteLine("{0} joined", member);
        }

        public void Chat(string member, string msg)
        {
            Console.WriteLine("[{0}] {1}", member, msg);
        }

        public void Leave(string member)
        {
            Console.WriteLine("[{0} left]", member);
        }

        // PeerNode event handlers
        static void OnOnline(object sender, EventArgs e)
        {
            Console.WriteLine("** Online");
        }

        static void OnOffline(object sender, EventArgs e)
        {
            Console.WriteLine("** Offline");
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
