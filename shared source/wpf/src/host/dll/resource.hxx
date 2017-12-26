//+-----------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//     Resource defines
//
//  History:
//     06/19/2002-murrayw
//          Created
//     2007/09/20-[....]
//          Ported Windows->DevDiv. See SourcesHistory.txt.
//
//------------------------------------------------------------------------

#pragma once 

#include "..\ShimImpl\Resource.hxx"

#define MENU_XAPP                       2000

#define IDR_FIRSTMENU                   0x3000 
#define IDR_EDITMENU                    IDR_FIRSTMENU + 0x020
#define IDR_VIEWMENU                    IDR_EDITMENU + 0x20

//
//IMPORTANT: IMPORTANT: IMPORTANT
//Start with 8001, the enum defined on the managed world starts with 8001 as well
//KEEP THESE IN [....]
//The ApplicationCommands enums in wcp\host\inc\hostservices.idl and IBrowserHostServices.cs 
//and the menuIDs wcp\host\docobj\resource.hxx and resources.rc
//
#define IDM_MENU_FIRST                  8001

#define IDM_EDIT_CUT                                (IDM_MENU_FIRST)
#define IDM_EDIT_COPY                               (IDM_EDIT_CUT+1)
#define IDM_EDIT_PASTE                              (IDM_EDIT_COPY+1)
#define IDM_EDIT_SELECTALL                          (IDM_EDIT_PASTE+1)
#define IDM_EDIT_FIND                               (IDM_EDIT_SELECTALL+1)

#define IDM_EDIT_DIGITALSIGNATURES                  (IDM_EDIT_FIND+1)
#define IDM_EDIT_DIGITALSIGNATURES_SIGNDOCUMENT     (IDM_EDIT_DIGITALSIGNATURES+1)
#define IDM_EDIT_DIGITALSIGNATURES_REQUESTSIGNATURE (IDM_EDIT_DIGITALSIGNATURES_SIGNDOCUMENT+1)
#define IDM_EDIT_DIGITALSIGNATURES_VIEWSIGNATURE    (IDM_EDIT_DIGITALSIGNATURES_REQUESTSIGNATURE+1)

#define IDM_EDIT_PERMISSION                         (IDM_EDIT_DIGITALSIGNATURES_VIEWSIGNATURE+1)
#define IDM_EDIT_PERMISSION_SET                     (IDM_EDIT_PERMISSION+1)
#define IDM_EDIT_PERMISSION_VIEW                    (IDM_EDIT_PERMISSION_SET+1)
#define IDM_EDIT_PERMISSION_RESTRICT                (IDM_EDIT_PERMISSION_VIEW+1)

#define IDM_VIEW_STATUSBAR                          (IDM_EDIT_PERMISSION_RESTRICT+1)
#define IDM_VIEW_STOP                               (IDM_VIEW_STATUSBAR+1)
#define IDM_VIEW_REFRESH                            (IDM_VIEW_STOP+1)
#define IDM_VIEW_FULLSCREEN                         (IDM_VIEW_REFRESH+1)

#define IDM_VIEW_ZOOM                               (IDM_VIEW_FULLSCREEN+1)
#define IDM_VIEW_ZOOM_IN                            (IDM_VIEW_ZOOM+1)
#define IDM_VIEW_ZOOM_OUT                           (IDM_VIEW_ZOOM_IN+1)
#define IDM_VIEW_ZOOM_400                           (IDM_VIEW_ZOOM_OUT+1)
#define IDM_VIEW_ZOOM_250                           (IDM_VIEW_ZOOM_400+1)
#define IDM_VIEW_ZOOM_150                           (IDM_VIEW_ZOOM_250+1)
#define IDM_VIEW_ZOOM_100                           (IDM_VIEW_ZOOM_150+1)
#define IDM_VIEW_ZOOM_75                            (IDM_VIEW_ZOOM_100+1)
#define IDM_VIEW_ZOOM_50                            (IDM_VIEW_ZOOM_75+1)
#define IDM_VIEW_ZOOM_25                            (IDM_VIEW_ZOOM_50+1)
#define IDM_VIEW_ZOOM_PAGEWIDTH                     (IDM_VIEW_ZOOM_25+1)
#define IDM_VIEW_ZOOM_WHOLEPAGE                     (IDM_VIEW_ZOOM_PAGEWIDTH+1)
#define IDM_VIEW_ZOOM_TWOPAGES                      (IDM_VIEW_ZOOM_WHOLEPAGE+1)
#define IDM_VIEW_ZOOM_THUMBNAILS                    (IDM_VIEW_ZOOM_TWOPAGES+1)

#define IDM_MENU_LAST                               (IDM_VIEW_ZOOM_THUMBNAILS+1)


///////////////////////////////////////////////////////////////
// Strings

#define IDS_APP_TYPE_DISABLED   100
