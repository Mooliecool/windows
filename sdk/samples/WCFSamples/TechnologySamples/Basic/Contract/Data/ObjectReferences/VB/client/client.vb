' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples
    'The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    'Client implementation code.
    Class Client

        Public Shared Sub Main()

            ' Create a client
            Dim sn As New SocialNetworkClient

            ' Create my social network
            Dim helena As New Person
            With helena
                .Name = "Helena"
                .Age = 35
                .Location = "Chicago"
            End With

            Dim andrew As New Person
            With andrew
                .Name = "Andrew"
                .Age = 25
                .Location = "Seattle"
            End With

            Dim paul As New Person
            With paul
                .Name = "Paul"
                .Age = 28
                .Location = "Bostan"
            End With
            Dim sarah As New Person
            With sarah
                .Name = "Sarah"
                .Age = 27
                .Location = "Seattle"
            End With
            Dim richard As New Person
            With richard
                .Name = "Richard"
                .Age = 40
                .Location = "New York"
            End With

            helena.Friends = New Person() {andrew, sarah, richard, paul}
            andrew.Friends = New Person() {helena, richard, paul}
            paul.Friends = New Person() {sarah, richard}
            sarah.Friends = New Person() {andrew}
            richard.Friends = New Person() {helena}

            ' Call the GetPeopleInNetwork to find people in Andrew's network
            Dim network As Person() = sn.GetPeopleInNetwork(andrew)
            Console.WriteLine("Andrew's Network: ")
            Dim p As Person
            For Each p In network
                Console.WriteLine(" " + p.Name)
            Next

            ' Call GetMutualFriends to find Helena's mutual friends
            Dim mutual As Person() = sn.GetMutualFriends(helena)
            Console.WriteLine("Helena's Mutual Friends: ")
            For Each p In mutual
                Console.WriteLine(" " + p.Name)
            Next

            ' Call GetcommonFriends to find common friends of Helena and Andrew
            Dim common As Person() = sn.GetCommonFriends(New Person() {helena, andrew})
            Console.WriteLine("Helena and Andew's Common Friends: ")
            For Each p In common
                Console.WriteLine(" " + p.Name)
            Next

            Console.ReadLine()

        End Sub

    End Class

End Namespace