//------------------------------------------------------------------------------
// <copyright file="ServiceObjectContainer.cs" company="Microsoft">
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
    using Microsoft.Win32;
    using System;
    using System.Collections;
    using System.Diagnostics;
    using System.Security.Permissions;

    /// <devdoc>
    ///     This is a simple implementation of IServiceContainer.
    /// </devdoc>
    [HostProtection(SharedState = true)]
    public class ServiceContainer : IServiceContainer, IDisposable
    {
        private Hashtable services;
        private IServiceProvider parentProvider;
        private static Type[] _defaultServices = new Type[] { typeof(IServiceContainer), typeof(ServiceContainer) };
        
        private static TraceSwitch TRACESERVICE = new TraceSwitch("TRACESERVICE", "ServiceProvider: Trace service provider requests.");
        
        /// <devdoc>
        ///     Creates a new service object container.  
        /// </devdoc>
        public ServiceContainer() {
        }
        
        /// <devdoc>
        ///     Creates a new service object container.  
        /// </devdoc>
        public ServiceContainer(IServiceProvider parentProvider) {
            this.parentProvider = parentProvider;
        }
        
        /// <devdoc>
        ///     Retrieves the parent service container, or null
        ///     if there is no parent container.
        /// </devdoc>
        private IServiceContainer Container { 
            get {
                IServiceContainer container = null;
                if (parentProvider != null) {
                    container = (IServiceContainer)parentProvider.GetService(typeof(IServiceContainer));
                }
                return container;
            }
        }

        /// <devdoc>
        ///     This property returns the default services that are implemented directly on this IServiceContainer.
        ///     the default implementation of this property is to return the IServiceContainer and ServiceContainer
        ///     types.  You may override this proeprty and return your own types, modifying the default behavior
        ///     of GetService.
        /// </devdoc>
        protected virtual Type[] DefaultServices {
            get {
                return _defaultServices;
            }
        }
        
        /// <devdoc>
        ///     Our hashtable of services.  The hashtable is demand
        ///     created here.
        /// </devdoc>
        private Hashtable Services {
            get {
                if (services == null) {
                    services = new Hashtable();
                }
                return services;
            }
        }
        
        /// <devdoc>
        ///     Adds the given service to the service container.
        /// </devdoc>
        public void AddService(Type serviceType, object serviceInstance) {
            AddService(serviceType, serviceInstance, false);
        }

        /// <devdoc>
        ///     Adds the given service to the service container.
        /// </devdoc>
        public virtual void AddService(Type serviceType, object serviceInstance, bool promote) {
            Debug.WriteLineIf(TRACESERVICE.TraceVerbose, "Adding service (instance) " + serviceType.Name + ".  Promoting: " + promote.ToString());
            if (promote) {
                IServiceContainer container = Container;
                if (container != null) {
                    Debug.Indent();
                    Debug.WriteLineIf(TRACESERVICE.TraceVerbose, "Promoting to container");
                    Debug.Unindent();
                    container.AddService(serviceType, serviceInstance, promote);
                    return;
                }
            }
            
            // We're going to add this locally.  Ensure that the service instance
            // is correct.
            //
            if (serviceType == null) throw new ArgumentNullException("serviceType");
            if (serviceInstance == null) throw new ArgumentNullException("serviceInstance");
            if (!(serviceInstance is ServiceCreatorCallback) && !serviceInstance.GetType().IsCOMObject && !serviceType.IsAssignableFrom(serviceInstance.GetType())) {
                throw new ArgumentException(SR.GetString(SR.ErrorInvalidServiceInstance, serviceType.FullName));
            }
            
            if (Services.ContainsKey(serviceType)) {
                throw new ArgumentException(SR.GetString(SR.ErrorServiceExists, serviceType.FullName), "serviceType");
            }
            
            Services[serviceType] = serviceInstance;
        }

        /// <devdoc>
        ///     Adds the given service to the service container.
        /// </devdoc>
        public void AddService(Type serviceType, ServiceCreatorCallback callback) {
            AddService(serviceType, callback, false);
        }

        /// <devdoc>
        ///     Adds the given service to the service container.
        /// </devdoc>
        public virtual void AddService(Type serviceType, ServiceCreatorCallback callback, bool promote) {
            Debug.WriteLineIf(TRACESERVICE.TraceVerbose, "Adding service (callback) " + serviceType.Name + ".  Promoting: " + promote.ToString());
            if (promote) {
                IServiceContainer container = Container;
                if (container != null) {
                    Debug.Indent();
                    Debug.WriteLineIf(TRACESERVICE.TraceVerbose, "Promoting to container");
                    Debug.Unindent();
                    container.AddService(serviceType, callback, promote);
                    return;
                }
            }
            
            // We're going to add this locally.  Ensure that the service instance
            // is correct.
            //
            if (serviceType == null) throw new ArgumentNullException("serviceType");
            if (callback == null) throw new ArgumentNullException("callback");
            
            if (Services.ContainsKey(serviceType)) {
                throw new ArgumentException(SR.GetString(SR.ErrorServiceExists, serviceType.FullName), "serviceType");
            }
            
            Services[serviceType] = callback;
        }

        /// <devdoc>
        ///     Disposes this service container.  This also walks all instantiated services within the container
        ///     and disposes any that implement IDisposable, and clears the service list.
        /// </devdoc>
        public void Dispose() {
            Dispose(true);
        }

        /// <devdoc>
        ///     Disposes this service container.  This also walks all instantiated services within the container
        ///     and disposes any that implement IDisposable, and clears the service list.
        /// </devdoc>
        protected virtual void Dispose(bool disposing) {
            if (disposing) {
                Hashtable serviceHash = services;
                services = null;
                if (serviceHash != null) {
                    foreach(object o in serviceHash.Values) {
                        if (o is IDisposable) {
                            ((IDisposable)o).Dispose();
                        }
                    }
                }
            }
        }

        /// <devdoc>
        ///     Retrieves the requested service.
        /// </devdoc>
        public virtual object GetService(Type serviceType) {
            object service = null;
            
            Debug.WriteLineIf(TRACESERVICE.TraceVerbose, "Searching for service " + serviceType.Name);
            Debug.Indent();
            
            // Try locally.  We first test for services we
            // implement and then look in our hashtable.
            //
            Type[] defaults = DefaultServices;
            for (int idx = 0; idx < defaults.Length; idx++) {
                if (serviceType == defaults[idx]) {
                    service = this;
                    break;
                }
            }

            if (service == null) {
                service = Services[serviceType];
            }
            
            // Is the service a creator delegate?
            //
            if (service is ServiceCreatorCallback) {
                Debug.WriteLineIf(TRACESERVICE.TraceVerbose, "Encountered a callback. Invoking it");
                service = ((ServiceCreatorCallback)service)(this, serviceType);
                Debug.WriteLineIf(TRACESERVICE.TraceVerbose, "Callback return object: " + (service == null ? "(null)" : service.ToString()));
                if (service != null && !service.GetType().IsCOMObject && !serviceType.IsAssignableFrom(service.GetType())) {
                    // Callback passed us a bad service.  NULL it, rather than throwing an exception.
                    // Callers here do not need to be prepared to handle bad callback implemetations.
                    Debug.Fail("Object " + service.GetType().Name + " was returned from a service creator callback but it does not implement the registered type of " + serviceType.Name);
                    Debug.WriteLineIf(TRACESERVICE.TraceVerbose, "**** Object does not implement service interface");
                    service = null;
                }
                
                // And replace the callback with our new service.
                //
                Services[serviceType] = service;
            }
            
            if (service == null && parentProvider != null) {
                Debug.WriteLineIf(TRACESERVICE.TraceVerbose, "Service unresolved.  Trying parent");
                service = parentProvider.GetService(serviceType);
            }
            
            #if DEBUG
            if (TRACESERVICE.TraceVerbose && service == null) {
                Debug.WriteLine("******************************************");
                Debug.WriteLine("FAILED to resolve service " + serviceType.Name);
                Debug.WriteLine("AT: " + Environment.StackTrace);
                Debug.WriteLine("******************************************");
            }
            #endif
            Debug.Unindent();
            
            return service;
        }
        
        /// <devdoc>
        ///     Removes the given service type from the service container.
        /// </devdoc>
        public void RemoveService(Type serviceType) {
            RemoveService(serviceType, false);
        }

        /// <devdoc>
        ///     Removes the given service type from the service container.
        /// </devdoc>
        public virtual void RemoveService(Type serviceType, bool promote) {
            Debug.WriteLineIf(TRACESERVICE.TraceVerbose, "Removing service: " + serviceType.Name + ", Promote: " + promote.ToString());
            if (promote) {
                IServiceContainer container = Container;
                if (container != null) {
                    Debug.Indent();
                    Debug.WriteLineIf(TRACESERVICE.TraceVerbose, "Invoking parent container");
                    Debug.Unindent();
                    container.RemoveService(serviceType, promote);
                    return;
                }
            }
            
            // We're going to remove this from our local list.
            //
            if (serviceType == null) throw new ArgumentNullException("serviceType");
            Services.Remove(serviceType);
        }
    }
}

