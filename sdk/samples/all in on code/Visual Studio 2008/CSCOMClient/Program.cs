/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSCOMClient
* Copyright (c) Microsoft Corporation.
* 
* This Visual C# code example demonstrates the access of COM components from 
* the .NET Framework by use of a runtime callable wrapper (RCW) or late 
* binding, and the host of ActiveX control in Windows Form.
* 
* A. Early binding of COM
* 
* Early-binding means that the compiler must have prior knowledge about COM  
* at compile time. Early-binding is supported by use of a runtime callable  
* wrapper (RCW). The wrapper turns the COM interfaces exposed by the COM 
* component into .NET Framework-compatible interfaces, and thus facilitates 
* communication between COM and .NET.
* 
* The Interop Assembly that is made from a type library is a form of RCW. It 
* defines managed interfaces that map to a COM-based type library and that a 
* managed client can interact with. To use an interop assembly in Visual 
* Studio, first add a reference to the corresponding COM component. Visual 
* Studio will automatically generate a local copy of the interop assembly. 
* Type Library Importer (Tlbimp.exe) is a standalone tool to convert the type 
* definitions found within a COM type library into equivalent definitions in 
* a common language runtime assembly. 
* 
* Without the use of the interop assembly, a developer may write a custom RCW 
* and manually map the types exposed by the COM interface to .NET Framework-
* compatible types. 
* 
* B. Late binding of COM
* 
* Late-binding means that the compiler does not have any prior knowledge  
* about the methods and properties in the COM component and it is delayed  
* until runtime. The advantage of late binding is that you do not have to use 
* an RCW (or build/ship a custom Interop Assembly), and it is more version 
* agnostic. The disadvantage is that it is more difficult to program this in 
* Visual C# than it is in Visual Basic, and late binding still suffers the   
* performance hit of having to find DISPIDs at runtime.
* 
* We do late binding in Visual C# through .NET reflection. Reflection is a  
* way to determine the type or information about the classes or interfaces. 
* We do not need to create RCW for the COM component in late binding as we 
* did in early binding. We use System.Type.GetTypeFromProgID to get the type 
* object for the COM object, then use System.Activator.CreateInstance to  
* instantiate the COM object.
* 
* C. Host of ActiveX control
* 
* Hosting an ActiveX control requires a RCW of the AcitveX COM object, and a 
* class that inherits from System.Windows.Forms.AxHost to wrap the ActiveX  
* COM object and expose it as fully featured Windows Forms controls. The 
* stuff can be automatically done using the Windows Forms ActiveX Control  
* Importer (Aximp.exe) or Visual Studio Windows Forms design environment.
* 
* The ActiveX Control Importer generates a class that is derived from the 
* AxHost class, and compiles it into a library file (DLL) that can be added 
* as a reference to your application. Alternatively, you can use the /source 
* switch with the ActiveX Control Importer and a C# file is generated for 
* your AxHost derived class. You can then make changes to the code and 
* recompile it into a library file. 
* 
* If you are using Visual Studio as your Windows Forms design environment,  
* you can make an ActiveX control available to your application by adding the 
* ActiveX control to your Toolbox. To accomplish this, right-click the 
* Toolbox, select Choose Items, then browse to the ActiveX control's .OCX or 
* .DLL file.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Globalization;
#endregion


namespace CSCOMClient
{
    class Program
    {
        static void Main(string[] args)
        {
            Thread thread = null;

            // Early binding of the component through interop assembly
            thread = new Thread(new ThreadStart(EarlyBindtoComponent));
            thread.SetApartmentState(ApartmentState.STA);
            thread.Start();
            thread.Join();  // Block the main thread

            // Late binding of the component through .NET reflection
            thread = new Thread(new ThreadStart(LateBindtoComponent));
            thread.SetApartmentState(ApartmentState.STA);
            thread.Start();
            thread.Join();  // Block the main thread

            // Windows Form hosts an ActiveX control
            thread = new Thread(new ThreadStart(HostActiveXControl));
            thread.SetApartmentState(ApartmentState.STA);
            thread.Start();
            thread.Join();  // Block the main thread
        }


        #region Early Binding

        /// <summary>
        /// 
        /// </summary>
        static void EarlyBindtoComponent()
        {
            /////////////////////////////////////////////////////////////////////
            // Create an ATLDllCOMServer.SimpleObject COM object.
            // 

            ATLDllCOMServerLib.SimpleObject simpleObj =
                new ATLDllCOMServerLib.SimpleObject();


            /////////////////////////////////////////////////////////////////////
            // Register the events of the COM object.
            // 

            simpleObj.FloatPropertyChanging += new ATLDllCOMServerLib.
                _ISimpleObjectEvents_FloatPropertyChangingEventHandler(
                simpleObj_FloatPropertyChanging);


            /////////////////////////////////////////////////////////////////////
            // Consume the properties and the methods of the COM object.
            // 

            try
            {
                // Set the property: FloatProperty, which raises the event
                // FloatPropertyChanging.
                {
                    Console.WriteLine("Set FloatProperty = {0,0:f2}", 1.2f);
                    simpleObj.FloatProperty = 1.2f;
                }

                // Get the property: FloatProperty.
                {
                    Console.WriteLine("Get FloatProperty = {0,0:f2}",
                        simpleObj.FloatProperty);
                }

                // Call the method: HelloWorld, that returns a string.
                {
                    string strResult = simpleObj.HelloWorld();
                    Console.WriteLine("Call HelloWorld => {0}", strResult);
                }

                // Call the method: GetProcessThreadID, that outputs two integers.
                {
                    Console.WriteLine("The client process and thread: {0}, {1}",
                        NativeMethod.GetCurrentProcessId(),
                        NativeMethod.GetCurrentThreadId());

                    int processId, threadId;
                    simpleObj.GetProcessThreadID(out processId, out threadId);
                    Console.WriteLine("Call GetProcessThreadID => {0}, {1}",
                        processId, threadId);
                }

                Console.WriteLine();
            }
            catch (Exception ex)
            {
                Console.WriteLine("The server throws the error: {0}", ex.Message);
                if (ex.InnerException != null)
                    Console.WriteLine("Description: {0}", ex.InnerException.Message);
            }


            /////////////////////////////////////////////////////////////////////
            // Release the COM object.
            // It is strongly recommended against using ReleaseComObject to 
            // manually release an RCW object that represents a COM component 
            // unless you absolutely have to. We should generally let CLR release 
            // the COM object in the garbage collector.
            // Ref: http://blogs.msdn.com/yvesdolc/archive/2004/04/17/115379.aspx
            // 

            Marshal.FinalReleaseComObject(simpleObj);
        }

        static void simpleObj_FloatPropertyChanging(float NewValue, ref bool Cancel)
        {
            // OK or cancel the change of FloatProperty
            Cancel = (DialogResult.Cancel == MessageBox.Show(
                String.Format("FloatProperty is being changed to {0,0:f2}", NewValue),
                "ATLDllCOMServer!FloatPropertyChanging", MessageBoxButtons.OKCancel));
        }

        #endregion


        #region Late Binding

        static void LateBindtoComponent()
        {
            /////////////////////////////////////////////////////////////////////
            // Create an ATLDllCOMServer.SimpleObject COM object.
            // 

            // Get type from ProgID
            Type simpleObjType = Type.GetTypeFromProgID(
                "ATLDllCOMServer.SimpleObject");
            // [-or-] Get type from CLSID
            //Type simpleObjType = Type.GetTypeFromCLSID(
            //    new Guid("7AD9E1C6-E804-43C2-9383-1C8F35283081"));
            if (simpleObjType == null)
            {
                Console.WriteLine("The ATLDllCOMServerLib.SimpleObject " +
                    "component is not registered");
                return;
            }
            object simpleObj = Activator.CreateInstance(simpleObjType);


            /////////////////////////////////////////////////////////////////////
            // Consume the properties and the methods of the COM object.
            // 

            try
            {
                // Set the property: FloatProperty
                {
                    Console.WriteLine("Set FloatProperty = {0,0:f2}", 1.2f);

                    simpleObjType.InvokeMember("FloatProperty",
                        BindingFlags.SetProperty, null, simpleObj,
                        new object[] { 1.2f });
                }

                // Get the property: FloatProperty
                {
                    float fProp = (float)simpleObjType.InvokeMember(
                        "FloatProperty", BindingFlags.GetProperty, null,
                        simpleObj, null);

                    Console.WriteLine("Get FloatProperty = {0,0:f2}", fProp);
                }

                // Call the method: HelloWorld, that returns a string.
                {
                    string strResult = simpleObjType.InvokeMember("HelloWorld",
                        BindingFlags.InvokeMethod, null, simpleObj, null)
                        as string;

                    Console.WriteLine("Call HelloWorld => {0}", strResult);
                }

                // Call the method: GetProcessThreadID, that outputs two integers.
                {
                    Console.WriteLine("The client process and thread: {0}, {1}",
                        NativeMethod.GetCurrentProcessId(),
                        NativeMethod.GetCurrentThreadId());

                    // Parameter modifiers indicate that both parameters are 
                    // output parameters.
                    ParameterModifier[] paramMods = new ParameterModifier[1];
                    paramMods[0] = new ParameterModifier(2);
                    paramMods[0][0] = true; // [out] parameter
                    paramMods[0][1] = true; // [out] parameter

                    object[] args = new object[2];
                    // Must initialize the array element. Otherwise, InvokeMember 
                    // throws the "Type mismatch" exception.
                    args[0] = new int();
                    args[1] = new int();

                    simpleObjType.InvokeMember("GetProcessThreadID",
                        BindingFlags.InvokeMethod, null, simpleObj, args,
                        paramMods, null, null);

                    Console.WriteLine("Call GetProcessThreadID => {0}, {1}",
                        args[0], args[1]);
                }

                Console.WriteLine();
            }
            catch (Exception ex)
            {
                Console.WriteLine("The server throws the error: {0}", ex.Message);
                if (ex.InnerException != null)
                    Console.WriteLine("Description: {0}", ex.InnerException.Message);
            }

            /////////////////////////////////////////////////////////////////////
            // Release the COM object.
            // It is strongly recommended against using ReleaseComObject to 
            // manually release an RCW object that represents a COM component 
            // unless you absolutely have to. We should generally let CLR release 
            // the COM object in the garbage collector.
            // Ref: http://blogs.msdn.com/yvesdolc/archive/2004/04/17/115379.aspx
            // 

            Marshal.FinalReleaseComObject(simpleObj);
        }

        #endregion


        #region ActiveX Control

        static void HostActiveXControl()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm());
        }

        #endregion

    }
}