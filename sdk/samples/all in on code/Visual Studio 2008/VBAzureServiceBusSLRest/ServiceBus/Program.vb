'****************************** Module Header ******************************\
' Module Name:	Program.vb
' Project:		VBAzureServiceBusSLRest
' Copyright (c) Microsoft Corporation.
' 
' This is the Main entry point. It hosts the service,
' and exposes it to the internet using Service Bus.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports System.Configuration
Imports Microsoft.ServiceBus
Imports System.ServiceModel.Web

Module Program

	Sub Main()
		Dim serviceNamespace As String = ConfigurationManager.AppSettings.Item("serviceNamespace")
		' By default, tranport level security is required for all *RelayBindings; hence, using https is required.
		Dim address As Uri = ServiceBusEnvironment.CreateServiceUri("https", serviceNamespace, "")
		Dim host As New WebServiceHost(GetType(RestService), New Uri() {address})
		host.Open()

		Console.WriteLine("Copy the following address into a browser to see the cross domain policy file:")
		Console.WriteLine((address.AbsoluteUri & "clientaccesspolicy.xml"))
		Console.WriteLine()
		Console.WriteLine("The WCF REST service is being listened on:")
		Console.WriteLine((address.AbsoluteUri & "file/"))
		Console.WriteLine()
		Console.WriteLine("Press [Enter] to exit")
		Console.ReadLine()
		host.Close()
	End Sub

End Module
