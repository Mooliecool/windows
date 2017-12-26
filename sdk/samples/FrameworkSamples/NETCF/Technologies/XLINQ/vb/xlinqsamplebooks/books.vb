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
Imports System.IO
Imports System.Reflection
Imports System.Collections.Specialized
Imports System.Globalization


''' <summary>
''' Class for the Books Form
''' </summary>
''' <remarks></remarks>
''' 

Public Class Books

#Region "MemberVariables"
    ''' <summary>
    ''' boolean used to track if the KeyWords for every book in the xml file has been processed and written back
    ''' </summary>
    Private KeyWordsBuilt As Boolean

    ''' <summary>
    ''' Maintains the list of search queries for autocomplete
    ''' </summary>
    Private searchHistory As SearchHistory

#End Region

#Region "HelperMethods"
    ''' <summary>
    ''' Returns the complete path of the XML document.
    ''' </summary>
    ''' <returns></returns>
    Private Shared Function GetXmlFilePath() As String
        'Assembly.GetExecutingAssembly().GetName().CodeBase return the full path of the current executing assembly
        Dim dir As String = Path.GetDirectoryName(Assembly.GetExecutingAssembly().GetName().CodeBase)
        Dim name As String = Path.Combine(dir, "XlinqBooks.xml")
        Return name
    End Function

    ''' <summary>
    ''' Returns Author's Country name if present in the node
    ''' </summary>
    ''' <param name="author"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Shared Function GetAuthorCountry(ByVal author As XElement)

        'if there is a country element, return its value, else return unknown
        If (author.Element("country") Is Nothing) Then
            Return "unknown"
        Else
            Return author.Elements("country").Value
        End If
    End Function


    ''' <summary>
    ''' Enumerates every book element in the XML and adds a new keywords Element to it.
    ''' keywords include value of the book's style, title, Authors first and last name, subject
    ''' </summary>
    Private Sub BuildKeyWords()

        'get the xml file path
        Dim name As String = GetXmlFilePath()

        'Load the xml document
        Dim doc As XDocument = XDocument.Load(name)

        'loop through every book element in the document's bookstore element.
        For Each book As XElement In doc.Elements("bookstore").Elements("book")


            'get the author element
            Dim Author As XElement = book.Element("author")


            'get style attribute of the book

            Dim style As String = book.Attribute("style").Value

            'get title value of book 
            Dim title As String = book.Element("title").Value

            'get first name value of the book's author 
            Dim fname As String = Author.Element("first-name").Value

            'get last name value of the book's author 
            Dim lname As String = Author.Element("last-name").Value

            'get subject value of book 
            Dim subject As String = book.Element("subject").Value

            ' keywords include value of the book's style, title, Authors first and last name, subject
            Dim keywords As String = style + " " + title + " " + fname + " " + lname + " " + subject

            'add new keywords element to the book element
            book.Add(New XElement("keywords", keywords))
        Next


        'finally save the xml back to tfile.
        doc.Save(name, SaveOptions.None)

        'set the KeyWordsBuilt boolean class member variable to true
        KeyWordsBuilt = True

    End Sub


    ''' <summary>
    ''' searches for books by keywords element
    ''' </summary>
    Private Sub UnifiedSearch()

        'get the xml file path
        Dim name As String = GetXmlFilePath()

        'Load the xml document
        Dim doc As XDocument = XDocument.Load(name)

        'case insensitive search
        Dim searchQuery As String = cbUnifiedSearch.Text.ToLower(CultureInfo.CurrentCulture).Trim()

        'add the search entry to the  Search History cache (used to autocomplete)
        If searchQuery <> String.Empty Then searchHistory.AddEntry(searchQuery)



        'XLINQ query to get the list of books that satisfy the keyword search criteria
        Dim q = From book In doc.Element("bookstore").Elements() _
                Where (book.Element("keywords").Value.ToLower().Contains(searchQuery)) _
                    Select New XElement("p", GetBookDetails(book))


        'create html, so we can display the results in the web browser control            
        Dim html As XElement = New XElement("html", _
                                From v In q _
                                    Select v _
                                )

        'set the web browser's DocumentText property
        wbUnifiedSearchResults.DocumentText = html.ToString()
    End Sub

    ''' <summary>
    ''' Return HTML table with following details of the Book
    '''     1)Book Title
    '''     2)Book Subject
    '''     3)Book Author (First Name + Last Name)
    '''     4)Book Style
    '''     5)Book Price
    ''' </summary>
    ''' <param name="b"></param>
    ''' <returns></returns>
    Private Shared Function GetBookDetails(ByVal b As XElement) As XElement


        'create the HTML table contents.
        Dim table As XElement = _
            New XElement("table", New XAttribute("border", 1), _
                New XElement("tr", _
                             New XElement("td", "Title"), _
                             New XElement("td", b.Element("title").Value) _
                             ), _
                New XElement("tr", _
                             New XElement("td", "Subject"), _
                             New XElement("td", b.Element("subject").Value) _
                             ), _
               New XElement("tr", _
                             New XElement("td", "Author"), _
                             New XElement("td", b.Element("author").Element("first-name").Value _
                             + " " + b.Element("author").Element("last-name").Value) _
                             ), _
               New XElement("tr", _
                             New XElement("td", "Style"), _
                             New XElement("td", b.Attribute("style").Value) _
                             ), _
                New XElement("tr", _
                             New XElement("td", "Price"), _
                             New XElement("td", String.Format(CultureInfo.CurrentCulture, "{0:c}", Double.Parse(b.Element("price").Value, CultureInfo.CurrentCulture))) _
                             ) _
                          )


        Return table
    End Function



    ''' <summary>
    ''' Returns a HTML table with two columns: Title and Price 
    ''' </summary>
    ''' <param name="books"></param>
    ''' <returns></returns>
    Private Shared Function GetBooksTable(ByVal books As IEnumerable(Of XElement)) As XElement
        'create table header
        Dim header = New XElement() {New XElement("th", "Title"), _
                                         New XElement("th", "Price") _
                                         }

        'from every book, get the title and price and create a HTML row
        Dim rows = From b In books _
                    Select _
                        New XElement("tr", _
                                 New XElement("td", _
                                              CStr(b.Element("title"))), _
                                 New XElement("td", String.Format("{0:c}", _
                                                Double.Parse(b.Element("price").Value))) _
                                 )



        'return HTML table by create new XElement
        Return New XElement("table", _
                                New XAttribute("border", 1), _
                                header, _
                                rows)


    End Function


    ''' <summary>
    ''' Returns HTML results based on the appropriate groupBy condition
    ''' </summary>
    ''' <param name="groupBy"></param>
    ''' <returns></returns>
    Private Shared Function GetHTML(ByVal groupBy As String) As XElement
        'get the xml file path
        Dim name As String = GetXmlFilePath()

        'Load the xml document
        Dim doc As XDocument = XDocument.Load(name)

        Dim q As IEnumerable(Of XElement) = Nothing







        Select Case groupBy.ToLower(CultureInfo.CurrentCulture)
            'if group by condition is Author, group all books by different Author's First name + " " + Last name
            Case "author"
                q = From b In doc.Descendants("book") _
                From a In b.Elements("author") _
                Group b By key = a.Element("first-name").Value + " " + a.Element("last-name").Value Into authorGroup = Group _
                Select New XElement("p", "Author: " + key, GetBooksTable(authorGroup))

                'if group by condition is Subject, group all books by different subjects
            Case "subject"
                q = From b In doc.Descendants("book") _
                From s In b.Elements("subject") _
                Group b By key = s.Value Into subjectGroup = Group _
                Select New XElement("p", "Subject: " + key, GetBooksTable(subjectGroup))

                'if group by condition is Style, group all books by the different styles
            Case "style"
                q = From b In doc.Descendants("book") _
                        Group b By key = b.Attribute("style").Value Into styleGroup = Group _
                        Select New XElement("p", "Style: " + key, _
                                           GetBooksTable(styleGroup))

        End Select

        'return HTML content by wrapping the contents of the query inside HTML section
        Dim html As XElement = New XElement("html", _
                                From v In q _
                                Select v _
                            )

        Return html

    End Function

#End Region


#Region "EventHandlers"


    ''' <summary>
    ''' Click event handler for btnSearch 
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSearch_Click_1(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSearch.Click

        'set the Current Cursor to WaitCursor
        Cursor.Current = Cursors.WaitCursor

        'get the xml file path
        Dim name As String = GetXmlFilePath()

        'case insensitive search
        Dim searchQuery As String = txtBookName.Text.ToLower(CultureInfo.CurrentCulture).Trim()


        'Load the xml document
        Dim doc As XDocument = XDocument.Load(name)

        'XLINQ query to search books based on the title of the book.
        'this query returns IEnumerable<Book>
        Dim query = From book In doc.Element("bookstore").Elements() _
                    Where (book.Element("title").Value.ToLower().Contains(searchQuery)) _
                    Select New Book With { _
                            .Title = book.Element("title").Value, _
                            .Author = New Author With { _
                                .FirstName = book.Element("author").Element("first-name").Value, _
                                .LastName = book.Element("author").Element("last-name").Value, _
                                .Country = GetAuthorCountry(book.Element("author")) _
                              }, _
                            .Price = Convert.ToDouble(book.Element("price").Value) _
                        }



        'get count of search results that query gives us
        Dim count As Integer = query.Count()

        'if count is 0 , then hide the DataGrid
        dgResults.Visible = count > 0

        'display Record Count.
        lblBookSearchCount.Visible = True
        lblBookSearchCount.Text = String.Format(CultureInfo.CurrentCulture, "{0} records found", count)

        'call the ToArray method to return an Array of Books.
        'query is IEnumerable<Book> which cannot be bound to a datagrid
        Dim _books As Book() = query.ToArray()


        'set this DataGrid TableStyle's Mapping Name (Array of Books)
        dgResults.TableStyles(0).MappingName = "Book[]"

        'Bind to DataGrid
        dgResults.DataSource = _books
        dgResults.Refresh()

        'set Cursor back to Default
        Cursor.Current = Cursors.Default
    End Sub


    ''' <summary>
    ''' Load event handler for Form 
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub Books_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        'get the xml file path
        Dim name As String = GetXmlFilePath()

        'Load the xml document
        Dim doc As XDocument = XDocument.Load(name)


        'check to see if the book elements already have a keywords element inside it
        KeyWordsBuilt = Not doc.Element("bookstore").Element("book").Element("keywords") Is Nothing


        If (Not KeyWordsBuilt) Then
            btnAction.Text = "Build Keywords and Search"
        Else
            btnAction.Text = "Search"
        End If

        'get the configuration file settings (Search History)
        Dim appSettings As NameValueCollection = ConfigurationManager.AppSettings

        'set the Search History
        searchHistory = New SearchHistory(appSettings)


    End Sub

    ''' <summary>
    ''' Click event handler for btnGo
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub btnGo_Click_1(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnGo.Click

        If (cbGroupBy.SelectedItem Is Nothing) Then Return

        'set the Current Cursor to WaitCursor
        Cursor.Current = Cursors.WaitCursor

        If (cbGroupBy.SelectedItem.ToString() <> String.Empty) Then
            'Get the HTML content
            Dim html As XElement = GetHTML(cbGroupBy.SelectedItem.ToString().Trim())
            'set the web browser's DocumentText property
            wbGroupByResults.DocumentText = html.ToString()
        End If

        'set Cursor back to Default
        Cursor.Current = Cursors.Default
    End Sub


    ''' <summary>
    ''' KeyUp event handler for cbUnifiedSearch combobox
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub cbUnifiedSearch_KeyUp_1(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles cbUnifiedSearch.KeyUp
        'if the key pressed is any of these , then dont do anything
        If (e.Control Or e.Shift Or e.KeyCode = Keys.Back Or e.KeyCode = Keys.Delete Or _
                e.KeyCode = Keys.Home Or e.KeyCode = Keys.ShiftKey) Then
            Return
        End If


        'get the text
        Dim text As String = cbUnifiedSearch.Text

        If text.Length > 0 Then
            'search to see if the text user is typing at the moment is in the history list
            Dim match As String = searchHistory.Match(text)

            'if there is a match
            If (match <> String.Empty) Then
                'set the Text property of the combobox to the match
                cbUnifiedSearch.Text = match

                'set selection
                cbUnifiedSearch.SelectionStart = text.Length
                cbUnifiedSearch.SelectionLength = match.Length - text.Length

            End If

        End If

    End Sub


    ''' <summary>
    ''' Click event handler for btnAction
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub btnAction_Click_1(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAction.Click
        'set the Current Cursor to WaitCursor
        Cursor.Current = Cursors.WaitCursor


        'if the book elements already has keywords element, then just search
        If (KeyWordsBuilt) Then

            UnifiedSearch()
        Else 'else build the keywords and write it back to the xml file and then search

            BuildKeyWords()
            UnifiedSearch()

            btnAction.Text = "Search"
        End If


        'set Cursor back to Default
        Cursor.Current = Cursors.Default
    End Sub


    ''' <summary>
    ''' Click event handler for menu item Exit
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub miExit_Click_1(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles miExit.Click
        'save the configuration settings(Search History)
        ConfigurationManager.AppSettings = searchHistory.SearchHistoryValues
        ConfigurationManager.Save()
        'close the Application
        Me.Close()
    End Sub

#End Region

End Class
