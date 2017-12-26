' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel
Imports System.Runtime.Serialization
Imports System.Collections.Generic

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface ISocialNetwork
        <OperationContract()> _
        Function GetPeopleInNetwork(ByVal p As Person) As List(Of Person)
        <OperationContract()> _
        Function GetMutualFriends(ByVal p As Person) As List(Of Person)
        <OperationContract()> _
        Function GetCommonFriends(ByVal p As List(Of Person)) As List(Of Person)
    End Interface


    <DataContract([IsReference]:=True)> _
    Public Class Person
        Private nameField As String
        Private locationField As String
        Private genderField As String
        Private ageField As Integer
        Private friendsField As List(Of Person)
        Public Sub New()
        End Sub
        <System.Runtime.Serialization.DataMemberAttribute()> _
        Public Property Age() As Integer
            Get
                Return Me.ageField
            End Get
            Set(ByVal value As Integer)
                Me.ageField = value
            End Set
        End Property
        <System.Runtime.Serialization.DataMemberAttribute()> _
        Public Property Name() As String
            Get
                Return Me.nameField
            End Get
            Set(ByVal value As String)
                Me.nameField = value
            End Set
        End Property
        <System.Runtime.Serialization.DataMemberAttribute()> _
        Public Property Location() As String
            Get
                Return Me.locationField
            End Get
            Set(ByVal value As String)
                Me.locationField = value
            End Set
        End Property
        <System.Runtime.Serialization.DataMemberAttribute()> _
        Public Property Gender() As String
            Get
                Return Me.genderField
            End Get
            Set(ByVal value As String)
                Me.genderField = value
            End Set
        End Property
        <System.Runtime.Serialization.DataMemberAttribute()> _
        Public Property Friends() As List(Of Person)
            Get
                If (Me.friendsField Is Nothing) Then
                    Me.friendsField = New List(Of Person)()
                End If
                Return Me.friendsField
            End Get
            Set(ByVal value As List(Of Person))
                Me.friendsField = value
            End Set
        End Property

    End Class


    ' Service class which implements the service contract.
    Public Class SocialNetworkService
        Implements ISocialNetwork
        Public Function GetPeopleInNetwork(ByVal p As Person) As List(Of Person) _
        Implements ISocialNetwork.GetPeopleInNetwork
            Dim people As List(Of Person) = New List(Of Person)()
            ListPeopleInNetwork(p, people)
            GetPeopleInNetwork = people
        End Function

        Public Function GetMutualFriends(ByVal p As Person) As List(Of Person) _
        Implements ISocialNetwork.GetMutualFriends
            Dim mutual As List(Of Person) = New List(Of Person)
            Dim f As Person
            For Each f In p.Friends
                If (f.Friends.Contains(p)) Then
                    mutual.Add(f)
                End If
            Next
            GetMutualFriends = mutual
        End Function

        Public Function GetCommonFriends(ByVal people As List(Of Person)) As List(Of Person) _
        Implements ISocialNetwork.GetCommonFriends
            Dim common As List(Of Person) = New List(Of Person)
            Dim f As Person
            For Each f In people.Item(0).Friends
                If (people.Item(1).Friends.Contains(f)) Then
                    common.Add(f)
                End If
            Next
            GetCommonFriends = common
        End Function

        Public Sub ListPeopleInNetwork(ByVal p As Person, ByVal lst As List(Of Person))
            If (Not lst.Contains(p)) Then
                lst.Add(p)
                Dim f As Person
                For Each f In p.Friends
                    ListPeopleInNetwork(f, lst)
                Next
            End If
        End Sub
    End Class


End Namespace