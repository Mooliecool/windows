
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Configuration;
using System.ServiceModel;
using System.IdentityModel.Claims;
using System.IdentityModel.Policy; 
using System.IdentityModel.Selectors;
using System.IdentityModel.Tokens;
using System.Security.Cryptography.X509Certificates;
using System.ServiceModel.Channels;
using System.ServiceModel.Security;


// M:N broadcast application that enables senders to send announcements to multiple receivers
// using Peer Channel (a multi-party channel). The receiver is implemented by a different program.

// If you are unfamiliar with new concepts used in this sample, refer to the WCF Basic\GettingStarted sample.

namespace Microsoft.ServiceModel.Samples
{
    // Service contract for Broadcast. It's manually generated since Peer Transport currently does not
    // support WSDL generation.
    // Applying [PeerBehavior] attribute on the service contract enables retrieval of PeerNode from IClientChannel.
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples.PeerChannel")]
    public interface IQuoteChange
    {
        [OperationContract(IsOneWay = true)]
        void PriceChange(string item, double change, double price);
    }

    public interface IQuoteChannel : IQuoteChange, IClientChannel
    {
    }
    
    class SenderValidator : X509CertificateValidator
    {
        string senderThumbprint;
        public SenderValidator(X509Certificate2 sender)
        {
            if (sender == null)
                throw new ArgumentException("sender");
            this.senderThumbprint = sender.Thumbprint;
        }
        public override void Validate(X509Certificate2 certificate)
        {
            if (0 != String.CompareOrdinal(certificate.Thumbprint, senderThumbprint))
                throw new SecurityTokenValidationException("Unrecognized sender");

        }
    }

    // Sender implementation code.
    // Host the sender within this EXE console application.
    public static class BroadcastSender
    {
        public static void Main()
        {
            // Get the sender ID from configuration
            string senderId = ConfigurationManager.AppSettings["sender"];
            string recognizedSender = "CN="+senderId;

            // Create the sender with the given endpoint configuration
            // Sender is an instance of the sender side of the broadcast application that has opened a channel to mesh
            using (ChannelFactory<IQuoteChannel> cf = new ChannelFactory<IQuoteChannel>("BroadcastEndpoint"))
            {
                X509Certificate2 senderCredentials = GetCertificate(StoreName.My, StoreLocation.CurrentUser, recognizedSender, X509FindType.FindBySubjectDistinguishedName);
                cf.Credentials.Peer.Certificate = senderCredentials;
                cf.Credentials.Peer.MessageSenderAuthentication.CertificateValidationMode = X509CertificateValidationMode.Custom;
                cf.Credentials.Peer.MessageSenderAuthentication.CustomCertificateValidator = new SenderValidator(senderCredentials);

                using (IQuoteChannel sender = (IQuoteChannel)cf.CreateChannel())
                {
                    // Retrieve the PeerNode associated with the sender and register for online/offline events
                    // PeerNode represents a node in the mesh. Mesh is the named collection of connected nodes.
                    IOnlineStatus ostat = sender.GetProperty<IOnlineStatus>();
                    ostat.Online += new EventHandler(OnOnline);
                    ostat.Offline += new EventHandler(OnOffline);

                    // Open the sender 
                    sender.Open();
                    Console.WriteLine("** {0} is ready", senderId);

                    //Info that sender sends out to the receivers (mesh))
                    string name = "InfoSolo";
                    double currentValue = Double.Parse("100");
                    double change = Double.Parse("45");

                    Console.WriteLine("Enter stock value and pass <ENTER> to broadcast update to receivers.");
                    Console.WriteLine("Enter a blank value to terminate application");
                    
                    while (true)
                    {
                        Console.WriteLine("New Value: ");
                        double newvalue = 0;
                        string value = Console.ReadLine();
                        
                        if (String.IsNullOrEmpty(value))
                        {
                            break;
                        }
                        
                        if (!Double.TryParse(value, out newvalue))
                        {
                            Console.WriteLine("Invalid value entered.  Please enter a numeric value.");
                            continue;
                        }
                        
                        change = newvalue - currentValue;
                        currentValue = newvalue;
                        sender.PriceChange(name, change, currentValue);

                        Console.WriteLine("Updated value sent.");
                    }
                }
            }
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
