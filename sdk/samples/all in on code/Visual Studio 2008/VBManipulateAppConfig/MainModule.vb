'*********************************** Module Header ***********************************\
'* Module Name:  MainModule.vb
'* Project:      VBManipulateAppConfig
'* Copyright (c) Microsoft Corporation.
'* 
'* VBManipulateAppConfig example demonstrates how to use ConfigurationManager to create,
'* read and delete node in the config file content and use XmlDocument to update
'* the config file content at runtime.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\*************************************************************************************


#Region "Imports directives"
Imports System
#End Region


Namespace ManipulateAppConfig
    Module MainModule
        Sub Main()
            Console.WriteLine("Original node in the config file.")

            ' Read nodes before operating the config file content.
            ManipulateAppConfig.ReadNode()

            Console.WriteLine("Create a new node in the config file.")

            ' Create a new node in the config file.
            ManipulateAppConfig.CreateNode("Setting3", "3")
            ManipulateAppConfig.ReadNode()

            Console.WriteLine("Update an existing node value.")

            ' Update an existing node value.
            ManipulateAppConfig.UpdateNode("Setting1", "New Value")
            ManipulateAppConfig.ReadNode()

            Console.WriteLine("Delete an existing node.")

            ' Delete an existing node.
            ManipulateAppConfig.DeleteNode("Setting2")
            ManipulateAppConfig.ReadNode()

            ' Pause to see the result.
            Console.ReadLine()
        End Sub
    End Module
End Namespace