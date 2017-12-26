Option Strict Off
Option Explicit On

Partial Class ImportedFormRegion
    Inherits Microsoft.Office.Tools.Outlook.ImportedFormRegion

    Private _RecipientControl2 As Microsoft.Office.Interop.Outlook._DRecipientControl
    Private WithEvents btnTo As Microsoft.Office.Interop.Outlook.OlkCommandButton
    Private WithEvents comboBox1 As Microsoft.Office.Interop.Outlook.OlkComboBox
    Private _DocSiteControl1 As Microsoft.Office.Interop.Outlook._DDocSiteControl
    Private WithEvents btnSend As Microsoft.Office.Interop.Outlook.OlkCommandButton
    Private WithEvents btnAdd As Microsoft.Office.Interop.Outlook.OlkCommandButton
    Private WithEvents btnRemove As Microsoft.Office.Interop.Outlook.OlkCommandButton
    Private WithEvents btnDisable As Microsoft.Office.Interop.Outlook.OlkCommandButton

    Public Sub New(ByVal formRegion As Microsoft.Office.Interop.Outlook.FormRegion)
        MyBase.New(formRegion)
    End Sub

    <System.Diagnostics.DebuggerStepThrough()> _
    Protected Overrides Sub InitializeControls()
        Me._RecipientControl2 = Me.GetFormRegionControl(Of Microsoft.Office.Interop.Outlook._DRecipientControl)("_RecipientControl2")
        Me.btnTo = Me.GetFormRegionControl(Of Microsoft.Office.Interop.Outlook.OlkCommandButton)("btnTo")
        Me.comboBox1 = Me.GetFormRegionControl(Of Microsoft.Office.Interop.Outlook.OlkComboBox)("ComboBox1")
        Me._DocSiteControl1 = Me.GetFormRegionControl(Of Microsoft.Office.Interop.Outlook._DDocSiteControl)("_DocSiteControl1")
        Me.btnSend = Me.GetFormRegionControl(Of Microsoft.Office.Interop.Outlook.OlkCommandButton)("btnSend")
        Me.btnAdd = Me.GetFormRegionControl(Of Microsoft.Office.Interop.Outlook.OlkCommandButton)("btnAdd")
        Me.btnRemove = Me.GetFormRegionControl(Of Microsoft.Office.Interop.Outlook.OlkCommandButton)("btnRemove")
        Me.btnDisable = Me.GetFormRegionControl(Of Microsoft.Office.Interop.Outlook.OlkCommandButton)("btnDisable")

    End Sub

    Partial Public Class ImportedFormRegionFactory
        Implements Microsoft.Office.Tools.Outlook.IFormRegionFactory

        Public Event FormRegionInitializing As System.EventHandler(Of Microsoft.Office.Tools.Outlook.FormRegionInitializingEventArgs)

        Private _Manifest As Microsoft.Office.Tools.Outlook.FormRegionManifest

        <System.Diagnostics.DebuggerNonUserCodeAttribute()> _
        Public Sub New()
            Me._Manifest = New Microsoft.Office.Tools.Outlook.FormRegionManifest()
            Me.InitializeManifest()
        End Sub

        <System.Diagnostics.DebuggerNonUserCodeAttribute()> _
        ReadOnly Property Manifest() As Microsoft.Office.Tools.Outlook.FormRegionManifest Implements Microsoft.Office.Tools.Outlook.IFormRegionFactory.Manifest
            Get
                Return Me._Manifest
            End Get
        End Property

        <System.Diagnostics.DebuggerNonUserCodeAttribute()> _
        Function CreateFormRegion(ByVal formRegion As Microsoft.Office.Interop.Outlook.FormRegion) As Microsoft.Office.Tools.Outlook.IFormRegion Implements Microsoft.Office.Tools.Outlook.IFormRegionFactory.CreateFormRegion
            Dim form as ImportedFormRegion = New ImportedFormRegion(formRegion)
            form.Factory = Me
            Return form
        End Function

        <System.Diagnostics.DebuggerNonUserCodeAttribute()> _
        Function GetFormRegionStorage(ByVal outlookItem As Object, ByVal formRegionMode As Microsoft.Office.Interop.Outlook.OlFormRegionMode, ByVal formRegionSize As Microsoft.Office.Interop.Outlook.OlFormRegionSize) As Byte() Implements Microsoft.Office.Tools.Outlook.IFormRegionFactory.GetFormRegionStorage
            Dim resources As System.Resources.ResourceManager = New System.Resources.ResourceManager(GetType(ImportedFormRegion))
            Return CType(resources.GetObject("ImportedFormRegion"), Byte())
        End Function

        <System.Diagnostics.DebuggerNonUserCodeAttribute()> _
        Function IsDisplayedForItem(ByVal outlookItem As Object, ByVal formRegionMode As Microsoft.Office.Interop.Outlook.OlFormRegionMode, ByVal formRegionSize As Microsoft.Office.Interop.Outlook.OlFormRegionSize) As Boolean Implements Microsoft.Office.Tools.Outlook.IFormRegionFactory.IsDisplayedForItem
            Dim cancelArgs As Microsoft.Office.Tools.Outlook.FormRegionInitializingEventArgs = New Microsoft.Office.Tools.Outlook.FormRegionInitializingEventArgs(outlookItem, formRegionMode, formRegionSize, False)
            cancelArgs.Cancel = False
            RaiseEvent FormRegionInitializing(Me, cancelArgs)
            Return Not cancelArgs.Cancel
        End Function

        <System.Diagnostics.DebuggerNonUserCodeAttribute()> _
        ReadOnly Property Kind() As String Implements Microsoft.Office.Tools.Outlook.IFormRegionFactory.Kind
            Get
                Return Microsoft.Office.Tools.Outlook.FormRegionKindConstants.Ofs
            End Get
        End Property
    End Class

End Class

Partial Class WindowFormRegionCollection

    Friend ReadOnly Property ImportedFormRegion() As ImportedFormRegion
        Get
            Return Me.FindFirst(Of ImportedFormRegion)()
        End Get
    End Property
End Class