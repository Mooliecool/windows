Imports System.ComponentModel
Imports System.Windows.Interop
Imports System.Runtime.InteropServices

' POINT structure required by WINDOWPLACEMENT structure
<Serializable(), StructLayout(LayoutKind.Sequential)> _
 Public Structure POINT
    Public Sub New(ByVal x As Integer, ByVal y As Integer)
        Me.X = x
        Me.Y = y
    End Sub
    Public X As Integer
    Public Y As Integer
End Structure

' RECT structure required by WINDOWPLACEMENT structure
<Serializable(), StructLayout(LayoutKind.Sequential)> _
Public Structure RECT
    Public Sub New(ByVal left As Integer, ByVal top As Integer, ByVal right As Integer, ByVal bottom As Integer)
        Me.Left = left
        Me.Top = top
        Me.Right = right
        Me.Bottom = bottom
    End Sub
    Public Left As Integer
    Public Top As Integer
    Public Right As Integer
    Public Bottom As Integer
End Structure

' WINDOWPLACEMENT stores the position, size, and state of a window
<Serializable(), StructLayout(LayoutKind.Sequential)> _
Public Structure WINDOWPLACEMENT
    Public length As Integer
    Public flags As Integer
    Public showCmd As Integer
    Public minPosition As POINT
    Public maxPosition As POINT
    Public normalPosition As RECT
End Structure

Partial Public Class MainWindow
    Inherits System.Windows.Window

    <DllImport("user32.dll")> _
    Private Shared Function GetWindowPlacement(ByVal hWnd As IntPtr, ByRef lpwndpl As WINDOWPLACEMENT) As Boolean
    End Function

    <DllImport("user32.dll")> _
    Private Shared Function SetWindowPlacement(ByVal hWnd As IntPtr, ByRef lpwndpl As WINDOWPLACEMENT) As Boolean
    End Function

    Private Const SW_SHOWMINIMIZED As Integer = 2
    Private Const SW_SHOWNORMAL As Integer = 1

    Public Sub New()
        InitializeComponent()
    End Sub

    Protected Overrides Sub OnSourceInitialized(ByVal e As EventArgs)
        MyBase.OnSourceInitialized(e)

        Try
            ' Load window placement details for previous application session from application settings
            ' Note - if window was closed on a monitor that is now disconnected from the computer,
            '        SetWindowPlacement will place the window onto a visible monitor.
            Dim wp As WINDOWPLACEMENT = My.Settings.WindowPlacement
            wp.length = Marshal.SizeOf(GetType(WINDOWPLACEMENT))
            wp.flags = 0
            wp.showCmd = IIf((wp.showCmd = SW_SHOWMINIMIZED), SW_SHOWNORMAL, wp.showCmd)
            MainWindow.SetWindowPlacement(New WindowInteropHelper(Me).Handle, wp)
        Catch ex As Exception
        End Try
    End Sub

    ' WARNING - Not fired when Application.SessionEnding is fired
    Protected Overrides Sub OnClosing(ByVal e As CancelEventArgs)
        MyBase.OnClosing(e)

        ' Persist window placement details to application settings
        Dim wp As WINDOWPLACEMENT = New WINDOWPLACEMENT
        MainWindow.GetWindowPlacement(New WindowInteropHelper(Me).Handle, wp)
        My.Settings.WindowPlacement = wp
        My.Settings.Save()
    End Sub

End Class
