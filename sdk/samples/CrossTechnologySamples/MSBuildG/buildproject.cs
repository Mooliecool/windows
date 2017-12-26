// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using Microsoft.Build;
using Microsoft.Build.BuildEngine;
using Microsoft.Build.Framework;
using System.Xml;
using System.Text;
using System.Threading;

namespace Microsoft.Samples.MSBuildG
{
    // Summary:
    // Wrapper class for MS Build Projects that handles all UI and Wizard Creation for this demo
    
    public class BuildProject : INotifyPropertyChanged
    {
        #region Private members and Constructor
        //Enumeration

        public enum BuildProjectType { None, CSharp, VisualBasic, JSharp };
        public enum BuildProjectOutputType { None, Winexe, Library, Exe };

        //Private variables

        Microsoft.Build.BuildEngine.Project m_Project;

        private static BuildLogger Logger = new BuildLogger();

        private string m_Filename="";
        private string m_ProjectName;
        private string m_ProjectNotes;
        private string m_SavePath;

        private BuildProjectType m_ProjectType;

        private List<IBuildProjectFilter> m_Filters;

        private IBuildProjectFilter m_ProjectFilter;

        private SortedList<string, BuildAction> m_Targets ;

        private bool m_Dirty = false;

        private List<ExternalProjectItem> m_ExternalProjects;

        //Item lists 

        private List<GroupElement> m_PropertyGroups;
        private List<GroupElement> m_ItemsGroups;
        private List<GroupElement> m_TargetList;

        private bool m_ShowImportedProperties = false;
        private string m_PropertyQuery;

        private bool m_ShowImportedItemGroups = false;
        private string m_ItemGroupQuery;

        private bool m_ShowImportedTargets = false;
        private string m_TargetQuery;

        //Run properties

        private bool m_IsRunning = false;
        private bool m_ShowErrors = true;
        private bool m_ShowWarnings = true;
        private bool m_ShowMessages = false;

        private List<BuildEventItem> m_EventItems;

        //Private properties used in the wizard for user *P*reference
        private string p_RootNamespace="";
        private string p_AssemblyName = "";
        private string p_UICulture = "";
        private BuildProjectOutputType p_OutputType;
        private BuildAction p_DefaultTarget;


        //Constructor

        public BuildProject()
        {
            //set the startup bin path
            Microsoft.Build.BuildEngine.Engine.GlobalEngine.BinPath = SettingsSystem.Storage.BinPath;

            if (!Logger.IsRegistered)
            {
                Logger.Register();
            }

            //Tell the logger to return information to an event we create
            Logger.LogChanged +=new EventHandler<BuildLogger.LoggerEventArgs>(Logger_LogChanged);

            m_Project = new Microsoft.Build.BuildEngine.Project();
            m_Targets = new SortedList<string, BuildAction>();
            m_ExternalProjects=new List<ExternalProjectItem>();
            m_PropertyGroups = new List<GroupElement>();
            m_ItemsGroups = new List<GroupElement>();
            m_TargetList = new List<GroupElement>();
        }

        #endregion

        #region Private Functions
        // Summary:
        // Helper function to construct the file as we wish
        
        private void ConstructFile()
        {
            m_Project.MarkProjectAsDirty();
        }

        // Summary:
        // Helper function to construct the project class (i.e. this) as we wish
        
        private void ConstructProject()
        {
            m_ProjectName = (string)m_Project.EvaluatedProperties["ProjectName"];

             if (m_ProjectName.Length == 0)
             {
                 m_ProjectName = (string)m_Project.EvaluatedProperties["AssemblyName"];
             }
             if (m_ProjectName.Length == 0)
             {
                 m_ProjectName = Path.GetFileNameWithoutExtension(m_Filename);
             }

             m_ProjectNotes = (string)m_Project.EvaluatedProperties["ProjectNotes"];

            //construct the targets list and targets dropdown

            m_Targets = new SortedList<string, BuildAction>();
            m_TargetList = new List<GroupElement>();

            foreach (Target currentTarget in m_Project.Targets)
            {
                m_Targets.Add(currentTarget.Name, new BuildAction(currentTarget));
                m_TargetList.Add(new GroupElement(currentTarget));
            }

            string[] defaultTargets = m_Project.DefaultTargets.Split(';');

            if (m_Targets.ContainsKey(defaultTargets[0]))
            {
               DefaultTarget = m_Targets[defaultTargets[0]];
            }

            //construct the property groups list

            m_PropertyGroups = new List<GroupElement>();

            foreach (BuildPropertyGroup propertyGroup in m_Project.PropertyGroups)
            {
                m_PropertyGroups.Add(new GroupElement(propertyGroup));
            }

            //construct the itemgroups list

            m_ItemsGroups = new List<GroupElement>();

            foreach (BuildItemGroup itemGroup in m_Project.ItemGroups)
            {
               m_ItemsGroups.Add(new GroupElement(itemGroup));
            }

            //construct the external projects list
            foreach (Import importedProject in m_Project.Imports)
            {
                m_ExternalProjects.Add(new ExternalProjectItem(importedProject.ProjectPath));
            }

            OnPropertyChanged("PropertyGroups");
            OnPropertyChanged("ItemGroups");
            OnPropertyChanged("TargetsList");
            OnPropertyChanged("ProjectName");
            OnPropertyChanged("ProjectNotes");
            OnPropertyChanged("Targets");
            OnPropertyChanged("ExternalProjects");
        }

        void Logger_LogChanged(object sender, BuildLogger.LoggerEventArgs e)
        {
            OnPropertyChanged("LogOutput");
        }

        // Summary:
        // Function that determines the visibility of every item in the run list
        
        private void processRunList()
        {
            if (m_EventItems != null)
            {
                foreach (BuildEventItem item in m_EventItems)
                {
                   item.DetermineVisibility(m_ShowErrors, m_ShowWarnings, m_ShowMessages);
                }
            }
        }

        // Summary:
        // Function that creates the local copy of the Build Events list following a Run
        
        private void createRunList()
        {
            foreach (BuildEventArgs item in Logger.Items)
            {
                m_EventItems.Add(new BuildEventItem(m_EventItems.Count,item));
            }

            OnPropertyChanged("EventItems");

            processRunList();
        }

        #endregion

        #region Public Functions

        // Summary:
        // Deletes a target from the project

        public void DeleteTarget(GroupElement groupElement)
        {
            if (groupElement == null)
            {
                throw new ArgumentNullException("groupElement");
            }

            TargetElement itemGroupElem = (TargetElement)groupElement.ElementBase;

            m_Project.Targets.RemoveTarget(itemGroupElem.TargetObject);

            m_TargetList.Remove(groupElement);
            m_Targets.Remove(itemGroupElem.TargetObject.Name);

            OnPropertyChanged("TargetsList");
        }

        // Summary:
        // Deletes an item group from the project

        public void DeleteItemGroup(GroupElement groupElement)
        {
            if (groupElement == null)
            {
                throw new ArgumentNullException("groupElement");
            }

            ItemGroupElement itemGroupElem = (ItemGroupElement)groupElement.ElementBase;

            m_Project.RemoveItemGroup(itemGroupElem.ItemGroup);

            m_ItemsGroups.Remove(groupElement);

            OnPropertyChanged("ItemGroups");
        }

        // Summary:
        // Deletes a property group from the project

        public void DeletePropertyGroup(GroupElement groupElement)
        {
            if (groupElement == null)
            {
                throw new ArgumentNullException("groupElement");
            }

            PropertyGroupElement propGroupElem = (PropertyGroupElement) groupElement.ElementBase;

            m_Project.RemovePropertyGroup(propGroupElem.PropertyGroup);

            m_PropertyGroups.Remove(groupElement);

            OnPropertyChanged("PropertyGroups");
        }

        // Summary:
        // Adds a property group to the project
        
        public void AddPropertyGroup(string condition)
        {
            if (condition == null)
            {
                throw new ArgumentNullException("conditon");
            }

            BuildPropertyGroup pGroup=null;
            try
            {
                pGroup = m_Project.AddNewPropertyGroup(true);

                pGroup.Condition = condition;

                m_PropertyGroups.Add(new GroupElement(pGroup));

            }
            catch (XmlException ex)
            {
                ErrorDialog dialog = new ErrorDialog(ex.Message);
                dialog.ShowDialog();
            }
            catch (ArgumentException ex)
            {
                ErrorDialog dialog = new ErrorDialog(ex.Message);
                dialog.ShowDialog();
            }


            OnPropertyChanged("PropertyGroups");
        }

        // Summary:
        // Adds an item group to the project
        
        public void AddItemGroup(string condition)
        {
            if (condition == null)
            {
                throw new ArgumentNullException("conditon");
            }

            BuildItemGroup iGroup=null;

            try
            {
                iGroup = m_Project.AddNewItemGroup();

                iGroup.Condition = condition;

                m_ItemsGroups.Add(new GroupElement(iGroup));
            }
            catch (XmlException ex)
            {
                ErrorDialog dialog = new ErrorDialog(ex.Message);
                dialog.ShowDialog();
            }
            catch (ArgumentException ex)
            {
                ErrorDialog dialog = new ErrorDialog(ex.Message);
                dialog.ShowDialog();
            }


            OnPropertyChanged("ItemGroups");
        }

        // Summary:
        // Adds an imported project to the project
        
        public void AddProjectImport(string filePath)
        {
            if (filePath == null)
            {
                throw new ArgumentNullException("filePath");
            }
            try
            {
                m_Project.AddNewImport(filePath, null);

                m_ExternalProjects.Add(new ExternalProjectItem(filePath));
            }
            catch (XmlException ex)
            {
                ErrorDialog dialog = new ErrorDialog(ex.Message);
                dialog.ShowDialog();
            }
            catch (ArgumentException ex)
            {
                ErrorDialog dialog = new ErrorDialog(ex.Message);
                dialog.ShowDialog();
            }


            OnPropertyChanged("ExternalProjects");
        }

        // Summary:
        // Adds a target to the project
        
        public void AddTarget(string name, string condition, string dependsOn)
        {
            if (condition == null)
            {
                throw new ArgumentNullException("conditon");
            }
            if (name == null)
            {
                throw new ArgumentNullException("name");
            }
            if (dependsOn == null)
            {
                throw new ArgumentNullException("dependsOn");
            }

            Target newTarget=null;

            try
            {
                newTarget = m_Project.Targets.AddNewTarget(name);

                newTarget.Condition = condition;

                newTarget.DependsOnTargets = dependsOn;

                m_TargetList.Add(new GroupElement(newTarget));
                m_Targets.Add(name, new BuildAction(newTarget));
            }
            catch (XmlException ex)
            {
                ErrorDialog dialog = new ErrorDialog(ex.Message);
                dialog.ShowDialog();
            }
            catch (ArgumentException ex)
            {
                ErrorDialog dialog = new ErrorDialog(ex.Message);
                dialog.ShowDialog();
            }
            catch (InvalidProjectFileException ex)
            {
                ErrorDialog dialog = new ErrorDialog(ex.Message);
                dialog.ShowDialog();
            }

            OnPropertyChanged("Targets");
            OnPropertyChanged("TargetsList");
        }

        // Summary:
        // Creates the list of "Actions" (Item grtoups and items) that will be taken by the Wizard when it creates the project
        
        public TreeViewItem[] CreateActionList()
         {
            SortedList<string, TreeViewItem> actionList = new SortedList<string, TreeViewItem>();
            List<TreeViewItem> tList = new List<TreeViewItem>();

            foreach (BuildItemGroup itemgroup in m_Project.ItemGroups)
            {
                TreeViewItem cAction=null;

                foreach (BuildItem cItem in itemgroup)
                {

                    if (!actionList.ContainsKey(cItem.Name))
                    {
                        TreeViewItem tvi = new TreeViewItem();

                        actionList.Add(cItem.Name, tvi);
                        tList.Add(tvi);
                    }

                    cAction = actionList[cItem.Name];

                    TextBlock tBlock = new TextBlock();
                    tBlock.Text = cItem.Name;
                    tBlock.Foreground = Brushes.White;

                    cAction.Header = tBlock;

                    TreeViewItem baItem = new TreeViewItem();

                    tBlock = new TextBlock();
                    tBlock.Text = cItem.Include;
                    tBlock.Foreground = Brushes.White;

                    baItem.Header = tBlock;

                    cAction.Items.Add(baItem);
                }
            }

            return (tList.ToArray());
        }

        // Summary:
        // Updates the local information stored in this class to reflect the MS Build project
        
        public void Update()
        {
            ConstructProject();
        }

        public void Save()
        {
            if (m_Filename.Length > 0)
            {
                ConstructFile();

                m_Project.Save(m_Filename);

                m_Dirty = false;
            }
        }

        public void LoadFromFile(string fileName)
        {
            if (fileName == null)
            {
                throw new ArgumentNullException("fileName");
            }

            if (File.Exists(fileName))
            {
                m_Project.Load(fileName);

                m_Filename = fileName;

                ConstructProject();
            }
        }

        // Summary:
        // Saves a new MS Build project with the required name, based on the language of the project
        
        public void Create()
        {
            m_Filename = m_SavePath + "\\" + p_AssemblyName + "." + m_ProjectFilter.ProjectFilePrefix + "proj";

            //Header info for compiling

            m_ProjectFilter.AddHeaderInformation(this);

            //Save the thing

            Save();
        }

        // Summary:
        // Creates a new MS Build project, based on the language of the source files sent in
        
        public void CreateTemplate(string[] fileNames)
        {
            if (fileNames == null)
            {
                throw new ArgumentNullException("fileNames");
            }

            m_Project = new Project();

            m_Dirty = true;

            m_SavePath = Path.GetDirectoryName(fileNames[0]);

            //add the filters we intend to use
            m_Filters = new List<IBuildProjectFilter>();
            m_Filters.Add(new CSharpFilter());
            m_Filters.Add(new VisualBasicFilter());

            //get the type of project (C#,VB,J#,etc) based on filters
            foreach (IBuildProjectFilter filter in m_Filters)
            {
                foreach (string fileName in fileNames)
                {
                    string fileExt = Path.GetExtension(fileName);

                    if (fileExt == filter.FileExtension)
                    {
                        m_ProjectFilter = filter;
                        m_ProjectType = filter.FilterType;
                        break;
                    }
                }

                if (m_ProjectFilter != null)
                {
                    break;
                }
            }

            //process the individual source files
            m_ProjectFilter.ProcessFiles(m_Project.AddNewItemGroup(), fileNames);

            //add the proper targets for this project type
            m_ProjectFilter.SetTargetInformation(m_Project);

            //add the references needed for compilation
            m_ProjectFilter.SetReferences(m_Project.AddNewItemGroup(), fileNames);

            //process external items (such as resources files)
            m_ProjectFilter.ProcessExternalItems(m_Project.AddNewItemGroup(), fileNames);

            //try to infer default types for the next window
            p_UICulture = "en-US";
            p_OutputType = BuildProjectOutputType.Winexe;

            //Determine the namespace and AssemblyName of the project
            m_ProjectFilter.SetNamespaceInformation(this, fileNames);
        }

        // Summary:
        // Runs MS Build on the specified BuildAction
        
        public void Run(BuildAction action)
        {
            if (action != null)
            {
                m_IsRunning = true;
                OnPropertyChanged("IsNotRunning");

                //Note: Although it would seem that using threads at this point to run MS Build in the background
                //is a good idea, unfortunately, due to a lack of support for multi-threading in both MS Build and the presentation system,
                //the run must be done under the same thread as the main application
                RunProject(action);
            }
        }


        private void RunProject(BuildAction action)
        {
            if (action == null)
            {
                throw new ArgumentNullException("action");
            }

            //Reset the logger
            Logger.Reset();

            //Create a new list of events
            m_EventItems = new List<BuildEventItem>();

            //Create a new dictionary to hold the outputs of MS Build
            Dictionary<string, ITaskItem[]> outputs = new Dictionary<string, ITaskItem[]>();

            m_Project.Build( new string[] { action.TargetName }, outputs);

            m_IsRunning = false;
            OnPropertyChanged("IsNotRunning");

            createRunList();
        }

#endregion

        #region Properties

        public string Filename
        {
            get { return m_Filename; }
            set { m_Filename = value; }
        }

        public string ProjectName
        {
            get { return m_ProjectName; }
            set
            {
                if (value != m_ProjectName)
                {
                    m_ProjectName = value;
                    m_Project.SetProperty("ProjectName", m_ProjectName, null);
                    OnPropertyChanged("ProjectName");
                }
            }
        }

        public string ProjectNotes
        {
            get { return m_ProjectNotes; }
            set
            {
                if (value != m_ProjectNotes)
                {
                    m_ProjectNotes = value;
                    m_Project.SetProperty("ProjectNotes", m_ProjectNotes, null);
                    OnPropertyChanged("ProjectNotes");
                }
            }
        }

        public string RootNamespace
        {
            get { return p_RootNamespace; }
            set
            {
                if (value != p_RootNamespace)
                {
                    p_RootNamespace = value;
                    OnPropertyChanged("RootNamespace");
                }
            }
        }

        public string AssemblyName
        {
            get { return p_AssemblyName; }
            set
            {
                if (value != p_AssemblyName)
                {
                    p_AssemblyName = value;
                    OnPropertyChanged("AssemblyName");
                }
            }
        }

        public string UICulture
        {
            get { return p_UICulture; }
            set
            {
                if (value != p_UICulture)
                {
                    p_UICulture = value;
                    OnPropertyChanged("UICulture");
                }
            }
        }

        public BuildProjectOutputType OutputType
        {
            get { return (p_OutputType); }
            set {

                //BuildProjectOutputType newValue = (BuildProjectOutputType)Enum.Parse(typeof(BuildProjectOutputType), value, true); ;

                p_OutputType = value;

                OnPropertyChanged("OutputType");
            }
        }

        public BuildProjectOutputType[] OutputTypes
        {
            get
            {
                return (BuildProjectOutputType[])Enum.GetValues(typeof(BuildProjectOutputType)); 
            }
        }

        public BuildProjectType ProjectType
        {
            get { return m_ProjectType; }
        }

        public BuildAction[] Targets
        {
            get
            {
                List<BuildAction> targetList=new List<BuildAction>();

                foreach (BuildAction action in m_Targets.Values)
                {
                    targetList.Add(action);
                }

                return (targetList.ToArray());
            }
        }

        public BuildAction DefaultTarget
        {
            get
            {
                return (p_DefaultTarget);
            }
            set
            {
                if (p_DefaultTarget != value)
                {
                    p_DefaultTarget = value;
                    OnPropertyChanged("DefaultTarget");
                }
            }
        }

        public GroupElement[] PropertyGroups
        {
            get
            {
                return (m_PropertyGroups.ToArray());
            }
        }

        public GroupElement[] ItemGroups
        {
            get
            {
                return (m_ItemsGroups.ToArray());
            }
        }

        public GroupElement[] TargetsList
        {
            get
            {
                return (m_TargetList.ToArray());
            }
        }

        public bool Dirty
        {
            get { return (m_Dirty || m_Project.IsDirty); }
        }

        public bool ShowImportedProperties
        {
            get { return m_ShowImportedProperties; }
            set { 
                m_ShowImportedProperties = value;

                foreach (GroupElement pgi in m_PropertyGroups)
                {
                    pgi.ShowImported = value;
                }
            }
        }

        public string PropertyQuery
        {
            get { return m_PropertyQuery; }
            set { 
                
                m_PropertyQuery = value;

                foreach (GroupElement pgi in m_PropertyGroups)
                {
                    pgi.Query=value;
                }

            }
        }

        public bool ShowImportedItemGroups
        {
            get { return m_ShowImportedItemGroups; }
            set
            {
                m_ShowImportedItemGroups = value;

                foreach (GroupElement igi in m_ItemsGroups)
                {
                    igi.ShowImported = value;
                }
            }
        }

        public string ItemGroupQuery
        {
            get { return m_ItemGroupQuery; }
            set
            {

                m_ItemGroupQuery = value;

                foreach (GroupElement igi in m_ItemsGroups)
                {
                    igi.Query = value;
                }

            }
        }

        public bool ShowImportedTargets
        {
            get { return m_ShowImportedTargets; }
            set
            {
                m_ShowImportedTargets = value;

                foreach (GroupElement ti in m_TargetList)
                {
                    ti.ShowImported = value;
                }
            }
        }

        public string TargetQuery
        {
            get { return m_TargetQuery; }
            set
            {

                m_TargetQuery = value;

                foreach (GroupElement ti in m_TargetList)
                {
                    ti.Query = value;
                }

            }
        }

        public string LogOutput
        {
            get
            {
                return (Logger.LogOutput);
            }
        }

        public LoggerVerbosity[] Verbosities
        {
            get
            {
                return (LoggerVerbosity[])Enum.GetValues(typeof(LoggerVerbosity));
            }
        }

        public LoggerVerbosity Verbosity
        {
            get
            {
                return (Logger.Verbosity);
            }
            set
            {
                Logger.Verbosity = value;
                OnPropertyChanged("Verbosity");
            }
        }

        public bool IsNotRunning
        {
            get
            {
                return (!m_IsRunning);
            }
        }

        public bool ShowErrors
        {
            get
            {
                return (m_ShowErrors);
            }
            set
            {
                m_ShowErrors = value;
                processRunList();
                OnPropertyChanged("ShowErrors");
            }
        }

        public bool ShowWarnings
        {
            get
            {
                return (m_ShowWarnings);
            }
            set
            {
                m_ShowWarnings = value;
                processRunList();
                OnPropertyChanged("ShowWarnings");
            }
        }

        public bool ShowMessages
        {
            get
            {
                return (m_ShowMessages);
            }
            set
            {
                m_ShowMessages = value;
                processRunList();
                OnPropertyChanged("ShowMessages");
            }
        }

        public BuildEventItem[] EventItems
        {
            get
            {
                return (m_EventItems.ToArray());
            }
        }

        public ExternalProjectItem[] ExternalProjects
        {
            get
            {
                return (m_ExternalProjects.ToArray());
            }
        }

        public string BinPath
        {
            get
            {
                return (Microsoft.Build.BuildEngine.Engine.GlobalEngine.BinPath);
            }
            set
            {
                Microsoft.Build.BuildEngine.Engine.GlobalEngine.BinPath = value;
                OnPropertyChanged("BinPath");
            }
        }

        //Data driven property changed handlers

        public event PropertyChangedEventHandler PropertyChanged;

        // Summary:
        // Data driven handler that tells the presentation system that a property's value has changed
        
        private void OnPropertyChanged(String info)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(info));
            }

            m_Dirty = true;
        }

        #endregion

        #region Filter Interfaces

        // Summary:
        // Interface for an object that will create an MS Build project for a given language type 
        
        public interface IBuildProjectFilter
        {
            // Summary:
            // Enumeration indicating the language this filter is tied to.
            
            BuildProjectType FilterType
            {
                get;
            }

            // Summary:
            // The file extension that this filter looks for, with the preceding period. Example: ".cs"
            
            string FileExtension
            {
                get;
            }

            // Summary:
            // The prefix that an MS Build project of this type will have. Example: "cs"
            
            string ProjectFilePrefix
            {
                get;
            }

            // Summary:
            // Type of the application being built (Windows Forms, etc. This is filter internal)
            
            string Type
            {
                get;
                set;
            }

            // Summary:
            // Method which sets the references for the given project
            
            void SetReferences(BuildItemGroup referenceGroup, string[] fileNames);

            // Summary:
            // Method which handles all custom types for the project (i.e. Settings, AssemblyInfo and Resources.resx)
            
            void ProcessExternalItems(BuildItemGroup externalGroup, string[] fileNames);

            // Summary:
            // Method which processes the source files and adds their information to the project
            
            void ProcessFiles(BuildItemGroup fileGroup, string[] fileNames);

            // Summary:
            // Method which sets the targets needed for this project
            
            void SetTargetInformation(Project currentProject);

            // Summary:
            // Internal method which keeps track of a list of targets and makes sure new ones added are not duplicates
            
            void AddTarget(string targetName);

            // Summary:
            // Internal method which keeps track of a list of references and makes sure new ones added are not duplicates
            
            void AddReference(string reference);

            // Summary:
            // Method which adds the necessary header information needed by the Microsoft Framework targets
            
            void AddHeaderInformation(BuildProject currentObject);

            // Summary:
            // Method which retrieves the namespace and assembly name 
            
            void SetNamespaceInformation(BuildProject currentObject, string[] fileNames);

        }


        // Summary:
        // File filter for supporting each file type within a programming language
        
        public interface IBuildFileFilter
        {
            // Summary:
            // The file extension for the file being processed, with the preceding period. Example: ".cs"
            
            string FileExtension
            {
                get;
            }

            // Summary:
            // Function which processes the file and adds all necessary information about it to the project
            
            void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup fileGroup, string filePath);
        }

        // Summary:
        // Generated filter for supporting files auto-generated by Visual Studio (such as Settings, AssemblyInfo, Resources.resx, etc)
        
        public interface IBuildGeneratedFilter
        {
            // Summary:
            // The file name for the file being processed.
            
            string FileIdentifier
            {
                get;
            }

            // Summary:
            // Function which processes the file and adds all necessary information about it to the project
            
            void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup externalGroup, string[] fileNames);
        }

        #endregion

        public abstract class CLRFilter : IBuildProjectFilter
        {

            #region Protected Members and Constructor

            protected List<IBuildFileFilter> m_Filters;
            protected List<IBuildGeneratedFilter> m_GeneratedFilters;
            protected List<string> m_CustomTargets;
            protected List<string> m_CustomReferences;

            protected string m_Type = "Console";

            public CLRFilter()
            {
                //Create the filters list
                m_Filters=new List<IBuildFileFilter>();

                //Create the generated filters list
                m_GeneratedFilters = new List<IBuildGeneratedFilter>();

                //Create the targets and references list
                m_CustomTargets = new List<string>();
                m_CustomReferences = new List<string>();
            }


            public string Type
            {
                get
                {
                    return (m_Type);
                }
                set
                {
                    m_Type = value;
                }
            }

            #endregion

            #region Static Properties

            public abstract string FileExtension
            {
                get;
            }

            public abstract string ProjectFilePrefix
            {
                get;
            }

            public abstract BuildProjectType FilterType
            {
                get;
            }

            #endregion

            #region Processing Methods

            public virtual void SetNamespaceInformation(BuildProject currentObject, string[] fileNames)
            {               
                switch (m_Type)
                {
                    case "WindowsForms":
                        currentObject.OutputType = BuildProject.BuildProjectOutputType.Winexe;
                        break;
                    case "Console":
                        currentObject.OutputType = BuildProject.BuildProjectOutputType.Exe;
                        break;
                    case "Windows":
                        currentObject.OutputType = BuildProject.BuildProjectOutputType.Library;
                        break;
                }
            }

            public virtual void ProcessFiles(BuildItemGroup fileGroup, string[] fileNames)
            {
                m_Type = "Console";

                foreach (string filePath in fileNames)
                {
                    string currentFileExtension = Path.GetExtension(filePath);
                    string fullName = Path.GetFileNameWithoutExtension(filePath);
                    string fileName=Path.GetFileName(filePath);

                    //check for a secondary extension

                    if (Path.HasExtension(fullName))
                    {
                        currentFileExtension = Path.GetExtension(fullName) + currentFileExtension;
                    }

                    //okay, lets process the file by extension type

                    bool fileProcessed = false;

                    foreach (IBuildFileFilter filter in m_Filters)
                    {
                        if (filter.FileExtension == currentFileExtension)
                        {
                            filter.ProcessFile(this,fileGroup, filePath);
                            fileProcessed = true;
                            break;
                        }
                    }

                    if ((!fileProcessed) && (Path.GetExtension(fileName)!=".resx"))
                    {
                        //we have a resource file, add it as such

                        BuildItem resourceItem = fileGroup.AddNewItem("Resource", fileName);
                        resourceItem.SetMetadata("CopyToOutputDirectory", "PreserveNewest");
                    }
                }
            }

            public virtual void SetReferences(BuildItemGroup referenceGroup, string[] fileNames)
            {
                //Add the default references list

                AddReference("System");
                AddReference("System.Data");
                AddReference("System.XML");

                foreach(string reference in m_CustomReferences) {
                    referenceGroup.AddNewItem("Reference",reference);
                }
            }

            public abstract void ProcessExternalItems(BuildItemGroup externalGroup, string[] fileNames);

            public void ProcessExternalItems(BuildItemGroup externalGroup, string[] fileNames, string folderName)
            {
                //look for the folder folder

                string filePath = Path.GetDirectoryName(fileNames[0]);

                string propPath = filePath + "\\" + folderName;

                if (Directory.Exists(propPath))
                {
                    //special rules for the properties folder (yay Visual Studio!)

                    string[] directoryContents = Directory.GetFiles(propPath);

                    foreach (string currentFilePath in directoryContents)
                    {
                        string currentFileName = Path.GetFileName(currentFilePath);

                        foreach (IBuildGeneratedFilter genFilter in m_GeneratedFilters)
                        {
                            if (genFilter.FileIdentifier == currentFileName)
                            {
                                genFilter.ProcessFile(this, externalGroup, directoryContents);
                                break;
                            }
                        }
                    }

                    externalGroup.AddNewItem("AppDesigner", "Properties");
                }

            }

            public abstract void AddHeaderInformation(BuildProject currentObject);

            public virtual void AddTarget(string targetName)
            {
                if (!m_CustomTargets.Contains(targetName))
                {
                    m_CustomTargets.Add(targetName);
                }
            }

            public virtual void AddReference(string reference)
            {
                if (!m_CustomReferences.Contains(reference))
                {
                    m_CustomReferences.Add(reference);
                }
            }

            public virtual void SetTargetInformation(Project currentProject)
            {
                foreach (string import in m_CustomTargets)
                {
                    currentProject.AddNewImport(import, null);
                }
            }

            #endregion

        }

        public class CSharpFilter : CLRFilter
        {
            #region Constructor
            public CSharpFilter() : base()
            {
                //Create the filters list
                base.m_Filters.Add(new CSharpCSFilter());
                base.m_Filters.Add(new CSharpXAMLFilter());
                base.m_Filters.Add(new CSharpXAMLCSFilter());
                base.m_Filters.Add(new CSharpDesignerFilter());

                //Create the generated filters list
                base.m_GeneratedFilters.Add(new CSharpResourcesFilter());
                base.m_GeneratedFilters.Add(new CSharpSettingsFilter());
                base.m_GeneratedFilters.Add(new CSharpAssemblyInfoFilter());
            }
            #endregion

            #region Processing Methods
            public override string FileExtension
            {
                get
                {
                    return (".cs");
                }
            }

            public override string ProjectFilePrefix
            {
                get
                {
                    return ("cs");
                }
            }

            public override BuildProjectType FilterType
            {
                get
                {
                    return (BuildProjectType.CSharp);
                }
            }

            public override void SetNamespaceInformation(BuildProject currentObject, string[] fileNames)
            {
                SortedList<string, int> Namespaces = new SortedList<string, int>();

                foreach (string filePath in fileNames)
                {
                    string currentFileName = Path.GetFileName(filePath);

                    string fileExtension = Path.GetExtension(currentFileName);

                    if (fileExtension == this.FileExtension)
                    {
                        TextReader fileReader = new StreamReader(filePath);

                        string fileContents = fileReader.ReadToEnd();

                        Regex namespaceRegex = new Regex(@".*namespace\s([A-za-z0-9\.]+)\s.*");

                        if (namespaceRegex.IsMatch(fileContents))
                        {
                            string namespaceValue = namespaceRegex.Match(fileContents).Groups[0].Value.Split(' ')[1].Trim();

                            if (!Namespaces.Keys.Contains(namespaceValue))
                            {
                                Namespaces.Add(namespaceValue, 0);
                            }

                            Namespaces[namespaceValue]++;
                        }

                        fileReader.Close();
                    }
                }

                int maxCount = 0;

                foreach (string nSpace in Namespaces.Keys)
                {
                    if (Namespaces[nSpace] > maxCount)
                    {
                        maxCount = Namespaces[nSpace];

                        currentObject.RootNamespace = nSpace;

                        int dIndex = nSpace.LastIndexOf(".") + 1;

                        currentObject.AssemblyName = currentObject.RootNamespace.Substring(dIndex, nSpace.Length - dIndex);
                    }
                }
               

                base.SetNamespaceInformation(currentObject, fileNames);
            }

            public override void ProcessExternalItems(BuildItemGroup externalGroup, string[] fileNames)
            {
                base.ProcessExternalItems(externalGroup, fileNames, "Properties");
            }

            public override void AddHeaderInformation(BuildProject currentObject)
            {
                Project currentProject = currentObject.m_Project;

                //default targets

                currentProject.DefaultTargets = "Build";

                //General configuration

                BuildPropertyGroup generalProperties = currentProject.AddNewPropertyGroup(false);

                //custom Properties

                generalProperties.AddNewProperty("Configuration", "Debug").Condition = " '$(Configuration)' == '' ";
                generalProperties.AddNewProperty("Platform", "AnyCPU").Condition = " '$(Platform)' == '' ";
                generalProperties.AddNewProperty("RootNamespace", currentObject.p_RootNamespace);
                generalProperties.AddNewProperty("AssemblyName", currentObject.p_AssemblyName);
                generalProperties.AddNewProperty("UICulture", currentObject.p_UICulture);
                generalProperties.AddNewProperty("OutputType", currentObject.p_OutputType.ToString());

                generalProperties.AddNewProperty("ProjectName", currentObject.m_ProjectName);
                generalProperties.AddNewProperty("ProjectNotes", currentObject.m_ProjectNotes);

                //Debug and Release

                BuildPropertyGroup debugGroup = currentProject.AddNewPropertyGroup(false);
                debugGroup.Condition = " '$(Configuration)|$(Platform)' == 'Debug|AnyCPU' ";

                debugGroup.AddNewProperty("DebugSymbols", "true");
                debugGroup.AddNewProperty("DebugType", "full");
                debugGroup.AddNewProperty("Optimize", "false");
                debugGroup.AddNewProperty("OutputPath", @".\bin\Debug\");
                debugGroup.AddNewProperty("DefineConstants", "DEBUG;TRACE");

                BuildPropertyGroup releaseGroup = currentProject.AddNewPropertyGroup(false);
                releaseGroup.Condition = " '$(Configuration)|$(Platform)' == 'Release|AnyCPU' ";

                releaseGroup.AddNewProperty("DebugSymbols", "false");
                releaseGroup.AddNewProperty("Optimize", "true");
                releaseGroup.AddNewProperty("OutputPath", @".\bin\Release\");
                releaseGroup.AddNewProperty("DefineConstants", "TRACE");
            }

            public override void SetTargetInformation(Project currentProject)
            {
                base.m_CustomTargets.Insert(0, @"$(MSBuildBinPath)\Microsoft.CSharp.targets");
                base.SetTargetInformation(currentProject);
            }
            #endregion

            #region File Filters

            private class CSharpCSFilter : IBuildFileFilter
            {
                public string FileExtension
                {
                    get
                    {
                        return (".cs");
                    }
                }

                public void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup fileGroup, string filePath)
                {
                    BuildItem fileItem = fileGroup.AddNewItem("Compile", Path.GetFileName(filePath));
                }
            }


            private class CSharpDesignerFilter : IBuildFileFilter
            {
                public string FileExtension
                {
                    get
                    {
                        return (".Designer.cs");
                    }
                }

                public void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup fileGroup, string filePath)
                {

                    string formName = Path.GetFileNameWithoutExtension(Path.GetFileNameWithoutExtension(filePath)) + ".cs";

                    fileGroup.AddNewItem("Compile", Path.GetFileName(filePath)).SetMetadata("DependentUpon", formName);

                    //stuff needed for Windows Forms

                    ParentFilter.AddReference("System.Drawing");
                    ParentFilter.AddReference("System.Windows.Forms");


                    //get the "type"

                    TextReader fileReader = new StreamReader(Path.GetDirectoryName(filePath) + @"\" + formName);

                    string fileContents = fileReader.ReadToEnd();

                    if ((fileContents.Contains(": System.Windows.Forms.UserControl")) || (fileContents.Contains(": UserControl")))
                    {
                        ParentFilter.Type = "Windows";
                    }
                    else
                    {
                        ParentFilter.Type = "WindowsForms";
                    }

                    fileReader.Close();

                }
            }


            private class CSharpXAMLFilter : IBuildFileFilter
            {
                public string FileExtension
                {
                    get
                    {
                        return (".xaml");
                    }
                }

                public void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup fileGroup, string filePath)
                {
                    fileGroup.AddNewItem("Page", Path.GetFileName(filePath));

                    //check for an application or NavigationApplication
                    //    <ApplicationDefinition Include="MyApp.xaml" />

                    XmlDocument xamlDocument = new XmlDocument();
                    xamlDocument.Load(filePath);

                    string xamlType = xamlDocument.DocumentElement.LocalName;

                    if ((xamlType == "Application") || (xamlType == "NavigationApplication"))
                    {
                        string fileName = Path.GetFileName(filePath);

                        fileGroup.AddNewItem("ApplicationDefinition", fileName);
                    }

                    //Targets and References needed for presentation
                    ParentFilter.AddTarget(@"$(MSBuildBinPath)\Microsoft.WinFX.targets");
                    ParentFilter.AddReference("WindowsBase");
                    ParentFilter.AddReference("PresentationCore");
                    ParentFilter.AddReference("PresentationFramework");
                    ParentFilter.AddReference("UIAutomationProvider");
                    ParentFilter.AddReference("UIAutomationTypes");

                    //Targets and References needed for WCF

                    ParentFilter.AddReference("System.Runtime.Serialization");
                    ParentFilter.AddReference("System.ServiceModel");

                    ParentFilter.Type = "Presentation";
                }
            }

            private class CSharpXAMLCSFilter : IBuildFileFilter
            {
                public string FileExtension
                {
                    get
                    {
                        return (".xaml.cs");
                    }
                }

                public void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup fileGroup, string filePath)
                {
                    BuildItem codeItem = fileGroup.AddNewItem("Compile", Path.GetFileName(filePath));

                    codeItem.SetMetadata("SubType", "Code");
                    codeItem.SetMetadata("DependentUpon", Path.GetFileNameWithoutExtension(filePath));
                }
            }

            #endregion

            #region Generated Filters

            private class CSharpAssemblyInfoFilter : IBuildGeneratedFilter
            {
                public string FileIdentifier
                {
                    get
                    {
                        return ("AssemblyInfo.cs");
                    }
                }

                public void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup externalGroup, string[] fileNames)
                {
                    externalGroup.AddNewItem("Compile", @"Properties\AssemblyInfo.cs");
                }
            }

            private class CSharpResourcesFilter : IBuildGeneratedFilter
            {
                public string FileIdentifier
                {
                    get
                    {
                        return ("Resources.resx");
                    }
                }

                public void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup externalGroup, string[] fileNames)
                {
                    BuildItem resItem = externalGroup.AddNewItem("EmbeddedResource", @"Properties\Resources.resx");

                    string reSourceFile = "";

                    //find the code file associated with this resources file

                    foreach (string filePath in fileNames)
                    {
                        string fileName = Path.GetFileName(filePath);

                        if ((fileName.StartsWith("Resources")) && (Path.GetExtension(filePath) == ".cs"))
                        {
                            reSourceFile = fileName;
                        }
                    }

                    resItem.SetMetadata("Generator", "ResXFileCodeGenerator");
                    resItem.SetMetadata("LastGenOutput", reSourceFile);

                    BuildItem codeItem = externalGroup.AddNewItem("Compile", @"Properties\" + reSourceFile);
                    codeItem.SetMetadata("DependentUpon", "Resources.resx");
                    codeItem.SetMetadata("AutoGen", "true");
                }

            }

            private class CSharpSettingsFilter : IBuildGeneratedFilter
            {
                public string FileIdentifier
                {
                    get
                    {
                        return ("Settings.settings");
                    }
                }

                public void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup externalGroup, string[] fileNames)
                {
                    BuildItem resItem = externalGroup.AddNewItem("None", @"Properties\Settings.settings");

                    string setSourceFile = "";

                    //find the code file associated with this resources file

                    foreach (string filePath in fileNames)
                    {
                        string fileName = Path.GetFileName(filePath);

                        if ((fileName.StartsWith("Settings")) && (Path.GetExtension(filePath) == ".cs"))
                        {
                            setSourceFile = fileName;
                        }
                    }

                    resItem.SetMetadata("Generator", "SettingsSingleFileGenerator");
                    resItem.SetMetadata("LastGenOutput", setSourceFile);

                    BuildItem codeItem = externalGroup.AddNewItem("Compile", @"Properties\" + setSourceFile);
                    codeItem.SetMetadata("DependentUpon", "Settings.settings");
                    codeItem.SetMetadata("AutoGen", "true");
                }
            }


            #endregion
        }

        public class VisualBasicFilter : CLRFilter
        {
            #region Constructor

            private string m_Startup = "Module1";
            private List<string> m_CustomImports;

            public VisualBasicFilter()
                : base()
            {
                m_Filters.Add(new VisualBasicVBFilter());
                m_Filters.Add(new VisualBasicDesignerFilter());
                m_Filters.Add(new VisualBasicXAMLFilter());
                m_Filters.Add(new VisualBasicXAMLVBFilter());

                m_GeneratedFilters.Add(new VisualBasicApplicationFile());
                m_GeneratedFilters.Add(new VisualBasicResourcesFilter());
                m_GeneratedFilters.Add(new VisualBasicSettingsFilter());

                m_CustomImports = new List<string>();
            }
            #endregion

            #region Processing Methods


            public string Startup
            {
                get
                {
                    return (m_Startup);
                }
                set
                {
                    m_Startup = value;
                }
            }

            public override string FileExtension
            {
                get
                {
                    return (".vb");
                }
            }

            public override string ProjectFilePrefix
            {
                get
                {
                    return ("vb");
                }
            }

            public override BuildProjectType FilterType
            {
                get
                {
                    return (BuildProjectType.VisualBasic);
                }
            }

            public void AddImport(string import)
            {
                if (!m_CustomImports.Contains(import))
                {
                    m_CustomImports.Add(import);
                }
            }

            public override void SetNamespaceInformation(BuildProject currentObject, string[] fileNames)
            {
                SortedList<string, int> Namespaces = new SortedList<string, int>();

                foreach (string filePath in fileNames)
                {
                    string currentFileName = Path.GetFileName(filePath);

                    string fileExtension = Path.GetExtension(currentFileName);

                    if (fileExtension == this.FileExtension)
                    {
                        TextReader fileReader = new StreamReader(filePath);

                        string fileContents = fileReader.ReadToEnd();

                        Regex namespaceRegex = new Regex(@".*Namespace\s([A-za-z0-9\.]+)\s.*");

                        if (namespaceRegex.IsMatch(fileContents))
                        {
                            string namespaceValue = namespaceRegex.Match(fileContents).Groups[0].Value.Split(' ')[1].Trim();

                            if (!Namespaces.Keys.Contains(namespaceValue))
                            {
                                Namespaces.Add(namespaceValue, 0);
                            }

                            Namespaces[namespaceValue]++;
                        }

                        fileReader.Close();
                    }
                }

                int maxCount = 0;

                foreach (string nSpace in Namespaces.Keys)
                {
                    if (Namespaces[nSpace] > maxCount)
                    {
                        maxCount = Namespaces[nSpace];

                        currentObject.RootNamespace = nSpace;

                        int dIndex = nSpace.LastIndexOf(".") + 1;

                        currentObject.AssemblyName = currentObject.RootNamespace.Substring(dIndex, nSpace.Length - dIndex);
                    }
                }

                if (currentObject.RootNamespace.Length == 0)
                {
                    currentObject.RootNamespace = currentObject.ProjectName.Replace(" ", "").Replace("#", "").Trim();
                    currentObject.AssemblyName = currentObject.RootNamespace;
                }

                base.SetNamespaceInformation(currentObject, fileNames);
            }

            public override void ProcessExternalItems(BuildItemGroup externalGroup, string[] fileNames)
            {
                base.ProcessExternalItems(externalGroup, fileNames, "My Project");
            }

            public override void AddHeaderInformation(BuildProject currentObject)
            {
                Project currentProject = currentObject.m_Project;

                //default targets

                currentProject.DefaultTargets = "Build";

                //General configuration

                BuildPropertyGroup generalProperties = currentProject.AddNewPropertyGroup(false);

                //custom Properties

                generalProperties.AddNewProperty("Configuration", "Debug").Condition = " '$(Configuration)' == '' ";
                generalProperties.AddNewProperty("Platform", "AnyCPU").Condition = " '$(Platform)' == '' ";

                if (m_Type == "WindowsForms")
                {
                    generalProperties.AddNewProperty("StartupObject", currentObject.p_RootNamespace + ".My.MyApplication");
                }

                if (m_Type == "Console")
                {
                    generalProperties.AddNewProperty("StartupObject", currentObject.p_RootNamespace + "." + m_Startup);
                }

                generalProperties.AddNewProperty("RootNamespace", currentObject.p_RootNamespace);
                generalProperties.AddNewProperty("AssemblyName", currentObject.p_AssemblyName);
                generalProperties.AddNewProperty("UICulture", currentObject.p_UICulture);
                generalProperties.AddNewProperty("OutputType", currentObject.p_OutputType.ToString());

                generalProperties.AddNewProperty("ProjectName", currentObject.m_ProjectName);
                generalProperties.AddNewProperty("ProjectNotes", currentObject.m_ProjectNotes);

                //add MyType in

                if (m_Type != "Presentation")
                {
                    generalProperties.AddNewProperty("MyType", m_Type);
                }

                //Debug and Release

                BuildPropertyGroup debugGroup = currentProject.AddNewPropertyGroup(false);
                debugGroup.Condition = " '$(Configuration)|$(Platform)' == 'Debug|AnyCPU' ";

                debugGroup.AddNewProperty("DebugSymbols", "true");
                debugGroup.AddNewProperty("DebugType", "full");
                debugGroup.AddNewProperty("DefineDebug", "true");
                debugGroup.AddNewProperty("OutputPath", @"bin\Debug\");
                debugGroup.AddNewProperty("NoWarn", "42016,41999,42017,42018,42019,42032,42036,42020,42021,42022");

                BuildPropertyGroup releaseGroup = currentProject.AddNewPropertyGroup(false);
                releaseGroup.Condition = " '$(Configuration)|$(Platform)' == 'Release|AnyCPU' ";

                releaseGroup.AddNewProperty("DebugType", "pdbonly");
                releaseGroup.AddNewProperty("DebugSymbols", "false");
                releaseGroup.AddNewProperty("Optimize", "true");
                releaseGroup.AddNewProperty("DefineDebug", "false");
                releaseGroup.AddNewProperty("OutputPath", @"bin\Release\");
                releaseGroup.AddNewProperty("NoWarn", "42016,41999,42017,42018,42019,42032,42036,42020,42021,42022");

                //Add Imports

                BuildItemGroup importGroup = currentProject.AddNewItemGroup();

                AddImport("Microsoft.VisualBasic");
                AddImport("System");
                AddImport("System.Collections");
                AddImport("System.Data");

                foreach (string import in m_CustomImports)
                {
                    importGroup.AddNewItem("Import", import);
                }
            }

            public override void SetTargetInformation(Project currentProject)
            {
                base.m_CustomTargets.Insert(0, @"$(MSBuildBinPath)\Microsoft.VisualBasic.targets");
                base.SetTargetInformation(currentProject);
            }
            #endregion

            #region File Filters

            private class VisualBasicVBFilter : IBuildFileFilter
            {
                public string FileExtension
                {
                    get
                    {
                        return (".vb");
                    }
                }

                public void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup fileGroup, string filePath)
                {
                    VisualBasicFilter ParentVBFilter = ParentFilter as VisualBasicFilter;

                    BuildItem fileItem = fileGroup.AddNewItem("Compile", Path.GetFileName(filePath));

                    TextReader fileReader = new StreamReader(filePath);

                    string fileContents = fileReader.ReadToEnd();

                    if (fileContents.Contains("Sub Main("))
                    {

                        Regex ModuleMatch = new Regex(@"Module\s([A-za-z0-9\.]+)*");

                        ParentVBFilter.Startup = ModuleMatch.Match(fileContents).Groups[0].Value.Split(' ')[1].Trim(); ;
                    }

                    fileReader.Close();
                }
            }


            private class VisualBasicDesignerFilter : IBuildFileFilter
            {
                public string FileExtension
                {
                    get
                    {
                        return (".Designer.vb");
                    }
                }

                public void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup fileGroup, string filePath)
                {
                    VisualBasicFilter ParentVBFilter = ParentFilter as VisualBasicFilter;

                    string formName = Path.GetFileNameWithoutExtension(Path.GetFileNameWithoutExtension(filePath));

                    fileGroup.AddNewItem("Compile", Path.GetFileName(filePath)).SetMetadata("DependentUpon", formName + ".vb");

                    //get the "type"

                    TextReader fileReader = new StreamReader(filePath);

                    string fileContents = fileReader.ReadToEnd();

                    if (fileContents.Contains("Inherits System.Windows.Forms.Form"))
                    {
                        //Windows Form >> WindowsForms
                        ParentFilter.Type = "WindowsForms";
                    }
                    if (fileContents.Contains("Inherits System.Windows.Forms.UserControl"))
                    {
                        //User Control >> Windows
                        ParentFilter.Type = "Windows";
                    }

                    fileReader.Close();

                    //check for a resources file

                    if (File.Exists(Path.GetDirectoryName(filePath) + formName + ".resx"))
                    {
                        fileGroup.AddNewItem("EmbeddedResource", formName + ".resx").SetMetadata("DependentUpon", formName + ".vb");
                    }

                    //stuff needed for Windows Forms

                    ParentFilter.AddReference("System.Windows.Forms");
                    ParentFilter.AddReference("System.Drawing");

                    ParentVBFilter.AddImport("System.Drawing");
                    ParentVBFilter.AddImport("System.Windows.Forms");
                }
            }



            private class VisualBasicXAMLFilter : IBuildFileFilter
            {
                public string FileExtension
                {
                    get
                    {
                        return (".xaml");
                    }
                }

                public void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup fileGroup, string filePath)
                {
                    fileGroup.AddNewItem("Page", Path.GetFileName(filePath));

                    //check for an application or NavigationApplication
                    //    <ApplicationDefinition Include="MyApp.xaml" />

                    XmlDocument xamlDocument = new XmlDocument();
                    xamlDocument.Load(filePath);

                    string xamlType = xamlDocument.DocumentElement.LocalName;

                    if ((xamlType == "Application") || (xamlType == "NavigationApplication"))
                    {
                        string fileName = Path.GetFileName(filePath);

                        fileGroup.AddNewItem("ApplicationDefinition", fileName);
                    }

                    //Stuff needed for Presentation
                    ParentFilter.AddTarget(@"$(MSBuildBinPath)\Microsoft.WinFX.targets");
                    ParentFilter.AddReference("WindowsBase");
                    ParentFilter.AddReference("PresentationCore");
                    ParentFilter.AddReference("PresentationFramework");
                    ParentFilter.AddReference("UIAutomationProvider");
                    ParentFilter.AddReference("UIAutomationTypes");

                    //stuff needed for WCF
                    ParentFilter.AddReference("System.Runtime.Serialization");
                    ParentFilter.AddReference("System.ServiceModel");

                    //Set type
                    ParentFilter.Type = "Presentation";

                    //Add Imports

                    VisualBasicFilter ParentVBFilter = ParentFilter as VisualBasicFilter;
                    ParentVBFilter.AddImport("System.Windows");
                    ParentVBFilter.AddImport("System.Windows.Controls");
                    ParentVBFilter.AddImport("System.Windows.Documents");
                    ParentVBFilter.AddImport("System.Windows.Shapes");
                    ParentVBFilter.AddImport("System.Windows.Media");
                    ParentVBFilter.AddImport("System.Windows.Navigation");
                    ParentVBFilter.AddImport("System.Windows.Data");
                }
            }

            private class VisualBasicXAMLVBFilter : IBuildFileFilter
            {
                public string FileExtension
                {
                    get
                    {
                        return (".xaml.vb");
                    }
                }

                public void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup fileGroup, string filePath)
                {
                    BuildItem codeItem = fileGroup.AddNewItem("Compile", Path.GetFileName(filePath));

                    // codeItem.SetMetadata("SubType", "Code");
                    //codeItem.SetMetadata("DependentUpon", Path.GetFileNameWithoutExtension(filePath));
                }
            }


            #endregion

            #region Generated Filters

            private class VisualBasicApplicationFile : IBuildGeneratedFilter
            {
                public string FileIdentifier
                {
                    get
                    {
                        return ("Application.myapp");
                    }
                }

                public void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup externalGroup, string[] fileNames)
                {
                    BuildItem resItem = externalGroup.AddNewItem("None", @"My Project\Application.myapp");

                    string appFile = "";

                    //find the code file associated with this resources file

                    foreach (string filePath in fileNames)
                    {
                        string fileName = Path.GetFileName(filePath);

                        if ((fileName.StartsWith("Application.")) && (Path.GetExtension(filePath) == ".vb"))
                        {
                            appFile = fileName;
                        }
                    }

                    resItem.SetMetadata("Generator", "MyApplicationCodeGenerator");
                    resItem.SetMetadata("LastGenOutput", appFile);

                    BuildItem codeItem = externalGroup.AddNewItem("Compile", @"My Project\" + appFile);
                    codeItem.SetMetadata("DependentUpon", "Application.myapp");
                    codeItem.SetMetadata("AutoGen", "true");
                }

            }


            private class VisualBasicResourcesFilter : IBuildGeneratedFilter
            {
                public string FileIdentifier
                {
                    get
                    {
                        return ("Resources.resx");
                    }
                }

                public void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup externalGroup, string[] fileNames)
                {
                    BuildItem resItem = externalGroup.AddNewItem("EmbeddedResource", @"My Project\Resources.resx");

                    string reSourceFile = "";

                    //find the code file associated with this resources file

                    foreach (string filePath in fileNames)
                    {
                        string fileName = Path.GetFileName(filePath);

                        if ((fileName.StartsWith("Resources")) && (Path.GetExtension(filePath) == ".vb"))
                        {
                            reSourceFile = fileName;
                        }
                    }

                    resItem.SetMetadata("Generator", "VbMyResourcesResXFileCodeGenerator");
                    resItem.SetMetadata("LastGenOutput", reSourceFile);
                    resItem.SetMetadata("CustomToolNamespace", "My.Resources");

                    BuildItem codeItem = externalGroup.AddNewItem("Compile", @"My Project\" + reSourceFile);
                    codeItem.SetMetadata("DependentUpon", "Resources.resx");
                    codeItem.SetMetadata("AutoGen", "true");
                }

            }

            private class VisualBasicSettingsFilter : IBuildGeneratedFilter
            {
                public string FileIdentifier
                {
                    get
                    {
                        return ("Settings.settings");
                    }
                }

                public void ProcessFile(IBuildProjectFilter ParentFilter, BuildItemGroup externalGroup, string[] fileNames)
                {
                    BuildItem resItem = externalGroup.AddNewItem("None", @"My Project\Settings.settings");

                    string setSourceFile = "";

                    //find the code file associated with this resources file

                    foreach (string filePath in fileNames)
                    {
                        string fileName = Path.GetFileName(filePath);

                        if ((fileName.StartsWith("Settings")) && (Path.GetExtension(filePath) == ".vb"))
                        {
                            setSourceFile = fileName;
                        }
                    }

                    resItem.SetMetadata("Generator", "SettingsSingleFileGenerator");
                    resItem.SetMetadata("LastGenOutput", setSourceFile);

                    BuildItem codeItem = externalGroup.AddNewItem("Compile", @"My Project\" + setSourceFile);
                    codeItem.SetMetadata("DependentUpon", "Settings.settings");
                    codeItem.SetMetadata("AutoGen", "true");
                }
            }


            #endregion        }
        }
    }

    // Summary:
    // Custom class deriving from the Microsoft.Build.Framework.ILogger interface which allows logging of MS Build events
    
    public class BuildLogger : Microsoft.Build.Framework.ILogger
    {
        #region Private Members, Initializer and Delegates
        private bool m_Registered = false;
        private LoggerVerbosity m_Verbosity = LoggerVerbosity.Normal;
        private StringBuilder m_Log = new StringBuilder();

        public event EventHandler<LoggerEventArgs> LogChanged;

        private List<BuildEventArgs> m_Items=null;

        public void Initialize(Microsoft.Build.Framework.IEventSource eventSource)
        {
            if (eventSource==null) {
                throw new ArgumentNullException("eventSource");
            }

            //setup all the events we wish to monitor here
            eventSource.ErrorRaised += new BuildErrorEventHandler(eventSource_ErrorRaised);
            eventSource.MessageRaised += new BuildMessageEventHandler(eventSource_MessageRaised);
            eventSource.WarningRaised += new BuildWarningEventHandler(eventSource_WarningRaised);
            eventSource.TargetStarted += new TargetStartedEventHandler(eventSource_TargetStarted);
            eventSource.TargetFinished += new TargetFinishedEventHandler(eventSource_TargetFinished);
            eventSource.TaskStarted += new TaskStartedEventHandler(eventSource_TaskStarted);
            eventSource.TaskFinished += new TaskFinishedEventHandler(eventSource_TaskFinished);
            eventSource.ProjectStarted += new ProjectStartedEventHandler(eventSource_ProjectStarted);
            eventSource.ProjectFinished += new ProjectFinishedEventHandler(eventSource_ProjectFinished);
            eventSource.BuildStarted += new BuildStartedEventHandler(eventSource_BuildStarted);
            eventSource.BuildFinished += new BuildFinishedEventHandler(eventSource_BuildFinished);
        }
        #endregion

        #region Build Events
        void eventSource_BuildFinished(object sender, BuildFinishedEventArgs e)
        {
            appendToLog(e.Timestamp + ":" + e.Message);
        }

        void eventSource_BuildStarted(object sender, BuildStartedEventArgs e)
        {
            appendToLog(e.Timestamp + ":" + e.Message);
        }

        void eventSource_ProjectFinished(object sender, ProjectFinishedEventArgs e)
        {
            appendToLog(e.Timestamp + ":" + e.Message);
        }

        void eventSource_ProjectStarted(object sender, ProjectStartedEventArgs e)
        {
            appendToLog(e.Timestamp + ":" + e.Message);
        }

        void eventSource_TaskStarted(object sender, TaskStartedEventArgs e)
        {
            if (m_Verbosity == LoggerVerbosity.Diagnostic)
            {
                appendToLog("Task Started: " + e.Timestamp + ":" + e.Message);
            }
        }

        void eventSource_TaskFinished(object sender, TaskFinishedEventArgs e)
        {
            if (m_Verbosity == LoggerVerbosity.Diagnostic)
            {
                appendToLog("Task Finished: " + e.Timestamp + ":" + e.Message);
            }
        }

        void eventSource_TargetFinished(object sender, TargetFinishedEventArgs e)
        {
            if ((m_Verbosity == LoggerVerbosity.Detailed) || (m_Verbosity == LoggerVerbosity.Diagnostic))
            {
                appendToLog("Target Finished: " + e.Timestamp + ":" + e.Message);
            }
        }

        void eventSource_TargetStarted(object sender, TargetStartedEventArgs e)
        {
            if ((m_Verbosity == LoggerVerbosity.Detailed) || (m_Verbosity == LoggerVerbosity.Diagnostic))
            {
                appendToLog("Target Started: " + e.Timestamp + ":" + e.Message);
            }
        }

        void eventSource_WarningRaised(object sender, BuildWarningEventArgs e)
        {
            bool displayMessage = true;

            if (m_Verbosity == LoggerVerbosity.Quiet)
            {
                displayMessage = false;
            }

            if (displayMessage)
            {
                appendToLog("Warning: " + e.Timestamp + ":" + e.Message,e);
            }
        }

        void eventSource_MessageRaised(object sender, BuildMessageEventArgs e)
        {
            bool displayMessage=true;

            if ((m_Verbosity == LoggerVerbosity.Detailed))
            {
                if (e.Importance == MessageImportance.Low)
                {
                    displayMessage = false;
                }
            }

            if ((m_Verbosity == LoggerVerbosity.Minimal) || (m_Verbosity == LoggerVerbosity.Normal))
            {
                if (e.Importance != MessageImportance.High)
                {
                    displayMessage = false;
                }
            }

            if (m_Verbosity == LoggerVerbosity.Quiet)
            {
                displayMessage = false;
            }

            if (displayMessage) {
                appendToLog(e.Timestamp + ":" + e.Message,e);
            }
        }

        void eventSource_ErrorRaised(object sender, BuildErrorEventArgs e)
        {
            bool displayMessage = true;

            if (m_Verbosity == LoggerVerbosity.Quiet)
            {
                displayMessage = false;
            }

            if (displayMessage)
            {
                appendToLog("Error: " + e.Timestamp + ":" + e.Message,e);
            }
        }

        #endregion

        #region Methods

        private void appendToLog(string newText)
        {
            appendToLog(newText, null);
        }

        private void appendToLog(string newText, BuildEventArgs e)
        {
            if (e != null)
            {
                m_Items.Add(e);
            }

            m_Log.Append(newText + "\r\n\r\n");

            if (LogChanged != null)
            {
                LogChanged(this,new LoggerEventArgs(e));
            }
        }


        public void Shutdown()
        {
        }

        // Summary:
        // Resets the logger for a new MS Build run
        
        public void Reset()
        {
            m_Items = new List<BuildEventArgs>();
            m_Log = new StringBuilder();
        }

        // Summary:
        // Registers the logger class with MS Build so it can recieve events
        
        public void Register()
        {
            Microsoft.Build.BuildEngine.Engine.GlobalEngine.RegisterLogger(this);
            m_Registered = true;
        }

        #endregion

        #region Properties

        public Microsoft.Build.Framework.LoggerVerbosity Verbosity
        {
            get
            {
                return (m_Verbosity);
            }
            set
            {
                m_Verbosity = value;
            }
        }

        public string LogOutput
        {
            get
            {
                return (m_Log.ToString());
            }
        }

        // Summary:
        // Tells if the Logger is Registered with MS Build
        
        public bool IsRegistered
        {
            get
            {
                return (m_Registered);
            }
        }

        public string Parameters
        {
            get
            {
                return ("");
            }
            set
            {
            }
        }

        public BuildEventArgs[] Items
        {
            get
            {
                return (m_Items.ToArray());
            }
        }

        #endregion

        // Summary:
        // Custom class deriving from System.EventArgs which allows passing of MS Build events during a Logger update
        
        public class LoggerEventArgs : System.EventArgs
        {
            public BuildEventArgs BuildEvent;

            public LoggerEventArgs(BuildEventArgs e)
            {
                BuildEvent = e;
            }
        }
    }

}
