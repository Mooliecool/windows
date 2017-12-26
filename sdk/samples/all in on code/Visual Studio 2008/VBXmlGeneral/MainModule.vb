'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBXmlGeneral
' Copyright (c) Microsoft Corporation.
' 
' This VB.NET sample project shows how to read a XML file by using 
' XmlTextReader or XmlNodeReader. It also shows, instead of using 
' forward-only reader, how to read, modify, and update Xml element using the 
' XmlDocument class. This class will load the whole document into memory for 
' modification and we can save the modified XML file to the file system.
' 
' The XML file used by the demo has this format:
' 
' <catalog>
'  <book id="bk101">
'    <author>Gambardella, Matthew</author>
'    <title>XML Developer's Guide</title>
'    <genre>Computer</genre>
'    <price>44.95</price>
'    <publish_date>2000-10-01</publish_date>
'    <description>
'      An in-depth look at creating applications
'      with XML.
'    </description>
'  </book>
'  <book>
'   ...
'  </book>
' <catalog>
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"

Imports System.Xml

#End Region


Module MainModule

    Sub Main()

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Read XML document using the XmlTextReader class.
        ' 

        ' The XmlTextReader acts as a reader pointer that only moves forward.
        ' Because it always moves forward and read a piece of data into 
        ' memory buffer, it has better performance than the XmlDocument 
        ' class which loads the whole document into memory.

        Console.WriteLine("Loading XML using XmlTextReader...")

        Dim xmlTextReader As New XmlTextReader("Books.xml")
        xmlTextReader.WhitespaceHandling = WhitespaceHandling.None

        Do While xmlTextReader.Read
            If xmlTextReader.Name = "book" Then

                Console.WriteLine((xmlTextReader.GetAttribute("id") & ": "))

                xmlTextReader.Read()
                Dim author = xmlTextReader.ReadElementContentAsString
                Dim title = xmlTextReader.ReadElementContentAsString
                Dim genre = xmlTextReader.ReadElementContentAsString
                Dim price = xmlTextReader.ReadElementContentAsString
                Dim publishDate = xmlTextReader.ReadElementContentAsString
                Dim description = xmlTextReader.ReadElementContentAsString

                Console.WriteLine(genre & " book """ & title & """ written by """ _
                                  & author & """, published on " & publishDate)
                Console.WriteLine(description)

            End If
        Loop

        xmlTextReader.Close()


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Read XML document using the XmlDocument and XmlNodeReader classes.
        ' 

        ' XmlNodeReader is similar to XmlTextReader but accepts an XmlNode 
        ' instance as target to read. The following code shows how to use 
        ' XmlDocument and XmlNodeReader to retrieve XML information. It is 
        ' also a forward-only reader.

        Console.WriteLine("Loading XML using XmlDocument & XmlNodeReader...")

        Dim xmlDocument As New XmlDocument
        xmlDocument.Load("Books.xml")
        Dim xmlNodes As XmlNodeList = xmlDocument.GetElementsByTagName("book")
        For Each node As XmlNode In xmlNodes

            Console.WriteLine(node.Attributes("id").Value & ":")

            Dim xmlNodeReader As New XmlNodeReader(node)
            xmlNodeReader.Read()
            xmlNodeReader.Read()
            Dim author = xmlNodeReader.ReadElementContentAsString
            Dim title = xmlNodeReader.ReadElementContentAsString
            Dim genre = xmlNodeReader.ReadElementContentAsString
            Dim price = xmlNodeReader.ReadElementContentAsString
            Dim publishDate = xmlNodeReader.ReadElementContentAsString
            Dim description = xmlNodeReader.ReadElementContentAsString

            Console.WriteLine(genre & " book """ & title & """ written by """ _
                                  & author & """, published on " & publishDate)
            Console.WriteLine(description)

        Next


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Make changes to the XmlDocument.
        ' 

        ' Modify a node value by first calling SelectSingleNode to navigate 
        ' to that node and by setting its InnerText property to change its 
        ' content.

        Dim nodeToModify As XmlNode = _
        xmlDocument.DocumentElement.SelectSingleNode("book/genre")
        nodeToModify.InnerText = "XML Tech"

        ' Add a new XML node. In XML programming, we always call 
        ' XMLDocument.Create*** to create an attribute or element. After 
        ' that, we can add it into where we want by calling Node.AppendChild
        Dim newElement As XmlElement = xmlDocument.CreateElement("book")
        Dim newAttribute As XmlAttribute = xmlDocument.CreateAttribute("id")
        newAttribute.Value = "bk103"
        Dim authorElement As XmlElement = xmlDocument.CreateElement("author")
        authorElement.InnerText = "Mark Russinovich,David Solomon,Alex Ionecsu"
        Dim titleElement As XmlElement = xmlDocument.CreateElement("title")
        titleElement.InnerText = "Windows Internals, 5th edition"
        Dim genreElement As XmlElement = xmlDocument.CreateElement("genre")
        genreElement.InnerText = "Windows Server 2008"
        Dim priceElement As XmlElement = xmlDocument.CreateElement("price")
        priceElement.InnerText = "69.99"
        Dim publishDateElement As XmlElement = xmlDocument.CreateElement("publish_date")
        publishDateElement.InnerText = "2009-6-17"
        Dim descriptionElement As XmlElement = xmlDocument.CreateElement("description")
        descriptionElement.InnerText = "Windows Internals, 5th edition is the" & _
            " update to Windows Internals, 4th edition to cover Windows Vista" & _
            " and Windows Server 2008 (32-bit and 64-bit)."

        newElement.Attributes.Append(newAttribute)
        newElement.AppendChild(authorElement)
        newElement.AppendChild(titleElement)
        newElement.AppendChild(genreElement)
        newElement.AppendChild(priceElement)
        newElement.AppendChild(publishDateElement)
        newElement.AppendChild(descriptionElement)
        xmlDocument.DocumentElement.AppendChild(newElement)

        ' Save the changes
        xmlDocument.Save("Modified Books.xml")

        ' XmlLDocument does not have Close or Dispose method because it is 
        ' an in-memory representation of an XML document. Once read, the 
        ' file is no-longer needed.

    End Sub

End Module