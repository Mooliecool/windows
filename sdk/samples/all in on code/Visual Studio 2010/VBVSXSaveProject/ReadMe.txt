========================================================================================
                      ProjectName: VBVSXSaveProject
========================================================================================

////////////////////////////////////////////////////////////////////////////////////////
Summary

This sample demonstrates that you can save an existing project to different location via
IDE menu. It supplies following features:
1. Copy the whole project to a new location.
2. Enable a user to select the files.
3. Open the new project in the current IDE.

NOTE: This sample only supports to copy the files in the project folder. 
  
////////////////////////////////////////////////////////////////////////////////////////
Requirement

Visual Studio 2010 and Visual Studio 2010 SDK.

You can download the Visual Studio 2010 SDK from
http://www.microsoft.com/downloads/en/details.aspx?FamilyID=47305CF4-2BEA-43C0-91CD-1B853602DCC5


////////////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Open the project in VS2010.

Step2. Set the VBVSXSaveProject as the StartUp Project, and open its property pages. 

       1. Select the Debug tab. Set the Start Option to Start external program and browse 
          the devenv.exe (The default location is C:\Program Files\Microsoft Visual Studio
          10.0\Common7\IDE\devenv.exe), and add "/rootsuffix Exp" (no quote) to the Command
          line arguments.
       
       2. Select the VSIX tab, make sure "Create VSIX Container during build" and 
          "Deploy VSIX content to Experimental Instance for debugging" are checked.

Step3. Build the solution.  

Step4. Press F5, and the Experimental Instance of Microsoft Visual Studio 2010 will 
       be launched.
       
       In the VS Experimental Instance, click Tool=>Extension Manager, you will find 
       CSVSXSaveProject is loaded.

Step5. In the VS Experimental Instance, open an existing project. 
       
Step6. Right click a project node in the Solution Explorer, then click the menu 
       "CSVSXSaveProject". Click this menu, the SaveProjectDialog will show.
       
Step7. Select the files you want to copy in the SaveProjectDialog, check "Open new project",
       and then click the "Save as" button and choose a folder.

       You will find the current VS open the new project.

////////////////////////////////////////////////////////////////////////////////////////
Code Logic:


A. Create a VS Package named CSVSXSaveProject.

   You can check the detailed steps in  http://msdn.microsoft.com/en-us/library/cc138589.aspx


B. Add the Command to File menu and the solution explorer context menu. 
        
		
   1. Add a menu item to File menu.
         <Group guid="guidCSVSXSaveProjectCmdSet" id="CSVSXSaveProjectGroup" priority="0x0600">
              <Parent guid="guidSHLMainMenu" id="IDM_VS_MENU_FILE"/>
            </Group>
          
         <Button guid="guidCSVSXSaveProjectCmdSet" id="cmdidCSVSXSaveProjectCommandID"
              priority="0x0100" type="Button">
             <Parent guid="guidCSVSXSaveProjectCmdSet" id="CSVSXSaveProjectGroup" />
             <Icon guid="guidImages" id="bmpPic1" />
             <Strings>
               <CommandName>cmdidCSVSXSaveProjectCommandID</CommandName>
               <ButtonText>CSVSXSaveProject</ButtonText>
             </Strings>
         </Button>
        
         <!--Dynamic visibility-->
         <VisibilityConstraints>
            <VisibilityItem guid="guidCSVSXSaveProjectCmdSet" 
         	id="cmdidCSVSXSaveProjectCommandID" context="UICONTEXT_SolutionExists"/>
         </VisibilityConstraints>

   2. Add the command to the solution explorer context menu. 
      
        <Group guid="guidCSVSXSaveProjectContextCmdSet" id="menuidCSVSXSaveProjectContextGroup" priority="0x01">
          <Parent guid="guidSolutionExplorerMenu" id="menuidSolutionExplorerMenu"/>
        </Group>

		<Button guid="guidCSVSXSaveProjectCmdSet" id="cmdidCSVSXSaveProjectCommandID"
		    priority="0x0100" type="Button">
			<Parent guid="guidCSVSXSaveProjectCmdSet" id="CSVSXSaveProjectGroup" />
			<Icon guid="guidImages" id="bmpPic1" />
        
			<!--Add the dynamic visibility about the command menu.-->
			<CommandFlag>DynamicVisibility</CommandFlag>
        
			<Strings>
			  <CommandName>cmdidCSVSXSaveProjectCommandID</CommandName>
			  <ButtonText>CSVSXSaveProject</ButtonText>
			</Strings>
		 </Button>	

C. Get the files included in the project or in the project folder.
   
   1. Get the files in the project folder.
	     Public Shared Function GetFilesInProjectFolder(ByVal projectFilePath As String) As List(Of ProjectFileItem)
            ' Get the folder that includes the project file.
            Dim projFile As New FileInfo(projectFilePath)
            Dim projFolder As DirectoryInfo = projFile.Directory

            If projFolder.Exists Then
                ' Get all files information in project folder.
                Dim files = projFolder.GetFiles("*", SearchOption.AllDirectories)

                Return files.Select(Function(f) New ProjectFileItem With _
                                                {.Fileinfo = f,
                                                 .IsUnderProjectFolder = True}).ToList()
            Else
                ' The project folder does not exist.
                Return Nothing
            End If
        End Function
    
   2. Get the files included in the project.

        Public Function GetIncludedFiles() As List(Of ProjectFileItem)
            Dim files = New List(Of ProjectFileItem)()

            ' Add the project file (*.csproj or *.vbproj...) to the list of files.
            files.Add(New ProjectFileItem With _
                      {.Fileinfo = New FileInfo(Project.FullName),
                       .NeedCopy = True,
                       .IsUnderProjectFolder = True})

            ' Add the files included in the project.
            For Each item As ProjectItem In Project.ProjectItems
                GetProjectItem(item, files)
            Next item

            Return files
        End Function

        Private Sub GetProjectItem(ByVal item As ProjectItem, ByVal files As List(Of ProjectFileItem))
            For i As Short = 0 To item.FileCount - 1
                If File.Exists(item.FileNames(i)) Then
                    Dim fileItem As New ProjectFileItem()

                    fileItem.Fileinfo = New FileInfo(item.FileNames(i))

                    If fileItem.FullName.StartsWith(Me.ProjectFolder.FullName,
                                                    StringComparison.OrdinalIgnoreCase) Then
                        fileItem.IsUnderProjectFolder = True
                        fileItem.NeedCopy = True
                    End If

                    files.Add(fileItem)
                End If
            Next i

            ' Get the files of sub node under this node.
            For Each subItem As ProjectItem In item.ProjectItems
                GetProjectItem(subItem, files)
            Next subItem
        End Sub


D. Design the UI of SaveProjectDialog
   
   This dialog is used to display the files included in the project, or under the project
   folder. Users can select the files that need to be copied.

E. Open new project in the current IDE
    
    Dim cmd As String = String.Format(
                            "File.OpenProject ""{0}""", newProjectPath)

    Me.DTEObject.ExecuteCommand(cmd)  
/////////////////////////////////////////////////////////////////////////////////////////
References:
Walkthroughs for Customizing Visual Studio By Using VSPackages
http://msdn.microsoft.com/en-us/library/cc138565.aspx

Creating Your First VSPackage
http://blogs.msdn.com/b/vsxue/archive/2007/11/15/tutorial-2-creating-your-first-vspackage.aspx

How to: Dynamically Add Menu Items 
ms-help://MS.VSCC.v90/MS.VSIPCC.v90/ms.vssdk.v90/dv_vsintegration/html/d281e9c9-b289-4d64-8d0a-094bac6c333c.htm

Dynamic Menu Commands in Visual Studio Packages 
http://blogs.rev-net.com/ddewinter/2008/03/14/dynamic-menu-commands-in-visual-studio-packages-part-1/

Managing Solutions, Projects, and Files
http://msdn.microsoft.com/en-us/library/wbzbtw81.aspx

_Solution Members
http://msdn.microsoft.com/en-us/library/envdte._solution_members(v=VS.90).aspx

/////////////////////////////////////////////////////////////////////////////////////////