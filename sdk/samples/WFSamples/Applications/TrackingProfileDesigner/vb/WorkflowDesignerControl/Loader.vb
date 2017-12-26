'---------------------------------------------------------------------
'   This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'   Copyright (C) Microsoft Corporation.  All rights reserved.
' 
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
' 
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System.ComponentModel.Design
Imports System.ComponentModel.Design.Serialization
Imports System.Drawing.Design
Imports System.IO
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.ComponentModel.Serialization
Imports System.XML

#Region "WorkflowLoader"
Friend NotInheritable Class WorkflowLoader
    Inherits WorkflowDesignerLoader
    Dim xamlValue As String = String.Empty

    Friend Sub New()
    End Sub

    Protected Overrides Sub Initialize()
        MyBase.Initialize()

        Dim host As IDesignerLoaderHost = LoaderHost
        If host IsNot Nothing Then
            host.RemoveService(GetType(IIdentifierCreationService))
            host.AddService(GetType(IIdentifierCreationService), New IdentifierCreationService(host))
            host.AddService(GetType(IMenuCommandService), New MenuCommandService(host))
            Dim typeProvider As New TypeProvider(host)
            typeProvider.AddAssemblyReference(GetType(String).Assembly.Location)
            host.AddService(GetType(ITypeProvider), typeProvider, True)
            host.AddService(GetType(IEventBindingService), New EventBindingService())
        End If
    End Sub

    Public Overrides Sub Dispose()
        Try
            Dim host As IDesignerLoaderHost = LoaderHost
            If host IsNot Nothing Then
                host.RemoveService(GetType(IIdentifierCreationService))
                host.RemoveService(GetType(IMenuCommandService))
                host.RemoveService(GetType(IToolboxService))
                host.RemoveService(GetType(ITypeProvider), True)
                host.RemoveService(GetType(IWorkflowCompilerOptionsService))
                host.RemoveService(GetType(IEventBindingService))
            End If
        Finally
            MyBase.Dispose()
        End Try
    End Sub

    Public Overrides Sub ForceReload()
    End Sub

    Dim workflowTypeValue As Type = Nothing

    Public Property WorkflowType() As Type
        Get
            Return Me.workflowTypeValue
        End Get
        Set(ByVal value As Type)
            Me.workflowTypeValue = value
        End Set
    End Property


    Public Property Xaml() As String
        Get
            Return Me.xamlValue
        End Get

        Set(ByVal value As String)
            Me.xamlValue = value
        End Set
    End Property

    Public Overrides ReadOnly Property FileName() As String
        Get
            Return String.Empty
        End Get
    End Property

    Public Overrides Function GetFileReader(ByVal filePath As String) As TextReader
        Return New StreamReader(New FileStream(filePath, FileMode.OpenOrCreate))
    End Function

    Public Overrides Function GetFileWriter(ByVal filePath As String) As TextWriter
        ' close the handle
        Return New StreamWriter(New FileStream(filePath, FileMode.OpenOrCreate))
    End Function

    Protected Overrides Sub PerformLoad(ByVal serializationManager As IDesignerSerializationManager)
        Dim designerHost As IDesignerHost = CType(GetService(GetType(IDesignerHost)), IDesignerHost)
        Dim rootActivity As Activity = Nothing
        ' First see if we have a workflow type
        If WorkflowType IsNot Nothing Then
            rootActivity = CType(Activator.CreateInstance(WorkflowType), Activity)
        Else
            ' Create a text reader out of the doc data, and ask
            Dim reader As TextReader = New StringReader(Me.xamlValue)
            Try
                Using xmlReader As XmlReader = Xml.XmlReader.Create(reader)
                    Dim xamlSerializer As New WorkflowMarkupSerializer()
                    rootActivity = TryCast(xamlSerializer.Deserialize(xmlReader), Activity)
                End Using
            Finally
                reader.Close()
            End Try
        End If


        If rootActivity IsNot Nothing AndAlso designerHost IsNot Nothing Then
            AddObjectGraphToDesignerHost(designerHost, rootActivity)
        End If
    End Sub

    Public Overrides Sub Flush()
        PerformFlush(Nothing)
    End Sub

    Protected Overrides Sub PerformFlush(ByVal manager As IDesignerSerializationManager)
        Dim host As IDesignerHost = CType(GetService(GetType(IDesignerHost)), IDesignerHost)
        If host IsNot Nothing AndAlso host.RootComponent IsNot Nothing Then
            Dim rootActivity As Activity = TryCast(host.RootComponent, Activity)
            If rootActivity IsNot Nothing Then
                Dim writer As New StringWriter()
                Try
                    Using xmlWriter As XmlWriter = Xml.XmlWriter.Create(writer)
                        Dim xamlSerializer As New WorkflowMarkupSerializer()
                        xamlSerializer.Serialize(xmlWriter, rootActivity)
                    End Using
                Finally
                    writer.Close()
                End Try
                Xaml = writer.ToString()
            End If
        End If
    End Sub

    Private Shared Sub AddObjectGraphToDesignerHost(ByVal designerHost As IDesignerHost, ByVal activityObject As Activity)
        Dim Definitions_Class As New Guid("3FA84B23-B15B-4161-8EB8-37A54EFEEFC7")

        If (designerHost Is Nothing) Then
            Throw New ArgumentNullException("designerHost")
        End If
        If (activityObject Is Nothing) Then
            Throw New ArgumentNullException("activity")
        End If

        Dim rootSiteName As String = activityObject.QualifiedName
        If (activityObject.Parent Is Nothing) Then
            Dim fullClassName As String = TryCast(activityObject.UserData(Definitions_Class), String)
            If (fullClassName Is Nothing) Then
                fullClassName = activityObject.GetType().FullName
            End If
            rootSiteName = IIf(Not (fullClassName.LastIndexOf(".") = -1), _
                fullClassName.Substring(fullClassName.LastIndexOf(".") + 1), _
                fullClassName)
            designerHost.Container.Add(activityObject, rootSiteName)
        Else
            designerHost.Container.Add(activityObject, activityObject.QualifiedName)
        End If

        If (TypeOf activityObject Is CompositeActivity) Then
            For Each activity2 As Activity In GetNestedActivities(TryCast(activityObject, CompositeActivity))
                designerHost.Container.Add(activity2, activity2.QualifiedName)
            Next
        End If
    End Sub

    Private Shared Function GetNestedActivities(ByVal compositeActivity As CompositeActivity) As Activity()
        If (compositeActivity Is Nothing) Then
            Throw New ArgumentNullException("compositeActivity")
        End If

        Dim childActivities As IList(Of Activity) = Nothing
        Dim nestedActivities As New ArrayList()
        Dim compositeActivities As New Queue()
        compositeActivities.Enqueue(compositeActivity)
        While (compositeActivities.Count > 0)
            Dim compositeActivity2 As CompositeActivity = CType(compositeActivities.Dequeue(), CompositeActivity)
            childActivities = compositeActivity2.Activities

            For Each activity As Activity In childActivities
                nestedActivities.Add(activity)
                If (activity Is compositeActivity) Then
                    compositeActivities.Enqueue(activity)
                End If
            Next
        End While
        Return CType(nestedActivities.ToArray(GetType(Activity)), Activity())
    End Function

    Friend Shared Sub DestroyObjectGraphFromDesignerHost(ByVal designerHost As IDesignerHost, ByVal activity As Activity)
        If designerHost Is Nothing Then
            Throw New ArgumentNullException("designerHost")
        End If

        If activity Is Nothing Then
            Throw New ArgumentNullException("activity")
        End If

        designerHost.DestroyComponent(activity)

        If TypeOf activity Is CompositeActivity Then
            For Each activity2 As Activity In GetNestedActivities(TryCast(activity, CompositeActivity))
                designerHost.DestroyComponent(activity2)
            Next
        End If
    End Sub
End Class
#End Region
