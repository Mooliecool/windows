================================================================================
       WPF APPLICATION : CSWPFTwoLevelGrouping Project Overview
       
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
                                <TextBlock Text="{Binding Items[0].Class}" TextBlock.FontWeight="Bold"/>
                            </Expander.Header>
                            <ItemsPresenter />
                        </Expander>
                    </ControlTemplate>
                </Setter.Value>
            </Setter>
        </Style> 
        

5. Create a custom StyleSelector to select style for different level of GroupItem.

    public class GroupItemStyleSelector : StyleSelector
    {
        public override Style SelectStyle(object item, DependencyObject container)
        {
            Style s;

            CollectionViewGroup group = item as CollectionViewGroup;
            Window window = Application.Current.MainWindow;

            if (!group.IsBottomLevel)
            {
                s = window.FindResource("GroupHeaderStyleForFirstLevel") as Style;
            }
            else
            {
                s = window.FindResource("GroupHeaderStyleForSecondLevel") as Style;
            }

            return s;
        }
    }
    
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
