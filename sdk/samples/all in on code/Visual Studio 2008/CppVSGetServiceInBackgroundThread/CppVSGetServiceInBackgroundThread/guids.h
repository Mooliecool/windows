// guids.h: definitions of GUIDs/IIDs/CLSIDs used in this VsPackage

/*
Do not use #pragma once, as this file needs to be included twice.  Once to declare the externs
for the GUIDs, and again right after including initguid.h to actually define the GUIDs.
*/



// package guid
// { 6b328311-b5ff-4361-9393-a4b8d800ba90 }
#define guidCppVSGetServiceInBackgroundThreadPkg { 0x6B328311, 0xB5FF, 0x4361, { 0x93, 0x93, 0xA4, 0xB8, 0xD8, 0x0, 0xBA, 0x90 } }
#ifdef DEFINE_GUID
DEFINE_GUID(CLSID_CppVSGetServiceInBackgroundThread,
0x6B328311, 0xB5FF, 0x4361, 0x93, 0x93, 0xA4, 0xB8, 0xD8, 0x0, 0xBA, 0x90 );
#endif

// Command set guid for our commands (used with IOleCommandTarget)
// { 44183908-37c3-4378-9193-f100f090c085 }
#define guidCppVSGetServiceInBackgroundThreadCmdSet { 0x44183908, 0x37C3, 0x4378, { 0x91, 0x93, 0xF1, 0x0, 0xF0, 0x90, 0xC0, 0x85 } }
#ifdef DEFINE_GUID
DEFINE_GUID(CLSID_CppVSGetServiceInBackgroundThreadCmdSet, 
0x44183908, 0x37C3, 0x4378, 0x91, 0x93, 0xF1, 0x0, 0xF0, 0x90, 0xC0, 0x85 );
#endif

//Guid for the image list referenced in the VSCT file
// { e6f39abb-af22-406a-a3e0-f26ed2a3bea4 }
#define guidImages { 0xE6F39ABB, 0xAF22, 0x406A, { 0xA3, 0xE0, 0xF2, 0x6E, 0xD2, 0xA3, 0xBE, 0xA4 } }
#ifdef DEFINE_GUID
DEFINE_GUID(CLSID_Images, 
0xE6F39ABB, 0xAF22, 0x406A, 0xA3, 0xE0, 0xF2, 0x6E, 0xD2, 0xA3, 0xBE, 0xA4 );
#endif


