Imports System
Imports System.IO
Imports System.Reflection
Imports System.Runtime.InteropServices
Imports System.Text

Namespace ManagedCardWriter
    <DefaultMember("Item")> _
    Public Class IniFile
        ' Methods
        Public Sub New(ByVal filename As String)
            If Not File.Exists(filename) Then
                Throw New FileNotFoundException(("File " & filename & " Not found"))
            End If
            Dim buffer1 As Byte() = New Byte(32767  - 1) {}
            Dim num1 As Integer = 0
            num1 = IniFile.GetPrivateProfileString(Nothing, Nothing, Nothing, buffer1, 32767, filename)
            If (num1 = 0) Then
                Me.sections = New section(0  - 1) {}
            Else
                Dim textArray1 As String() = Encoding.GetEncoding(1252).GetString(buffer1, 0, num1).TrimEnd(New Char(1  - 1) {}).Split(New Char(1  - 1) {})
                Me.sections = New section(textArray1.Length  - 1) {}
                Dim num2 As Integer = 0
                Do While (num2 < textArray1.Length)
                    num1 = IniFile.GetPrivateProfileString(textArray1(num2), Nothing, Nothing, buffer1, 32767, filename)
                    Dim textArray2 As String() = Encoding.GetEncoding(1252).GetString(buffer1, 0, num1).TrimEnd(New Char(1  - 1) {}).Split(New Char(1  - 1) {})
                    Dim keyArray1 As key() = New key(textArray2.Length  - 1) {}
                    Dim num3 As Integer = 0
                    Do While (num3 < textArray2.Length)
                        num1 = IniFile.GetPrivateProfileString(textArray1(num2), textArray2(num3), Nothing, buffer1, 32767, filename)
                        keyArray1(num3) = New key(textArray2(num3), Encoding.GetEncoding(1252).GetString(buffer1, 0, num1).TrimEnd(New Char(1  - 1) {}))
                        num3 += 1
                    Loop
                    Me.sections(num2) = New section(textArray1(num2), keyArray1)
                    num2 += 1
                Loop
            End If
        End Sub

        <DllImport("kernel32", SetLastError:=True)> _
        Private Shared Function GetPrivateProfileString(ByVal pSection As String, ByVal pKey As String, ByVal pDefault As String, ByVal prReturn As Byte(), ByVal pBufferLen As Integer, ByVal pFile As String) As Integer
        End Function

        ' Properties
        Public ReadOnly Property Item(ByVal index As String) As section
            Get
                Dim section1 As section
                For Each section1 In Me.sections
                    If section1.name.Equals(index, StringComparison.CurrentCultureIgnoreCase) Then
                        Return section1
                    End If
                Next
                Return New section("", New key(0 - 1) {})
            End Get
        End Property


        ' Fields
        Public sections As section()
    End Class
End Namespace


