// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==
BOOL CreateGUI();
void GUISetModule(__in __nullterminated char *pszModule);
void GUIMainLoop();
void GUIAddOpcode(__inout_opt __nullterminated char *szString, __in_opt void *GUICookie);
BOOL GUIAddItemsToList();
void GUIAddOpcode(__inout __nullterminated char *szString);
void DestroyGUI();
UINT GetDasmMBRTLStyle();

BOOL DisassembleMemberByName(__in __nullterminated char *pszClassName, __in __nullterminated char *pszMemberName, __in __nullterminated char *pszSig);
BOOL IsGuiILOnly();
