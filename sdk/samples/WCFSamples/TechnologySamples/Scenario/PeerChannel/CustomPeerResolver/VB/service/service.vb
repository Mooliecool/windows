' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections.Generic
Imports System.Collections.ObjectModel
Imports System.Configuration
Imports System.ServiceModel

' Custom peer resolver service implementation. Implements registration, unregistration, and resolution 
' of mesh ID and associated addresses. Mesh ID identifies the mesh (a named collection of nodes identified 
' by the mesh ID). An example of mesh ID is chatMesh and identifies the host name portion of an EndpointAddresss,
' net.p2p://chatMesh/servicemodesamples/chat"
' Mesh IDs are expected to be unique and if multiple applications use the same custom peer resolver service, 
' they should choose different mesh IDs to avoid conflict.

' If you are unfamiliar with new concepts used in this sample, refer to the WCF Basic\GettingStarted sample.

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface ICustomPeerResolver

        <OperationContract()> _
        Function Register(ByVal meshId As String, ByVal nodeAddresses As PeerNodeAddress) As Integer
        <OperationContract()> _
        Sub Unregister(ByVal registrationId As Integer)
        <OperationContract()> _
        Sub Update(ByVal registrationId As Integer, ByVal updatedNodeAddress As PeerNodeAddress)
        <OperationContract()> _
        Function Resolve(ByVal meshId As String, ByVal maxAddresses As Integer) As PeerNodeAddress()

    End Interface

    ' Service class which implements the service contract
    ' It is a singleton service
    <ServiceBehavior(InstanceContextMode:=InstanceContextMode.[Single])> _
    Public Class CustomPeerResolverService
        Implements ICustomPeerResolver

        ' The resolver maintains two dictionaries:
        ' a) A registration table that is keyed by the registration Id (int). 
        ' b) A mesh Id table that is keyed by the mesh ID and has a dictionary of registrationId, PeerNodeAddress 
        '      in the value part. The value part is a dictionary rather than a list to facilitate faster lookups.
        ' A more sophisticated cache could be used if the number of meshIds is large or if the number of nodes 
        ' in the meshes is large.

        ' Registration class. Each time Register() is called, a Registration object is created and added to
        ' Registration table.
        Class Registration

            Private m_meshId As String
            Private m_nodeAddress As PeerNodeAddress

            Public Sub New(ByVal meshId As String, ByVal nodeAddress As PeerNodeAddress)

                Me.m_meshId = meshId
                Me.m_nodeAddress = nodeAddress

            End Sub

            Public ReadOnly Property MeshId() As String

                Get

                    Return Me.m_meshId

                End Get

            End Property

            Public Property NodeAddress() As PeerNodeAddress

                Get

                    Return Me.m_nodeAddress

                End Get
                Set(ByVal value As PeerNodeAddress)

                    Me.m_nodeAddress = value

                End Set

            End Property

        End Class

        Shared registrationTable As New Dictionary(Of Integer, Registration)()
        Shared meshIdTable As New Dictionary(Of String, Dictionary(Of Integer, PeerNodeAddress))()
        Shared nextRegistrationId As Integer = 0

        ReadOnly random As New Random()

        ' Register a node address for a given mesh ID
        Public Function Register(ByVal meshId As String, ByVal nodeAddress As PeerNodeAddress) As Integer Implements ICustomPeerResolver.Register

            Dim newMeshId As Boolean = False
            Dim registrationId As Integer
            Dim registration As New Registration(meshId, nodeAddress)

            ' Add the new registration to the registration table; update meshIdTable with the newly registered nodeAddress
            SyncLock registrationTable

                registrationId = System.Math.Max(System.Threading.Interlocked.Increment(nextRegistrationId), nextRegistrationId - 1)
                SyncLock meshIdTable

                    ' Update the meshId table
                    Dim addresses As New Dictionary(Of Integer, PeerNodeAddress)
                    If Not meshIdTable.TryGetValue(meshId, addresses) Then

                        ' MeshID doesn't exist and needs to be added to meshIdTable
                        newMeshId = True
                        addresses = New Dictionary(Of Integer, PeerNodeAddress)()
                        meshIdTable(meshId) = addresses

                    End If
                    addresses(registrationId) = nodeAddress

                    ' Add an entry to the registration table
                    registrationTable(registrationId) = New Registration(meshId, nodeAddress)

                End SyncLock

            End SyncLock
            If newMeshId Then
                Console.WriteLine("Registered new meshId {0}", meshId)
            End If
            Return registrationId

        End Function

        ' Unregister addresses for a given mesh ID
        Public Sub Unregister(ByVal registrationId As Integer) Implements ICustomPeerResolver.Unregister

            Dim unregisteredMeshId As Boolean = False

            Dim registration As Registration
            SyncLock registrationTable

                ' We expect the registration to exist. Find and remove it from registrationTable.
                registration = registrationTable(registrationId)
                registrationTable.Remove(registrationId)

                ' Remove the registration from the meshIdTable
                SyncLock meshIdTable

                    Dim addresses As Dictionary(Of Integer, PeerNodeAddress) = meshIdTable(registration.MeshId)
                    addresses.Remove(registrationId)

                    ' If this was the only node registered for the mesh, remove the meshId entry
                    If addresses.Count = 0 Then

                        meshIdTable.Remove(registration.MeshId)
                        unregisteredMeshId = True

                    End If

                End SyncLock

            End SyncLock
            If unregisteredMeshId Then
                Console.WriteLine("Unregistered meshId {0}", registration.MeshId)
            End If

        End Sub

        ' Update an existing registration (can be called if the machine IP addresses change etc.)
        Public Sub Update(ByVal registrationId As Integer, ByVal updatedNodeAddress As PeerNodeAddress) Implements ICustomPeerResolver.Update

            Dim registration As Registration
            SyncLock registrationTable

                ' We expect the registration to exist.
                registration = registrationTable(registrationId)

                ' Update registrationTable and meshIdTable
                SyncLock meshIdTable

                    Dim addresses As Dictionary(Of Integer, PeerNodeAddress) = meshIdTable(registration.MeshId)
                    addresses(registrationId) = updatedNodeAddress

                End SyncLock

                registrationTable(registrationId).NodeAddress = updatedNodeAddress

            End SyncLock

        End Sub

        ' Resolve addresses for a given mesh ID
        Public Function Resolve(ByVal meshId As String, ByVal maxAddresses As Integer) As PeerNodeAddress() Implements ICustomPeerResolver.Resolve

            If maxAddresses <= 0 Then
                Throw New ArgumentOutOfRangeException("maxAddresses")
            End If

            Dim copyOfAddresses() As PeerNodeAddress
            SyncLock meshIdTable

                Dim addresses As New Dictionary(Of Integer, PeerNodeAddress)

                ' Check if the meshId is known and if so, make a copy of the addresses to process outside of lock
                If meshIdTable.TryGetValue(meshId, addresses) Then

                    ' you may want to avoid the copy approach in case of a large mesh
                    copyOfAddresses = New PeerNodeAddress(addresses.Count - 1) {}
                    addresses.Values.CopyTo(copyOfAddresses, 0)

                Else
                    copyOfAddresses = New PeerNodeAddress() {}
                End If

            End SyncLock

            ' If there are <= maxAddresses for this meshId, return them all
            If copyOfAddresses.Length <= maxAddresses Then

                Return copyOfAddresses

            Else

                ' Otherwise, pick maxAddresses at random
                Dim indices As New List(Of Integer)(maxAddresses)
                ' may want to use a more performant data structure if maxAddresses is larger
                While indices.Count < maxAddresses

                    Dim listIndex As Integer = Me.random.[Next]() Mod copyOfAddresses.Length
                    If Not indices.Contains(listIndex) Then
                        indices.Add(listIndex)
                        ' check if we've already seen this index
                    End If

                End While
                Dim randomAddresses As PeerNodeAddress() = New PeerNodeAddress(maxAddresses) {}
                For i As Integer = 0 To randomAddresses.Length - 1
                    randomAddresses(i) = copyOfAddresses(indices(i))
                Next
                Return randomAddresses

            End If

        End Function

        ' Host the service within this EXE console application.
        Public Shared Sub Main()

            ' Get base address from app settings in configuration
            Dim baseAddress As New Uri(ConfigurationManager.AppSettings("baseAddress"))

            ' Create a ServiceHost for the CalculatorService type and provide the base address.
            Using serviceHost As New ServiceHost(GetType(CustomPeerResolverService), baseAddress)

                ' Open the ServiceHostBase to create listeners and start listening for messages.
                serviceHost.Open()

                ' The service can now be accessed.
                Console.WriteLine("The service is ready.")
                Console.WriteLine("Press <ENTER> to terminate service.")
                Console.WriteLine()
                Console.ReadLine()

            End Using
            Console.WriteLine("The service has shutdown.")

        End Sub

    End Class

End Namespace
