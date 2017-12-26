//------------------------------------------------------------------------------
/// <copyright from='2004' to='2005' company='Microsoft Corporation'>
///    Copyright (c) Microsoft Corporation. All Rights Reserved.
///    Information Contained Herein is Proprietary and Confidential.
/// </copyright>
//------------------------------------------------------------------------------

using System;
using System.Diagnostics.CodeAnalysis;

namespace System.AddIn.Contract
{
    /// <summary>
    /// IContract is the base interface for all
    /// Contracts.
    /// </summary>
    public interface IContract
    {
        /// <summary>
        /// The QueryContract method allows Contract
        /// implementations to expose other Contracts, on the same
        /// object. This serves a similar purpose to
        /// IUnknown.QueryInterface in the COM world.
        /// <remarks>
        /// The string name of the contract is implementation defined.
        /// </remarks>
        /// <param name="contractIdentifier">string</param>
        /// <returns>IContract</returns>
        IContract QueryContract(string contractIdentifier);

        /// <summary>
        /// This method is for identity. The identity of the object on the other
        /// side is determined by this code -- serving the same purpose as the
        /// pointer address for identity in C++ COM code.
        /// </summary>
        /// <returns></returns>
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Maintain parity with System.Object")]
        int GetRemoteHashCode();

        /// <summary>
        /// Remote Equals is used to maintain object identity. Proxies should
        /// override Equals and potentially the == and != operators to call
        /// this method for object equality\identity.
        /// </summary>
        /// <param name="contract"></param>
        /// <returns></returns>
        bool RemoteEquals(IContract contract);

        /// <summary>
        /// Remote version of the ToString method. It is useful in numerous
        /// scenarios, so it part of the base contract.
        /// </summary>
        /// <returns></returns>
        string RemoteToString();

        /// <summary>
        /// Clients of the service can use this method to "lock"
        /// it in memory guaranteeing that it won't go away until this
        /// particular lock is released
        /// </summary>
        /// <returns>
        /// Implementation dependent "token" to identify the
        /// particular lock
        /// </returns>
        int AcquireLifetimeToken();

        /// <summary>
        /// Releases a specific lock returned from AcquireLifetimeToken
        /// </summary>
        /// <param name="cookie">
        /// The "token" previously returned from AcquireLifetimeToken
        /// </param>
        void RevokeLifetimeToken(int token);

    }
}
