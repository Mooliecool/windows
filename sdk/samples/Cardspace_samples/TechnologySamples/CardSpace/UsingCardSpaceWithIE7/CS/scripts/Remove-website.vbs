'use common library
ExecuteGlobal CreateObject("Scripting.FileSystemObject").OpenTextFile( left( WScript.ScriptFullName , len (WScript.ScriptFullName) -len( WScript.ScriptName ) ) & "\vb-script-library.vbs",1).ReadAll()
InitScript("Website Removal")

function GetWebPath( )
	GetWebPath= fso.GetFolder(GetScriptPath).ParentFolder & "\website"
end function

'---------------------------------------------------------------------------------
REM SCRIPT STARTS HERE :D
On Error Resume Next


Dim folder
Dim subfolders

Set subfolders = fso.GetFolder(GetWebPath).SubFolders 
For Each folder in subfolders
    DeleteVirtualWebDir folder.name 
Next  

fso.DeleteFile wso.SpecialFolders("Desktop") & "\" &  fso.GetFolder(GetScriptPath).ParentFolder.Name & " Sample Website.lnk"
fso.DeleteFile wso.SpecialFolders("Desktop") & "\" &  fso.GetFolder(GetScriptPath).ParentFolder.Name & " Sample Files.lnk"

finished 