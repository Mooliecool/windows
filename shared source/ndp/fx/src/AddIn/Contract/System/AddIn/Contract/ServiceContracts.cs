//------------------------------------------------------------------------------
/// <copyright from='2004' to='2005' company='Microsoft Corporation'>
///    Copyright (c) Microsoft Corporation. All Rights Reserved.
///    Information Contained Herein is Proprietary and Confidential.
/// </copyright>
//------------------------------------------------------------------------------

using System;

namespace System.AddIn.Contract
{

    //the two Contracts defined here allow for clients to query for addtional Contracts on
    //different objects, as well as proffer the same to be queried for through
    //an intermediary

    //the IServiceProviderContract is equivalent to the COM IServiceProvider
    public interface IServiceProviderContract : IContract
    {
        /// <summary>
        /// This contracts allows a service provider to expose multiple services
        /// -- which may or may not implement the same contracts.
        /// </summary>
        /// <param name="serviceIdentifier">This param will typically contain a URI specifing which service instance the caller is interested in.</param>
        /// <param name="serviceContractIdentifier">The parm will typically contain the AssemblyQualifiedName of the type of the contract implemented by the service that the caller wishes to obtain.</param>
        /// <returns></returns>
        IContract QueryService(string serviceIdentifier, string serviceContractIdentifier);
    }

    /// <summary>
    /// This contract allow chains of ServiceProviders to work in concert with each other.
    /// Typically a "host" will expose this contract on the same object on which it implements
    /// IServiceProviderContract. Clients will Query for this Contract and add their serviceProvider to the chain
    /// </summary>
    public interface IProfferServiceContract : IContract
    {
        /// <summary>
        /// Clients call this method to add services to a host for access by other clients
        /// </summary>
        /// <param name="serviceIdentifier">This param will typically be a URI identifying the service.</param>
        /// <param name="service">The implementation of IServiceProviderContract that is passed in.</param>
        void ProfferService(string serviceIdentifier, IServiceProviderContract service);

        /// <summary>
        /// Clients call this method to remove services previously proffered.
        /// <param name="serviceIdentifier">This param will typically be a URI identifying the service to remove.</param>
        /// </summary>
        void RevokeService(string serviceIdentifier);
    }
}
