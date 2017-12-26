'*********************************** Module Header ***********************************'
' Module Name:  MainModule.vb
' Project:      VBWindowsServiceRecoveryProperty
' Copyright (c) Microsoft Corporation.
' 
' VBWindowsServiceRecoveryProperty example demonstrates how to use ChangeServiceConfig2
' to configure the service "Recovery" properties in C#. This example operates all the 
' options you can see on the service "Recovery" tab, including setting the “Enable 
' actions for stops with errors” option in Windows Vista and later operating systems. 
' This example also include how to grant the shut down privilege to the process, so 
' that we can configure a special option in the "Recovery" tab - "Restart Computer 
' Options...".
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************************'

Imports System
Imports System.Collections.Generic


Module MainModule
    Sub Main()
        Dim FailureActions As New List(Of SC_ACTION)()

        ' First Failure Actions and Delay (msec).
        FailureActions.Add(New SC_ACTION() With { _
            .Type = DirectCast(SC_ACTION_TYPE.RestartService, Integer), _
            .Delay = 1000 * 60 * 5 _
        })

        ' Second Failure Actions and Delay (msec).
        FailureActions.Add(New SC_ACTION() With { _
            .Type = DirectCast(SC_ACTION_TYPE.Run_Command, Integer), _
            .Delay = 1000 * 2 _
        })

        ' Subsequent Failures Actions and Delay (msec).
        FailureActions.Add(New SC_ACTION() With { _
            .Type = DirectCast(SC_ACTION_TYPE.RebootComputer, Integer), _
            .Delay = 1000 * 60 * 3 _
        })

        ' Configure service recovery property.
        Try
            ServiceRecoveryProperty.ChangeRecoveryProperty( _
                "VBWindowsService", FailureActions, 60 * 60 * 24 * 4, _
                "C:\Windows\System32\cmd.exe /help /fail=%1%", _
                True, "reboot message")

            Console.WriteLine("The service recovery property is modified successfully")
        Catch ex As Exception
            Console.WriteLine(ex.Message)
        End Try
    End Sub
End Module