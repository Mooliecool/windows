Partial Public Class ColorComb
    Inherits UserControl

    ''' <summary>
    ''' Class members.
    ''' </summary>
    Dim m_selectedColor As Color

    ''' <summary>
    ''' Initialization.
    ''' </summary>
    Public Sub New()

        InitializeComponent()

        InitializeChildren()

    End Sub

    ''' <summary>
    ''' Interface.
    ''' </summary>
    Public Property SelectedColor() As Color
        Get
            Return m_selectedColor
        End Get
        Set(ByVal value As Color)
            m_selectedColor = value
        End Set
    End Property

    Public Event ColorSelected As EventHandler(Of ColorEventArgs)

    Const MaxGenerations As Int32 = 6  '127 tiles

    Dim _rootCell As HexagonButton


    ''' <summary>
    ''' Define honeycomb of the color picker
    ''' </summary>
    Sub InitializeChildren()

        ' Define honeycomb of 127 hexagons, starting in the center of the canvas.
        _rootCell = New HexagonButton(CType(FindResource("HexagonButtonStyle"), Style))
        Canvas.SetLeft(_rootCell, canvas1.Width / 2)
        Canvas.SetTop(_rootCell, canvas1.Height / 2)
        canvas1.Children.Add(_rootCell)

        AddHandler _rootCell.Click, AddressOf cell_clicked

        ' Expand outward (recursive loop).
        InitializeChildrenR(_rootCell, 1)

        ' Apply nominal color gradients.
        CascadeChildColors()

    End Sub

    ''' <summary>
    ''' Create surrounding nodes of the color picker
    ''' </summary>
    Sub InitializeChildrenR(ByVal parent As HexagonButton, ByVal generation As Int32)

        If generation > MaxGenerations Then Return

        For i As Int32 = 0 To 5

            If parent.Neighbors(i) Is Nothing Then

                Dim cell As HexagonButton = New HexagonButton(CType(FindResource("HexagonButtonStyle"), Style))
                Dim dx As Double = Canvas.GetLeft(parent) + HexagonButton.Offset * Math.Cos(i * Math.PI / 3D)
                Dim dy As Double = Canvas.GetTop(parent) + HexagonButton.Offset * Math.Sin(i * Math.PI / 3D)
                Canvas.SetLeft(cell, dx)
                Canvas.SetTop(cell, dy)
                canvas1.Children.Add(cell)
                parent.Neighbors(i) = cell

                AddHandler cell.Click, AddressOf cell_Clicked

            End If
        Next

        ' Set the cross-pointers on the 6 surrounding nodes.
        For i As Int32 = 0 To 5

            Dim child As HexagonButton = parent.Neighbors(i)
            If Not child Is Nothing Then

                Dim ip3 As Int32 = (i + 3) Mod 6
                child.Neighbors(ip3) = parent

                Dim ip1 As Int32 = (i + 1) Mod 6
                Dim ip2 As Int32 = (i + 2) Mod 6
                Dim im1 As Int32 = (i + 5) Mod 6
                Dim im2 As Int32 = (i + 4) Mod 6
                child.Neighbors(ip2) = parent.Neighbors(ip1)
                child.Neighbors(im2) = parent.Neighbors(im1)

            End If
        Next

        ' Recurse into each of the 6 surrounding nodes.

        For i As Int32 = 0 To 5

            InitializeChildrenR(parent.Neighbors(i), generation + 1)

        Next
    End Sub

    Sub CascadeChildColors()

        _rootCell.NominalColor = Color.FromScRgb(1.0F, 1.0F, 1.0F, 1.0F)
        CascadeChildColorsR(_rootCell)

        For Each h As HexagonButton In canvas1.Children

            h.Visited = False

        Next

    End Sub

    Sub CascadeChildColorsR(ByVal parent As HexagonButton)

        Dim delta As Single = 1.0F / MaxGenerations
        Dim ceiling As Single = 0.99F

        Dim visitedNodes As System.Collections.Generic.List(Of HexagonButton) = _
                            New System.Collections.Generic.List(Of HexagonButton)(6)

        For i As Int32 = 0 To 5

            Dim child As HexagonButton = parent.Neighbors(i)
            If Not child Is Nothing AndAlso Not child.visited Then

                Dim c As Color = parent.NominalColor
                Select Case i

                    Case 0 ' increase cyan; else reduce red
                        If c.ScG < ceiling And c.ScB < ceiling Then

                            c.ScG = Math.Min(Math.Max(0.0F, c.ScG + delta), 1.0F)
                            c.ScB = Math.Min(Math.Max(0.0F, c.ScB + delta), 1.0F)

                        Else

                            c.ScR = Math.Min(Math.Max(0.0F, c.ScR - delta), 1.0F)

                        End If

                    Case 1 ' increase blue; else reduce yellow
                        If c.ScB < ceiling Then

                            c.ScB = Math.Min(Math.Max(0.0F, c.ScB + delta), 1.0F)

                        Else

                            c.ScR = Math.Min(Math.Max(0.0F, c.ScR - delta), 1.0F)
                            c.ScG = Math.Min(Math.Max(0.0F, c.ScG - delta), 1.0F)

                        End If

                    Case 2 ' increase magenta; else reduce green
                        If c.ScR < ceiling And c.ScB < ceiling Then

                            c.ScR = Math.Min(Math.Max(0.0F, c.ScR + delta), 1.0F)
                            c.ScB = Math.Min(Math.Max(0.0F, c.ScB + delta), 1.0F)

                        Else

                            c.ScG = Math.Min(Math.Max(0.0F, c.ScG - delta), 1.0F)

                        End If

                    Case 3 ' increase red; else reduce cyan
                        If c.ScR < ceiling Then

                            c.ScR = Math.Min(Math.Max(0.0F, c.ScR + delta), 1.0F)

                        Else

                            c.ScG = Math.Min(Math.Max(0.0F, c.ScG - delta), 1.0F)
                            c.ScB = Math.Min(Math.Max(0.0F, c.ScB - delta), 1.0F)

                        End If

                    Case 4 ' increase yellow; else reduce blue
                        If c.ScR < ceiling And c.ScG < ceiling Then

                            c.ScR = Math.Min(Math.Max(0.0F, c.ScR + delta), 1.0F)
                            c.ScG = Math.Min(Math.Max(0.0F, c.ScG + delta), 1.0F)

                        Else

                            c.ScB = Math.Min(Math.Max(0.0F, c.ScB - delta), 1.0F)

                        End If

                    Case 5 ' increase green; else reduce magenta
                        If c.ScG < ceiling Then

                            c.ScG = Math.Min(Math.Max(0.0F, c.ScG + delta), 1.0F)

                        Else

                            c.ScR = Math.Min(Math.Max(0.0F, c.ScR - delta), 1.0F)
                            c.ScB = Math.Min(Math.Max(0.0F, c.ScB - delta), 1.0F)

                        End If
                End Select

                child.NominalColor = c
                child.visited = True
                visitedNodes.Add(child)

            End If
        Next

        parent.Visited = True '  ensures root node not over-visited
        For Each child As HexagonButton In visitedNodes

            CascadeChildColorsR(child)

        Next
    End Sub

#Region "Event Handlers"

    Sub cell_clicked(ByVal sender As Object, ByVal e As RoutedEventArgs)

        Dim cell As HexagonButton = CType(sender, HexagonButton)

        m_selectedColor = cell.NominalColor
        FireColorSelected()

    End Sub

    Sub FireColorSelected()

        RaiseEvent ColorSelected(Me, New ColorEventArgs(m_selectedColor))

    End Sub

#End Region

End Class

Public Class HexagonButton
    Inherits Button

    Public Const Radius As Double = 12D ' matches pathgeometry coded in XAML
    Public Shared ReadOnly Offset As Double = Radius * 2 * Math.Cos(30D * Math.PI / 180D) + 1.5

    '
    ' Interface

    Public Sub New(ByVal style As Style)

        Me.Style = style

        _neighbors = New HexagonButton(6) {}
        _nominalColor = Color.FromScRgb(1.0F, 1.0F, 1.0F, 1.0F)

    End Sub

    Public Property NominalColor() As Color
        Get
            Return _nominalColor
        End Get
        Set(ByVal value As Color)
            _nominalColor = value
            Background = ConstructBackgroundGradient(0.5F)
        End Set
    End Property

    '
    ' Internals

    Friend ReadOnly Property Neighbors() As HexagonButton()
        Get
            Return _neighbors
        End Get
    End Property

    Friend visited As Boolean

    '
    ' Implementation

    Dim _neighbors As HexagonButton()
    Dim _nominalColor As Color

    Function ConstructBackgroundGradient(ByVal inflection As Single) As Brush

        Dim a As Color = NominalColor
        Dim z As Color = Colors.Black
        Dim m As Color = Color.FromScRgb(a.ScA / 2, a.ScR / 2, a.ScG / 2, a.ScB / 2)

        Dim lgb As LinearGradientBrush = New LinearGradientBrush()
        lgb.ColorInterpolationMode = ColorInterpolationMode.ScRgbLinearInterpolation
        lgb.StartPoint = New Point(0.5, 0)
        lgb.EndPoint = New Point(0, 0.75)
        lgb.GradientStops.Add(New GradientStop(a, 0D))
        lgb.GradientStops.Add(New GradientStop(m, 0.8 * (1D - inflection)))
        lgb.GradientStops.Add(New GradientStop(z, 1D))
        Return lgb

    End Function

    Protected Overrides Sub OnMouseEnter(ByVal e As System.Windows.Input.MouseEventArgs)

        MyBase.OnMouseEnter(e)
        Background = ConstructBackgroundGradient(0.2F)

    End Sub

    Protected Overrides Sub OnMouseLeave(ByVal e As System.Windows.Input.MouseEventArgs)

        MyBase.OnMouseLeave(e)
        Background = ConstructBackgroundGradient(0.5F)

    End Sub

    Protected Overrides Sub OnIsPressedChanged(ByVal e As System.Windows.DependencyPropertyChangedEventArgs)

        MyBase.OnIsPressedChanged(e)
        If IsPressed Then

            Background = ConstructBackgroundGradient(0.8F)

        Else

            Background = ConstructBackgroundGradient(0.5F)

        End If
    End Sub
End Class

Public Class ColorEventArgs
    Inherits EventArgs

    Dim m_color As Color

    Public Sub New()
    End Sub

    Public Sub New(ByVal c As Color)
        m_color = c
    End Sub

    Public Property Color() As Color
        Get
            Return m_color
        End Get
        Set(ByVal value As Color)
            m_color = value
        End Set
    End Property
End Class