''---------------------------------------------------------------------
''  This file is part of the Microsoft .NET Framework SDK Code Samples.
'' 
''  Copyright (C) Microsoft Corporation.  All rights reserved.
'' 
''This source code is intended only as a supplement to Microsoft
''Development Tools and/or on-line documentation.  See these other
''materials for detailed information regarding Microsoft code samples.
'' 
''THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
''KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
''IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
''PARTICULAR PURPOSE.
''---------------------------------------------------------------------
Imports System.Collections.Specialized
Imports System.Reflection
Imports System.Linq
Imports System.Xml.Linq
Imports System.IO
Imports System.Globalization


''' <summary>
''' Class to Read /Write Configuration Settings from the App Config file
''' </summary>
Class ConfigurationManager

    ''' <summary>
    ''' private NameValueCollection that stores the key value config settings
    ''' </summary>
    Private Shared appSettingsValue As NameValueCollection

    ''' <summary>
    ''' Public property that returns the app config settings
    ''' </summary>
    Public Shared Property AppSettings() As NameValueCollection
        Get
            'if not loaded, then load app config settings
            If appSettingsValue Is Nothing Then
                LoadApplicationSettings()
            End If

            Return appSettingsValue
        End Get
        Set(ByVal value As NameValueCollection)
            appSettingsValue = value
        End Set
    End Property

    ''' <summary>
    ''' Save the new set of config settings back to the app config file.
    ''' </summary>
    Public Shared Sub Save()

        'if no settings to save, just return
        If (AppSettings Is Nothing) Then Return

        'get the config file name
        Dim name As String = String.Format(CultureInfo.CurrentCulture, "{0}.config", Assembly.GetExecutingAssembly().GetName().CodeBase)

        'check for existence
        'if config file does not exist, then create new one , else load existing one
        Dim doc As XDocument = Nothing

        If (Not File.Exists(name)) Then
            doc = New XDocument(New XElement("configuration", New XElement("appSettings")))
        Else
            doc = XDocument.Load(name)
        End If

        'retrieve the appSettings element in the XML app config file
        Dim appSetttingsElement As XElement = doc.Element("configuration").Element("appSettings")

        'remove all the nodes in the appSettings Elements
        appSetttingsElement.RemoveNodes()

        'create a new XElement with the key value pairs as child nodes.
        Dim newAppSettingsElement As XElement = New XElement("appSettings", _
                                                From key In AppSettings.Keys.OfType(Of String)() _
                                                Select New XElement("add", New XAttribute("key", key), _
                                                             New XAttribute("value", AppSettings(key))))

        'replace the existing appSettings element with the newly created one
        appSetttingsElement.ReplaceWith(newAppSettingsElement)

        'finally save the xml back to the app config file.
        doc.Save(name)

    End Sub



    ''' <summary>
    ''' Loads the key value pairs on appSettings Element to a NameValueCollection 
    ''' </summary>
    Private Shared Sub LoadApplicationSettings()
        AppSettings = New NameValueCollection()

        'config file name is {applicationName}.exe.config
        'Assembly.GetExecutingAssembly().GetName().CodeBase returns full path of the current executable
        Dim name As String = String.Format(CultureInfo.CurrentCulture, "{0}.config", Assembly.GetExecutingAssembly().GetName().CodeBase)

        'check if the config file exists.
        If (Not File.Exists(name)) Then Return


        'load the configuration file
        Dim doc As XDocument = XDocument.Load(name)


        'check existence configuration element and existence of the appSettings element inside configuration element
        If (Not doc.Element("configuration") Is Nothing And _
            Not doc.Element("configuration").Element("appSettings") Is Nothing) Then

            'select the elements inside appSettings section named add. 
            'the add element will have two attributes 1)key 2)value
            Dim q = From e In doc.Element("configuration").Element("appSettings").Elements() _
                        Where Not e.Attribute("key") Is Nothing And Not e.Attribute("value") Is Nothing _
                        Select New With {.key = e.Attribute("key").Value, .value = e.Attribute("value").Value}


            'loop through every element and add the key ,value pair to the NamedValueCollection
            For Each v In q
                AppSettings.Add(v.key, v.value)
            Next

        End If


    End Sub


End Class
