
Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Data
Imports System.Windows.Documents
Imports System.Windows.Input
Imports System.Windows.Media
Imports System.Windows.Media.Imaging
Imports System.Windows.Navigation
Imports System.Windows.Shapes


'/ <summary>
'/ Interaction logic for NumericUpDownControl.xaml
'/ </summary>

Class NumericUpDownControl
    Inherits System.Windows.Controls.UserControl

    Public Sub New()
        InitializeComponent()

        UpdateTextBlock()

    End Sub 'New
    
    '/ <summary>
    '/ Identifies the DecreaseButtonContent property.
    '/ </summary>
    Public Shared DecreaseButtonContentProperty As DependencyProperty = _
                            DependencyProperty.Register("DecreaseButtonContent", _
                                GetType(Object), GetType(NumericUpDownControl), _
                                New PropertyMetadata( _
                                    New PropertyChangedCallback(AddressOf OnDecreaseTextChanged)))
    
    
    
    Private Shared Sub OnDecreaseTextChanged( _
                    ByVal obj As DependencyObject, _
                    ByVal args As DependencyPropertyChangedEventArgs)

        Dim control As NumericUpDownControl = CType(obj, NumericUpDownControl)
        control.downButton.Content = args.NewValue

    End Sub 'OnDecreaseTextChanged 
    
    
    '/ <summary>
    '/ Gets or sets the content in the Button that
    '/ Decreases Value.
    '/ </summary>
    
    Public Property DecreaseButtonContent() As Object 
        Get
            Return GetValue(DecreaseButtonContentProperty)
        End Get
        Set
            SetValue(DecreaseButtonContentProperty, value)
        End Set
    End Property 
    
    
    '//////////////////////////////////////////////////////////////
    '/ <summary>
    '/ Identifies the IncreaseButtonContent property.
    '/ </summary>
    Public Shared IncreaseButtonContentProperty As DependencyProperty = _
                        DependencyProperty.Register("IncreaseButtonContent", _
                            GetType(Object), GetType(NumericUpDownControl), _
                            New PropertyMetadata( _
                            New PropertyChangedCallback(AddressOf OnIncreaseTextChanged)))
    
    
    
    Private Shared Sub OnIncreaseTextChanged( _
                    ByVal obj As DependencyObject, _
                    ByVal args As DependencyPropertyChangedEventArgs)

        Dim control As NumericUpDownControl = CType(obj, NumericUpDownControl)
        control.upButton.Content = args.NewValue

    End Sub 'OnIncreaseTextChanged 
    
    
    '/ <summary>
    '/ Gets or sets the content in the Button that
    '/ increases Value.
    '/ </summary>
    
    Public Property IncreaseButtonContent() As Object 
        Get
            Return GetValue(IncreaseButtonContentProperty)
        End Get
        Set
            SetValue(IncreaseButtonContentProperty, value)
        End Set
    End Property 
    
    '/ <summary>
    '/ Gets or sets the value assigned to the control.
    '/ </summary>
    
    Public Property Value() As Decimal 
        Get
            Return System.Convert.ToDecimal(GetValue(ValueProperty))
        End Get
        Set
            SetValue(ValueProperty, value)
        End Set
    End Property 
    '/ <summary>
    '/ Identifies the Value dependency property.
    '/ </summary>
    Public Shared ValueProperty As DependencyProperty = _
                        DependencyProperty.Register("Value", _
                            GetType(Decimal), GetType(NumericUpDownControl), _
                            New FrameworkPropertyMetadata( _
                                New PropertyChangedCallback(AddressOf OnValueChanged)))
    
    
    Private Shared Sub OnValueChanged( _
                        ByVal obj As DependencyObject, _
                        ByVal args As DependencyPropertyChangedEventArgs)

        Dim control As NumericUpDownControl = CType(obj, NumericUpDownControl)

        control.UpdateTextBlock()

        Dim e As New RoutedPropertyChangedEventArgs(Of Decimal)( _
                   System.Convert.ToDecimal(args.OldValue), _
                   System.Convert.ToDecimal(args.NewValue), ValueChangedEvent) '

        control.OnValueChanged(e)

    End Sub 'OnValueChanged
    
    '/ <summary>
    '/ Identifies the ValueChanged routed event.
    '/ </summary>
    Public Shared ValueChangedEvent As RoutedEvent = _
            EventManager.RegisterRoutedEvent("ValueChanged", RoutingStrategy.Bubble, _
                        GetType(RoutedPropertyChangedEventHandler(Of Decimal)), _
                        GetType(NumericUpDownControl))

    '/// <summary>
    '/// Occurs when the Value property changes.
    '/// </summary>
    Public Custom Event ValueChanged As RoutedPropertyChangedEventHandler(Of Decimal)

        AddHandler(ByVal value As RoutedPropertyChangedEventHandler(Of Decimal))

            Me.AddHandler(ValueChangedEvent, value)
        End AddHandler

        RemoveHandler(ByVal value As RoutedPropertyChangedEventHandler(Of Decimal))
            Me.RemoveHandler(ValueChangedEvent, value)

        End RemoveHandler

        RaiseEvent(ByVal sender As Object, ByVal e As RoutedPropertyChangedEventArgs(Of Decimal))

            Me.RaiseEvent(e)
        End RaiseEvent


    End Event

    '/ <summary>
    '/ Raises the ValueChanged event.
    '/ </summary>
    '/ <param name="args">Arguments associated with the ValueChanged event.</param>
    Protected Overridable Sub OnValueChanged(ByVal args As RoutedPropertyChangedEventArgs(Of Decimal))
        Me.RaiseEvent(args)
    End Sub

    Sub upButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Value += 1
    End Sub

    Sub downButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Value -= 1
    End Sub

    Sub UpdateTextBlock()

        valueText.Text = Value.ToString()

    End Sub
End Class 'NumericUpDownControl 