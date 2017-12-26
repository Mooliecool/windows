'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'-----------------------------------------------------------------------

'=====================================================================
'  File:      WordCount.vb
'
'  Summary:   Demonstrates how to create a WordCount application
'             using various .NET Framework library types.
'
'=====================================================================

Option Explicit On 
Option Strict On


' Add the classes in the following namespaces to our namespace
Imports System
Imports System.Globalization
Imports System.IO
Imports System.Text
Imports System.Threading
Imports System.Security
Imports System.Collections
Imports Microsoft.VisualBasic

Namespace Microsoft.Samples
    Class WordCountArgumentParser
        Inherits ArgumentParser

        ' Members identifying command-line argument settings
        Private showAlphabeticalWordUsageFlag As Boolean
        Private showOccurrenceWordUsageFlag As Boolean
        Private outputFileName As String
        Private fileEncodingType As Encoding = Encoding.UTF8
        Private pathnames As New ArrayList()

        ' Give the set of valid command-line switches to the base class
        Public Sub New()
            MyBase.New(New String() {"?", "a", "o", "f", "C"})
        End Sub 'New


        ' Returns the name of the user-specified output file or null if not specified

        Public ReadOnly Property OutputFile() As String
            Get
                Return outputFileName
            End Get
        End Property
        ' Indicates whether the user wanted to see all the words sorted alphabetically

        Public ReadOnly Property ShowAlphabeticalWordUsage() As Boolean
            Get
                Return showAlphabeticalWordUsageFlag
            End Get
        End Property

        ' Indicates whether the user wanted to see all the words sorted by occurrence

        Public ReadOnly Property ShowOccurrenceWordUsage() As Boolean
            Get
                Return showOccurrenceWordUsageFlag
            End Get
        End Property

        Public ReadOnly Property FileEncoding() As Encoding
            Get
                Return fileEncodingType
            End Get
        End Property

        ' Shows application's usage info and also reports command-line argument errors.
        Public Overrides Sub OnUsage(ByVal errorInfo As String)
            If Not (errorInfo Is Nothing) Then
                ' An command-line argument error occurred, report it to user
                ' errInfo identifies the argument that is in error.
                Console.WriteLine("Command-line switch error: {0}" & ControlChars.CrLf, errorInfo)
            End If

            Console.WriteLine("Usage: WordCount [-a] [-o] [-f<output-pathname>] [-c<codepage>] pathname...")
            Console.WriteLine("   -?  Show this usage information")
            Console.WriteLine("   -a  Word usage sorted alphabetically")
            Console.WriteLine("   -o  Word usage sorted by occurrence and then alphabetically")
            Console.WriteLine("   -f  Send output to specified pathname instead of the console")
            Console.WriteLine("   -c  Specify the codepage to use to read the file")
        End Sub 'OnUsage



        ' Called for each non-switch command-line argument (filespecs)
        Protected Overrides Function OnNonSwitch(ByVal switchValue As String) As ArgumentParser.SwitchStatus

            Dim ss As SwitchStatus = SwitchStatus.NoError
            ' Add command-line argument to array of pathnames.
            ' Convert switchValue to set of pathnames, add each pathname to the pathnames ArrayList.
            Try
                Dim d As String = Path.GetDirectoryName(switchValue)
                Dim dir As DirectoryInfo
                If (d.Length = 0) Then
                    dir = New DirectoryInfo(".")
                Else
                    dir = New DirectoryInfo(d)
                End If
                Dim f As FileInfo
                For Each f In dir.GetFiles(Path.GetFileName(switchValue))
                    pathnames.Add(f.FullName)
                Next f
            Catch SecEx As System.Security.SecurityException
                Throw SecEx
            Catch
                ss = SwitchStatus.YesError
            End Try

            If pathnames.Count = 0 Then
                Console.WriteLine("None of the specified files exists.")
                ss = SwitchStatus.YesError
            End If

            Return ss
        End Function 'OnNonSwitch



        ' Returns an enumerator that includes all the user-desired files.
        Public Function GetPathnameEnumerator() As IEnumerator
            Return pathnames.GetEnumerator(0, pathnames.Count)
        End Function 'GetPathnameEnumerator



        ' Called for each switch command-line argument
        Protected Overrides Function OnSwitch(ByVal switchSymbol As String, ByVal switchValue As String) As ArgumentParser.SwitchStatus
            ' NOTE: For case-insensitive switches, 
            '       switchSymbol will contain all lower-case characters
            Dim ss As SwitchStatus = SwitchStatus.NoError
            Select Case switchSymbol

                Case "?" ' User wants to see Usage
                    ss = SwitchStatus.ShowUsage

                Case "a" ' User wants to see all words sorted alphabetically
                    showAlphabeticalWordUsageFlag = True

                Case "o" ' User wants to see all words sorted by occurrence
                    showOccurrenceWordUsageFlag = True

                Case "f" ' User wants output redirected to a specified file
                    If switchValue.Length < 1 Then
                        Console.WriteLine("No output file specified.")
                        ss = SwitchStatus.YesError
                    Else
                        outputFileName = switchValue
                    End If
                Case "c" ' User wants a specific codepage to be used to open the file
                    If switchValue.Length < 1 Then
                        Console.WriteLine("No codepage specified")
                        ss = ArgumentParser.SwitchStatus.YesError
                    Else
                        Try
                            Dim codePage As Integer = System.Int32.Parse(switchValue, CultureInfo.CurrentCulture)
                            fileEncodingType = Encoding.GetEncoding(codePage)
                        Catch
                            Console.WriteLine("No valid codepage specified.")
                            ss = ArgumentParser.SwitchStatus.YesError
                        End Try
                    End If
                Case Else
                    Console.WriteLine(("Invalid switch: """ & switchSymbol & """." & ControlChars.CrLf))
                    ss = SwitchStatus.YesError
            End Select
            Return ss
        End Function 'OnSwitch



        ' Called when all command-line arguments have been parsed
        Protected Overrides Function OnDoneParse() As ArgumentParser.SwitchStatus
            Dim ss As SwitchStatus = SwitchStatus.NoError
            ' Sort all the pathnames in the list
            If pathnames.Count = 0 Then
                ' No pathnames were specified
                ss = SwitchStatus.YesError
            Else
                ' Sort all the pathnames in the list
                pathnames.Sort(0, pathnames.Count, Nothing)
            End If
            Return ss
        End Function 'OnDoneParse
    End Class 'WordCountArgumentParser



    '/////////////////////////////////////////////////////////////////////////////

    ' The WordCounter class
    Public Class WordCounter

        Public Sub New() ' No interesting construction 
        End Sub 'New 
        ' Each object of this class keeps a running total of the files its processed
        ' The following members hold this running information
        Private totalLinesCount As Int64 = 0
        Private totalWordsCount As Int64 = 0
        Private totalCharsCount As Int64 = 0
        Private totalBytesCount As Int64 = 0

        ' The set of all words seen (sorted alphabetically)
        Private wordCounter As New SortedList()

        ' The following methods return the running-total info

        Public ReadOnly Property TotalLines() As Int64
            Get
                Return totalLinesCount
            End Get
        End Property

        Public ReadOnly Property TotalWords() As Int64
            Get
                Return totalWordsCount
            End Get
        End Property

        Public ReadOnly Property TotalChars() As Int64
            Get
                Return totalCharsCount
            End Get
        End Property

        Public ReadOnly Property TotalBytes() As Int64
            Get
                Return totalBytesCount
            End Get
        End Property
        ' This method calculates the statistics for a single file.
        ' This file's info is returned via the out parameters
        ' The running total of all files is maintained in the data members
        Public Function CountStats(ByVal pathname As String, ByVal fileEncoding As Encoding, ByRef numLines As Int64, ByRef numWords As Int64, ByRef numChars As Int64, ByRef numBytes As Int64) As Boolean

            Dim Ok As Boolean = True ' Assume success
            numLines = 0
            numWords = 0
            numChars = 0
            numBytes = 0

            ' Attempt to open the input file for read-only access
            Using fsIn As FileStream = New FileStream(pathname, FileMode.Open, FileAccess.Read, FileShare.Read)
                numBytes = fsIn.Length
                Using sr As StreamReader = New StreamReader(fsIn, fileEncoding, True)

                    ' Process every line in the file
                    Dim Line As String = sr.ReadLine()

                    While Not (Line Is Nothing)
                        numLines += 1
                        numChars += Line.Length
                        Dim Words As String() = Line.Split(Nothing) ' Split the line into words
                        Dim Word As Integer
                        For Word = 0 To Words.Length - 1
                            If Words(Word) <> "" Then ' Don't count empty strings
                                numWords += 1
                                If Not wordCounter.ContainsKey(Words(Word)) Then
                                    ' If we've never seen this word before, add it to the sorted list with a count of 1
                                    wordCounter.Add(Words(Word), 1)
                                Else
                                    ' If we have seen this word before, just increment its count
                                    wordCounter(Words(Word)) = CType(wordCounter(Words(Word)), Int32) + 1
                                End If
                            End If
                        Next Word
                        Line = sr.ReadLine()
                    End While

                End Using
            End Using

            ' Increment the running totals with whatever was discovered about this file
            totalLinesCount += numLines
            totalWordsCount += numWords
            totalCharsCount += numChars
            totalBytesCount += numBytes
            Return Ok
        End Function 'CountStats


        ' Returns an enumerator for the words (sorted alphabetically)
        Public Function GetWordsAlphabeticallyEnumerator() As IDictionaryEnumerator
            Return CType(wordCounter.GetEnumerator(), IDictionaryEnumerator)
        End Function 'GetWordsAlphabeticallyEnumerator

        ' Returns an enumerator for the words (sorted by occurrence)
        Public Function GetWordsByOccurrenceEnumerator() As IDictionaryEnumerator
            ' To create a list of words sorted by occurrence, we need another SortedList object
            Dim sl As New SortedList()

            ' Now, we'll iterate through the words alphabetically
            Dim de As IDictionaryEnumerator = GetWordsAlphabeticallyEnumerator()
            While de.MoveNext()

                ' For each word, we create a new WordOccurrence object which
                ' contains the word and its occurrence value.
                ' The WordOccurrence class contains a CompareTo method which knows
                ' to sort WordOccurrence objects by occurrence value and then alphabetically by the word itself.
                sl.Add(New WordOccurrence(CInt(de.Value), CStr(de.Key)), Nothing)
            End While
            ' Return an enumerator for the words (sorted by occurrence)
            Return CType(sl.GetEnumerator(), IDictionaryEnumerator)
        End Function 'GetWordsByOccurrenceEnumerator

        ' Returns the number of unique words processed

        Public ReadOnly Property UniqueWords() As Integer
            Get
                Return wordCounter.Count
            End Get
        End Property
    End Class 'WordCounter

    ' This class is only used to sort the words by occurrence
    ' An instance of this class is created for each word
    Public Class WordOccurrence
        Implements System.IComparable

        ' Members indicating the number of times this word occurred and the word itself
        Private occurrencesCount As Integer
        Private wordString As String


        ' Constructor
        Public Sub New(ByVal occurrences As Integer, ByVal word As String)
            Me.occurrencesCount = occurrences
            Me.wordString = word
        End Sub 'New


        ' Sorts two WordOccurrence objects by occurrence first, then by word
        Public Function CompareTo(ByVal o As Object) As Integer Implements System.IComparable.CompareTo
            ' Compare the occurance of the two objects
            Dim n As Integer = occurrencesCount - CType(o, WordOccurrence).occurrencesCount
            If n = 0 Then
                ' Both objects have the same ccurrance, sort alphabetically by word
                n = String.Compare(wordString, CType(o, WordOccurrence).wordString, False, Thread.CurrentThread.CurrentCulture)
            End If
            Return n
        End Function 'CompareTo

        ' Return the occurrence value of this word

        Public ReadOnly Property Occurrences() As Integer
            Get
                Return occurrencesCount
            End Get
        End Property ' Return this word

        Public ReadOnly Property Word() As String
            Get
                Return wordString
            End Get
        End Property
    End Class 'WordOccurrence

    '/////////////////////////////////////////////////////////////////////////////

    ' This class represents the application itself
    Public NotInheritable Class App

        Private Sub New()
        End Sub

        Public Shared Function Main(ByVal args() As String) As Integer
            ' Parse the command-line arguments
            Dim ap As New WordCountArgumentParser()
            If Not ap.Parse(args) Then
                ' An error occurrend while parsing
                Return 1
            End If

            ' If an output file was specified on the command-line, use it
            Dim fsOut As FileStream = Nothing
            Dim sw As StreamWriter = Nothing
            Try
                If Not (ap.OutputFile Is Nothing) Then
                    fsOut = New FileStream(ap.OutputFile, FileMode.Create, FileAccess.Write, FileShare.None)
                    sw = New StreamWriter(fsOut, ap.FileEncoding)

                    ' By associating the StreamWriter with the console, the rest of 
                    ' the code can think it's writing to the console but the console 
                    ' object redirects the output to the StreamWriter
                    Console.SetOut(sw)
                End If

                ' Create a WordCounter object to keep running statistics
                Dim wc As New WordCounter()

                ' Write the table header
                Console.WriteLine("Lines" & ControlChars.Tab & "Words" & ControlChars.Tab & "Chars" & ControlChars.Tab & "Bytes" & ControlChars.Tab & "Pathname")

                ' Iterate over the user-specified files
                Dim e As IEnumerator = ap.GetPathnameEnumerator()
                While e.MoveNext()
                    Dim numLines, numWords, numChars, numBytes As Int64
                    ' Calculate the words stats for this file
                    wc.CountStats(CType(e.Current, String), ap.FileEncoding, numLines, numWords, numChars, numBytes)

                    ' Display the results
                    Dim StrArgs() As String = {numLines.ToString(CultureInfo.CurrentCulture), numWords.ToString(CultureInfo.CurrentCulture), numChars.ToString(CultureInfo.CurrentCulture), numBytes.ToString(CultureInfo.CurrentCulture), CType(e.Current, String)}
                    Dim Out As String

                    'The following six lines simulate what this commented line would do if the VB compiler didn't use Format incorrectly
                    'Console.WriteLine(String.Format("{0,5}" & ControlChars.Tab & "{1,5}" & ControlChars.Tab & "{2,5}" & ControlChars.Tab & "{3,5}" & ControlChars.Tab & "{4,5}", StrArgs))
                    Out = String.Format("{0,5}" & ControlChars.Tab, StrArgs(0))
                    Out += String.Format("{0,5}" & ControlChars.Tab, StrArgs(1))
                    Out += String.Format("{0,5}" & ControlChars.Tab, StrArgs(2))
                    Out += String.Format("{0,5}" & ControlChars.Tab, StrArgs(3))
                    Out += String.Format("{0,5}" & ControlChars.Tab, StrArgs(4))
                    Console.WriteLine(Out)

                End While

                ' Done processing all files, show the totals
                Console.WriteLine("-----" & ControlChars.Tab & "-----" & ControlChars.Tab & "-----" & ControlChars.Tab & "-----" & ControlChars.Tab & "---------------------")
                Console.WriteLine(String.Format("{0,5}" & ControlChars.Tab & "{1,5}" & ControlChars.Tab & "{2,5}" & ControlChars.Tab & "{3,5}" & ControlChars.Tab & "Total in all files", New Object() {wc.TotalLines, wc.TotalWords, wc.TotalChars, wc.TotalBytes}))

                ' If the user wants to see the word usage alphabetically, show it
                If ap.ShowAlphabeticalWordUsage Then
                    Dim de As IDictionaryEnumerator = wc.GetWordsAlphabeticallyEnumerator()
                    Console.WriteLine(String.Format("Word usage sorted alphabetically ({0} unique words)", wc.UniqueWords))
                    While de.MoveNext()
                        Console.WriteLine(String.Format("{0,5}: {1}", de.Value, de.Key))
                    End While
                End If

                ' If the user wants to see the word usage by occurrence, show it
                If ap.ShowOccurrenceWordUsage Then
                    Dim de As IDictionaryEnumerator = wc.GetWordsByOccurrenceEnumerator()
                    Console.WriteLine(String.Format("Word usage sorted by occurrence ({0} unique words)", wc.UniqueWords))
                    While de.MoveNext()
                        Console.WriteLine(String.Format("{0,5}: {1}", CType(de.Key, WordOccurrence).Occurrences, CType(de.Key, WordOccurrence).Word))
                    End While
                End If
            Catch e As Exception
                Console.WriteLine("Exception: " + e.Message)

            Finally
                ' Explicitly close the console to guarantee that the StreamWriter object (sw) is flushed
                Console.Out.Close()
                If Not (fsOut Is Nothing) Then
                    fsOut.Close()
                End If

            End Try
        End Function 'Main
    End Class 'App
End Namespace

'/////////////////////////////// End of File /////////////////////////////////
