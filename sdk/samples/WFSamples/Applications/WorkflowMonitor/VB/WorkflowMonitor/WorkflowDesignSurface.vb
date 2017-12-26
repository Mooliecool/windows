'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
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
'---------------------------------------------------------------------

Imports System.ComponentModel.Design
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.ComponentModel.Compiler

Namespace WorkflowMonitor
    Friend NotInheritable Class WorkflowDesignSurface
        Inherits DesignSurface

        Friend Sub New(ByVal memberCreationService As IMemberCreationService)
            Me.ServiceContainer.AddService(GetType(ITypeProvider), New TypeProvider(Me.ServiceContainer), True)
            Me.ServiceContainer.AddService(GetType(IMemberCreationService), memberCreationService)
            Me.ServiceContainer.AddService(GetType(IMenuCommandService), New MenuCommandService(Me.ServiceContainer))
        End Sub

    End Class
End Namespace

