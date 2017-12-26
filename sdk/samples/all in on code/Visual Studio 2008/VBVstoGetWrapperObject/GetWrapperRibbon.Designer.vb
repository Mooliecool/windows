Partial Class GetWrapperRibbon
    Inherits Microsoft.Office.Tools.Ribbon.OfficeRibbon

    <System.Diagnostics.DebuggerNonUserCode()> _
   Public Sub New(ByVal container As System.ComponentModel.IContainer)
        MyClass.New()

        'Required for Windows.Forms Class Composition Designer support
        If (container IsNot Nothing) Then
            container.Add(Me)
        End If

    End Sub

    <System.Diagnostics.DebuggerNonUserCode()> _
    Public Sub New()
        MyBase.New()

        'This call is required by the Component Designer.
        InitializeComponent()

    End Sub

    'Component overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Component Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Component Designer
    'It can be modified using the Component Designer.
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.tabCfx = New Microsoft.Office.Tools.Ribbon.RibbonTab
        Me.grpGetWrapperObject = New Microsoft.Office.Tools.Ribbon.RibbonGroup
        Me.btnShowGetWrapperVB = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.tabCfx.SuspendLayout()
        Me.grpGetWrapperObject.SuspendLayout()
        Me.SuspendLayout()
        '
        'tabCfx
        '
        Me.tabCfx.Groups.Add(Me.grpGetWrapperObject)
        Me.tabCfx.Label = "VSTO Samples"
        Me.tabCfx.Name = "tabCfx"
        '
        'grpGetWrapperObject
        '
        Me.grpGetWrapperObject.Items.Add(Me.btnShowGetWrapperVB)
        Me.grpGetWrapperObject.Label = "GetVstoObject Sample"
        Me.grpGetWrapperObject.Name = "grpGetWrapperObject"
        '
        'btnShowGetWrapperVB
        '
        Me.btnShowGetWrapperVB.ControlSize = Microsoft.Office.Core.RibbonControlSize.RibbonControlSizeLarge
        Me.btnShowGetWrapperVB.Label = "Get VSTO Wrapper (VB)"
        Me.btnShowGetWrapperVB.Name = "btnShowGetWrapperVB"
        Me.btnShowGetWrapperVB.OfficeImageId = "UpgradeWorkbook"
        Me.btnShowGetWrapperVB.ShowImage = True
        '
        'GetWrapperRibbon
        '
        Me.Name = "GetWrapperRibbon"
        Me.RibbonType = "Microsoft.Excel.Workbook"
        Me.Tabs.Add(Me.tabCfx)
        Me.tabCfx.ResumeLayout(False)
        Me.tabCfx.PerformLayout()
        Me.grpGetWrapperObject.ResumeLayout(False)
        Me.grpGetWrapperObject.PerformLayout()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents tabCfx As Microsoft.Office.Tools.Ribbon.RibbonTab
    Friend WithEvents grpGetWrapperObject As Microsoft.Office.Tools.Ribbon.RibbonGroup
    Friend WithEvents btnShowGetWrapperVB As Microsoft.Office.Tools.Ribbon.RibbonButton
End Class

Partial Class ThisRibbonCollection
    Inherits Microsoft.Office.Tools.Ribbon.RibbonReadOnlyCollection

    <System.Diagnostics.DebuggerNonUserCode()> _
    Friend ReadOnly Property GetWrapperRibbon() As GetWrapperRibbon
        Get
            Return Me.GetRibbon(Of GetWrapperRibbon)()
        End Get
    End Property
End Class
