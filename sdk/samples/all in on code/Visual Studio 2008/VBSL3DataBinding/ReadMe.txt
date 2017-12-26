========================================================================
    SILVERLIGHT APPLICATION : VBSL3DataBinding Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example demonstrates how to work with Data Binding Silverlight 3.
It includes the following general scenarios:

How to use onetime/oneway/twoway Data Binding.
What is IValueConverter and how to use it.
How to handle data validation in Data Binding.
How to use Element Data Binding to bind to the property of other controls.
What is ObservableCollection.
General problems you may encounter with when using Data Binding.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Chained Installer
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. How to use onetime/oneway/twoway Data Binding.

Please run the project and following instruction to edit TextBox. You'll notice what's the
difference among binding modes.

By specifying Mode like below you can control the Binding Mode.

<TextBlock HorizontalAlignment="Left" Margin="20,0" FontSize="11" TextWrapping="Wrap" Text="{Binding Name, Mode=OneTime}"/>
<TextBlock HorizontalAlignment="Left" Margin="20,0" FontSize="11" TextWrapping="Wrap" Text="{Binding Name, Mode=OneWay}"/>
<TextBox HorizontalAlignment="Left" Margin="20,0,0,0" FontSize="11" 
TextWrapping="Wrap" Text="{Binding Name, Mode=TwoWay,ValidatesOnExceptions=true, NotifyOnValidationError=true}"/>   

For two way databinding to work property, the source class must implement INotifyPropertyChanged.
Please refer to Customer class in DAL.vb to learn how to implement this interface.

2. What is IValueConverter and how to use it.

By specifying the Converter of a Binding we can use our custom ValueConverter class that implements IValueConverter.
Converters can change data from one type to another, translate data based on cultural information, 
or modify other aspects of the presentation. Core code logic is like below:

<UserControl.Resources>
        <local:MyConverter x:Key="myconverter"></local:MyConverter>
</UserControl.Resources>
    ...
<StackPanel Background="{Binding ID,Converter={StaticResource myconverter}}">


''' <summary>
''' This is a custom ValueConverter class. It converts int to Brush. 
''' If ID is larger than 1 a redbrush will be returned.
''' </summary>
Public Class MyConverter
    Implements IValueConverter
    Private redbrush As Brush = New SolidColorBrush(Color.FromArgb(255, 255, 0, 0))
    Public Function Convert(ByVal value As Object, ByVal targetType As Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements IValueConverter.Convert

        If TypeOf value Is Integer Then

            If CInt(Fix(value)) > 1 Then
                Return redbrush
            Else
                Return Nothing
            End If
        Else
            Return Nothing
        End If
    End Function

    Public Function ConvertBack(ByVal value As Object, ByVal targetType As Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements IValueConverter.ConvertBack
        Throw New NotImplementedException()
    End Function

3. How to handle data validation in Data Binding.

In two way binding scenario validation is always needed.

The validation will be done when exception is thrown during update of source objects,
as long as you've set ValidatesOnExceptions to ture. 

If you set NotifyOnValidationError to ture as well you can notify contol by triggering
BindingValidationError event. You don't have to write code to manually do so. 
All Controls have this feature.

To set ValidatesOnExceptions and BindingValidationErrorin XAML, please use the following code:

<TextBox HorizontalAlignment="Left" Margin="20,0,0,0" FontSize="11" 
TextWrapping="Wrap" Text="{Binding Name, Mode=TwoWay,ValidatesOnExceptions=true, NotifyOnValidationError=true}"/>

In paticular, we can use Attributes in System.ComponentModel.DataAnnotations
namespace to help us define restrictions of the value passed to a property pending update
and throw exception when it's invalid. 

To do that,in binding source property's set accessor, add following code. MemberName is the property name.

 Dim vc As ValidationContext = New ValidationContext(Me, Nothing, Nothing)
                vc.MemberName = "Name"
                Validator.ValidateProperty(value, vc)
                                       
In most cases the built-in Attributes is enough for use.

For advanced scenarios we may need custom validation means. To do this we can attach CustomValidationAttribute,
to that property, like below. The first parameter is the type of the custom validation class. The
second parameter is the validate method in the custom validation class:

<CustomValidation(GetType(MyValidation), "Validate")> _
  Public Property ID() As Integer
...

  Public Class MyValidation
        Private Sub New()
        End Sub
        Public Shared Function Validate(ByVal [property] As Object, ByVal context As ValidationContext) As ValidationResult
            ' You can get Property name from context.MemberName. It's not used here for simplicity.
            Dim customerid As Integer
            ' For the simplicity, the change is invalid if new ID equals 11.
            If Int32.TryParse([property].ToString(), customerid) AndAlso customerid = 11 Then
                Dim validationResult As ValidationResult = New ValidationResult("Custom Validation Failed. ID cannot be 11")
                Return validationResult
            Else
                Return ValidationResult.Success
            End If
        End Function
    End Class

4. How to use Element Data Binding to bind to the property of other controls.

To use Element binding, we need to specify the ElementName for Binding and set binding source
crrectly. The following code in the sample binds the Text property of the TextBlock to the Name
property of the SelectedItem of the DataGrid. In this sample, SelectedItem is a Customer object so we
can set SelectedItem.Name as binding source.

<TextBlock Text="{Binding SelectedItem.Name, ElementName=ConverterScenarioDataGrid}"></TextBlock>

5. What is ObservableCollection.

To notify collection elements' change (add or remove), collection class should implement 
INotifyCollectionChanged interface. ObservableCollection(Of T) has already implemented it so it's
always recommended to use it as your collection class.


6. General problems you may encounter with when using Data Binding.

Please run the project and click GeneralProblems tab for more details.


/////////////////////////////////////////////////////////////////////////////
References:

Data Binding
http://msdn.microsoft.com/en-us/library/cc278072(VS.95).aspx

Dependency Properties Overview
http://msdn.microsoft.com/en-us/library/cc221408(VS.95).aspx

Using Data Annotations to Customize Data Classes
http://msdn.microsoft.com/en-us/library/dd901590(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////
