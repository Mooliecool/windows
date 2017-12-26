=============================================================================
        WINDOWS APPLICATION : VBReceiveWM_COPYDATA Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

Inter-process Communication (IPC) based on the Windows message WM_COPYDATA is 
a mechanism for exchanging data among Windows applications in the local 
machine. The receiving application must be a Windows application. The data 
being passed must not contain pointers or other references to objects not 
accessible to the application receiving the data. While WM_COPYDATA is being 
sent, the referenced data must not be changed by another thread of the 
sending process. The receiving application should consider the data read-only. 
If the receiving application must access the data after SendMessage returns, 
it needs to copy the data into a local buffer.

This code sample demonstrates receiving a custom data structure (MyStruct) 
from the sending application (VBSendWM_COPYDATA) by handling WM_COPYDATA 
messages.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the WM_COPYDATA samples.

Step1. After you successfully build the VBSendWM_COPYDATA and 
VBReceiveWM_COPYDATA sample projects in Visual Studio 2008, you will get the 
applications: VBSendWM_COPYDATA.exe and VBReceiveWM_COPYDATA.exe. 

Step2. Run VBSendWM_COPYDATA.exe and VBReceiveWM_COPYDATA.exe. In 
VBSendWM_COPYDATA, input the Number and Message fields.

  Number: 123456
  Message: Hello World

Then click the SendMessage button. The number and the message will be sent 
to VBReceiveWM_COPYDATA through a WM_COPYDATA message. When 
VBReceiveWM_COPYDATA receives the message, the application extracts the 
number and the message, and display them in the window.


/////////////////////////////////////////////////////////////////////////////
Sample Relation:
(The relationship between the current sample and the rest samples in 
Microsoft All-In-One Code Framework http://1code.codeplex.com)

VBSendWM_COPYDATA -> VBReceiveWM_COPYDATA
VBSendWM_COPYDATA sends data to VBReceiveWM_COPYDATA through the WM_COPYDATA 
message.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Override the WndProc method in the Windows Form.

    Protected Overrides Sub WndProc(ByRef m As System.Windows.Forms.Message)
    End Sub

2. Handle the WM_COPYDATA message in WndProc 

    If (m.Msg = WM_COPYDATA) Then
    End If
        
3. Get the COPYDATASTRUCT struct from lParam of the WM_COPYDATA message, and 
fetch the data (MyStruct object) from COPYDATASTRUCT.lpData.

    ' Get the COPYDATASTRUCT struct from lParam.
    Dim cds As COPYDATASTRUCT = m.GetLParam(GetType(COPYDATASTRUCT))

    ' If the size matches
    If (cds.cbData = Marshal.SizeOf(GetType(MyStruct))) Then
        ' Marshal the data from the unmanaged memory block to a MyStruct 
        ' managed struct.
        Dim myStruct As MyStruct = Marshal.PtrToStructure(cds.lpData, _
            GetType(MyStruct))
    End If

4. Display the data in the form.


/////////////////////////////////////////////////////////////////////////////
References:

WM_COPYDATA Message
http://msdn.microsoft.com/en-us/library/ms649011.aspx


/////////////////////////////////////////////////////////////////////////////
