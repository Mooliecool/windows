=============================================================================
    WPF/WF4 APPLICATION : VBWF4CustomeSequenceActivity
=============================================================================
/////////////////////////////////////////////////////////////////////////////
Summary:

Generally, we use the built in Sequence activity to build a sequence workflow.
Sequence activity is a composite activity. in this sample I demonstrated
creating an customized composite activity: MySequenceActivity. and also 
its activity designer so that you can use it in a workflow designer. If you 
are going to create your own WF4 activity, you can use this sample as a
reference. 

/////////////////////////////////////////////////////////////////////////////
Demo:

1. Open VBWF4CustomSequenceActivity.sln with Visual Studio 2010
2. Press Ctrl+F5.


/////////////////////////////////////////////////////////////////////////////
Prerequisite

1. Visual Studio 2010
2. .NET Framework 4.0


/////////////////////////////////////////////////////////////////////////////
Implementation:

1. Create a Workflow Console Application base on .NET Framework 4.0.

2. Add a new Activity Designer to the project by following actions:
   a. Right click the project name and select Add|New Item...
   b. Click Workflow|Activity Designer. then name the file MySequenceDesigner.xaml

3. Open file MySequenceDesigner.xaml.cs and alter its code to:
<sap:ActivityDesigner x:Class="MySequenceDesigner"
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:sap="clr-namespace:System.Activities.Presentation;assembly=System.Activities.Presentation"
    xmlns:sapv="clr-namespace:System.Activities.Presentation.View;assembly=System.Activities.Presentation">
    <Grid>
        <StackPanel>
            <sap:WorkflowItemsPresenter HintText="Drop Activities Here"
                                        Items="{Binding Path=ModelItem.Branches,Mode=TwoWay}">
                <sap:WorkflowItemsPresenter.SpacerTemplate>
                    <DataTemplate>
                        <Path Margin="0,15,0,0"                                             
                              Stretch="Fill" 
                                    StrokeMiterLimit="2.75" 
                                    Stroke="#FFA8B3C2" Fill="#FFFFFFFF" 
                                    Data="F1 M 675.738,744.979L 665.7,758.492L 655.66,744.979L 675.738,744.979 Z " 
                                    Width="16" Height="10" />
                    </DataTemplate>
                </sap:WorkflowItemsPresenter.SpacerTemplate>
                <sap:WorkflowItemsPresenter.ItemsPanel>
                    <ItemsPanelTemplate>
                        <StackPanel Orientation="Vertical"/>
                    </ItemsPanelTemplate>
                </sap:WorkflowItemsPresenter.ItemsPanel>
            </sap:WorkflowItemsPresenter>
        </StackPanel>
    </Grid>
</sap:ActivityDesigner>
Then, save and build the project.

4. Add a new code file named MySequenceActivity.vb, fill the file with the 
   following code:
Imports System.ComponentModel
Imports System.Activities
Imports System.Collections.ObjectModel

<Designer(GetType(MySequenceDesigner))>
Public Class MySequenceActivity
    Inherits NativeActivity

    Public Property Branches() As Collection(Of Activity)
        Get
            Return _Branches
        End Get
        Set(ByVal value As Collection(Of Activity))
            _Branches = value
        End Set
    End Property
    Private _Branches As Collection(Of Activity)

    Public Property Variables() As Collection(Of Variable)
        Get
            Return _Variables
        End Get
        Set(ByVal value As Collection(Of Variable))
            _Variables = value
        End Set
    End Property
    Private _Variables As Collection(Of Variable)

    Public Sub New()
        Branches = New Collection(Of Activity)()
        Variables = New Collection(Of Variable)()
    End Sub

    Dim activityCounter As Integer

    Protected Overrides Sub CacheMetadata(ByVal metadata As System.Activities.NativeActivityMetadata)

        metadata.SetChildrenCollection(Branches)
        metadata.SetVariablesCollection(Variables)

    End Sub

    Protected Overrides Sub Execute(ByVal context As System.Activities.NativeActivityContext)

        ScheduleActivities(context)

    End Sub

    Protected Sub ScheduleActivities(ByVal context As NativeActivityContext)

        If activityCounter < Branches.Count Then

            context.ScheduleActivity(Me.Branches(activityCounter), AddressOf OnActivityCompleted)
            activityCounter = activityCounter + 1

        End If

    End Sub

    Protected Sub OnActivityCompleted(ByVal context As NativeActivityContext, ByVal completedInstance As ActivityInstance)

        ScheduleActivities(context)

    End Sub

End Class
Then, save and build the project. 

5. Open the default created Workflow1.xaml and drag the MySequenceActivity
   from the toolbox panel to the workflow designer. and create a simple
   workflow to test it.


/////////////////////////////////////////////////////////////////////////////
Reference:

A Developer's Introduction to Windows Workflow Foundation (WF) in .NET 4
http://msdn.microsoft.com/en-us/library/ee342461.aspx