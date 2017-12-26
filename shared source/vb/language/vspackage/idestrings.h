//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  STRIDs should be defined in the following ranges:
//
//   1     -  100    - non localized STRID (main common compiler DLLs)
//   101   -  200    - non localized STRID (main specific compiler DLLs)
//   1000  - 1008    - localized STRID (intl common compiler DLLs)
//   1009  - 1022    - localized STRID (intl specific compiler DLLs)
//   1023  - 1030    - localized STRID (intl common compiler DLLs)
//   1031  - 1100    - localized STRID (intl specific compiler DLLs)
//   1101  - 1199    - localized STRID (intl common compiler DLLs)
//   1200  - 29999   - localized STRID (intl specific compiler DLLs)
//
//  The convention for naming STRID's that take replacment strings is to give
//  them a number following the name (from 1-9) that indicates how many
//  arguments they expect.
//
//  This file should only contain string resources that are used by msvbideui.dll
//  Make sure to only use the appropriate ranges specified above.
//
//  50000 - 53000 are reserved for VB Keyword Statement Completion list descriptions
//  syntax tips IDEHlpStrs: see IdeHlpStrs.h.
//
//  You can Start VS and open msvbideui.DLL as a resource and view/edit the stringtable to see
//  what strings are in it.
//
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//
// Non-localized strings (use STRID_NOLOC macro to define these)
//
// RESERVED         1 - 100:       Used by common dll's.
//
STRID_NOLOC(IntlDllBaseName,         101,        "msvbideui.dll")
STRID_NOLOC(MFUHeader,               102,        "VBMRU0001")
STRID_NOLOC(MFUFileName,             103,        "VBMRU.dat")
STRID_NOLOC(XMLDocConfig,            104,        "VBXMLDoc.xml")
STRID_NOLOC(FSR_ReferenceFormat,     105,       "|1 - |2(|3, |4)")
STRID_NOLOC(IT_SnippetExtension,     106,        ".snippet")
STRID_NOLOC(IT_MySnippets,           107,        "VB Snippets")
STRID_NOLOC(EEERR_DefaultValue,      108,        "Nothing")

//
// AVAILABLE         109 - 500
//

//-------------------------------------------------------------------------------------------------
//
// Localized strings (use STRID macro to define these)
//
// RESERVED         1000 - 1008:       Used by common dll's.
//
//STRID(CollapseRegion                ,1009   , "Collapse |0")
//STRID(ExpandRegion                  ,1010   , "Expand |0")

//  Text displayed in Save As dialog
STRID(FormatFilter                  ,1011   , "All Files (*.*)\n*.*\nVisual Basic Files (*.vb)\n*.vb\n")

// Text used by package.rgs, msvb7.vrg, and msvb7_vsa.vrg
STRID(MSVBCodePageEditor            ,1012   , "Microsoft Visual Basic Code Page Editor")
STRID(MSVBEditor                    ,1013   , "Microsoft Visual Basic Editor")
STRID(OptionPageDes                 ,1014   , "VB Specific")

//Text used by VB language/colorizer service
STRID(MSVBLanguageName              ,1015   , "Microsoft Visual Basic")

//----------------------------------------------------------
// RESERVED         1023 - 1030:       Used by common dll's.
//----------------------------------------------------------
STRID(CodeModelEdit                 ,1031   , "Visual Basic CodeModel Edit")
STRID(DD_General                    ,1033   , "(General)")

// Use by IntelliSense for the tips in the completion list
STRID(IntelliSense_More             ,1034   , "more...")

//Used by the Object Browser Description Pane:
STRID(OB_Description_Summary        ,1035   , "Summary:")
STRID(OB_Description_Parameters     ,1036   , "Parameters:")
STRID(OB_Description_ReturnValues   ,1037   , "Return Values:")
STRID(OB_Description_Remarks        ,1038   , "Remarks:")
STRID(OB_Description_MemberOf1      ,1039   , "     Member of |1") // This string can only have 1 argument.
STRID(OB_BaseClasses                ,1040   ,"Base Types")
STRID(OB_Project                    ,1041    ,"Project")

STRID(HTM_Collapse_Comment          ,1042   , "Collapse Comment")
STRID(HTM_Expand_Comment            ,1043   , "Expand Comment")

STRID(OB_Assembly                   ,1044    ,"Assembly")
STRID(OB_InheritedMembers           ,1045    ,"Inherited Members")
STRID(OB_References                 ,1046    ,"References")

STRID(RenameSymbol                  ,1048   , "Symbolic Rename")
STRID(OverrideMember                ,1049   , "Override member")
STRID(XMLDocEdit                    ,1050   , "XML Doc Completion")
//STRID(BuilderEdit                   ,1051   , "Builder Edit")

// Used by the browser DropDowns
STRID(DD_MethodName                 ,1052   , "Method Name")
STRID(DD_ClassName                  ,1053   , "Class Name")
STRID(DD_Declarations               ,1054   , "(Declarations)")
STRID(DD_Empty                      ,1055   , "Empty")
//STRID(DD_MyBase                     ,1056   , "(Overrides)")
STRID(DD_ClassEvents                ,1057   , "(|1 Events)")

// Used by the VB Code Gen
STRID(CG_DesignerRequired           ,1058   , "This call is required by the designer.")
STRID(CG_UserCode                   ,1059   , "Add any initialization after the InitializeComponent() call.")

// Used by the OB
STRID(OB_Description_GenericParameters,1060   , "Type parameters:")
STRID(OB_Description_Exceptions     ,1061   , "Exceptions:")

STRID(DD_Updating                   ,1069   , "Updating...")

// Used by the find symbol results window
STRID(FSR_NoReferencesFound         ,1070   , "No references available for this item.")
STRID(FSR_HiddenDefinition          ,1071   , "The definition of the object is hidden.")
STRID(FSR_SearchCancelled           ,1072   , "Search for references was canceled.")

// Tools options - Used in package.rgs
STRID(Options_Basic                      ,1080   , "Basic")
STRID(Options_BasicSpecific              ,1081   , "Basic-Specific")
STRID(Options_Basic_Description          ,1082   , "Options that control general editor features including Intellisense statement completion, line number display, and single-click URL navigation.")
STRID(Options_BasicSpecific_Description  ,1083   , "Options that control Visual Basic editor features including automatic insertion of end constructs, procedure line separators, and automatic code formatting.")

// For the overloads tip
STRID(Intellisense_Overloads        ,1090   , " (+ |1 overloads)")

// For the completion list tool tip and quick info
STRID(Intellisense_Capabilities     ,1091   , "Associated Capabilities:")

// for awaitable quick info and parameter help
STRID(Intellisense_Usage            ,1092   , "Usage:")
STRID(Intellisense_Awaitable        ,1093   , "Awaitable")

STRID(Intellisense_Deprecated             ,1094   , "Deprecated")
STRID(Intellisense_Deprecated_Colon       ,1095   , "Deprecated:")
STRID(Intellisense_Extension              ,1096   , "Extension")

// XML comment block banner
STRID(XMLDocCommentBanner           ,1100   , "XML Doc Comment")

//----------------------------------------------------------
// RESERVED         1101 - 1199:       Used by common dll's.
//----------------------------------------------------------
// Symbol rename error messages
STRID(SR_Invalid_Name               ,1200   , "The name is not a valid identifier.")
STRID(SR_Name_Conflict              ,1201   , "The name conflicts with an existing symbol with the same name.")
STRID(SR_Files_Not_Checked_Out      ,1202   , "The rename operation cannot be completed because one or more files are not checked out.")
STRID(SR_Potential_Name_Conflict    ,1203   , "The specified name conflicts with another symbol currently accessible in this project. Changing the name could potentially cause the the compiler to bind exisitng code to a different type.\nDo you want to proceed with this operation?")
STRID(SR_Invalid_Xmlns              ,1204   , "The Xml prefix conflicts with the namespace.")
STRID(SR_Rename1To2                 ,1300   , "Rename '|1' to '|2'")
STRID(SR_SupportUndo                ,1301   , "This Rename operation will modify too many files to support a Global Undo. If you continue, the Rename operation will change matching items in all files in the solution, but an Undo operation will only affect files that are currently open in the Code Editor. Do you want to proceed?")

// IDisposable template comment strings
STRID(IDisposable_Todo1             ,1400   , "TODO: dispose managed state (managed objects).")
STRID(IDisposable_Todo2             ,1401   , "TODO: free unmanaged resources (unmanaged objects) and override Finalize() below.")
STRID(IDisposable_Comment1          ,1402   , "This code added by Visual Basic to correctly implement the disposable pattern.")
STRID(IDisposable_Comment2          ,1403   , "Do not change this code.  Put cleanup code in Dispose(disposing As Boolean) above.")
STRID(IDisposable_Comment3          ,1404   , "To detect redundant calls")
STRID(IDisposable_Todo3             ,1405   , "TODO: set large fields to null.")
STRID(IDisposable_Todo4             ,1406   , "TODO: override Finalize() only if Dispose(ByVal disposing As Boolean) above has code to free unmanaged resources.")
STRID(IDisposable_Comment4          ,1407   , "   ' Do not change this code.  Put cleanup code in Dispose(ByVal disposing As Boolean) above.")

// Warning marker description
STRID(WarningMarker                 ,1450   , "Visual Basic Warning")

// Used by Intellitask
STRID(IT_SnippetPaste, 1501, "Snippet Paste")
STRID(IT_SnippetCompoundUndo, 1502, "Snippet Deletion")
STRID(IT_SnippetMarkerChange, 1503, "Snippet Change")
STRID(IT_SnippetBadReferencesTitle, 1504, "Missing References")
STRID(IT_SnippetBadReferencesFormat, 1505, "The following references were not found.\nPlease locate and add them manually.\n\n%s\n")
STRID(IT_InsertSnippet, 1532, "Insert Snippet")
STRID(IT_LanguageDisplayName, 1533, "Visual Basic")
STRID(IT_HelpURL1, 1534, "Click for more: '|1'");
STRID(IT_ReplaceLinkedMarkers, 1539, "Replace Linked Markers");
STRID(IT_ShowSnippetHighlighting, 1540, "Show Snippet Highlighting");
STRID(IT_HideSnippetHighlighting, 1541, "Hide Snippet Highlighting");

// Used by Edit and Continue
STRID(ENC_ConfigurableMarker, 1600, "Visual Basic Read Only Marker")
STRID(ENC_MarkerChangeUndo, 1601, "Marker Undo")
STRID(ENC_ErrorPrefix, 1602, "Unable to apply this change while debugging.")
STRID(ENC_ErrorSuffix, 1603, "You must revert the change or stop the debugging session.")
STRID(ENC_TypeChange, 1604, "Variable type '|1' was changed to '|2'.")
STRID(ENC_StatementChange, 1605, "'|1' was changed to '|2'.")
STRID(ENC_AttributeChange, 1606, "Attribute '|1' was changed to '|2'.")
STRID(ENC_AccessChange, 1607, "Variable access type '|1' was changed to '|2'.")
STRID(ENC_AddStatement, 1608, "'|1' was added.")
STRID(ENC_AddStatement1, 1609, "'|1' was added which modifies public interface of '|2'.")
STRID(ENC_DeleteStatement, 1610, "'|1' was deleted.")
STRID(ENC_ImportChange, 1611, "Import directive '|1' was changed to '|2'.")
STRID(ENC_NameChange, 1612, "Name '|1' was renamed to '|2'.")
STRID(ENC_AddType, 1613, "Type '|1' was added.")
STRID(ENC_DeleteType, 1614, "Type '|1' was deleted.")
STRID(ENC_AddName, 1615, "Name '|1' was added.")
STRID(ENC_DeleteName, 1616, "Name '|1' was deleted.")
STRID(ENC_ArraySizeChange, 1617, "Size of the array '|1' was changed.")
STRID(ENC_ArrayDimensionChange, 1618, "Dimension of array '|1' was changed.")
STRID(ENC_InitializerChange, 1619, "Variable initializer '|1' was changed to '|2'.")
STRID(ENC_ModifierChange, 1620, "Modifier '|1' was changed to '|2'.")
STRID(ENC_AddModifier, 1621, "Modifier '|1' was added.")
STRID(ENC_DeleteModifier, 1622, "Modifier '|1' was deleted.")
STRID(ENC_ValueChange, 1623, "Value '|1' was changed to '|2'.")
STRID(ENC_AddValue, 1624, "Value '|1' was added.")
STRID(ENC_DeleteValue, 1625, "Value '|1' was deleted.")
STRID(ENC_StringModeChange, 1627, "String mode '|1' was changed to '|2'.")
STRID(ENC_AddArgument, 1628, "Argument '|1' was added.")
STRID(ENC_DeleteArgument, 1629, "Argument '|1' was deleted.")
STRID(ENC_AddParameter, 1630, "Parameter '|1' was added.")
STRID(ENC_DeleteParameter, 1631, "Parameter '|1' was deleted.")
STRID(ENC_AddAttribute, 1632, "Attribute '|1' was added.")
STRID(ENC_DeleteAttribute, 1633, "Attribute '|1' was deleted.")
STRID(ENC_AddVariable, 1634, "Variable '|1' was added.")
STRID(ENC_DeleteVariable, 1635, "Variable '|1' was deleted.")
STRID(ENC_DeleteStatement1, 1636, "'|1' was deleted that modifies '|2'.")
STRID(ENC_AddArrayDeclaration, 1637, "Array declaration '|1' was added.")
STRID(ENC_DeleteArrayDeclaration, 1638, "Array declaration '|1' was deleted.")
STRID(ENC_ModifyStatement1, 1639, "'|1' was modified.")
STRID(ENC_InitializerChange1, 1640, "Initializer '|1' was changed to '|2'.")
STRID(ENC_ConstraintChange, 1641, "Constraint '|1' was changed to '|2'.")
STRID(ENC_AddConstraint, 1642, "Constraint '|1' was added.")
STRID(ENC_DeleteConstraint, 1643, "Constraint '|1' was deleted.")
STRID(ENC_FileReloaded, 1644, "File '|1' was reloaded.")
STRID(ENC_DeletedReadOnlyStmt, 1645, "'|1' was deleted from a disallowed region.")
STRID(ENC_ModifiedReadOnlyStmt, 1646, "'|1' was changed in a disallowed region.")
STRID(ENC_GenericChange, 1647, "'|1' was changed to '|2' in a generic type or method.")
STRID(ENC_GenericAdd, 1648, "'|1' was added in a generic type or method.")
STRID(ENC_GenericDelete, 1649, "'|1' was deleted in a generic type or method.")
STRID(ENC_ImportAdd, 1650, "Import directive '|1' was added.")
STRID(ENC_ImportDelete, 1651, "Import directive '|1' was deleted.")
STRID(ENC_GenericChange1, 1652, "Statements were changed in generic type or method '|1'.")
STRID(ENC_AsyncBreakNoEE, 1653, "Execution is stopped in external code.")
STRID(ENC_ShadowingChange, 1655, "New variable '|1' shadows existing variable '|1'.")
STRID(ENC_Deletion, 1656, "A deletion was made.")
STRID(ENC_Transients, 1657, "A change was made requiring code generation that cannot be done while debugging.")
STRID(ENC_ExtensionMethod, 1658, "An extension method was added.")
STRID(ENC_ChangedStmtContainingAnonymousTypeInitializer, 1659, "Statement containing an anonymous type initializer was changed.")
STRID(ENC_AddedStmtContainingAnonymousTypeInitializer, 1660, "Statement containing an anonymous type initializer was added.")
STRID(ENC_DeletedStmtContainingAnonymousTypeInitializer, 1661, "Statement containing an anonymous type initializer was deleted.")
STRID(ENC_ChangedStmtContainingQueryExpression, 1662, "Statement containing a query expression was changed.")
STRID(ENC_AddedStmtContainingQueryExpression, 1663, "Statement containing a query expression was added.")
STRID(ENC_DeletedStmtContainingQueryExpression, 1664, "Statement containing a query expression was deleted.")
STRID(ENC_ChangedLocalUsedInStmtContainingAnonymousTypeInitializer, 1665, "Variable '|1' was changed, which is referenced in a statement containing an anonymous type initialization.")
STRID(ENC_ChangedLocalUsedInStmtContainingQuery, 1666, "Variable '|1' was changed, which is referenced in a statement containing a query expression.")
STRID(ENC_XmlImportChange, 1667, "Xml import directive was added or removed.")
STRID(ENC_XmlImportAdd, 1668, "Xml import directive was added.")
STRID(ENC_XmlImportDelete, 1669, "Xml import directive was deleted.")
STRID(ENC_ChangedStmtContainingLambdaExpression, 1670, "Statement containing a lambda expression was changed.")
STRID(ENC_AddedStmtContainingLambdaExpression, 1671, "Statement containing a lambda expression was added.")
STRID(ENC_DeletedStmtContainingLambdaExpression, 1672, "Statement containing a lambda expression was deleted.")
STRID(ENC_ChangedLocalUsedInStmtContainingLambdaExpression, 1673, "Variable '|1' was changed, which is referenced in a statement containing a lambda expression.")
STRID(ENC_PartialMethodImplementation, 1674, "A partial method implementation was added for an existing declaration.")
STRID(ENC_PartialMethodDeclaration, 1675, "A partial method declaration was added for an existing implementation.")
STRID(ENC_Nullable, 1676, "nullable")
STRID(ENC_NonNullable, 1677, "non-nullable")
STRID(ENC_ChangedStatementInsideOfStatementLambda, 1678, "Statement inside of a statement lambda was changed and/or moved.")
STRID(ENC_AddedStatementInsideOfStatementLambda, 1679, "Statement inside of a statement lambda was added: '|1'.")
STRID(ENC_DeletedStatementInsideOfStatementLambda, 1680, "Statement inside of a statement lambda was deleted: '|1'.")
STRID(ENC_ChangedStatementLambda, 1681, "Statement lambda was changed.")
STRID(ENC_GenericParameterVarianceChanged, 1682, "Generic parameter variance kind was changed.")
STRID(ENC_ChangedStatementInsideOfResumable, 1683, "Statement inside of an Async or Iterator method was changed and/or moved.")
STRID(ENC_AddedStatementInsideOfResumable, 1684, "Statement inside of an Async or Iterator method was added: '|1'.")
STRID(ENC_DeletedStatementInsideOfResumable, 1685, "Statement inside of an Async or Iterator method was deleted: '|1'.")

// VB Profile
STRID(PROFILE_FriendlyName         ,1700   , "Visual Basic Development Settings")
STRID(PROFILE_Description          ,1701   , "Optimizes the environment so you can focus on building world-class applications.  This collection of settings contains customizations to the window layout, command menus and keyboard shortcuts to make common Visual Basic commands more accessible.")
STRID(PROFILE_ExpressFriendlyName  ,1702   , "Visual Basic Express Development Settings")
STRID(PROFILE_ExpressDescription   ,1703   , "Optimizes the environment so you can focus on building world-class applications.  This collection of settings contains customizations to the window layout, command menus and keyboard shortcuts to make common Visual Basic commands more accessible.")

//Used by the auto error correction engine as error fix description
//9000 - 9900 is reserved range for this feature.
STRID(ECD_Replace1With2                                     ,9001   ,"Replace '|1' with '|2'.")
STRID(ECD_Change1To2                                        ,9002   ,"Change '|1' to '|2'.")
STRID(ECD_Delete1Statement                                  ,9003   ,"Delete the '|1' statement.")
STRID(ECD_DeleteTypeChar                                    ,9004   ,"Delete the type character(s) '|1'.")
STRID(ECD_Delete                                            ,9005   ,"Delete '|1'.")
STRID(ECD_Change1ToListItem                                 ,9006   ,"Change '|1' to '|DropDown'.")
STRID(ECD_SymbolFromUnreferencedProject3                    ,9007   ,"Add reference '|1' to project '|2'.")
STRID(ECD_MoveStatementToLine                               ,9008   ,"Move the '|1' statement to line |2.")
STRID(ECD_Insert1AtEndOf2                                   ,9009   ,"Insert the '|1' statement at the end of '|2'.")
STRID(ECD_Insert1After2                                     ,9010   ,"Insert '|1' after '|2'.")
STRID(ECD_InitWithMultipleDeclarators                       ,9011   ,"Replace variable declaration to be individual variables declared and initialized on each line.")
STRID(ECD_Insert1At2                                        ,9012   ,"Insert '|1' at '|2'.")
STRID(ECD_InsertListItem                                    ,9013   ,"Insert '|DropDown'.")
STRID(ECD_InsertMissing                                     ,9014   ,"Insert the missing '|1'.")
STRID(ECD_InsertTwoMissing                                  ,9015   ,"Insert the missing '|1' and '|2'.")
STRID(ECD_DeleteDeclarator                                  ,9016   ,"Delete the '|1' declarator.")
STRID(ECD_DeleteSpecifier                                   ,9017   ,"Delete the '|1' specifier.")
STRID(ECD_InsertSpecifier                                   ,9018   ,"Insert the '|1' specifier.")
STRID(ECD_RemoveClause                                      ,9019   ,"Remove the '|1' clause.")
STRID(ECD_MoveFirstMethodBodyStatementToNextLine            ,9020   ,"Move the first method body statement to the next line.")
STRID(ECD_MoveStatementToNextLine                           ,9021   ,"Move the '|1' statement to the next line.")
STRID(ECD_DeleteDuplicateSpecifier                          ,9022   ,"Delete the duplicate '|1' specifier(s).")
STRID(ECD_Prepend1To2                                       ,9023   ,"Prepend '|1' to '|2'.")
STRID(ECD_RemoveMethod                                      ,9024   ,"Remove the '|1' method.")
STRID(ECD_DeleteAccessModifiers                             ,9025   ,"Remove the access modifiers on '|1'.")
STRID(ECD_MoveStatementToMainClass                          ,9026   ,"Move the '|1' statement to the main |2 '|3'.")
STRID(ECD_InsertMissingQuote                                ,9027   ,"Insert the missing double quote.")
STRID(ECD_DeleteTwoSpecifiers                               ,9028   ,"Delete the '|1' and '|2' specifiers.")
STRID(ECD_DeleteSpecifierAndMethod                          ,9029   ,"Delete the '|1' specifier and the '|2' method.")
STRID(ECD_Replace1With2OnMembers                            ,9030   ,"Replace '|1' with '|2' on the following member(s):")
STRID(ECD_DeleteSpecifiersAndAttributes                     ,9031   ,"Delete the specifers and attributes.")
STRID(ECD_RemoveAllParametersOnSubNew                       ,9032   ,"Remove all the parameters of 'Sub New'.")
STRID(ECD_MoveConstructorCallToLine                         ,9033   ,"Move the constructor call to line |1.")
STRID(ECD_ChangeIntoSub                                     ,9034   ,"Change the method declaration to a 'Sub'.")
STRID(ECD_RemoveExtraReferences                             ,9035   ,"Remove the extra references to '|1'.")
STRID(ECD_InsertMissingGetSet                               ,9036   ,"Insert the missing 'Get' and 'Set'.")
STRID(ECD_ChangeGetToSet                                    ,9037   ,"Change the 'Get' to a 'Set'.")
STRID(ECD_RemoveAndInsertMissing                            ,9038   ,"Remove '|1' and insert the missing '|2'.")
STRID(ECD_DeleteOptionalSpecifierAndDefaultValue            ,9039   ,"Delete the 'Optional' specifier and the default value.")
STRID(ECD_DeleteAppliedAttribute                            ,9040   ,"Delete the applied '|1' attribute.")
STRID(ECD_DeleteAppliedAttributes                           ,9041   ,"Delete the applied attributes.")
STRID(ECD_Delete2AppliedAttributes                          ,9042   ,"Delete the applied '|1' and '|2' attributes.")
STRID(ECD_RemoveImports                                     ,9043   ,"Remove the imports for '|1'.")
STRID(ECD_ChangeAccessOfMember                              ,9044   ,"Change the member's access to '|1'.")
STRID(ECD_MoveCaseBeforeCaseElse                            ,9045   ,"Move the 'Case' before the 'Case Else' statement.")
STRID(ECD_MoveCatchBeforeFinally                            ,9046   ,"Move the 'Catch' before the 'Finally' statement.")
STRID(ECD_ChangeMemberType                                  ,9047   ,"Change the member's type to '|1'.")
STRID(ECD_DeleteAccessModifiersOnProperty                   ,9048   ,"Remove the access modifiers on property '|1'.")
STRID(ECD_ChangePropertyAndSetParamType                     ,9049   ,"Change the property and 'Set' parameter type to '|DropDown'.")
STRID(ECD_ChangeClassBase                                   ,9050   ,"Change class '|1' to inherit from '|2'.")
STRID(ECD_ImportListItem                                    ,9051   ,"Import '|DropDown'.")

// strings for the Add Imports Dialog
STRID(AID_MainDialogText                                    ,9052   ,"Importing '|1' would change the meaning of other identifiers in this file.  To correct '|2' and preserve the meaning of other identifiers, choose one of the following options:")
STRID(AID_ImportOption                                      ,9053   ,"&Import '|1', and qualify the affected identifiers")
STRID(AID_QualifyOption                                     ,9054   ,"&Do not import '|2', but change '|1' to '|2.|1'");
STRID(AID_ReentrancyMessage                                 ,9055   ,"Auto correction has encountered and error.  Try the correction again.")
STRID(AID_MainExtensionDialogText                           ,9056   ,"Importing '|1' would change the meaning of other identifiers in this file, including extension methods.")
STRID(AID_SecondaryExtensionDialogText                      ,9057   ,"Because importing '|1' in this case would lead to errors, would you like to change '|2' to '|1.|2' instead? This will help correct '|2' while still preserving the meaning of other identifiers in the file.")

STRID(EC_No_Corrections                 ,9898   ,"(no correction suggestions)")
STRID(EC_Two_Sentences                  ,9899   ,"|1  |2")
STRID(EC_Edit_Description               ,9900   ,"Visual Basic Error Correction")
STRID(EC_Options                        ,9901   ,"Error Correction Options")
STRID(EC_ExpandAllPreviews              ,9902   ,"&Expand All Previews")
STRID(EC_ExpandPreview                  ,9903   ,"Expand Preview")
STRID(EC_CollapsePreview                ,9904   ,"Collapse Preview")
STRID(EC_ApplyFix                       ,9905   ,"Apply Fix")

//Used by the visual basic expression evaluator as error strings
//9950 - 10100 is reserved range for this. Please see vbee\resource.h
STRID(EEERR_Proxy                   ,9950   , "Cannot evaluate field of a proxy object.")
STRID(EEERR_CorInOptimizedCode      ,9951   , "Cannot evaluate expression because the code of the current method is optimized.")
STRID(EEERR_IndexProperty           ,9952   , "In order to evaluate an indexed property, the property must be qualified and the arguments must be explicitly supplied by the user.")
STRID(EEERR_Disable                 ,9953   , "Property evaluation is disabled in debugger windows. Check your settings in Tools.Options.Debugging.General.")
STRID(EEERR_Exception               ,9954   , "Exception thrown during property evaluation.")
STRID(EEERR_Exception1              ,9955   , "Exception of type: '|1' occurred.")
STRID(EEERR_CorInGCUnsafePoint      ,9956   , "Cannot evaluate expression because we are stopped in a place where garbage collection is impossible, possibly because the code of the current method may be optimized.")
STRID(EEERR_PropEvalFailed          ,9957   , "Property evaluation failed.")
STRID(EEERR_CorInNativeCode         ,9958   , "Cannot evaluate expression because we are stopped in native code.")

//Used by the EE for UI user messages
//10101 - 10150 is reserved for this
STRID(EEMSG_ResultsNode             ,10103  , "Expanding will process the collection")
STRID(EEMSG_EnumerableProxyName     ,10105  , "Results")
STRID(EEMSG_QueryTypeInMemory       ,10106  , "In-Memory Query")
STRID(EEMSG_QueryTypeDatabase       ,10107  , "Database Query")
STRID(EEMSG_QueryTypeXml            ,10108  , "Xml Query")
STRID(EEMSG_RefreshRootResults      ,10109  , "Refresh only supported on top level Results node");

STRID(REF_ReferenceToWrongTargetType    ,10151  , "Reference to project '|1' cannot be added because it targets a different version of the .NET Framework.")
STRID(SCC_RELOADED_FILES            ,10152   , "The operation was aborted because one or more files were reloaded during the check out process. Try the operation again.")

// Used by the Background Compilation Wait Dialog
STRID(BC_WAIT_Processing            ,10153   , "Processing project |1 of |2 : |3")
STRID(REF_CircularReference         ,10154  , "Reference to project '|1' cannot be added. Adding this project as a reference would cause a circular dependency.")

// Used by the Wait Dialog when searching for references to a symbol.
STRID(SEARCH_WAIT_SearchText        ,10155   , "Scanning projects for references to '|1'")
STRID(SEARCH_WAIT_Processing        ,10156   , "Processing file |1 of |2 : |3")

// Used when cancelling retargeting (VSP will add 'attempt to retargeting failed')
STRID(RetargetingCanceledDueToMissingAssembly  ,10158   , "'|1' does not contain |2, which is required for Visual Basic projects.")

STRID(InitializingLanguageService  ,10159   , "Initializing Visual Basic...")

// Used by Quick Access keyword
STRID(TextEditor_Basic_VBSpecific_Keyword   ,10160   , "Automatic insertion of end constructs;Change pretty listing settings;Change outlining mode;Automatic insertion of Interface and MustOverride members;Show or hide procedure line separators;Turn error correction suggestions on or off;Turn highlighting of references and keywords on or off")

