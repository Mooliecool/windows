//------------------------------------------------------------------------------
/// <copyright from='2004' to='2005' company='Microsoft Corporation'>
///    Copyright (c) Microsoft Corporation. All Rights Reserved.
///    Information Contained Herein is Proprietary and Confidential.
/// </copyright>
//------------------------------------------------------------------------------

using System;
using System.AddIn.Contract.Collections;
using System.Diagnostics.CodeAnalysis;
using System.Globalization;
using System.Reflection;

namespace System.AddIn.Contract.Automation
{
    /// <summary>
    /// this is the main object being remoted
    /// its purpose is to expose the IRemoteTypeContract
    /// </summary>
    public interface IRemoteObjectContract : IContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        IRemoteTypeContract GetRemoteType();

        RemoteArgument RemoteCast(string canonicalName);
    }

    /// <summary>
    /// The IRemoteTypeContract is the remote analog of the Type class
    /// Necessarily it exposes no Reflection types, and its funtionality
    /// is reduced from the real Type object due to the scenario
    /// </summary>
    public interface IRemoteTypeContract : IContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        RemoteTypeData GetTypeData();

        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        string GetCanonicalName();

        IArrayContract<IRemoteEventInfoContract> GetEvents(BindingFlags bindingFlags);
        IRemoteEventInfoContract GetEvent(string name, BindingFlags bindingFlags);

        IRemoteFieldInfoContract GetField(string name, BindingFlags bindingFlags);
        IArrayContract<IRemoteFieldInfoContract> GetFields(BindingFlags bindingFlags);

        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        IArrayContract<IRemoteTypeContract> GetInterfaces();
        IRemoteTypeContract GetInterface(string canonicalName);

        IArrayContract<IContract> GetMembers(BindingFlags bindingFlags);
        IArrayContract<IContract> GetMember(string name, MemberTypes memberTypes, BindingFlags bindingFlags);

        IArrayContract<IRemoteMethodInfoContract> GetMethods(BindingFlags bindingFlags);
        IRemoteMethodInfoContract GetMethod(string name, BindingFlags bindingFlags, IArrayContract<IRemoteTypeContract> remoteTypes);

        IArrayContract<IRemotePropertyInfoContract> GetProperties(BindingFlags bindingFlags);
        IRemotePropertyInfoContract GetProperty(string name, BindingFlags bindingFlags, IRemoteTypeContract remoteReturnType, IArrayContract<IRemoteTypeContract> remoteTypes);

        RemoteArgument InvokeMember(string name, BindingFlags bindingFlags, IRemoteObjectContract target, IRemoteArgumentArrayContract remoteArgs, bool[] remoteArgModifiers, int localeId);
    }

    public interface IRemoteEventInfoContract : IContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        RemoteMemberData GetMemberData();

        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        IRemoteMethodInfoContract GetAddMethod();

        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        IRemoteMethodInfoContract GetRemoveMethod();
    }

    public interface IRemoteFieldInfoContract : IContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        RemoteFieldData GetFieldData();

        RemoteArgument GetValue(IRemoteObjectContract obj);
        void SetValue(IRemoteObjectContract obj, RemoteArgument value, int localeId);
    }

    public interface IRemoteMethodInfoContract : IContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        RemoteMethodData GetMethodData();

        RemoteArgument Invoke(IRemoteObjectContract target, BindingFlags bindingFlags, IRemoteArgumentArrayContract parameters, int localeId);
    }

    public interface IRemotePropertyInfoContract : IContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        RemotePropertyData GetPropertyData();

        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        IRemoteMethodInfoContract GetGetMethod();

        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        IRemoteMethodInfoContract GetSetMethod();

        RemoteArgument GetValue(IRemoteObjectContract obj, BindingFlags bindingFlags, IRemoteArgumentArrayContract index, int localeId);

        [SuppressMessage("Microsoft.Design", "CA1045:DoNotPassTypesByReference")]
        void SetValue(IRemoteObjectContract target, ref RemoteArgument value, BindingFlags bindingFlags, IRemoteArgumentArrayContract index, int localeId);
    }

    public interface IRemoteDelegateContract : IRemoteObjectContract
    {
        RemoteArgument InvokeDelegate(IRemoteArgumentArrayContract args);
    }
}
