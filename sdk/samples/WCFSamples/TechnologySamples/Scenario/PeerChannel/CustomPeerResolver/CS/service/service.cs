
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Configuration;
using System.ServiceModel;


// Custom peer resolver service implementation. Implements registration, unregistration, and resolution 
// of mesh ID and associated addresses. Mesh ID identifies the mesh (a named collection of nodes identified 
// by the mesh ID). An example of mesh ID is chatMesh and identifies the host name portion of an EndpointAddresss,
// net.p2p://chatMesh/servicemodesamples/chat"
// Mesh IDs are expected to be unique and if multiple applications use the same custom peer resolver service, 
// they should choose different mesh IDs to avoid conflict.

// If you are unfamiliar with new concepts used in this sample, refer to the WCF Basic\GettingStarted sample.

namespace Microsoft.ServiceModel.Samples
{
    // Define a service contract.
    [ServiceContract(Namespace="http://Microsoft.ServiceModel.Samples")]
    public interface ICustomPeerResolver
    {
        [OperationContract]
        int Register(string meshId, PeerNodeAddress nodeAddresses);
        [OperationContract]
        void Unregister(int registrationId);
        [OperationContract]
        void Update(int registrationId, PeerNodeAddress updatedNodeAddress);
        [OperationContract]
        PeerNodeAddress[] Resolve(string meshId, int maxAddresses);
    }

    // Service class which implements the service contract
    // It is a singleton service
    [ServiceBehavior(InstanceContextMode = InstanceContextMode.Single)]
    public class CustomPeerResolverService : ICustomPeerResolver
    {
        // The resolver maintains two dictionaries:
        // a) A registration table that is keyed by the registration Id (int). 
        // b) A mesh Id table that is keyed by the mesh ID and has a dictionary of registrationId, PeerNodeAddress 
        //      in the value part. The value part is a dictionary rather than a list to facilitate faster lookups.
        // A more sophisticated cache could be used if the number of meshIds is large or if the number of nodes 
        // in the meshes is large.

        // Registration class. Each time Register() is called, a Registration object is created and added to
        // Registration table.
        class Registration
        {
            string meshId;
            PeerNodeAddress nodeAddress;

            public Registration(string meshId, PeerNodeAddress nodeAddress)
            {
                this.meshId = meshId;
                this.nodeAddress = nodeAddress;
            }

            public string MeshId 
            { 
                get { return this.meshId; } 
            }
            public PeerNodeAddress NodeAddress 
            { 
                get { return this.nodeAddress; }
                set { this.nodeAddress = value; }
            }
        }

        static Dictionary<int, Registration> registrationTable = new Dictionary<int, Registration>();
        static Dictionary<string, Dictionary<int, PeerNodeAddress>> meshIdTable = new Dictionary<string, Dictionary<int, PeerNodeAddress>>();
        static int nextRegistrationId = 0;

        readonly Random random = new Random();

        // Register a node address for a given mesh ID
        public int Register(string meshId, PeerNodeAddress nodeAddress)
        {
            bool newMeshId = false;
            int registrationId;
            Registration registration = new Registration(meshId, nodeAddress);

            // Add the new registration to the registration table; update meshIdTable with the newly registered nodeAddress
            lock (registrationTable)
            {
                registrationId = nextRegistrationId++;
                lock (meshIdTable)
                {
                    // Update the meshId table
                    Dictionary<int, PeerNodeAddress> addresses;
                    if (!meshIdTable.TryGetValue(meshId, out addresses))
                    {
                        // MeshID doesn't exist and needs to be added to meshIdTable
                        newMeshId = true;
                        addresses = new Dictionary<int, PeerNodeAddress>();
                        meshIdTable[meshId] = addresses;
                    }
                    addresses[registrationId] = nodeAddress;

                    // Add an entry to the registration table
                    registrationTable[registrationId] = new Registration(meshId, nodeAddress);
                }
            }
            if (newMeshId)
                Console.WriteLine("Registered new meshId {0}", meshId);
            return registrationId;
        }

        // Unregister addresses for a given mesh ID
        public void Unregister(int registrationId)
        {
            bool unregisteredMeshId = false;

            Registration registration;
            lock (registrationTable)
            {
                // We expect the registration to exist. Find and remove it from registrationTable.
                registration = registrationTable[registrationId];
                registrationTable.Remove(registrationId);

                // Remove the registration from the meshIdTable
                lock (meshIdTable)
                {
                    Dictionary<int, PeerNodeAddress> addresses = meshIdTable[registration.MeshId];
                    addresses.Remove(registrationId);

                    // If this was the only node registered for the mesh, remove the meshId entry
                    if (addresses.Count == 0)
                    {
                        meshIdTable.Remove(registration.MeshId);
                        unregisteredMeshId = true;
                    }
                }
            }
            if (unregisteredMeshId)
                Console.WriteLine("Unregistered meshId {0}", registration.MeshId);
        }

        // Update an existing registration (can be called if the machine IP addresses change etc.)
        public void Update(int registrationId, PeerNodeAddress updatedNodeAddress)
        {
            Registration registration;
            lock (registrationTable)
            {
                // We expect the registration to exist.
                registration = registrationTable[registrationId];

                // Update registrationTable and meshIdTable
                lock (meshIdTable)
                {
                    Dictionary<int, PeerNodeAddress> addresses = meshIdTable[registration.MeshId];
                    addresses[registrationId] = updatedNodeAddress;
                }
                registrationTable[registrationId].NodeAddress = updatedNodeAddress;
            }
        }

        // Resolve addresses for a given mesh ID
        public PeerNodeAddress[] Resolve(string meshId, int maxAddresses)
        {
            if (maxAddresses <= 0)
                throw new ArgumentOutOfRangeException("maxAddresses");

            PeerNodeAddress [] copyOfAddresses;
            lock (meshIdTable)
            {
                Dictionary<int, PeerNodeAddress> addresses;
    
                // Check if the meshId is known and if so, make a copy of the addresses to process outside of lock
                if (meshIdTable.TryGetValue(meshId, out addresses))
                {
                    // you may want to avoid the copy approach in case of a large mesh
                    copyOfAddresses = new PeerNodeAddress[addresses.Count];
                    addresses.Values.CopyTo(copyOfAddresses, 0);
                }
                else
                    copyOfAddresses = new PeerNodeAddress[0];
            }

            // If there are <= maxAddresses for this meshId, return them all
            if (copyOfAddresses.Length <= maxAddresses)
            {
                return copyOfAddresses;
            }
            else
            {
                // Otherwise, pick maxAddresses at random
                List<int> indices = new List<int>(maxAddresses);    // may want to use a more performant data structure if maxAddresses is larger
                while (indices.Count < maxAddresses)
                {
                    int listIndex = this.random.Next() % copyOfAddresses.Length;
                    if (!indices.Contains(listIndex))  // check if we've already seen this index
                        indices.Add(listIndex);
                }
                PeerNodeAddress[] randomAddresses = new PeerNodeAddress[maxAddresses];
                for (int i = 0; i < randomAddresses.Length; i++)
                    randomAddresses[i] = copyOfAddresses[indices[i]];
                return randomAddresses;
            }
        }

        // Host the service within this EXE console application.
        public static void Main()
        {
            // Create a ServiceHost for the CalculatorService type
            using (ServiceHost serviceHost = new ServiceHost(typeof(CustomPeerResolverService)))
            {
                // Open the ServiceHostBase to create listeners and start listening for messages.
                serviceHost.Open();

                // The service can now be accessed.
                Console.WriteLine("The service is ready.");
                Console.WriteLine("Press <ENTER> to terminate service.");
                Console.WriteLine();
                Console.ReadLine();
            }
            Console.WriteLine("The service has shutdown.");
        }
    }
}
