=============================================================================
         WPF/WF4 APPLICATION : CSWF4CustomSequenceActivity
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

Generally, we use the built-in Sequence activity to build a sequence workflow.
Sequence activity is a composite activity. in this sample I demonstrated
creating an customized composite activity: MySequenceActivity. and also 
its activity designer so that you can use it in a workflow designer. If you 
are going to create your own WF4 activity, you can use this sample as a
reference. 

To run the sample:
1. Open CSWF4CustomSequenceActivity.sln with Visual Studio 2010
2. Press Ctrl+F5.


/////////////////////////////////////////////////////////////////////////////
Creation:

1. Create a Workflow Console Application base on .NET Framework 4.0.

2. Add a new Activity Designer to the project by following actions:

   a. Right click the project name and select Add|New Item...
   b. Click Workflow|Activity Designer. then name the file MySequenceDesigner.xaml

3. Open file MySequenceDesigner.xaml.cs and alter its code to:

	<sap:ActivityDesigner x:Class="CSWF4CustomSequenceActivity.MySequenceDesigner"
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

4. Add a new code file named MySequenceActivity.cs, fill the file with the 
   following code:
	using System.ComponentModel;
	using System.Activities;
	using System.Collections.ObjectModel;
	namespace CSWF4CustomSequenceActivity {
		[Designer(typeof(MySequenceDesigner))]
		public class MySequenceActivity : NativeActivity {
			public Collection<Activity> Branches { get; set; }
			public Collection<Variable> Variables { get; set; }
			public MySequenceActivity() {
				Branches = new Collection<Activity>();
				Variables = new Collection<Variable>();
			}
			int activityCounter = 0;
			protected override void CacheMetadata(NativeActivityMetadata metadata) {
				metadata.SetChildrenCollection(Branches);
				metadata.SetVariablesCollection(Variables);
			}

			protected override void Execute(NativeActivityContext context) {
				ScheduleActivities(context);
			}

			void ScheduleActivities(NativeActivityContext context) {
				if (activityCounter < Branches.Count)
					context.ScheduleActivity(this.Branches[activityCounter++], 
						OnActivityCompleted);
			}

			void OnActivityCompleted(NativeActivityContext context, 
				ActivityInstance completedInstance) {
				ScheduleActivities(context);
			}
		}
	}

Then, save and build the project. 

5. Open the default created Workflow1.xaml and drag the MySequenceActivity
   from the toolbox panel to the workflow designer. and create a simple
   workflow to test it.


/////////////////////////////////////////////////////////////////////////////
References:

Build Custom Activities To Extend The Reach Of Your Workflows
http://msdn.microsoft.com/en-us/magazine/cc163504.aspx


/////////////////////////////////////////////////////////////////////////////