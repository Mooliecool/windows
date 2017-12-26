'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBAutomateVisio
' Copyright (c) Microsoft Corporation.
' 
' The VBAutomateVisio sample demonstrates how to use Visio Object Model to 
' automate Office Visio 2007.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'


Module MainModule

    <STAThread()> _
    Sub Main()

        Solution1.AutomateVisio()

        Console.WriteLine()

        Solution2.AutomateVisio()

    End Sub

End Module
