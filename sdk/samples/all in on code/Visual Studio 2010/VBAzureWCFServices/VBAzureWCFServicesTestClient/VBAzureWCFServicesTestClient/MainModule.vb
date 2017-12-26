Imports System.ServiceModel
Imports WCFContract

Public Module MainModule
    Sub Main(ByVal args() As String)
        Talk2WebRole()
        Talk2WorkroleViaWebRole()
        'Talk2Workrole();
    End Sub


    Private Sub Talk2WebRole()
        Dim cc As New ServiceReference1.ContractClient()
        Dim result = cc.GetRoleInfo()
        Console.WriteLine(result)
        Console.ReadLine()
    End Sub

    Private Sub Talk2WorkroleViaWebRole()
        Dim cc As New ServiceReference2.ContractClient()
        Dim result = cc.GetRoleInfo()
        Console.WriteLine(result)
        Console.ReadLine()
    End Sub

    Private Sub Talk2Workrole()
        Dim factory As ChannelFactory(Of WCFContract.IContract)
        Dim channel As WCFContract.IContract

        ' You have to modify the endpoint address to fit yours.
        Dim endpoint As New EndpointAddress("net.tcp://d1818141d3994b2c9b7d696d1a50ccd1.cloudapp.net:10000/External")

        Dim binding As New NetTcpBinding(SecurityMode.None, False)
        factory = New ChannelFactory(Of WCFContract.IContract)(binding)

        channel = factory.CreateChannel(endpoint)

        Console.WriteLine(channel.GetRoleInfo())
        Console.Read()
    End Sub

End Module
