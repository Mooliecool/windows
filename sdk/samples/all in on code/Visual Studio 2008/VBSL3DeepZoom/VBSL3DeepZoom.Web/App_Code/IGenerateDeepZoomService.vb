'****************************** Module Header ******************************\
' Module Name:  Page.cs
' Project:      CSSL3DeepZoomProject
' Copyright (c) Microsoft Corporation.
' 
' The interface for the WCF service.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 9/4/2009 17:12 Yilun Luo Created
'***************************************************************************/

Imports System.ServiceModel

' NOTE: If you change the class name "IGenerateDeepZoomService" here, you must also update the reference to "IGenerateDeepZoomService" in Web.config.
<ServiceContract(Namespace:="http://code.fx/")> _
Public Interface IGenerateDeepZoomService

	<OperationContract()> _
   Function PrepareDeepZoom(ByVal forceGenerateDeepZoom As Boolean) As Boolean

End Interface
