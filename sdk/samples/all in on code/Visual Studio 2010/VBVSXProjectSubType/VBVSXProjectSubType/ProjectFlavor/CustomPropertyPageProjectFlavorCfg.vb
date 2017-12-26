'************************** Module Header ******************************'
' Module Name:  CustomPropertyPageProjectFlavorCfg.vb
' Project:      VBVSXProjectSubType
' Copyright (c) Microsoft Corporation.
' 
' The project subtype configuration object implements IVsProjectFlavorCfg to 
' give the project subtype access to various configuration interfaces.
' 
' The base project asks the project subtype to create an IVsProjectFlavorCfg 
' object corresponding to each of its (project subtype's) configuration objects.
' The IVsProjectFlavorCfg objects can then, for example, implement IPersistXMLFragment
' to manage persistence into the project file. The base project system calls 
' IPersistXMLFragment methods InitNew, Load and Save as appropriate.
' 
' The IVsProjectFlavorCfg object can hold and add a referenced pointer to the 
' IVsCfg object of the base project.
' 
' The IPersistXMLFragment is used to persist non-build related data in free-form XML. 
' The methods provided by IPersistXMLFragment are called by Visual Studio whenever 
' Visual Studio needs to persist non-build related data in the project file.
' http://msdn.microsoft.com/en-us/library/bb166204.aspx
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Imports System.Runtime.InteropServices
Imports System.Xml
Imports Microsoft.VisualStudio
Imports Microsoft.VisualStudio.Shell.Interop

Namespace ProjectFlavor

    <ComVisible(False)>
Friend Class CustomPropertyPageProjectFlavorCfg
        Implements IVsProjectFlavorCfg, IPersistXMLFragment

        ' This allow the property page to map a IVsCfg object (the baseConfiguration) 
        ' to an actual instance of CustomPropertyPageProjectFlavorCfg.
        Private Shared _mapIVsCfgToCustomPropertyPageProjectFlavorCfg As _
            New Dictionary(Of IVsCfg, CustomPropertyPageProjectFlavorCfg)()

        Friend Shared Function GetCustomPropertyPageProjectFlavorCfgFromIVsCfg(ByVal configuration As IVsCfg) _
            As CustomPropertyPageProjectFlavorCfg

            If _mapIVsCfgToCustomPropertyPageProjectFlavorCfg.ContainsKey(configuration) Then
                Return CType(_mapIVsCfgToCustomPropertyPageProjectFlavorCfg(configuration), 
                    CustomPropertyPageProjectFlavorCfg)
            Else
                Throw New ArgumentOutOfRangeException(
                    "Cannot find configuration in mapIVsCfgToSpecializedCfg.")
            End If
        End Function

        ' Specify whether this is changed.
        Private _isDirty As Boolean = False

        ' Store all the Properties of this configuration.
        Private _propertiesList As New Dictionary(Of String, String)()

        ' Store the CustomPropertyPageProjectFlavor object when this instance is
        ' initialized.
        ' This field is not used to customize the PropertyPage, but it is useful to
        ' customize the debug behavior.
        Private _project As IVsHierarchy

        ' The IVsCfg object of the base project.
        Private _baseConfiguration As IVsCfg

        ' The IVsProjectFlavorCfg object of the inner project subtype. 
        Private _innerConfiguration As IVsProjectFlavorCfg

        ''' <summary>
        ''' Get or set a Property.
        ''' </summary>
        ''' <param name="propertyName"></param>
        ''' <returns></returns>
        Default Public Property Item(ByVal propertyName As String) As String
            Get
                If _propertiesList.ContainsKey(propertyName) Then
                    Return _propertiesList(propertyName)
                End If
                Return String.Empty
            End Get
            Set(ByVal value As String)
                ' Don't do anything if there isn't any real change
                If Me(propertyName) = value Then
                    Return
                End If

                _isDirty = True
                If _propertiesList.ContainsKey(propertyName) Then
                    _propertiesList.Remove(propertyName)
                End If
                _propertiesList.Add(propertyName, value)
            End Set
        End Property

        ''' <summary>
        ''' Initialize the CustomPropertyPageProjectFlavorCfg instance.
        ''' </summary>
        Public Sub Initialize(ByVal project As CustomPropertyPageProjectFlavor,
                              ByVal baseConfiguration As IVsCfg,
                              ByVal innerConfiguration As IVsProjectFlavorCfg)
            Me._project = project
            Me._baseConfiguration = baseConfiguration
            Me._innerConfiguration = innerConfiguration
            _mapIVsCfgToCustomPropertyPageProjectFlavorCfg.Add(baseConfiguration, Me)
        End Sub

#Region "IVsProjectFlavorCfg Members"

        ''' <summary>
        ''' Provides access to a configuration interfaces such as IVsBuildableProjectCfg2
        ''' or IVsDebuggableProjectCfg.
        ''' </summary>
        ''' <param name="iidCfg">IID of the interface that is being asked</param>
        ''' <param name="ppCfg">Object that implement the interface</param>
        ''' <returns>HRESULT</returns>
        Public Function get_CfgType(ByRef iidCfg As Guid,
                                    <Out()> ByRef ppCfg As IntPtr) _
                                As Integer Implements IVsProjectFlavorCfg.get_CfgType
            ppCfg = IntPtr.Zero
            If Me._innerConfiguration IsNot Nothing Then
                Return Me._innerConfiguration.get_CfgType(iidCfg, ppCfg)
            End If
            Return VSConstants.S_OK
        End Function

        ''' <summary>
        ''' Closes the IVsProjectFlavorCfg object.
        ''' </summary>
        ''' <returns></returns>
        Public Function Close() As Integer Implements IVsProjectFlavorCfg.Close

            _mapIVsCfgToCustomPropertyPageProjectFlavorCfg.Remove(Me._baseConfiguration)
            Dim hr As Integer = Me._innerConfiguration.Close()

            If Me._project IsNot Nothing Then
                Me._project = Nothing
            End If

            If Me._baseConfiguration IsNot Nothing Then
                If Marshal.IsComObject(Me._baseConfiguration) Then
                    Marshal.ReleaseComObject(Me._baseConfiguration)
                End If
                Me._baseConfiguration = Nothing
            End If

            If Me._innerConfiguration IsNot Nothing Then
                If Marshal.IsComObject(Me._innerConfiguration) Then
                    Marshal.ReleaseComObject(Me._innerConfiguration)
                End If
                Me._innerConfiguration = Nothing
            End If
            Return hr
        End Function

#End Region

#Region "IPersistXMLFragment Members"

        ''' <summary>
        ''' Implement the InitNew method to initialize the project extension properties
        ''' and other build-independent data. This method is called if there is no XML
        ''' configuration data present in the project file.
        ''' </summary>
        ''' <param name="guidFlavor">
        ''' GUID of the project subtype.
        ''' </param>
        ''' <param name="storage"></param>
        ''' <returns></returns>
        Public Function InitNew(ByRef guidFlavor As Guid, ByVal storage As UInteger) _
            As Integer Implements IPersistXMLFragment.InitNew

            'Return,if it is our guid.
            If IsMyFlavorGuid(guidFlavor) Then
                Return VSConstants.S_OK
            End If

            'Forward the call to inner flavor(s).
            If Me._innerConfiguration IsNot Nothing _
                AndAlso TypeOf Me._innerConfiguration Is IPersistXMLFragment Then
                Return (CType(Me._innerConfiguration, IPersistXMLFragment)).InitNew(
                    guidFlavor, storage)
            End If

            Return VSConstants.S_OK
        End Function

        ''' <summary>
        ''' Implement the IsFragmentDirty method to determine whether an XML fragment has 
        ''' changed since it was last saved to its current file.
        ''' </summary>
        ''' <param name="storage">
        ''' Storage type of the file in which the XML is persisted. Values are taken
        ''' from _PersistStorageType enumeration.
        ''' </param>
        ''' <param name="pfDirty"></param>
        ''' <returns></returns>
        Public Function IsFragmentDirty(ByVal storage As UInteger,
                                        <Out()> ByRef pfDirty As Integer) _
                                    As Integer Implements IPersistXMLFragment.IsFragmentDirty
            pfDirty = 0
            Select Case storage
                ' Specifies storage file type to project file.
                Case CUInt(_PersistStorageType.PST_PROJECT_FILE)
                    If _isDirty Then
                        pfDirty = pfDirty Or 1
                    End If

                    ' Specifies storage file type to user file.
                Case CUInt(_PersistStorageType.PST_USER_FILE)
                    ' Do not store anything in the user file.
            End Select

            ' Forward the call to inner flavor(s) 
            If pfDirty = 0 AndAlso Me._innerConfiguration IsNot Nothing _
                AndAlso TypeOf Me._innerConfiguration Is IPersistXMLFragment Then
                Return (CType(Me._innerConfiguration, IPersistXMLFragment)).IsFragmentDirty(
                    storage, pfDirty)
            End If
            Return VSConstants.S_OK

        End Function

        ''' <summary>
        ''' Implement the Load method to load the XML data from the project file.
        ''' </summary>
        ''' <param name="guidFlavor">
        ''' GUID of the project subtype.
        ''' </param>
        ''' <param name="storage">
        ''' Storage type of the file in which the XML is persisted. Values are taken
        ''' from _PersistStorageType enumeration.
        ''' </param>
        ''' <param name="pszXMLFragment">
        ''' String containing the XML fragment.
        ''' </param>
        Public Function Load(ByRef guidFlavor As Guid,
                             ByVal storage As UInteger,
                             ByVal pszXMLFragment As String) _
                         As Integer Implements IPersistXMLFragment.Load
            If IsMyFlavorGuid(guidFlavor) Then
                Select Case storage
                    Case CUInt(_PersistStorageType.PST_PROJECT_FILE)
                        ' Load our data from the XML fragment.
                        Dim doc As New XmlDocument()
                        Dim node As XmlNode = doc.CreateElement(Me.GetType().Name)
                        node.InnerXml = pszXMLFragment
                        If node Is Nothing OrElse node.FirstChild Is Nothing Then
                            Exit Select
                        End If

                        ' Load all the properties
                        For Each child As XmlNode In node.FirstChild.ChildNodes
                            _propertiesList.Add(child.Name, child.InnerText)
                        Next child
                    Case CUInt(_PersistStorageType.PST_USER_FILE)
                        ' Do not store anything in the user file.
                End Select
            End If

            ' Forward the call to inner flavor(s)
            If Me._innerConfiguration IsNot Nothing _
                AndAlso TypeOf Me._innerConfiguration Is IPersistXMLFragment Then
                Return (CType(Me._innerConfiguration, IPersistXMLFragment)).Load(
                    guidFlavor, storage, pszXMLFragment)
            End If

            Return VSConstants.S_OK

        End Function


        ''' <summary>
        ''' Implement the Save method to save the XML data in the project file.
        ''' </summary>
        ''' <param name="guidFlavor">
        ''' GUID of the project subtype.
        ''' </param>
        ''' <param name="storage">
        ''' Storage type of the file in which the XML is persisted. Values are taken
        ''' from _PersistStorageType enumeration.
        ''' </param>
        ''' <param name="pszXMLFragment">
        ''' String containing the XML fragment.
        ''' </param>
        ''' <param name="fClearDirty">
        ''' Indicates whether to clear the dirty flag after the save is complete. 
        ''' If true, the flag should be cleared. If false, the flag should be left 
        ''' unchanged.
        ''' </param>
        ''' <returns></returns>
        Public Function Save(ByRef guidFlavor As Guid,
                             ByVal storage As UInteger,
                             <Out()> ByRef pbstrXMLFragment As String,
                             ByVal fClearDirty As Integer) _
                         As Integer Implements IPersistXMLFragment.Save

            pbstrXMLFragment = Nothing

            If IsMyFlavorGuid(guidFlavor) Then
                Select Case storage
                    Case CUInt(_PersistStorageType.PST_PROJECT_FILE)

                        ' Create XML for our data (a string and a bool).
                        Dim doc As New XmlDocument()
                        Dim root As XmlNode = doc.CreateElement(Me.GetType().Name)

                        For Each [property] As KeyValuePair(Of String, String) In _propertiesList
                            Dim node As XmlNode = doc.CreateElement([property].Key)
                            node.AppendChild(doc.CreateTextNode([property].Value))
                            root.AppendChild(node)
                        Next [property]

                        doc.AppendChild(root)
                        ' Get XML fragment representing our data
                        pbstrXMLFragment = doc.InnerXml

                        If fClearDirty <> 0 Then
                            _isDirty = False
                        End If
                    Case CUInt(_PersistStorageType.PST_USER_FILE)
                        ' Do not store anything in the user file.
                End Select
            End If

            ' Forward the call to inner flavor(s)
            If Me._innerConfiguration IsNot Nothing _
                AndAlso TypeOf Me._innerConfiguration Is IPersistXMLFragment Then
                Return (CType(Me._innerConfiguration, IPersistXMLFragment)).Save(
                    guidFlavor, storage, pbstrXMLFragment, fClearDirty)
            End If

            Return VSConstants.S_OK
        End Function

#End Region

        Private Function IsMyFlavorGuid(ByRef guidFlavor As Guid) As Boolean
            Return guidFlavor.Equals(
                CustomPropertyPageProjectFactory.CustomPropertyPageProjectFactoryGuid)
        End Function

    End Class
End Namespace
