/****************************** Module Header ******************************\
* Module Name:  SimpleObject.cs
* Project:      CSRegFreeCOMServer
* Copyright (c) Microsoft Corporation.
* 
* The CSRegFreeCOMServer sample demonstrates a .NET Framework-based component 
* which is ready for registration-free activation. 
* 
* Registration-free COM is a mechanism available on the Microsoft Windows XP 
* (SP2 for .NET Framework-based components), Microsoft Windows Server 2003 
* and newer platforms. As the name suggests, the mechanism enables easy (e.g. 
* XCOPY) deployment of COM components to a machine without the need to 
* register them.
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
using System.Runtime.InteropServices;
#endregion


namespace CSRegFreeCOMServer
{
    #region Interfaces

    /// <summary>
    /// The public interface describing the COM interface of the coclass 
    /// </summary>
    [Guid("95A215C3-FC49-4f4e-9647-638521470D42")]          // IID
    [ComVisible(true)]
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

    /// <summary>
    /// The public interface describing the events the coclass can sink
    /// </summary>
    [Guid("94BAC1E3-1172-41ee-BB83-AE451C8F3C12")]
    [InterfaceType(ComInterfaceType.InterfaceIsIDispatch)]
    [ComVisible(true)]
    public interface ISimpleObjectEvents
    {
        #region Events

        [DispId(1)]
        void FloatPropertyChanging(float NewValue, ref bool Cancel);

        #endregion
    }

    #endregion

    [ClassInterface(ClassInterfaceType.None)]              // No ClassInterface
    [ComSourceInterfaces(typeof(ISimpleObjectEvents))]
    [Guid("2384B3E7-4FFD-460d-A5B9-284182707922")]         // CLSID
    //[ProgId("CSRegFreeCOMServer.CustomSimpleObject")]  // ProgID
    [ComVisible(true)]
    public class SimpleObject : ISimpleObject
    {
        #region Properties

        /// <summary>
        /// The private members don't make it into the type-library and are 
        /// hidden from the COM clients.
        /// </summary>
        private float fField = 0;

        /// <summary>
        /// A public property with both get and set accessor methods.
        /// </summary>
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

        /// <summary>
        /// A public method that returns a string "HelloWorld".
        /// </summary>
        /// <returns>"HelloWorld"</returns>
        public string HelloWorld()
        {
            return "HelloWorld";
        }

        /// <summary>
        /// A public method with two outputs: the current process Id and the
        /// current thread Id.
        /// </summary>
        /// <param name="processId">[out] The current process Id</param>
        /// <param name="threadId">[out] The current thread Id</param>
        public void GetProcessThreadID(out uint processId, out uint threadId)
        {
            processId = NativeMethod.GetCurrentProcessId();
            threadId = NativeMethod.GetCurrentThreadId();
        }

        /// <summary>
        /// A hidden method (ComVisible = false)
        /// </summary>
        public void HiddenFunction()
        {
            Console.WriteLine("HiddenFunction is called.");
        }

        #endregion

        #region Events

        [ComVisible(false)]
        public delegate void FloatPropertyChangingEventHandler(float NewValue, ref bool Cancel);

        /// <summary>
        /// A public event that is fired before new value is set to the
        /// FloatProperty property. The Cancel parameter allows the client 
        /// to cancel the change of FloatProperty.
        /// </summary>
        public event FloatPropertyChangingEventHandler FloatPropertyChanging;

        #endregion
    }
}