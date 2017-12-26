Partial Class InteropRibbon
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
        Me.grpVBAInteropSample = New Microsoft.Office.Tools.Ribbon.RibbonGroup
        Me.btnShowFormVB = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.tabCfx.SuspendLayout()
        Me.grpVBAInteropSample.SuspendLayout()
        Me.SuspendLayout()
        '
        'tabCfx
        '
        Me.tabCfx.Groups.Add(Me.grpVBAInteropSample)
        Me.tabCfx.Label = "VSTO Samples"
        Me.tabCfx.Name = "tabCfx"
        '
        'grpVBAInteropSample
        '
        Me.grpVBAInteropSample.Items.Add(Me.btnShowFormVB)
        Me.grpVBAInteropSample.Label = "VBA Interop Sample"
        Me.grpVBAInteropSample.Name = "grpVBAInteropSample"
        '
        'btnShowFormVB
        '
        Me.btnShowFormVB.ControlSize = Microsoft.Office.Core.RibbonControlSize.RibbonControlSizeLarge
        Me.btnShowFormVB.Label = "Interop Form (VB.NET)"
        Me.btnShowFormVB.Name = "btnShowFormVB"
        Me.btnShowFormVB.OfficeImageId = "VisualBasic"
        Me.btnShowFormVB.ShowImage = True
        '
        'InteropRibbon
        '
        Me.Name = "InteropRibbon"
        Me.RibbonType = "Microsoft.Excel.Workbook"
        Me.Tabs.Add(Me.tabCfx)
        Me.tabCfx.ResumeLayout(False)
        Me.tabCfx.PerformLayout()
        Me.grpVBAInteropSample.ResumeLayout(False)
        Me.grpVBAInteropSample.PerformLayout()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents tabCfx As Microsoft.Office.Tools.Ribbon.RibbonTab
    Friend WithEvents grpVBAInteropSample As Microsoft.Office.Tools.Ribbon.RibbonGroup
    Friend WithEvents btnShowFormVB As Microsoft.Office.Tools.Ribbon.RibbonButton
End Class

Partial Class ThisRibbonCollection
    Inherits Microsoft.Office.Tools.Ribbon.RibbonReadOnlyCollection

    <System.Diagnostics.DebuggerNonUserCode()> _
    Friend ReadOnly Property InteropRibbon() As InteropRibbon
        Get
            Return Me.GetRibbon(Of InteropRibbon)()
        End Get
    End Property
End Class
