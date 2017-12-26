'use common library
ExecuteGlobal CreateObject("Scripting.FileSystemObject").OpenTextFile( left( WScript.ScriptFullName , len (WScript.ScriptFullName) -len( WScript.ScriptName ) ) & "\vb-script-library.vbs",1).ReadAll()
InitScript("Website Installation")


function GetWebPath( )
	GetWebPath= fso.GetFolder(GetScriptPath).ParentFolder & "\website"
end function

'---------------------------------------------------------------------------------
REM SCRIPT STARTS HERE :D
On Error Resume Next
Dim folder
Dim subfolders
Dim Link

Set subfolders = fso.GetFolder(GetWebPath).SubFolders 
For Each folder in subfolders
    DeleteVirtualWebDir folder.name 
    CreateVirtualWebDir folder.name,GetWebPath & "\"&folder.name
Next  

SetSSLSite "www.fabrikam.com"

On Error GoTo 0 
Set Link = wso.CreateShortcut( wso.SpecialFolders("Desktop") & "\" &  fso.GetFolder(GetScriptPath).ParentFolder.Name & " Sample Website.lnk")
Link.TargetPath = "c:\Program Files\Internet Explorer\iexplore.exe"
Link.Arguments = "https://www.fabrikam.com/CardSpace/"
Link.Description = fso.GetFolder(GetScriptPath).ParentFolder.Name & " Start Page"
Link.Save

Set Link = wso.CreateShortcut(wso.SpecialFolders("Desktop") & "\" &  fso.GetFolder(GetScriptPath).ParentFolder.Name & " Sample Files.lnk")
Link.TargetPath = fso.GetFolder(GetScriptPath).ParentFolder
Link.Description = fso.GetFolder(GetScriptPath).ParentFolder.Name & " Sample Files"
Link.Save

wso.run("https://www.fabrikam.com/CardSpace/")

finished 