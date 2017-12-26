/****************************** Module Header ******************************\
* Module Name:  SimpleObject.cs
* Project:      CSServicedComponent
* Copyright (c) Microsoft Corporation.
* 
* CSServicedComponent demonstrates a serviced component written in Visua C#. 
* A serviced component is a class that is authored in a CLS-compliant  
* language and that derives directly or indirectly from the 
* System.EnterpriseServices.ServicedComponent class. Classes configured in 
* this way can be hosted in a COM+ application and can use COM+ services by 
* way of the EnterpriseServices namespace. 
* 
* CSServicedComponent exposes the following component:
* 
* Program ID: CSServicedComponent.SimpleObject
* CLSID_SimpleObject: 14EAD156-F55E-4d9b-A3C5-658D4BB56D30
* IID_ISimpleObject: 2A59630E-4232-4582-AE47-706E2B648579
* DIID_ISimpleObjectEvents: A06C000A-7A85-42dc-BA6D-BE736B6EB97A
* LIBID_CSServicedComponent: 3308202E-A355-4C3D-805D-B527D1EF5FA3
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
* // Transactional operation
* void DoTransaction();
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
using System.EnterpriseServices;
using System.Runtime.InteropServices;
#endregion


namespace CSServicedComponent
{
    #region Interfaces

    [Description("ISimpleObject description")]
    [Guid("2A59630E-4232-4582-AE47-706E2B648579")]
    // The public interface describing the COM interface of the coclass 
    public interface ISimpleObject
    {
        #region Properties

        float FloatProperty { get; set; }

        #endregion

        #region Methods

        [Description("HelloWorld description")]
        string HelloWorld();

        void GetProcessThreadID(out uint processId, out uint threadId);

        [Description("DoTransaction description")]
        void DoTransaction();

        #endregion
    }

    [Guid("A06C000A-7A85-42dc-BA6D-BE736B6EB97A")]
    [InterfaceType(ComInterfaceType.InterfaceIsIDispatch)]
    // The public interface describing the events the coclass can sink
    public interface ISimpleObjectEvents
    {
        #region Events

        [DispId(1)]
        void FloatPropertyChanging(float NewValue, ref bool Cancel);

        #endregion
    }

    #endregion

    // The ObjectPooling attribute is used to configure the component's object 
    // pooling. It can enable or disables object pooling and set the min. or  
    // max. pool size and object creation timeout. 
    [ObjectPooling(MinPoolSize=2, MaxPoolSize=10, CreationTimeout=20)]
    // Creates the component with a new transaction, regardless of the state 
    // of the current context.
    [Transaction(TransactionOption.Required)]
    [ClassInterface(ClassInterfaceType.None)]
    [ComSourceInterfaces(typeof(ISimpleObjectEvents))]
    [Guid("14EAD156-F55E-4d9b-A3C5-658D4BB56D30")]
    public class SimpleObject : ServicedComponent, ISimpleObject
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

        public void DoTransaction()
        {
            try
            {
                // Operate on the resource managers like DBMS
                // ...
		
                ContextUtil.SetComplete();  // Commit
            }
            catch (Exception ex)
            {
                ContextUtil.SetAbort();     // Rollback
                throw ex;
            }
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
