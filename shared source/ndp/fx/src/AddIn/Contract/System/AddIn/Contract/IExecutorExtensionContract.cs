//------------------------------------------------------------------------------
/// <copyright from='1997' to='2005' company='Microsoft Corporation'>
///    Copyright (c) Microsoft Corporation. All Rights Reserved.
///    Information Contained Herein is Proprietary and Confidential.
/// </copyright>
//------------------------------------------------------------------------------

using System;
using System.AddIn.Contract.Collections;

namespace System.AddIn.Contract
{
    public interface IExecutorExtensionContract : IContract
    {
        void ExecutorCreated();
        void AssemblyLoading(string assemblyName);
        void AssemblyLoadingFrom(string assemblyFile);
        void AssemblyLoaded(string assemblyName);
        void AssemblyLoadedFrom(string assemblyFile);
        void EntryPointStarting(string assemblyName, string startupClass, IRemoteArgumentArrayContract initArgs);
        void EntryPointStarted(IContract entryPoint);
    }
}
