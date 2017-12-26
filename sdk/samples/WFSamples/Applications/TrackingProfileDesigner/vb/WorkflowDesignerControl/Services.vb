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

Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Text
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Design
Imports System.Globalization

#Region "IdentifierCreationService"
Friend NotInheritable Class IdentifierCreationService
    Implements IIdentifierCreationService
    Dim serviceProvider As IServiceProvider = Nothing

    Friend Sub New(ByVal serviceProvider As IServiceProvider)
        Me.serviceProvider = serviceProvider
    End Sub

    Sub ValidateIdentifier(ByVal activity As Activity, ByVal identifier As String) Implements IIdentifierCreationService.ValidateIdentifier
        If identifier Is Nothing Then
            Throw New ArgumentNullException("identifier")
        End If
        If activity Is Nothing Then
            Throw New ArgumentNullException("activity")
        End If


        If (activity.Name.ToLower(CultureInfo.InvariantCulture).Equals(identifier.ToLower(CultureInfo.InvariantCulture))) Then
            Return
        End If

        Dim identifiers As New ArrayList()
        Dim rootActivity As Activity = GetRootActivity(activity)
        identifiers.AddRange(GetIdentifiersInCompositeActivity(TryCast(rootActivity, CompositeActivity)))
        identifiers.Sort()
        If identifiers.BinarySearch(identifier.ToLower(CultureInfo.InvariantCulture), StringComparer.OrdinalIgnoreCase) >= 0 Then
            Throw New ArgumentException(String.Format("Duplicate Component Identifier {0}", identifier))
        End If
    End Sub

    Sub EnsureUniqueIdentifiers(ByVal parentActivity As CompositeActivity, ByVal childActivities As ICollection) Implements IIdentifierCreationService.EnsureUniqueIdentifiers
        If parentActivity Is Nothing Then
            Throw New ArgumentNullException("parentActivity")
        End If
        If childActivities Is Nothing Then
            Throw New ArgumentNullException("childActivities")
        End If

        Dim allActivities As New ArrayList()

        Dim activities As New Queue(childActivities)
        While (activities.Count > 0)
            Dim activity As Activity = CType(activities.Dequeue(), Activity)
            If (TypeOf activity Is CompositeActivity) Then
                For Each child As Activity In (CType(activity, CompositeActivity)).Activities
                    activities.Enqueue(child)
                Next
            End If

            ' If we are moving activities, we need not regenerate their identifiers
            If (CType(activity, IComponent)).Site IsNot Nothing Then
                Continue While
            End If
            allActivities.Add(activity)
        End While

        ' get the root activity
        Dim rootActivity As CompositeActivity = TryCast(GetRootActivity(parentActivity), CompositeActivity)
        Dim identifiers As New ArrayList() ' all the identifiers in the workflow
        identifiers.AddRange(GetIdentifiersInCompositeActivity(rootActivity))

        For Each activity As Activity In allActivities
            Dim finalIdentifier As String = activity.Name

            ' now loop until we find a identifier that hasn't been used.
            Dim baseIdentifier As String = GetBaseIdentifier(activity)
            Dim index As Integer = 0

            identifiers.Sort()
            While finalIdentifier Is Nothing Or finalIdentifier.Length = 0 Or identifiers.BinarySearch(finalIdentifier.ToLower(CultureInfo.InvariantCulture), StringComparer.OrdinalIgnoreCase) >= 0
                index += 1
                finalIdentifier = String.Format("{0}{1}", baseIdentifier, index)
            End While

            ' add new identifier to collection 
            identifiers.Add(finalIdentifier)
            activity.Name = finalIdentifier
        Next
    End Sub

    Shared Function GetIdentifiersInCompositeActivity(ByVal compositeActivity As CompositeActivity) As IList
        Dim identifiers As New ArrayList()
        If compositeActivity IsNot Nothing Then
            identifiers.Add(compositeActivity.Name)
            Dim allChildren As IList(Of Activity) = GetAllNestedActivities(compositeActivity)
            For Each activity As Activity In allChildren
                identifiers.Add(activity.Name)
            Next
        End If
        Return ArrayList.ReadOnly(identifiers)
    End Function

    Private Shared Function GetBaseIdentifier(ByVal activity As Activity) As String
        Dim baseIdentifier As String = activity.GetType().Name
        Dim b As New StringBuilder(baseIdentifier.Length)
        For i As Integer = 0 To baseIdentifier.Length - 1
            If (Char.IsUpper(baseIdentifier(i)) And (i = 0 Or i = baseIdentifier.Length - 1 Or Char.IsUpper(baseIdentifier(i + 1)))) Then
                b.Append(Char.ToLower(baseIdentifier(i), CultureInfo.InvariantCulture))
            Else
                b.Append(baseIdentifier.Substring(i))
            End If
            next
        Return b.ToString()
    End Function

    Shared Function GetRootActivity(ByVal activity As Activity) As Activity
        If activity Is Nothing Then
            Throw New ArgumentException("activity")
        End If

        While activity.Parent IsNot Nothing
            activity = activity.Parent
        End While

        Return activity
    End Function

    Shared Function GetAllNestedActivities(ByVal compositeActivity As CompositeActivity) As Activity()
        If compositeActivity Is Nothing Then
            Throw New ArgumentNullException("compositeActivity")
        End If

        Dim nestedActivities As New ArrayList()
        Dim compositeActivities As New Queue()
        compositeActivities.Enqueue(compositeActivity)
        While compositeActivities.Count > 0
            Dim compositeActivity2 As CompositeActivity = CType(compositeActivities.Dequeue(), CompositeActivity)

            For Each activity As Activity In compositeActivity2.Activities
                nestedActivities.Add(activity)
                If TypeOf activity Is CompositeActivity Then
                    compositeActivities.Enqueue(activity)
                End If
                next

            For Each activity As Activity In compositeActivity2.EnabledActivities
                If Not (nestedActivities.Contains(activity)) Then
                    nestedActivities.Add(activity)
                    If TypeOf activity Is CompositeActivity Then
                        compositeActivities.Enqueue(activity)
                    End If
                End If
                next
        End While
        Return CType(nestedActivities.ToArray(GetType(Activity)), Activity())
    End Function

End Class
#End Region

#Region "WorkflowCompilerOptionsService"
Friend Class WorkflowCompilerOptionsService
    Implements IWorkflowCompilerOptionsService

    Public Sub New()

    End Sub

#Region "IWorkflowCompilerOptionsService Members"
    Public ReadOnly Property RootNamespace() As String Implements IWorkflowCompilerOptionsService.RootNamespace
        Get
            Return String.Empty
        End Get
    End Property

    Public ReadOnly Property Language() As String Implements IWorkflowCompilerOptionsService.Language
        Get
            Return "VisualBasic"
        End Get
    End Property

    Public ReadOnly Property CheckTypes() As Boolean Implements IWorkflowCompilerOptionsService.CheckTypes
        Get
            Throw New Exception("The method or operation is not implemented.")
        End Get
    End Property
#End Region
End Class

#End Region

#Region "Class EventBindingService"
Friend Class EventBindingService
    Implements IEventBindingService

    Public Sub New()
    End Sub

    Public Function CreateUniqueMethodName(ByVal component As IComponent, ByVal e As EventDescriptor) As String Implements IEventBindingService.CreateUniqueMethodName
        Return e.DisplayName
    End Function

    Public Function GetCompatibleMethods(ByVal e As EventDescriptor) As ICollection Implements IEventBindingService.GetCompatibleMethods
        Return New ArrayList()
    End Function

    Public Function GetEvent(ByVal propertyDescriptor As PropertyDescriptor) As EventDescriptor Implements IEventBindingService.GetEvent
        Return iif(TypeOf propertyDescriptor Is EventPropertyDescriptor, (CType(propertyDescriptor, EventPropertyDescriptor)).EventDescriptor, Nothing)
    End Function

    Public Function GetEventProperties(ByVal events As EventDescriptorCollection) As PropertyDescriptorCollection Implements IEventBindingService.GetEventProperties
        Dim descriptors() As PropertyDescriptor = Nothing
        Return New PropertyDescriptorCollection(descriptors, True)
    End Function

    Public Function GetEventProperty(ByVal e As EventDescriptor) As PropertyDescriptor Implements IEventBindingService.GetEventProperty
        Return New EventPropertyDescriptor(e)
    End Function

    Public Function ShowCode() As Boolean Implements IEventBindingService.ShowCode
        Return False
    End Function

    Public Function ShowCode(ByVal lineNumber As Integer) As Boolean Implements IEventBindingService.ShowCode
        Return False
    End Function

    Public Function ShowCode(ByVal component As IComponent, ByVal e As EventDescriptor) As Boolean Implements IEventBindingService.ShowCode
        Return False
    End Function

    Private Class EventPropertyDescriptor
        Inherits PropertyDescriptor

        Dim eventDescriptorValue As EventDescriptor

        Public ReadOnly Property EventDescriptor() As EventDescriptor
            Get
                Return Me.eventDescriptorValue
            End Get
        End Property


        Public Sub New(ByVal eventDescriptor As EventDescriptor)
            MyBase.New(eventDescriptor, Nothing)
            Me.eventDescriptorValue = eventDescriptor
        End Sub

        Public Overrides ReadOnly Property ComponentType() As Type
            Get
                Return Me.eventDescriptorValue.ComponentType
            End Get
        End Property

        Public Overrides ReadOnly Property PropertyType() As Type
            Get
                Return Me.eventDescriptorValue.EventType
            End Get
        End Property

        Public Overrides ReadOnly Property IsReadOnly() As Boolean
            Get
                Return True
            End Get
        End Property

        Public Overrides Function CanResetValue(ByVal Component As Object) As Boolean
            Return False
        End Function

        Public Overrides Function GetValue(ByVal component As Object) As Object
            Return Nothing
        End Function

        Public Overrides Sub ResetValue(ByVal component As Object)
        End Sub

        Public Overrides Sub SetValue(ByVal component As Object, ByVal value As Object)
        End Sub

        Public Overrides Function ShouldSerializeValue(ByVal component As Object) As Boolean
            Return False
        End Function
    End Class

End Class

#End Region
