using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;

namespace Microsoft.Samples.MSBuildG
{
    // Summary:
    // Interaction logic for DesignerWindow.xaml
    
    public partial class DesignerWindow : Window
    {
        private BuildProject m_Project;

        public DesignerWindow() 
        {
            m_Project = new BuildProject();
            m_Project.ProjectName = "New Project";

            Initialize();
        }

        public DesignerWindow(BuildProject project)
        {
            if (project == null)
            {
                throw new ArgumentNullException("project");
            }

            m_Project = project;
            m_Project.Update();

            Initialize();
        }

        private void Initialize()
        {
            InitializeComponent();

            this.InitializeData();
        }

        private void InitializeData()
        {
            this.MainGrid.DataContext = m_Project;
            this.MainGrid.InvalidateProperty(Grid.DataContextProperty);
        }

        private void NewProject(object sender, RoutedEventArgs e)
        {
            //Create a new project
            if (CheckProject())
            {

                m_Project = new BuildProject();
                this.InitializeData();
                m_Project.ProjectName = "New Project";
            }
        }

        private void SaveProject(object sender, RoutedEventArgs e)
        {
            //Save the project if needed
            if (m_Project.Dirty)
            {
                if (m_Project.Filename.Length == 0)
                {
                    System.Windows.Forms.SaveFileDialog fileDialog = new System.Windows.Forms.SaveFileDialog();
                    fileDialog.Filter = "MS Build Project Files|*.*proj";

                    System.Windows.Forms.DialogResult dr = fileDialog.ShowDialog();

                    if (dr == System.Windows.Forms.DialogResult.OK)
                    {
                        m_Project.Filename = fileDialog.FileName;
                    }
                }

                m_Project.Save();
            }
        }

        private void OpenProject(object sender, RoutedEventArgs e)
        {
            if (CheckProject())
            {
                System.Windows.Forms.OpenFileDialog fileDialog = new System.Windows.Forms.OpenFileDialog();
                fileDialog.Filter = "MS Build Project Files|*.*proj";

                System.Windows.Forms.DialogResult dr = fileDialog.ShowDialog();

                if (dr == System.Windows.Forms.DialogResult.OK)
                {
                    m_Project = new BuildProject();
                    this.InitializeData();
                    m_Project.LoadFromFile(fileDialog.FileName);
                }
            }
        }

        private void Exit(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void AddPropertyGroup(object sender, RoutedEventArgs e) 
        {
            AddPropertyGroupDialog dialog = new AddPropertyGroupDialog();
            bool? result=dialog.ShowDialog();

            if (result.HasValue)
            {
                if (result.Value)
                {
                    m_Project.AddPropertyGroup(dialog.Condition);
                }
            }
        }

        private void DeletePropertyGroup(object sender, RoutedEventArgs e)
        {
            GroupElement groupElement = PropertyGroups.SelectedItem as GroupElement;

            if (groupElement != null)
            {
                PropertyGroupElement selElm = groupElement.ElementBase as PropertyGroupElement;

                if (selElm != null)
                {
                    if (!selElm.IsImported)
                    {
                        DeleteQuestionDialog dialog = new DeleteQuestionDialog();
                        bool? dialogResult=dialog.ShowDialog();

                        if (dialogResult.Value)
                        {
                            m_Project.DeletePropertyGroup(groupElement);
                        }
                    }
                }
                else
                {
                    DeletePropertyGroupBtn.IsEnabled = false;
                }
            }
        }

        private void AddProperty(object sender, RoutedEventArgs e)
        {
            GroupElement groupElement = PropertyGroups.SelectedItem as GroupElement;

            if (groupElement != null)
            {
                PropertyGroupElement selElm = groupElement.ElementBase as PropertyGroupElement;

                if (selElm != null)
                {
                    if (!selElm.IsImported)
                    {
                        AddPropertyDialog dialog = new AddPropertyDialog();
                        bool? dialogResult = dialog.ShowDialog();

                        if (!dialogResult.HasValue)
                        {
                            return;
                        }

                        if (dialogResult.Value)
                        {
                            //Catch any errors and display them to the user
                            try
                            {
                                selElm.PropertyGroup.AddNewProperty(dialog.PropertyName, "");
                                groupElement.Refresh();
                            }
                            catch (System.Xml.XmlException ex)
                            {
                                ErrorDialog errorMessage = new ErrorDialog(ex.Message);
                                errorMessage.ShowDialog();
                            }
                            catch (ArgumentException ex)
                            {
                                ErrorDialog errorMessage = new ErrorDialog(ex.Message);
                                errorMessage.ShowDialog();
                            }
                        }
                    }
                }
                else
                {
                    AddPropertyBtn.IsEnabled = false;
                }
            }
        }

        private void PropertyGroupsChanged(object sender, RoutedEventArgs e)
        {
            GroupElement groupElement = PropertyGroups.SelectedItem as GroupElement;

            if (groupElement != null)
            {
                PropertyGroupElement selElm = groupElement.ElementBase as PropertyGroupElement;

                DeletePropertyGroupBtn.IsEnabled = (selElm != null);
                AddPropertyBtn.IsEnabled = (selElm != null);
            }
        }

        private void SearchPropertyList(object sender, RoutedEventArgs e)
        {
            TextBox tBox = (TextBox)sender;

            m_Project.PropertyQuery = tBox.Text;
        }


        private void AddItemGroup(object sender, RoutedEventArgs e)
        {
            AddItemGroupDialog dialog = new AddItemGroupDialog();
            bool? result = dialog.ShowDialog();

            if (result.HasValue)
            {
                if (result.Value)
                {
                    m_Project.AddItemGroup(dialog.Condition);
                }
            }
        }

        private void DeleteItemGroup(object sender, RoutedEventArgs e)
        {
            GroupElement groupElement = ItemGroups.SelectedItem as GroupElement;

            if (groupElement != null)
            {
                ItemGroupElement selElm = groupElement.ElementBase as ItemGroupElement;

                if (selElm != null)
                {
                    if (!selElm.IsImported)
                    {
                        DeleteQuestionDialog dialog = new DeleteQuestionDialog();
                        bool? dialogResult = dialog.ShowDialog();

                        if (!dialogResult.HasValue)
                        {
                            return;
                        }

                        if (dialogResult.Value)
                        {
                            m_Project.DeleteItemGroup(groupElement);
                        }
                    }
                }
                else
                {
                    DeleteItemGroupBtn.IsEnabled = false;
                }
            }
        }


        private void ItemGroupsChanged(object sender, RoutedEventArgs e)
        {
            bool isEnabled = false;

            GroupElement groupElement = ItemGroups.SelectedItem as GroupElement;

            if (groupElement != null)
            {
                ItemGroupElement selElm = groupElement.ElementBase as ItemGroupElement;
                isEnabled = (selElm != null);
            }

            DeleteItemGroupBtn.IsEnabled = isEnabled;
            AddItemBtn.IsEnabled = isEnabled;

        }
        private void AddItem(object sender, RoutedEventArgs e)
        {
            GroupElement groupElement = ItemGroups.SelectedItem as GroupElement;

            if (groupElement != null)
            {
                ItemGroupElement selElm = groupElement.ElementBase as ItemGroupElement;

                if (selElm != null)
                {
                    if (!selElm.IsImported)
                    {
                        AddItemDialog dialog = new AddItemDialog(selElm.PredominateType);
                        bool? dialogResult = dialog.ShowDialog();

                        if (!dialogResult.HasValue)
                        {
                            return;
                        }

                        if (dialogResult.Value)
                        {
                            //Catch any errors and display them to the user
                            try
                            {
                                selElm.ItemGroup.AddNewItem(dialog.ItemName, "(Nothing)");
                                groupElement.Refresh();
                            }
                            catch (System.Xml.XmlException ex)
                            {
                                ErrorDialog errorMessage = new ErrorDialog(ex.Message);
                                errorMessage.ShowDialog();
                            }
                            catch (ArgumentException ex)
                            {
                                ErrorDialog errorMessage = new ErrorDialog(ex.Message);
                                errorMessage.ShowDialog();
                            }
                        }
                    }
                }
                else
                {
                    AddItemBtn.IsEnabled = false;
                }
            }
        }

        private void SearchItemList(object sender, RoutedEventArgs e)
        {
            TextBox tBox = (TextBox)sender;

            m_Project.ItemGroupQuery = tBox.Text;
        }


        private void AddTarget(object sender, RoutedEventArgs e)
        {
            AddTargetDialog dialog = new AddTargetDialog(m_Project);
            bool? result = dialog.ShowDialog();

            if (result.HasValue)
            {
                if (result.Value)
                {
                    m_Project.AddTarget(dialog.TargetName, dialog.Condition, dialog.DependsOn);
                }
            }
        }

        private void DeleteTarget(object sender, RoutedEventArgs e)
        {
            GroupElement groupElement = Targets.SelectedItem as GroupElement;

            if (groupElement != null)
            {
                TargetElement selElm = groupElement.ElementBase as TargetElement;

                if (selElm != null)
                {
                    if (!selElm.IsImported)
                    {
                        DeleteQuestionDialog dialog = new DeleteQuestionDialog();
                        bool? dialogResult = dialog.ShowDialog();

                        if (!dialogResult.HasValue)
                        {
                            return;
                        }

                        if (dialogResult.Value)
                        {
                            m_Project.DeleteTarget(groupElement);
                        }
                    }
                }
                else
                {
                    DeleteTargetBtn.IsEnabled = false;
                }
            }
        }

        private void AddTask(object sender, RoutedEventArgs e)
        {
            GroupElement groupElement = Targets.SelectedItem as GroupElement;

            if (groupElement != null)
            {
                TargetElement selElm = groupElement.ElementBase as TargetElement;

                if (selElm != null)
                {
                    if (!selElm.IsImported)
                    {
                        AddTaskDialog dialog = new AddTaskDialog();
                        bool? dialogResult = dialog.ShowDialog();

                        if (!dialogResult.HasValue)
                        {
                            return;
                        }

                        if (dialogResult.Value)
                        {
                            try
                            {
                                Microsoft.Build.BuildEngine.BuildTask elm = selElm.TargetObject.AddNewTask(dialog.TaskName);

                                elm.Condition = dialog.Condition;

                                if (dialog.TaskParam.Length > 0)
                                {
                                    string[] pList = dialog.TaskParam.Split(',');

                                    foreach (string param in pList)
                                    {
                                        string[] pPart = param.Split('=');

                                        if (pPart.Length == 2)
                                        {
                                            elm.SetParameterValue(pPart[0], pPart[1]);
                                        }
                                    }
                                }
                            }
                            catch (System.Xml.XmlException ex)
                            {
                                ErrorDialog errorMessage = new ErrorDialog(ex.Message);
                                errorMessage.ShowDialog();
                            }
                            catch (ArgumentException ex)
                            {
                                ErrorDialog errorMessage = new ErrorDialog(ex.Message);
                                errorMessage.ShowDialog();
                            }

                            groupElement.Refresh();
                        }
                    }
                }
                else
                {
                    AddTaskBtn.IsEnabled = false;
                }
            }
        }

        private void TargetsChanged(object sender, RoutedEventArgs e)
        {
            bool isEnabled = false;

            GroupElement groupElement = Targets.SelectedItem as GroupElement;

            if (groupElement != null)
            {
                TargetElement selElm = groupElement.ElementBase as TargetElement;
                isEnabled = (selElm != null);
            }

            DeleteTargetBtn.IsEnabled = isEnabled;
            AddTaskBtn.IsEnabled = isEnabled;


        }

        private void AddProject(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.OpenFileDialog fileDialog = new System.Windows.Forms.OpenFileDialog();
            fileDialog.Filter = "MS Build Project Files|*.*proj;*.targets";

            System.Windows.Forms.DialogResult dr = fileDialog.ShowDialog();

            if (dr == System.Windows.Forms.DialogResult.OK)
            {
                if (fileDialog.FileName != m_Project.Filename)
                {
                    m_Project.AddProjectImport(fileDialog.FileName);
                }
                else
                {
                    ErrorDialog dialog = new ErrorDialog("You cannot add a project to itself.");
                    dialog.ShowDialog();
                }
            }
        }

        private void SearchTargetList(object sender, RoutedEventArgs e)
        {
            TextBox tBox = (TextBox)sender;

            m_Project.TargetQuery = tBox.Text;
        }


        private void RunTarget(object sender, RoutedEventArgs e) 
        {
            m_Project.Run(m_Project.DefaultTarget);
        }

        private void WindowLoaded(object sender, EventArgs e)
        {
        }

        private bool CheckProject()
        {
            if (m_Project.Dirty)
            {
                SaveQuestionDialog saveDialog=new SaveQuestionDialog();

                bool? saveProject = saveDialog.ShowDialog();

                if (!saveProject.HasValue)
                {
                    return (false);
                }

                if (saveProject.Value == true)
                {
                    this.SaveProject(this, new RoutedEventArgs());
                }
            }

            return (true);
        }

        private void WindowClosing(object sender, System.ComponentModel.CancelEventArgs e)
        {

            bool rtn = CheckProject();

            if (rtn)
            {
                PickerWindow pickerDialog = new PickerWindow();
                pickerDialog.Show();
            }
            else
            {
                e.Cancel = true;
            }
        }

    }
}