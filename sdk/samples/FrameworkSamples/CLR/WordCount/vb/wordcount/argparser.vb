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
'  File:      ArgumentParser.vb
'
'  Summary:   Reusable class for parsing command-line arguments.
'
'=====================================================================*/

Option Explicit On
Option Strict On


Imports System
Imports System.Globalization

Namespace Microsoft.Samples
    Public MustInherit Class ArgumentParser
        Private switchChars() As String   ' For example: "/", "-"
        Private switchSymbols() As String           ' Switch character(s)
        Private caseSensitiveSwitches As Boolean    ' Are switches case-sensitive?

        ' Domain of results when processing a command-line argument switch
        Public Enum SwitchStatus
            NoError
            YesError
            ShowUsage
        End Enum 'SwitchStatus


        ' Constructor that defaults to case-insensitive switches and 
        ' defaults to "/" and "-" as the only valid switch characters
        Protected Sub New(ByVal switchSymbols() As String)
            MyClass.New(switchSymbols, False, New String() {"/", "-"})
        End Sub 'New



        ' Constructor that defaults to "/" and "-" as the only valid switch characters
        Protected Sub New(ByVal switchSymbols() As String, ByVal caseSensitiveSwitches As Boolean)
            MyClass.New(switchSymbols, caseSensitiveSwitches, New String() {"/", "-"})
        End Sub 'New



        ' Constructor with no defaults
        Protected Sub New(ByVal switchSymbols() As String, ByVal caseSensitiveSwitches As Boolean, ByVal switchChars() As String)
            Me.switchSymbols = switchSymbols
            Me.caseSensitiveSwitches = caseSensitiveSwitches
            Me.switchChars = switchChars
        End Sub 'New



        ' Every derived class must implement an OnUsage method
        Public MustOverride Sub OnUsage(ByVal errorInfo As String)


        ' Every derived class must implement an OnSwitch method or a switch is considered an error
        Protected Overridable Function OnSwitch(ByVal switchSymbol As String, ByVal switchValue As String) As SwitchStatus
            Return SwitchStatus.YesError
        End Function 'OnSwitch



        ' Every derived class must implement an OnNonSwitch method or a non-switch is considerred an error
        Protected Overridable Function OnNonSwitch(ByVal value As String) As SwitchStatus
            Return SwitchStatus.YesError
        End Function 'OnNonSwitch



        ' The derived class is notified after all command-line switches have been parsed.
        ' The derived class can perform any sanity checking required at this time.
        Protected Overridable Function OnDoneParse() As SwitchStatus
            ' By default, we'll assume that all parsing was successful
            Return SwitchStatus.YesError
        End Function 'OnDoneParse



        ' This Parse method always parses the application's command-line arguments
        Public Overloads Function Parse() As Boolean
            ' Visual Basic will use this method since its entry-point function 
            ' doesn't get the command-line arguments passed to it.
            Return Parse(Environment.GetCommandLineArgs())
        End Function 'Parse



        ' This Parse method parses an arbitrary set of arguments
        Public Overloads Function Parse(ByVal args() As String) As Boolean
            Dim ss As SwitchStatus = SwitchStatus.NoError ' Assume parsing is sucessful.
            Dim ArgNum As Integer
            For ArgNum = 0 To (args.Length - 1)

                If Not (ss = SwitchStatus.NoError) Then Exit For

                ' Determine if this argument starts with a valid switch character
                Dim fIsSwitch As Boolean = False
                Dim n As Integer
                For n = 0 To (switchChars.Length - 1)
                    If (fIsSwitch = True) Then Exit For
                    fIsSwitch = (0 = String.CompareOrdinal(args(ArgNum), 0, switchChars(n), 0, 1))
                Next n

                If fIsSwitch Then
                    ' Does the switch begin with a legal switch symbol?
                    Dim fLegalSwitchSymbol As Boolean = False
                    For n = 0 To (switchSymbols.Length - 1) ' re-using local var n to traverse an entirely seperate loop
                        If caseSensitiveSwitches Then
                            fLegalSwitchSymbol = (0 = String.CompareOrdinal(args(ArgNum), 1, switchSymbols(n), 0, switchSymbols(n).Length))
                        Else
                            fLegalSwitchSymbol = (0 = String.CompareOrdinal(args(ArgNum).ToUpper(CultureInfo.InvariantCulture), 1, switchSymbols(n).ToUpper(CultureInfo.InvariantCulture), 0, switchSymbols(n).Length))
                        End If
                        If fLegalSwitchSymbol Then
                            Exit For
                        End If
                    Next n
                    If Not fLegalSwitchSymbol Then
                        ' User specified an unrecognized switch, exit
                        ss = SwitchStatus.YesError
                        Exit For
                    Else
                        ' This is a legal switch, notified the derived class of this switch and its value
                        If (caseSensitiveSwitches) Then
                            ss = OnSwitch(switchSymbols(n), args(ArgNum).Substring((1 + switchSymbols(n).Length)))
                        Else
                            ss = OnSwitch(switchSymbols(n).ToLower(CultureInfo.InvariantCulture), args(ArgNum).Substring((1 + switchSymbols(n).Length)))
                        End If
                    End If
                Else
                    ' This is not a switch, notified the derived class of this "non-switch value"
                    ss = OnNonSwitch(args(ArgNum))
                End If
            Next ArgNum

            ' Finished parsing arguments
            If ss = SwitchStatus.NoError Then
                ' No error occurred while parsing, let derived class perform a 
                ' sanity check and return an appropraite status
                ss = OnDoneParse()
            End If

            If ss = SwitchStatus.ShowUsage Then
                ' Status indicates that usage should be shown, show it
                OnUsage(Nothing)
            End If

            If ss = SwitchStatus.YesError Then
                ' Status indicates that an error occurred, show it and the proper usage
                If (ArgNum = args.Length) Then
                    OnUsage(Nothing)
                Else
                    OnUsage(args(ArgNum))
                End If
            End If

            ' Return whether all parsing was sucessful.
            Return ss = SwitchStatus.NoError
        End Function 'Parse
    End Class 'ArgumentParser
End Namespace

'/////////////////////////////// End of File /////////////////////////////////
