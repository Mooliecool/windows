Imports System.Windows.Ink

Partial Public Class ColorPicker
    Inherits Canvas

    ''' <summary>
    ''' Class members
    ''' </summary>
    Dim theDrawingAttributes As DrawingAttributes
    Dim _notUserInitiated As Boolean

    ''' <summary>
    ''' Initialization
    ''' </summary>
    Public Sub New()

        InitializeComponent()

    End Sub

    ''' <summary>
    ''' Completes initialization after all XAML member vars have been initialized.
    ''' </summary>
    ''' <param name="e"></param>
    Protected Overrides Sub OnInitialized(ByVal e As System.EventArgs)

        MyBase.OnInitialized(e)

        theDrawingAttributes = New DrawingAttributes()
        UpdateControlValues()
        UpdateControlVisuals()

        AddHandler colorComb.ColorSelected, AddressOf colorComb_ColorSelected
        AddHandler brightnessSlider.ValueChanged, AddressOf brightnessSlider_ValueChanged
        AddHandler opacitySlider.ValueChanged, AddressOf opacitySlider_ValueChanged
        AddHandler ellipticalRadio.Checked, AddressOf radio_Click
        AddHandler rectangularRadio.Checked, AddressOf radio_Click
        AddHandler ignorepsiCheckbox.Click, AddressOf checkbox_Click
        AddHandler fitcurveCheckbox.Click, AddressOf checkbox_Click
        AddHandler decrementThickness.Click, AddressOf decrementThickness_Click
        AddHandler incrementThickness.Click, AddressOf incrementThickness_Click

    End Sub

    ''' <summary>
    ''' Interface
    ''' </summary>
    Public Property SelectedDrawingAttributes() As DrawingAttributes
        Get
            Return theDrawingAttributes
        End Get
        Set(ByVal value As DrawingAttributes)
            theDrawingAttributes = value
            UpdateControlValues()
            UpdateControlVisuals()
        End Set
    End Property

    ''' <summary>
    ''' Updates values of controls when new DA is set (or upon initialization).
    ''' </summary>
    Public Sub UpdateControlValues()

        _notUserInitiated = True
        Try

            ' Set nominal color on comb.
            Dim nc As Color = theDrawingAttributes.Color
            Dim f As Single = Math.Max(Math.Max(nc.ScR, nc.ScG), nc.ScB)
            If f < 0.001F Then  'black
                nc = Color.FromScRgb(1.0F, 1.0F, 1.0F, 1.0F)
            Else
                nc = Color.FromScRgb(1.0F, nc.ScR / f, nc.ScG / f, nc.ScB / f)
            End If

            colorComb.SelectedColor = nc

            ' Set brightness and opacity.
            brightnessSlider.Value = f
            opacitySlider.Value = theDrawingAttributes.Color.ScA

            ' Set stylus characteristics.
            ellipticalRadio.IsChecked = (theDrawingAttributes.StylusTip = StylusTip.Ellipse)
            rectangularRadio.IsChecked = (theDrawingAttributes.StylusTip = StylusTip.Rectangle)
            ignorepsiCheckbox.IsChecked = Not (theDrawingAttributes.IgnorePressure)
            fitcurveCheckbox.IsChecked = (theDrawingAttributes.FitToCurve)

        Finally

            _notUserInitiated = False

        End Try

    End Sub

    ''' <summary>
    ''' Updates visual properties of all controls, in response to any change.
    ''' </summary>
    Public Sub UpdateControlVisuals()

        Dim c As Color = theDrawingAttributes.Color

        ' Update LGB for brightnessSlider
        Dim sb1 As Border = CType(brightnessSlider.Parent, Border)
        Dim lgb1 As LinearGradientBrush = CType(sb1.Background, LinearGradientBrush)
        lgb1.GradientStops(1).Color = colorComb.SelectedColor

        ' Update LGB for opacitySlider
        Dim c2a As Color = Color.FromScRgb(0.0F, c.ScR, c.ScG, c.ScB)
        Dim c2b As Color = Color.FromScRgb(1.0F, c.ScR, c.ScG, c.ScB)
        Dim sb2 As Border = CType(opacitySlider.Parent, Border)
        Dim lgb2 As LinearGradientBrush = CType(sb2.Background, LinearGradientBrush)
        lgb2.GradientStops(0).Color = c2a
        lgb2.GradientStops(1).Color = c2b

        ' Update controls
        theDrawingAttributes.Width = Math.Round(theDrawingAttributes.Width, 2)
        thicknessTextbox.Text = theDrawingAttributes.Width.ToString()
        fitcurveCheckbox.IsChecked = theDrawingAttributes.FitToCurve
        ignorepsiCheckbox.IsChecked = Not theDrawingAttributes.IgnorePressure
        ellipticalRadio.IsChecked = (theDrawingAttributes.StylusTip = StylusTip.Ellipse)
        rectangularRadio.IsChecked = (theDrawingAttributes.StylusTip = StylusTip.Rectangle)

    End Sub

#Region "Event Handlers to update color picker UI"

    Sub colorComb_ColorSelected(ByVal sender As Object, ByVal e As ColorEventArgs)

        If _notUserInitiated Then Return

        Dim a, f, r, g, b As Single
        a = CType(opacitySlider.Value, Single)
        f = CType(brightnessSlider.Value, Single)

        Dim nc As Color = e.Color
        r = f * nc.ScR
        g = f * nc.ScG
        b = f * nc.ScB

        theDrawingAttributes.Color = Color.FromScRgb(a, r, g, b)
        UpdateControlVisuals()

    End Sub

    Sub brightnessSlider_ValueChanged(ByVal sender As Object, ByVal e As RoutedPropertyChangedEventArgs(Of Double))

        If _notUserInitiated Then Return

        Dim nc As Color = colorComb.SelectedColor
        Dim f As Single = CType(e.NewValue, Single)

        Dim a, r, g, b As Single
        a = CType(opacitySlider.Value, Single)
        r = f * nc.ScR
        g = f * nc.ScG
        b = f * nc.ScB

        theDrawingAttributes.Color = Color.FromScRgb(a, r, g, b)
        UpdateControlVisuals()

    End Sub

    Sub opacitySlider_ValueChanged(ByVal sender As Object, ByVal e As RoutedPropertyChangedEventArgs(Of Double))

        If _notUserInitiated Then Return

        Dim c As Color = theDrawingAttributes.Color
        Dim a As Single = CType(e.NewValue, Single)

        theDrawingAttributes.Color = Color.FromScRgb(a, c.ScR, c.ScG, c.ScB)
        UpdateControlVisuals()

    End Sub

    Sub radio_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        If _notUserInitiated Then Return

        If sender Is ellipticalRadio Then
            theDrawingAttributes.StylusTip = StylusTip.Ellipse
        End If
        If sender Is rectangularRadio Then
            theDrawingAttributes.StylusTip = StylusTip.Rectangle
        End If

        UpdateControlVisuals()

    End Sub

    Sub checkbox_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        If _notUserInitiated Then Return

        If sender Is ignorepsiCheckbox Then
            theDrawingAttributes.IgnorePressure = IIf(ignorepsiCheckbox.IsChecked, False, True)
        End If

        If sender Is ignorepsiCheckbox Then
            theDrawingAttributes.IgnorePressure = IIf(fitcurveCheckbox.IsChecked, True, False)
        End If

        UpdateControlVisuals()

    End Sub

    Sub incrementThickness_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        If _notUserInitiated Then Return

        If theDrawingAttributes.Width < 1.0 Then

            theDrawingAttributes.Width += 0.1
            theDrawingAttributes.Height += 0.1

        ElseIf theDrawingAttributes.Width < 10.0 Then

            theDrawingAttributes.Width += 0.5
            theDrawingAttributes.Height += 0.5

        Else

            theDrawingAttributes.Width += 1D
            theDrawingAttributes.Height += 1D

        End If

        UpdateControlVisuals()

    End Sub


    Sub decrementThickness_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        If _notUserInitiated Then Return

        If theDrawingAttributes.Width < 0.1001 Then Return

        If theDrawingAttributes.Width < 1.001 Then

            theDrawingAttributes.Width -= 0.1
            theDrawingAttributes.Height -= 0.1

        ElseIf theDrawingAttributes.Width < 10.001 Then

            theDrawingAttributes.Width -= 0.5
            theDrawingAttributes.Height -= 0.5

        Else

            theDrawingAttributes.Width -= 1D
            theDrawingAttributes.Height -= 1D

        End If

        UpdateControlVisuals()

    End Sub

#End Region

End Class