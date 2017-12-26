'************************** Module Header ******************************'
' Module Name:  CustomPropertyPagePropertyStore.vb
' Project:      VBVSXProjectSubType
' Copyright (c) Microsoft Corporation.
' 
' The CustomPropertyPagePropertyStore Class implements the IPropertyStore 
' Interface. It is used to store the Properties of a PropertyPage object.
' 
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Imports VBVSXProjectSubType.PropertyPageBase
Imports Microsoft.VisualStudio.Shell.Interop

Namespace ProjectFlavor

    Public Class CustomPropertyPagePropertyStore
        Implements IDisposable, IPropertyStore

        Private _disposed As Boolean = False

        Private _configs As New List(Of CustomPropertyPageProjectFlavorCfg)()

        Public Event StoreChanged As StoreChangedDelegate _
            Implements IPropertyStore.StoreChanged

#Region "IPropertyStore Members"
        ''' <summary>
        ''' Use the data passed in to initialize the Properties. 
        ''' </summary>
        ''' <param name="dataObject">
        ''' This is normally only one our configuration object, which means that 
        ''' there will be only one elements in configs.
        ''' If it is null, we should release it.
        ''' </param>
        Public Sub Initialize(ByVal dataObjects() As Object) _
            Implements IPropertyStore.Initialize

            ' If we are editing multiple configuration at once, we may get multiple objects.
            For Each dataObject As Object In dataObjects
                If TypeOf dataObject Is IVsCfg Then
                    ' This should be our configuration object, so retrive the specific
                    ' class so we can access its properties.
                    Dim config As CustomPropertyPageProjectFlavorCfg =
                        CustomPropertyPageProjectFlavorCfg.GetCustomPropertyPageProjectFlavorCfgFromIVsCfg(CType(dataObject, IVsCfg))

                    If Not _configs.Contains(config) Then
                        _configs.Add(config)
                    End If
                End If
            Next dataObject
        End Sub

        ''' <summary>
        ''' Set the value of the specified property in storage.
        ''' </summary>
        ''' <param name="propertyName">Name of the property to set.</param>
        ''' <param name="propertyValue">Value to set the property to.</param>
        Public Sub Persist(ByVal propertyName As String, ByVal propertyValue As String) _
            Implements IPropertyStore.Persist

            ' If the value is null, make it empty.
            If propertyValue Is Nothing Then
                propertyValue = String.Empty
            End If

            For Each config As CustomPropertyPageProjectFlavorCfg In _configs
                ' Set the property
                config(propertyName) = propertyValue

            Next config

            RaiseEvent StoreChanged()

        End Sub

        ''' <summary>
        ''' Retreive the value of the specified property from storage
        ''' </summary>
        ''' <param name="propertyName">Name of the property to retrieve</param>
        ''' <returns></returns>
        Public Function PropertyValue(ByVal propertyName As String) As String _
            Implements IPropertyStore.PropertyValue

            Dim value As String = Nothing
            If _configs.Count > 0 Then
                value = _configs(0)(propertyName)
            End If
            For Each config As CustomPropertyPageProjectFlavorCfg In _configs
                If config(propertyName) <> value Then

                    ' multiple config with different value for the property
                    value = String.Empty

                    Exit For
                End If
            Next config

            Return value
        End Function

#End Region

#Region "IDisposable Members"

        Public Sub Dispose() _
            Implements IDisposable.Dispose, IPropertyStore.Dispose
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub

        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            ' Protect from being called multiple times.
            If _disposed Then
                Return
            End If

            If disposing Then
                _configs.Clear()
            End If
            _disposed = True
        End Sub
#End Region
    End Class
End Namespace
