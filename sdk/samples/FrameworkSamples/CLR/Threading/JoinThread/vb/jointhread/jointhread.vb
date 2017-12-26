'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
' =====================================================================
'
'  File:      JoinThread.vb
'
'  Summary:   Demonstrates how to wait for another thread to exit
'
'=====================================================================*/

Option Explicit On 
Option Strict On


Imports System
Imports System.Threading

class App 

   Shared Sub MyThreadMethod() 
      Console.WriteLine("This is the secondary thread running.")
   End sub

   Shared Sub Main() 
      Console.WriteLine("This is the primary thread running.")
      ' MyThreadMethod is the secondary thread's entry point.
      Dim t as New Thread(New ThreadStart(AddressOf MyThreadMethod))

      ' Start the thread
      t.Start()

      ' Wait for the thread to exit
      t.Join()
      Console.WriteLine("The secondary thread has terminated.")
    End Sub
End class
