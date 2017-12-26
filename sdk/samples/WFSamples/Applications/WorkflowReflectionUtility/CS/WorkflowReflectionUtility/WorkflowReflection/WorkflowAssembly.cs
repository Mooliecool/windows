//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

namespace Microsoft.Samples.Workflow.WorkflowReflectionUtility.Reflection
{
    using System;
    using System.Collections.Generic;
    using System.Reflection;
    using System.Workflow.Activities;
    using System.Workflow.ComponentModel;

    /// <summary>
    /// Representation of an assembly containing workflow
    /// related data.  Provides caching for performance reasons.
    /// </summary>
    public class WorkflowAssembly
    {
        private Assembly assembly;
        private Type[] workflowTypes;
        private Type[] localServiceInterfaces;
        private Type[] localServices;
        private Type[] activities;

        /// <summary>
        /// Create a new WorkflowAssembly based on the
        /// given assembly.
        /// </summary>
        /// <param name="asm"></param>
        public WorkflowAssembly(Assembly assembly)
        {
            this.assembly = assembly;

            this.workflowTypes = null;
            this.localServiceInterfaces = null;
            this.localServices = null;
            this.activities = null;
        }

        /// <summary>
        /// Underlying System.Reflection Assembly object.
        /// </summary>
        public Assembly Assembly
        {
            get
            {
                return assembly;
            }
        }

        /// <summary>
        /// Get the array of workflow Types contained
        /// in this assembly.
        /// </summary>
        /// <returns>Array of workflow Types.</returns>
        public Type[] GetWorkflowTypes()
        {
            if (this.workflowTypes == null)
            {
                if (!(CollectTypes(false)))
                    return null;
            }

            return this.workflowTypes;
        }

        /// <summary>
        /// Get the array of Local Service Interface Types
        /// contained in this assembly.
        /// </summary>
        /// <returns>Array of Local Service Interface
        /// types.</returns>
        public Type[] GetLocalServiceInterfaces()
        {
            if (this.localServiceInterfaces == null)
            {
                if (!(CollectTypes(false)))
                    return null;
            }

            return this.localServiceInterfaces;
        }

        /// <summary>
        /// Get the array of custom activity Types 
        /// contained in this assembly.
        /// </summary>
        /// <returns>Array of custom activity Types.</returns>
        public Type[] GetActivities()
        {
            if (this.activities == null)
            {
                if (!(CollectTypes(false)))
                    return null;
            }

            return this.activities;
        }

        /// <summary>
        /// Get the array of Local Service Types
        /// contained in this assembly.
        /// </summary>
        /// <returns>Array of Local Service Types.</returns>
        public Type[] GetLocalServices()
        {
            if (this.localServices == null)
            {
                if (this.workflowTypes == null)
                {
                    if (!(CollectTypes(true)))
                        return null;
                }
                else
                {
                    List<Type> localServiceList = new List<Type>();

                    Type[] assemblyTypes = assembly.GetTypes();

                    if (assemblyTypes == null)
                        return null;

                    foreach (Type service in assemblyTypes)
                    {
                        Type[] localServices = service.GetInterfaces();

                        foreach (Type localService in localServices)
                        {
                            if (localService.GetCustomAttributes(typeof(ExternalDataExchangeAttribute), true).Length > 0)
                            {
                                localServiceList.Add(service);
                            }
                        }
                    }

                    this.localServices = localServiceList.ToArray();
                }
            }

            return this.localServices;
        }

        private bool CollectTypes(bool includeLocalService)
        {
            List<Type> workflowsList = new List<Type>();
            List<Type> localServiceInterfaceList = new List<Type>();
            List<Type> localServicesList = new List<Type>();
            List<Type> activitiesList = new List<Type>();
            Type[] assemblyTypes;
            try
            {
                assemblyTypes = assembly.GetTypes();
            }
            catch (ReflectionTypeLoadException)
            {
                System.Windows.Forms.MessageBox.Show("Unable to load workflow assembly.");
                return false;
            }


            foreach (Type type in assemblyTypes)
            {
                if (typeof(Activity).IsAssignableFrom(type)
                    && type.GetConstructor(Type.EmptyTypes) != null)
                {
                    workflowsList.Add(type);
                }
                else if (type.GetCustomAttributes(typeof(ExternalDataExchangeAttribute), true).Length > 0)
                {
                    localServiceInterfaceList.Add(type);
                }
                else if (typeof(Activity).IsAssignableFrom(type))
                {
                    activitiesList.Add(type);
                }
                else if (includeLocalService)
                {
                    Type[] interfaces = type.GetInterfaces();

                    foreach (Type service in interfaces)
                    {
                        if (service.GetCustomAttributes(typeof(ExternalDataExchangeAttribute), true).Length > 0)
                        {
                            localServicesList.Add(type);
                        }
                    }
                }
            }

            this.workflowTypes = workflowsList.ToArray();
            this.localServiceInterfaces = localServiceInterfaceList.ToArray();
            this.activities = activitiesList.ToArray();

            if (includeLocalService)
            {
                this.localServices = localServicesList.ToArray();
            }
            return true;
        }

        /// <summary>
        /// Load an assembly with the given path.
        /// </summary>
        /// <param name="fName">Path to the assembly.</param>
        /// <returns>A WorkflowAssembly object wrapping the
        /// System.Reflection Assembly from that location.</returns>
        public static WorkflowAssembly LoadFrom(string fileName)
        {
            return new WorkflowAssembly(Assembly.LoadFrom(fileName));
        }
    }
}
