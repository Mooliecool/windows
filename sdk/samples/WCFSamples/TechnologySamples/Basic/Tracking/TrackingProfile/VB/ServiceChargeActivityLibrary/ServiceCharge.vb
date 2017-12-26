' Copyright (c) Microsoft Corporation. All rights reserved.

 '---------------------------------------------------------------------
 ' This file is part of the Windows Workflow Foundation SDK Code Samples.
 '
 ' Copyright (C) Microsoft Corporation. All rights reserved.
 '
 'This source code is intended only as a supplement to Microsoft
 'Development Tools and/or on-line documentation. See these other
 'materials for detailed information regarding Microsoft code samples.
 '
 'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
 'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 'PARTICULAR PURPOSE.
 '---------------------------------------------------------------------

 Imports System
 Imports System.Collections.Generic
 Imports System.Text
 Imports System.ComponentModel
 Imports System.Workflow.ComponentModel.Design
 Imports System.Workflow.ComponentModel


 Namespace Microsoft.Samples.Workflow.ServiceChargeActivityLibrary
     Public Class ServiceCharge
         Inherits Activity
         Private feeValue As Double
        
         <Category("Service Charges")> _
         Public Property Fee() As Double
             Get
                 Return feeValue
             End Get
             Set
                 feeValue = value
             End Set
         End Property
        
        
         Protected Overloads Overrides Function Execute(ByVal executionContext As ActivityExecutionContext) As ActivityExecutionStatus
             Console.WriteLine("You will be charged a service fee of {0}", feeValue)
             Return MyBase.Execute(executionContext)
            
         End Function
     End Class
 End Namespace