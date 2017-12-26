/****************************** Module Header ******************************\
* Module Name:  SimpleObject.cs
* Project:      CSCOMService
* Copyright (c) Microsoft Corporation.
* 
* The definition of the COM class, SimpleObject, and its ClassFactory, 
* SimpleObjectClassFactory.
* 
* Program ID: CSCOMService.SimpleObject
* CLSID_SimpleObject: E2EDB864-02DB-4130-BEE4-2E35B30BBF3B
* IID_ISimpleObject: 83C40736-3189-44bc-AB0F-9FB3703EA172
* DIID_ISimpleObjectEvents: 7A11E6DA-DD09-404c-8731-DB917E783501
* AppID: 2E78BFC7-FDD9-4b87-BB6F-470D08399DD1
* 
* Properties:
* // With both get and set accessor methods
* float FloatProperty
* 
* Methods:
* // HelloWorld returns a string "HelloWorld"
* string HelloWorld();
* // GetProcessThreadID outputs the running process ID and thread ID
* void GetProcessThreadID(out uint processId, out uint threadId);
* 
* Events:
* // FloatPropertyChanging is fired before new value is set to the 
* // FloatProperty property. The Cancel parameter allows the client to cancel 
* // the change of FloatProperty.
* void FloatPropertyChanging(float NewValue, ref bool Cancel);
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.ComponentModel;
#endregion


namespace CSCOMService
{
    #region Interfaces

    [Guid(SimpleObject.InterfaceId), ComVisible(true)]
    public interface ISimpleObject
    {
        #region Properties

        float FloatProperty { get; set; }

        #endregion

        #region Methods

        string HelloWorld();

        void GetProcessThreadID(out uint processId, out uint threadId);

        #endregion
    }

    [Guid(SimpleObject.EventsId), ComVisible(true)]
    [InterfaceType(ComInterfaceType.InterfaceIsIDispatch)]
    public interface ISimpleObjectEvents
    {
        #region Events

        [DispId(1)]
        void FloatPropertyChanging(float NewValue, ref bool Cancel);

        #endregion
    }

    #endregion

    [ClassInterface(ClassInterfaceType.None)]           // No ClassInterface
    [ComSourceInterfaces(typeof(ISimpleObjectEvents))]
    [Guid(SimpleObject.ClassId), ComVisible(true)]
    public class SimpleObject : ISimpleObject
    {
        #region COM Component Registration

        internal const string ClassId =
            "E2EDB864-02DB-4130-BEE4-2E35B30BBF3B";
        internal const string InterfaceId = 
            "83C40736-3189-44bc-AB0F-9FB3703EA172";
        internal const string EventsId =
            "7A11E6DA-DD09-404c-8731-DB917E783501";
        internal const string AppId = 
            "2E78BFC7-FDD9-4b87-BB6F-470D08399DD1";
        internal const string ServiceName = "CSCOMService";

        // These routines perform the additional COM registration needed by 
        // the service.

        [EditorBrowsable(EditorBrowsableState.Never)]
        [ComRegisterFunction()]
        public static void Register(Type t)
        {
            try
            {
                COMHelper.RegasmRegisterLocalService(t, new Guid(AppId), ServiceName);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message); // Log the error
                throw ex; // Re-throw the exception
            }
        }

        [EditorBrowsable(EditorBrowsableState.Never)]
        [ComUnregisterFunction()]
        public static void Unregister(Type t)
        {
            try
            {
                COMHelper.RegasmUnregisterLocalService(t, new Guid(AppId));
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message); // Log the error
                throw ex; // Re-throw the exception
            }
        }

        #endregion

        #region Properties

        private float fField = 0;

        public float FloatProperty
        {
            get { return this.fField; }
            set
            {
                bool cancel = false;
                // Raise the event FloatPropertyChanging
                if (null != FloatPropertyChanging)
                    FloatPropertyChanging(value, ref cancel);
                if (!cancel)
                    this.fField = value;
            }
        }

        #endregion

        #region Methods

        public string HelloWorld()
        {
            return "HelloWorld";
        }

        public void GetProcessThreadID(out uint processId, out uint threadId)
        {
            processId = NativeMethod.GetCurrentProcessId();
            threadId = NativeMethod.GetCurrentThreadId();
        }

        #endregion

        #region Events

        [ComVisible(false)]
        public delegate void FloatPropertyChangingEventHandler(float NewValue, ref bool Cancel);
        public event FloatPropertyChangingEventHandler FloatPropertyChanging;

        #endregion
    }

    /// <summary>
    /// Class factory for the class SimpleObject.
    /// </summary>
    internal class SimpleObjectClassFactory : IClassFactory
    {
        public int CreateInstance(
            IntPtr pUnkOuter, ref Guid riid, out IntPtr ppvObject)
        {
            ppvObject = IntPtr.Zero;

            if (pUnkOuter != IntPtr.Zero)
            {
                // The pUnkOuter parameter was non-NULL and the object does 
                // not support aggregation.
                Marshal.ThrowExceptionForHR(COMNative.CLASS_E_NOAGGREGATION);
            }

            if (riid == new Guid(SimpleObject.ClassId) || 
                riid == new Guid(COMNative.GuidIUnknown))
            {
                // Create the instance of the .NET object
                ppvObject = Marshal.GetComInterfaceForObject(
                    new SimpleObject(), typeof(ISimpleObject));
            }
            else
            {
                // The object that ppvObject points to does not support the 
                // interface identified by riid.
                Marshal.ThrowExceptionForHR(COMNative.E_NOINTERFACE);
            }
            return 0;   // S_OK
        }

        public int LockServer(bool fLock)
        {
            return 0;   // S_OK
        }
    }
}
