'*********************************** Module Header ***********************************\
'* Module Name:  ManipulateAppConfig.vb
'* Project:      VBManipulateAppConfig
'* Copyright (c) Microsoft Corporation.
'* 
'* Use ConfigurationManager to create, read, update and delete node in the config file content or 
'* use XmlDocument to update the config file content at runtime.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\*************************************************************************************


#Region "Imports directives"
Imports System
Imports System.Configuration
Imports System.Xml
#End Region

Namespace ManipulateAppConfig
    Public Class ManipulateAppConfig
        ' Open the specified client configuration file as a Configuration object.
        ' To get the Configuration object that applies to all users, set userLevel to None.
        Private Shared config As Configuration = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None)

        ''' <summary>
        ''' Create a new node in config file.
        ''' </summary>
        ''' <param name="key">The key of the node.</param>
        ''' <param name="value">The value of the node.</param>
        Public Shared Sub CreateNode(ByVal key As String, ByVal value As String)
            ' Add an Application Setting.
            config.AppSettings.Settings.Add(key, value)

            ' Write the configuration settings contained within this Configuration object 
            ' to the current XML configuration file, even if the configuration was not modified.
            config.Save(ConfigurationSaveMode.Modified, True)

            ' Refresh the named section so the next time that it is retrieved it will be re-read from disk.
            ConfigurationManager.RefreshSection("appSettings")
        End Sub

        ''' <summary>
        ''' Read nodes in config file.
        ''' </summary>
        Public Shared Sub ReadNode()
            ' Enumerate settings information in configuration file.
            For Each keyValuecfg As KeyValueConfigurationElement In config.AppSettings.Settings
                Console.WriteLine([String].Format("Key: {0}, Value: {1}", keyValuecfg.Key, keyValuecfg.Value))
            Next
        End Sub

        ''' <summary>
        ''' Update node in config file.
        ''' </summary>
        ''' <param name="key">The key of the node.</param>
        ''' <param name="value">The new value of the node.</param>
        Public Shared Sub UpdateNode(ByVal key As String, ByVal value As String)
            config.AppSettings.Settings(key).Value = value
            config.Save(ConfigurationSaveMode.Modified, True)

            ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' ' another method
            ' ' Because there're some settings are read-only or cannot be modified successfully at runtime, 
            ' ' in order to modify the current application settings value, 
            ' ' we must use the XmlDocument class to directly update the 
            ' ' application configuration file as an XML document.
            '
            'Dim xmlDoc As New XmlDocument()
            '
            ' ' Load the application configuration file.
            'xmlDoc.Load(AppDomain.CurrentDomain.SetupInformation.ConfigurationFile)

            '' Enumerate nodes in configuration file.
            'For Each element As XmlElement In xmlDoc.DocumentElement
            '    If element.Name.Equals("appSettings") Then
            '        For Each node As XmlNode In element.ChildNodes
            '            If node.Attributes(0).Value.Equals(key) Then
            '                ' Update the node value.
            '                node.Attributes(1).Value = value
            '            End If
            '        Next
            '    End If
            'Next
            '
            ' ' Save the XML document to the specified file.
            'xmlDoc.Save(AppDomain.CurrentDomain.SetupInformation.ConfigurationFile)
            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

            ' Refresh the named section so the next time that it is retrieved it will be re-read from disk.
            ConfigurationManager.RefreshSection("appSettings")
        End Sub

        ''' <summary>
        ''' Delete node in config file.
        ''' </summary>
        ''' <param name="key">The key of the node.</param>
        Public Shared Sub DeleteNode(ByVal key As String)
            ' Remove the node.
            config.AppSettings.Settings.Remove(key)

            ' Write the configuration settings contained within this Configuration object 
            ' to the current XML configuration file, even if the configuration was not modified.
            config.Save(ConfigurationSaveMode.Modified, True)

            ' Refresh the named section so the next time that it is retrieved it will be re-read from disk.
            ConfigurationManager.RefreshSection("appSettings")
        End Sub
    End Class
End Namespace