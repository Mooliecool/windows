'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBCOMClient
' Copyright (c) Microsoft Corporation.
' 
' This Visual Basic code example demonstrates the access of COM components  
' from the .NET Framework by use of a runtime callable wrapper (RCW) or late  
' binding, and the host of ActiveX control in Windows Form.
' 
' A. Early binding of COM
' 
' Early-binding means that the compiler must have prior knowledge about COM  
' at compile time. Early-binding is supported by use of a runtime callable  
' wrapper (RCW). The wrapper turns the COM interfaces exposed by the COM 
' component into .NET Framework-compatible interfaces, and thus facilitates  
' communication between COM and .NET.
' 
' The Interop Assembly that is made from a type library is a form of RCW. It 
' defines managed interfaces that map to a COM-based type library and that a 
' managed client can interact with. To use an interop assembly in Visual 
' Studio, first add a reference to the corresponding COM component. Visual 
' Studio will automatically generate a local copy of the interop assembly. 
' Type Library Importer (Tlbimp.exe) is a standalone tool to convert the type 
' definitions found within a COM type library into equivalent definitions in 
' a common language runtime assembly. 
' 
' Without the use of the interop assembly, a developer may write a custom RCW 
' and manually map the types exposed by the COM interface to .NET Framework-
' compatible types. 
' 
' B. Late binding of COM
' 
' Late-binding means that the compiler does not have any prior knowledge  
' about the methods and properties in the COM component and it is delayed  
' until runtime. The advantage of late binding is that you do not have to use 
' an RCW (or build/ship a custom Interop Assembly), and it is more version 
' agnostic. The disadvantage is that it is more difficult to program this in 
' Visual C# than it is in Visual Basic, and late binding still suffers the   
' performance hit of having to find DISPIDs at runtime.
' 
' We do late binding in Visual Basic through .NET reflection (
' System.Type.GetTypeFromProgID, System.Activator.CreateInstance, 
' System.Type.InvokeMember, etc), or using the Visual Basic specific 
' late-binding syntax:
' 
'   Dim simpleObj As Object
' 	simpleObj.FloatProperty = 1.2F
' 
' The latter is much eaiser for developers to write. The general practice is 
' to first write early-binding Visual Basic code by importing the type 
' library. The developers can benefit from the intellisense of the type 
' information in the process. Then remove the interop assembly, change the 
' type of the COM object to System.Object, and instantiate the object using 
' CreateObject("[ProgID]"). No change is necessary for the access of the 
' component's properties and methods.
' 
' C. Host of ActiveX control
' 
' Hosting an ActiveX control requires a RCW of the AcitveX COM object, and a 
' class that inherits from System.Windows.Forms.AxHost to wrap the ActiveX 
' COM object and expose it as fully featured Windows Forms controls. The 
' stuff can be automatically done using the Windows Forms ActiveX Control  
' Importer (Aximp.exe) or Visual Studio Windows Forms design environment.
' 
' The ActiveX Control Importer generates a class that is derived from the 
' AxHost class, and compiles it into a library file (DLL) that can be added 
' as a reference to your application. Alternatively, you can use the /source 
' switch with the ActiveX Control Importer and a C# file is generated for 
' your AxHost derived class. You can then make changes to the code and 
' recompile it into a library file. 
' 
' If you are using Visual Studio as your Windows Forms design environment, 
' you can make an ActiveX control available to your application by adding the 
' ActiveX control to your Toolbox. To accomplish this, right-click the 
' Toolbox, select Choose Items, then browse to the ActiveX control's .OCX or 
' .DLL file. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"

Imports System.Threading
Imports System.Runtime.InteropServices

#End Region


Module MainModule

    Sub Main()

        Dim thread As Thread = Nothing

        ' Early binding of the component through interop assembly
        thread = New Thread(AddressOf EarlyBindtoComponent)
        thread.SetApartmentState(ApartmentState.STA)
        thread.Start()
        thread.Join()   ' Block the main thread

        ' Late binding of the component using VB late-binding syntax
        thread = New Thread(AddressOf LateBindtoComponent)
        thread.SetApartmentState(ApartmentState.STA)
        thread.Start()
        thread.Join()   ' Block the main thread

        ' Windows Form hosts an ActiveX control
        thread = New Thread(AddressOf HostActiveXControl)
        thread.SetApartmentState(ApartmentState.STA)
        thread.Start()
        thread.Join()   ' Block the main thread

    End Sub


#Region "Early Binding"

    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    ' Create an ATLDllCOMServer.SimpleObject COM object with events.
    ' 

    Dim WithEvents simpleObj As New ATLDllCOMServerLib.SimpleObject

    Sub EarlyBindtoComponent()

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Consume the properties and the methods of the COM object.
        ' 

        Try
            ' Set the property: FloatProperty, which raises the event
            ' FloatPropertyChanging.
            Console.WriteLine("Set FloatProperty = {0,0:f2}", 1.2F)
            simpleObj.FloatProperty = 1.2F

            ' Get the property: FloatProperty.
            Console.WriteLine("Get FloatProperty = {0,0:f2}", _
                              simpleObj.FloatProperty)

            ' Call the method: HelloWorld, that returns a string.
            Dim strResult As String = simpleObj.HelloWorld()
            Console.WriteLine("Call HelloWorld => {0}", strResult)

            ' Call the method: GetProcessThreadID, that outputs two integers.
            Console.WriteLine("The client process and thread: {0}, {1}", _
                    NativeMethod.GetCurrentProcessId(), _
                    NativeMethod.GetCurrentThreadId())

            Dim processId, threadId As Integer
            simpleObj.GetProcessThreadID(processId, threadId)
            Console.WriteLine("Call GetProcessThreadID => {0}, {1}", _
                    processId, threadId)

            Console.WriteLine()
        Catch ex As Exception
            Console.WriteLine("The server throws the error: {0}", ex.Message)
            If Not ex.InnerException Is Nothing Then
                Console.WriteLine("Description: {0}", ex.InnerException.Message)
            End If
        End Try


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Release the COM object.
        ' It is strongly recommended against using ReleaseComObject to 
        ' manually release an RCW object that represents a COM component 
        ' unless you absolutely have to. We should generally let CLR release 
        ' the COM object in the garbage collector.
        ' Ref: http://blogs.msdn.com/yvesdolc/archive/2004/04/17/115379.aspx
        ' 

        Marshal.FinalReleaseComObject(simpleObj)

    End Sub

    Sub simpleObj_FloatPropertyChanging(ByVal NewValue As Single, _
                                        ByRef Cancel As Boolean) _
                                        Handles simpleObj.FloatPropertyChanging
        ' OK or cancel the change of FloatProperty
        Cancel = (DialogResult.Cancel = MessageBox.Show( _
            String.Format("FloatProperty is being changed to {0,0:f2}", NewValue), _
            "ATLDllCOMServer!FloatPropertyChanging", MessageBoxButtons.OKCancel))
    End Sub
#End Region


#Region "Late Binding"

    Sub LateBindtoComponent()

        Try

            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Create an ATLDllCOMServer.SimpleObject COM object.
            ' 

            Dim simpleObj As Object = CreateObject( _
            "ATLDllCOMServer.SimpleObject")


            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Consume the properties and the methods of the COM object.
            ' 

            ' Set the property: FloatProperty, which raises the event
            ' FloatPropertyChanging.
            Console.WriteLine("Set FloatProperty = {0,0:f2}", 1.2F)
            simpleObj.FloatProperty = 1.2F

            ' Get the property: FloatProperty.
            Console.WriteLine("Get FloatProperty = {0,0:f2}", _
                              simpleObj.FloatProperty)

            ' Call the method: HelloWorld, that returns a string.
            Dim strResult As String = simpleObj.HelloWorld()
            Console.WriteLine("Call HelloWorld => {0}", strResult)

            ' Call the method: GetProcessThreadID, that outputs two integers.
            Console.WriteLine("The client process and thread: {0}, {1}", _
                    NativeMethod.GetCurrentProcessId(), _
                    NativeMethod.GetCurrentThreadId())

            Dim processId, threadId As Integer
            simpleObj.GetProcessThreadID(processId, threadId)
            Console.WriteLine("Call GetProcessThreadID => {0}, {1}", _
                    processId, threadId)

            Console.WriteLine()


            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Release the COM object.
            ' It is strongly recommended against using ReleaseComObject to 
            ' manually release an RCW object that represents a COM component 
            ' unless you absolutely have to. We should generally let CLR  
            ' release the COM object in the garbage collector.
            ' http://blogs.msdn.com/yvesdolc/archive/2004/04/17/115379.aspx
            ' 

            Marshal.FinalReleaseComObject(simpleObj)

        Catch ex As Exception
            Console.WriteLine("The server throws the error: {0}", ex.Message)
            If Not ex.InnerException Is Nothing Then
                Console.WriteLine("Description: {0}", ex.InnerException.Message)
            End If
        End Try

    End Sub

#End Region


#Region "ActiveX Control"

    Sub HostActiveXControl()

        Application.EnableVisualStyles()
        Application.SetCompatibleTextRenderingDefault(False)
        Application.Run(New MainForm())

    End Sub

#End Region

End Module
