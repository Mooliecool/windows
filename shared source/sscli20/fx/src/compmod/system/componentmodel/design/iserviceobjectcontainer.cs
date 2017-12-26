//------------------------------------------------------------------------------
// <copyright file="IServiceObjectContainer.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

/*
 */
namespace System.ComponentModel.Design {
    using System;

    /// <devdoc>
    ///     This interface provides a container for services.  A service container
    ///     is, by definition, a service provider.  In addition to providing services
    ///     it also provides a mechanism for adding and removing services.
    /// </devdoc>
    [System.Runtime.InteropServices.ComVisible(true)]
    public interface IServiceContainer : IServiceProvider {

        /// <devdoc>
        ///     Adds the given service to the service container.
        /// </devdoc>
        void AddService(Type serviceType, object serviceInstance);

        /// <devdoc>
        ///     Adds the given service to the service container.
        /// </devdoc>
        void AddService(Type serviceType, object serviceInstance, bool promote);

        /// <devdoc>
        ///     Adds the given service to the service container.
        /// </devdoc>
        void AddService(Type serviceType, ServiceCreatorCallback callback);

        /// <devdoc>
        ///     Adds the given service to the service container.
        /// </devdoc>
        void AddService(Type serviceType, ServiceCreatorCallback callback, bool promote);

        /// <devdoc>
        ///     Removes the given service type from the service container.
        /// </devdoc>
        void RemoveService(Type serviceType);

        /// <devdoc>
        ///     Removes the given service type from the service container.
        /// </devdoc>
        void RemoveService(Type serviceType, bool promote);
   }
}

