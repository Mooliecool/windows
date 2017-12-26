//******************************************************************************
//
// File: guids.hxx
//
// Avalon Guids     
//
// History:
//
//     10/15/2002: murrayw:       Created
//     10/26/2002: lgolding:      Replace multiple GUIDs with a single GUID
//                                 and a set of flags that can be OR'd into it.
//     2007/09/20: [....]        Ported Windows->DevDiv. See SourcesHistory.txt.
//
// Copyright (C) 2002 by Microsoft Corporation.  All rights reserved.
// 
//******************************************************************************



//******************************************************************************
//
// NOTE: This file has been copied from the Windows/SP1 enlistment.  If you make any edits
//       to this file be sure they are reflected in windows\wcp\Host\inc\guids.hxx
// 
//******************************************************************************



#ifndef GUIDS_HXX
#define GUIDS_HXX

// MIME-type Handler GUIDS
// These GUIDs are invoked by shdocvw to create the appropriate docobj.
// We have more than one GUID because each MIME-type should get its own
// ProgId, and each ProgId should have its own ClsId. It doesn't really
// matter if the same object is always created. These GUIDs are also used
// in journaling.

// This is the MIME-type handler for .xps
//  {7dda204b-2097-47c9-8323-c40bb840ae44}
DEFINE_GUID(CLSID_DocObjPackage, 
            0x7dda204b, 
            0x2097, 
            0x47c9, 
            0x83, 
            0x23, 
            0xc4, 
            0x0b, 
            0xb8, 
            0x40, 
            0xae, 
            0x44);

// This is the MIME-type handler for Xapp
// {adbe6dec-9b04-4a3d-a09c-4bb38ef1351c}
DEFINE_GUID(CLSID_DocObjXapp,
            0xadbe6dec,
            0x9b04,
            0x4a3d,
            0xa0,
            0x9c,
            0x4b,
            0xb3,
            0x8e,
            0xf1,
            0x35,
            0x1c);

// This is the MIME-type handler for Xaml
// {CF1BF3B6-7AD0-4410-996B-C78EAFCD3269}
DEFINE_GUID(CLSID_DocObjXaml, 
            0xcf1bf3b6, 
            0x7ad0, 
            0x4410, 
            0x99, 
            0x6b, 
            0xc7, 
            0x8e, 
            0xaf, 
            0xcd, 
            0x32, 
            0x69);


// {4B181F0F-48C8-4e80-A2AF-E3099AAC069B}
DEFINE_GUID(CLSID_PresentationHostInprocHandler, 
0x4b181f0f, 0x48c8, 0x4e80, 0xa2, 0xaf, 0xe3, 0x9, 0x9a, 0xac, 0x6, 0x9b);

// {94DAF911-D097-4c53-A185-75C9C1004237}
DEFINE_GUID(SID_XXWebOCHost, 
0x94daf911, 0xd097, 0x4c53, 0xa1, 0x85, 0x75, 0xc9, 0xc1, 0x0, 0x42, 0x37);

#endif // GUIDS_HXX
