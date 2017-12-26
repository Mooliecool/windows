'***************************** Module Header ******************************\
'* Module Name:	TravelDataService.svc.vb
'* Project:		AzureBingMaps
'* Copyright (c) Microsoft Corporation.
'* 
'* The data service.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************

Imports System.Data.Services
Imports System.Data.Services.Common
Imports System.ServiceModel
Imports System.ServiceModel.Activation
Imports System.ServiceModel.Web
Imports Microsoft.SqlServer.Types

Namespace AzureBingMaps.WebRole.DataService
    ' Recommend set IncludeExceptionDetailInFaults to false for production deployments.
    ' But set it to true during development to debug the application.
    <ServiceBehavior(IncludeExceptionDetailInFaults:=True, AddressFilterMode:=AddressFilterMode.Any)> _
    <AspNetCompatibilityRequirements(RequirementsMode:=AspNetCompatibilityRequirementsMode.Allowed)> _
    Public Class TravelDataService
        Inherits DataService(Of TravelDataServiceContext)
        Public Shared Sub InitializeService(ByVal config As DataServiceConfiguration)
            config.SetEntitySetAccessRule("*", EntitySetRights.All)
            config.SetServiceOperationAccessRule("*", ServiceOperationRights.All)
            config.DataServiceBehavior.MaxProtocolVersion = DataServiceProtocolVersion.V2
        End Sub

        ''' <summary>
        ''' Expose a custom operation from the WCF Data Services.
        ''' Not used by clients in this version of the sample.
        ''' </summary>
        <WebGet()> _
        Public Function DistanceBetweenPlaces(ByVal latitude1 As Double, ByVal latitude2 As Double, ByVal longitude1 As Double, ByVal longitude2 As Double) As Double
            Dim geography1 As SqlGeography = SqlGeography.Point(latitude1, longitude1, 4326)
            Dim geography2 As SqlGeography = SqlGeography.Point(latitude2, longitude2, 4326)
            Return geography1.STDistance(geography2).Value
        End Function
    End Class
End Namespace