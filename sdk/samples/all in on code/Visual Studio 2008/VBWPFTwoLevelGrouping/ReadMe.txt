================================================================================
       WPF APPLICATION : VBWPFTwoLevelGrouping Project Overview
       
                       WPF Two Level Grouping Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The sample demonstrates how to display two level grouped data in WPF.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:


1. Create a Student class with properties of ID, Name, Class, Grade, etc.

2. Define a ListView with columns binding to each properties of the Student object;

        <ListView Margin="14,17,16,14" Name="listView1" ItemsSource="{Binding}">
            ....
            <!-- Set up columns -->
            <ListView.View>
                <GridView>
                    <GridViewColumn DisplayMemberBinding="{Binding ID}"    Header="ID"    Width="50"/>
                    <GridViewColumn DisplayMemberBinding="{Binding Name}"  Header="Name"  Width="100"/>
                    <GridViewColumn DisplayMemberBinding="{Binding Class}" Header="Class" Width="50"/>
                    <GridViewColumn DisplayMemberBinding="{Binding Grade}" Header="Grade" Width="50"/>
                </GridView>
            </ListView.View>
            
        </ListView>
     
3. Create a list of Student objects, and bind it to ListView.

4. Define a style for each GroupItem.

        <!-- Style for the first level GroupItem -->
        <Style x:Key="GroupHeaderStyleForFirstLevel" TargetType="{x:Type GroupItem}">
            <Setter Property="Template">
                <Setter.Value>
                    <ControlTemplate TargetType="{x:Type GroupItem}">
                        <Expander IsExpanded="True">
                            <Expander.Header>
                                <TextBlock Text="{Binding Name}" TextBlock.FontWeight="Bold"/>
                            </Expander.Header>
                            <ItemsPresenter />
                        </Expander>
                    </ControlTemplate>
                </Setter.Value>
            </Setter>
        </Style>
        
        <!-- Style for the second level GroupItem -->
        <Style x:Key="GroupHeaderStyleForSecondLevel" TargetType="{x:Type GroupItem}">
            <Setter Property="Template">
                <Setter.Value>
                    <ControlTemplate TargetType="{x:Type GroupItem}">
                        <Expander IsExpanded="True" Margin="15,0,0,0">
                            <Expander.Header>
                                <TextBlock Text="{Binding Name}" TextBlock.FontWeight="Bold"/>
                            </Expander.Header>
                            <ItemsPresenter />
                        </Expander>
                    </ControlTemplate>
                </Setter.Value>
            </Setter>
        </Style> 
        

5. Create a custom StyleSelector to select style for different level of GroupItem.

    Public Class GroupItemStyleSelector
        Inherits StyleSelector
        Public Overloads Overrides Function SelectStyle(ByVal item As Object, ByVal container As DependencyObject) As Style
            Dim s As Style

            Dim group As CollectionViewGroup = TryCast(item, CollectionViewGroup)
            Dim window As Window = Application.Current.MainWindow

            If Not group.IsBottomLevel Then
                s = TryCast(window.FindResource("GroupHeaderStyleForFirstLevel"), Style)
            Else
                s = TryCast(window.FindResource("GroupHeaderStyleForSecondLevel"), Style)
            End If

            Return s
        End Function
    End Class
    
6. Use the StyleSelector in the ListView.

        <ListView Margin="14,17,16,14" Name="listView1" ItemsSource="{Binding}">
            <ListView.GroupStyle>
                <GroupStyle ContainerStyleSelector="{StaticResource groupItemStyleSelector}" />
            </ListView.GroupStyle>
            
            ....
            
        </ListView>

/////////////////////////////////////////////////////////////////////////////
References:

   

/////////////////////////////////////////////////////////////////////////////
Demo

Step1. Build the sample project in Visual Studio 2008.

Step2. Start Without Debugging, and the mian window of the project will show.

Step3. The tems displayed in the window by groups, and they all expanded, the root group name is Grade, and then the Class.

Step4. Click on the group name will toggle the expanding state of the selected group.

/////////////////////////////////////////////////////////////////////////////