/****************************** Module Header ******************************\
Module Name:  SD.cpp
Project:      CppCheckSD
Copyright (c) Microsoft Corporation

This module contains functions to obtain a variety of different components 
contained in a Security Descriptor.  This includes an owner (SID), a 
primary group (SID), a discretionary ACL (DACL), and a system ACL (SACL).

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "CheckSD.h"
#include <stdio.h>
#include <lmcons.h>
#include <sddl.h>
#include <strsafe.h>


//
//   FUNCTION: DumpAclInfo(PACL, BOOL)
//
//   PURPOSE: Obtains a variety of information on an ACL applies to
//   both DACLs and SACLs
//   CppCheckSD.exe
//
//   PARAMETERS:
//   * pacl  - pointer to the ACL
//   * bDacl - TRUE indicates a DACL while FALSE indicates a SACL
//
//   RETURN VALUE: The function returns a zero if there was a failure
///  or if it succeeds the size of the ACL specified in the first param.
//
DWORD DumpAclInfo(PACL pacl, BOOL bDacl)
{
    BYTE pByte[2][12];

    // Is the acl valid
    wprintf(L"\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    if (bDacl)
		wprintf(L">>                 DACL INFORMATION                    >>\n");
    else
		wprintf(L">>                 SACL INFORMATION                    >>\n");

    wprintf(L">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n");
    wprintf(L"valid .............. ");

	if (!IsValidAcl(pacl))
	{
		wprintf(L"no\n");
		return 0;
	}
	else
		wprintf(L"yes\n");

    // i represents:
    // typedef enum _ACL_INFORMATION_CLASS {
    //              AclRevisionInformation   = 1,
    //              AclSizeInformation 
    //              ACL_INFORMATION_CLASS;}

	for (int i = 1; i < 3; i++)
	{
		if (!GetAclInformation(pacl, (LPVOID)pByte[i-1], sizeof(ACL_SIZE_INFORMATION), (ACL_INFORMATION_CLASS)i))
			DisplayError(GetLastError(), L"GetAclInformation");
	}

    wprintf(L"revision ........... %u\n\n", *((PACL_REVISION_INFORMATION)pByte[0]));
    wprintf(L"ace count .......... %u\n", ((PACL_SIZE_INFORMATION)pByte[1])->AceCount);
    wprintf(L"acl bytes in use ... %u byte(s)\n", ((PACL_SIZE_INFORMATION)pByte[1])->AclBytesInUse);
    wprintf(L"acl bytes free ..... %u byte(s)\n", ((PACL_SIZE_INFORMATION)pByte[1])->AclBytesFree);

    return ((PACL_SIZE_INFORMATION)pByte[1])->AceCount;
}

//
//   FUNCTION: DumpControl(PSECURITY_DESCRIPTOR)
//
//   PURPOSE: Returns control information which describes the format
//   of the security descriptor, absolute or relative, whether a
//   DACL and/or SACL is present and whether they were set using
//   a default mechanism 
//   CppCheckSD.exe
//
//   PARAMETERS:
//   * psd - pointer to a SECURITY_DESCRIPTOR
//
//   RETURN VALUE: The function returns one of the following values:
//    0 = bDacl and bSacl not present
//    1 = bDacl present and bSacl NOT present
//    2 = bDacl no present and bSacl present
//    3 = bDacl and bSacl present
//
DWORD DumpControl(PSECURITY_DESCRIPTOR psd)
{
	BOOL                        bDaclPresent = FALSE;
	BOOL                        bSaclPresent = FALSE;
    DWORD                       dwRevision;
	SECURITY_DESCRIPTOR_CONTROL sdc;

	if (!GetSecurityDescriptorControl(psd, &sdc, &dwRevision))
		DisplayError(GetLastError(), L"GetSecurityDescriptorControl");

	wprintf(L"revision ........... %u\n", dwRevision);
    wprintf(L"control bits ....... 0x%X\n", sdc);

	if ((sdc & SE_OWNER_DEFAULTED) == SE_OWNER_DEFAULTED)
		wprintf(L".................... SE_OWNER_DEFAULTED: 0x%X\n", SE_OWNER_DEFAULTED);
	if ((sdc & SE_GROUP_DEFAULTED) == SE_GROUP_DEFAULTED)
		wprintf(L".................... SE_GROUP_DEFAULTED: 0x%X\n", SE_GROUP_DEFAULTED);	
	if ((sdc & SE_DACL_PRESENT) == SE_DACL_PRESENT)
	{
		wprintf(L".................... SE_DACL_PRESENT: 0x%X\n", SE_DACL_PRESENT);
		bDaclPresent = TRUE;
	}
    if ((sdc & SE_DACL_DEFAULTED) == SE_DACL_DEFAULTED)
		wprintf(L".................... SE_DACL_DEFAULTED: 0x%X\n", SE_DACL_DEFAULTED);
	if ((sdc & SE_SACL_PRESENT) == SE_SACL_PRESENT)
	{
		wprintf(L".................... SE_SACL_PRESENT: 0x%X\n", SE_SACL_PRESENT);
		bSaclPresent = TRUE;
	}
    if ((sdc & SE_SACL_DEFAULTED) == SE_SACL_DEFAULTED)
		wprintf(L".................... SE_SACL_DEFAULTED: 0x%X\n", SE_SACL_DEFAULTED);
	if ((sdc & SE_DACL_AUTO_INHERIT_REQ) == SE_DACL_AUTO_INHERIT_REQ)
         wprintf(L".................... SE_DACL_AUTO_INHERIT_REQ: 0x%X\n", SE_DACL_AUTO_INHERIT_REQ);
    if ((sdc & SE_SACL_AUTO_INHERIT_REQ) == SE_SACL_AUTO_INHERIT_REQ) 
         wprintf(L".................... SE_SACL_AUTO_INHERIT_REQ: 0x%X\n", SE_SACL_AUTO_INHERIT_REQ);
	if ((sdc & SE_DACL_AUTO_INHERITED) == SE_DACL_AUTO_INHERITED) // NT5.0
         wprintf(L".................... SE_DACL_AUTO_INHERITED: 0x%X\n", SE_DACL_AUTO_INHERITED);
    if ((sdc & SE_SACL_AUTO_INHERITED) == SE_SACL_AUTO_INHERITED) // NT5.0
         wprintf(L".................... SE_SACL_AUTO_INHERITED: 0x%X\n", SE_SACL_AUTO_INHERITED);
	if ((sdc & SE_DACL_PROTECTED) == SE_DACL_PROTECTED) // NT5.0
         wprintf(L".................... SE_DACL_PROTECTED: 0x%X\n", SE_DACL_PROTECTED);
	if ((sdc & SE_SACL_PROTECTED) == SE_SACL_PROTECTED) // NT5.0
         wprintf(L".................... SE_SACL_PROTECTED: 0x%X\n", SE_SACL_PROTECTED);
	if ((sdc & SE_RM_CONTROL_VALID) == SE_RM_CONTROL_VALID)
         wprintf(L".................... SE_RM_CONTROL_VALID: 0x%X\n", SE_RM_CONTROL_VALID);
    if ((sdc & SE_SELF_RELATIVE) == SE_SELF_RELATIVE)
         wprintf(L".................... SE_SELF_RELATIVE: 0x%X\n", SE_SELF_RELATIVE);

	//
	// 0 = bDacl and bSacl not present
	// 1 = bDacl present and bSacl NOT present
	// 2 = bDacl no present and bSacl present
	// 3 = bDacl and bSacl present
	//
	if (bDaclPresent && bSaclPresent)
		return 3;

	if (!bDaclPresent && !bSaclPresent)
		return 0;

	if (bDaclPresent && !bSaclPresent)
		return 1;

	if (!bDaclPresent && bSaclPresent)
		return 2;

	return 0;
}

//
//   FUNCTION: PrintAccessRights(ACCESS_ALLOWED_ACE, WCHAR, BOOL)
//
//   PURPOSE: Displays Access Rights for the specific object
//   CppCheckSD.exe
//
//   PARAMETERS:
//   * pace  - ACCESS_ALLOWED_ACE
//   * c     - type of kernel object
//   * bDacl - TRUE indicates the ACE passed in is part of a DACL instead 
//             of a SACL
//
//   RETURN VALUE: none
//
void PrintAccessRights(ACCESS_ALLOWED_ACE *pace, WCHAR c, BOOL bDacl)
{
	if (bDacl)
	{
		switch (c)
		{
			case 'a': // mailslot
			case 'n': // named pipe
				if ((pace->Mask & FILE_READ_DATA) == FILE_READ_DATA)
					wprintf(L".................... FILE_READ_DATA: 0x%X\n", FILE_READ_DATA);
				if ((pace->Mask & FILE_WRITE_DATA) == FILE_WRITE_DATA)
					wprintf(L".................... FILE_WRITE_DATA: 0x%X\n", FILE_WRITE_DATA);
				if ((pace->Mask & FILE_CREATE_PIPE_INSTANCE) == FILE_CREATE_PIPE_INSTANCE)
					wprintf(L".................... FILE_CREATE_PIPE_INSTANCE: 0x%X\n", FILE_CREATE_PIPE_INSTANCE);
				if ((pace->Mask & FILE_READ_ATTRIBUTES) == FILE_READ_ATTRIBUTES)
					wprintf(L".................... FILE_READ_ATTRIBUTES: 0x%X\n", FILE_READ_ATTRIBUTES);
				if ((pace->Mask & FILE_WRITE_ATTRIBUTES) == FILE_WRITE_ATTRIBUTES)
					wprintf(L".................... FILE_WRITE_ATTRIBUTES: 0x%X\n", FILE_WRITE_ATTRIBUTES);
				break;

			case 'c': // service control manager
				if ((pace->Mask & SC_MANAGER_CONNECT) == SC_MANAGER_CONNECT)
					wprintf(L".................... SC_MANAGER_CONNECT: 0x%X\n", SC_MANAGER_CONNECT);
				if ((pace->Mask & SC_MANAGER_CREATE_SERVICE) == SC_MANAGER_CREATE_SERVICE)
					wprintf(L".................... SC_MANAGER_CREATE_SERVICE: 0x%X\n", SC_MANAGER_CREATE_SERVICE);
				if ((pace->Mask & SC_MANAGER_ENUMERATE_SERVICE) == SC_MANAGER_ENUMERATE_SERVICE)
					wprintf(L".................... SC_MANAGER_ENUMERATE_SERVICE: 0x%X\n", SC_MANAGER_ENUMERATE_SERVICE);
				if ((pace->Mask & SC_MANAGER_LOCK) == SC_MANAGER_LOCK)
					wprintf(L".................... SC_MANAGER_LOCK: 0x%X\n", SC_MANAGER_LOCK);
				if ((pace->Mask & SC_MANAGER_QUERY_LOCK_STATUS) == SC_MANAGER_QUERY_LOCK_STATUS)
					wprintf(L".................... SC_MANAGER_QUERY_LOCK_STATUS: 0x%X\n", SC_MANAGER_QUERY_LOCK_STATUS);
				if ((pace->Mask & SC_MANAGER_MODIFY_BOOT_CONFIG) == SC_MANAGER_MODIFY_BOOT_CONFIG)
					wprintf(L".................... SC_MANAGER_MODIFY_BOOT_CONFIG: 0x%X\n", SC_MANAGER_MODIFY_BOOT_CONFIG);
				if ((pace->Mask & SC_MANAGER_ALL_ACCESS) == SC_MANAGER_ALL_ACCESS)
					wprintf(L".................... SC_MANAGER_ALL_ACCESS: 0x%X\n", SC_MANAGER_ALL_ACCESS);
				break;

			case 'd': // directory
				if ((pace->Mask & FILE_LIST_DIRECTORY) == FILE_LIST_DIRECTORY)
					wprintf(L".................... FILE_LIST_DIRECTORY: 0x%X\n", FILE_LIST_DIRECTORY);
				if ((pace->Mask & FILE_ADD_FILE) == FILE_ADD_FILE)
					wprintf(L".................... FILE_ADD_FILE: 0x%X\n", FILE_ADD_FILE);
				if ((pace->Mask & FILE_ADD_SUBDIRECTORY) == FILE_ADD_SUBDIRECTORY)
					wprintf(L".................... FILE_ADD_SUBDIRECTORY: 0x%X\n", FILE_ADD_SUBDIRECTORY);
				if ((pace->Mask & FILE_READ_EA) == FILE_READ_EA)
					wprintf(L".................... FILE_READ_EA: 0x%X\n", FILE_READ_EA);
				if ((pace->Mask & FILE_WRITE_EA) == FILE_WRITE_EA)
					wprintf(L".................... FILE_WRITE_EA: 0x%X\n", FILE_WRITE_EA);
				if ((pace->Mask & FILE_TRAVERSE) == FILE_TRAVERSE)
					wprintf(L".................... FILE_TRAVERSE: 0x%X\n", FILE_TRAVERSE);
				if ((pace->Mask & FILE_DELETE_CHILD) == FILE_DELETE_CHILD)
					wprintf(L".................... FILE_DELETE_CHILD: 0x%X\n", FILE_DELETE_CHILD);
				if ((pace->Mask & FILE_READ_ATTRIBUTES) == FILE_READ_ATTRIBUTES)
					wprintf(L".................... FILE_READ_ATTRIBUTES: 0x%X\n", FILE_READ_ATTRIBUTES);
				if ((pace->Mask & FILE_WRITE_ATTRIBUTES) == FILE_WRITE_ATTRIBUTES)
					wprintf(L".................... FILE_WRITE_ATTRIBUTES: 0x%X\n", FILE_WRITE_ATTRIBUTES);
				break;

			case 'e': // event
				if ((pace->Mask & EVENT_MODIFY_STATE) == EVENT_MODIFY_STATE)
					wprintf(L".................... EVENT_MODIFY_STATE: 0x%X\n", EVENT_MODIFY_STATE);
				if ((pace->Mask & EVENT_ALL_ACCESS) == EVENT_ALL_ACCESS)
					wprintf(L".................... EVENT_ALL_ACCESS: 0x%X\n", EVENT_ALL_ACCESS);
				break;

			case 'f': // file
				if ((pace->Mask & FILE_READ_DATA) == FILE_READ_DATA)
					wprintf(L".................... FILE_READ_DATA: 0x%X\n", FILE_READ_DATA);
				if ((pace->Mask & FILE_WRITE_DATA) == FILE_WRITE_DATA)
					wprintf(L".................... FILE_WRITE_DATA: 0x%X\n", FILE_WRITE_DATA);;
				if ((pace->Mask & FILE_APPEND_DATA) == FILE_APPEND_DATA)
					wprintf(L".................... FILE_APPEND_DATA: 0x%X\n", FILE_APPEND_DATA);
				if ((pace->Mask & FILE_READ_EA) == FILE_READ_EA)
					wprintf(L".................... FILE_READ_EA: 0x%X\n", FILE_READ_EA);
				if ((pace->Mask & FILE_WRITE_EA) == FILE_WRITE_EA)
					wprintf(L".................... FILE_WRITE_EA: 0x%X\n", FILE_WRITE_EA);
				if ((pace->Mask & FILE_EXECUTE) == FILE_EXECUTE)
					wprintf(L".................... FILE_EXECUTE: 0x%X\n", FILE_EXECUTE);
				if ((pace->Mask & FILE_READ_ATTRIBUTES) == FILE_READ_ATTRIBUTES)
					wprintf(L".................... FILE_READ_ATTRIBUTES: 0x%X\n", FILE_READ_ATTRIBUTES);
				if ((pace->Mask & FILE_WRITE_ATTRIBUTES) == FILE_WRITE_ATTRIBUTES)
					wprintf(L".................... FILE_WRITE_ATTRIBUTES: 0x%X\n", FILE_WRITE_ATTRIBUTES);
				if ((pace->Mask & FILE_ALL_ACCESS) == FILE_ALL_ACCESS)
					wprintf(L".................... FILE_ALL_ACCESS: 0x%X\n", FILE_ALL_ACCESS);
				if ((pace->Mask & FILE_GENERIC_READ) == FILE_GENERIC_READ)
					wprintf(L".................... FILE_GENERIC_READ: 0x%X\n", FILE_GENERIC_READ);
				if ((pace->Mask & FILE_GENERIC_WRITE) == FILE_GENERIC_WRITE)
					wprintf(L".................... FILE_GENERIC_WRITE: 0x%X\n", FILE_GENERIC_WRITE);
				if ((pace->Mask & FILE_GENERIC_EXECUTE) == FILE_GENERIC_EXECUTE)
					wprintf(L".................... FILE_GENERIC_EXECUTE: 0x%X\n", FILE_GENERIC_EXECUTE);
				break;
			
			case 'h': // thread
				if ((pace->Mask & THREAD_TERMINATE) == THREAD_TERMINATE)
					wprintf(L".................... THREAD_TERMINATE: 0x%X\n", THREAD_TERMINATE);
				if ((pace->Mask & THREAD_SUSPEND_RESUME) == THREAD_SUSPEND_RESUME)
					wprintf(L".................... THREAD_SUSPEND_RESUME: 0x%X\n", THREAD_SUSPEND_RESUME);
				if ((pace->Mask & THREAD_GET_CONTEXT) == THREAD_GET_CONTEXT)
					wprintf(L".................... THREAD_GET_CONTEXT: 0x%X\n", THREAD_GET_CONTEXT);
				if ((pace->Mask & THREAD_SET_CONTEXT) == THREAD_SET_CONTEXT)
			 		wprintf(L".................... THREAD_SET_CONTEXT: 0x%X\n", THREAD_SET_CONTEXT);
				if ((pace->Mask & THREAD_QUERY_INFORMATION) == THREAD_QUERY_INFORMATION)
	 				wprintf(L".................... THREAD_QUERY_INFORMATION: 0x%X\n", THREAD_QUERY_INFORMATION);
				if ((pace->Mask & THREAD_SET_INFORMATION) == THREAD_SET_INFORMATION)
					wprintf(L".................... THREAD_SET_INFORMATION: 0x%X\n", THREAD_SET_INFORMATION);
				if ((pace->Mask & THREAD_SET_THREAD_TOKEN) == THREAD_SET_THREAD_TOKEN)
					wprintf(L".................... THREAD_SET_THREAD_TOKEN: 0x%X\n", THREAD_SET_THREAD_TOKEN);
				if ((pace->Mask & THREAD_IMPERSONATE) == THREAD_IMPERSONATE)
					wprintf(L".................... THREAD_IMPERSONATE: 0x%X\n", THREAD_IMPERSONATE);
				if ((pace->Mask & THREAD_DIRECT_IMPERSONATION) == THREAD_DIRECT_IMPERSONATION)
					wprintf(L".................... THREAD_DIRECT_IMPERSONATION: 0x%X\n", THREAD_DIRECT_IMPERSONATION);
				if ((pace->Mask & THREAD_SET_LIMITED_INFORMATION) == THREAD_SET_LIMITED_INFORMATION)
					wprintf(L".................... THREAD_SET_LIMITED_INFORMATION: 0x%X\n", THREAD_SET_LIMITED_INFORMATION);
				if ((pace->Mask & THREAD_QUERY_LIMITED_INFORMATION) == THREAD_QUERY_LIMITED_INFORMATION)
					wprintf(L".................... THREAD_QUERY_LIMITED_INFORMATION: 0x%X\n", THREAD_QUERY_LIMITED_INFORMATION);
				if ((pace->Mask & THREAD_ALL_ACCESS) == THREAD_ALL_ACCESS) // TODO: value is different on VISTA and beyond
					wprintf(L".................... THREAD_ALL_ACCESS: 0x%X\n", THREAD_ALL_ACCESS);
				break;
			
			case 'i': // memory mapped file
				if ((pace->Mask & SECTION_QUERY) == SECTION_QUERY)
					wprintf(L".................... SECTION_QUERY: 0x%X\n", SECTION_QUERY);
				if ((pace->Mask & SECTION_MAP_WRITE) == SECTION_MAP_WRITE)
					wprintf(L".................... SECTION_MAP_WRITE: 0x%X\n", SECTION_MAP_WRITE);
				if ((pace->Mask & SECTION_MAP_READ) == SECTION_MAP_READ)
					wprintf(L".................... SECTION_MAP_READ: 0x%X\n", SECTION_MAP_READ);
				if ((pace->Mask & SECTION_MAP_EXECUTE) == SECTION_MAP_EXECUTE)
					wprintf(L".................... SECTION_MAP_EXECUTE: 0x%X\n", SECTION_MAP_EXECUTE);
				if ((pace->Mask & SECTION_EXTEND_SIZE) == SECTION_EXTEND_SIZE)
					wprintf(L".................... SECTION_EXTEND_SIZE: 0x%X\n", SECTION_EXTEND_SIZE);
				if ((pace->Mask & SECTION_MAP_EXECUTE_EXPLICIT) == SECTION_MAP_EXECUTE_EXPLICIT)
					wprintf(L".................... SECTION_EXTEND_SIZE: 0x%X\n", SECTION_MAP_EXECUTE_EXPLICIT);
				if ((pace->Mask & SECTION_ALL_ACCESS) == SECTION_ALL_ACCESS)
					wprintf(L".................... SECTION_ALL_ACCESS: 0x%X\n", SECTION_ALL_ACCESS);
				break;

			case 'j': //job
				if ((pace->Mask & JOB_OBJECT_ASSIGN_PROCESS) == JOB_OBJECT_ASSIGN_PROCESS)
					wprintf(L".................... JOB_OBJECT_ASSIGN_PROCESS: 0x%X\n", JOB_OBJECT_ASSIGN_PROCESS);
				if ((pace->Mask & JOB_OBJECT_SET_ATTRIBUTES) == JOB_OBJECT_SET_ATTRIBUTES)
					wprintf(L".................... JOB_OBJECT_SET_ATTRIBUTES: 0x%X\n", JOB_OBJECT_SET_ATTRIBUTES);
				if ((pace->Mask & JOB_OBJECT_QUERY) == JOB_OBJECT_QUERY)
					wprintf(L".................... JOB_OBJECT_QUERY: 0x%X\n", JOB_OBJECT_QUERY);
				if ((pace->Mask & JOB_OBJECT_TERMINATE) == JOB_OBJECT_TERMINATE)
					wprintf(L".................... JOB_OBJECT_TERMINATE: 0x%X\n", JOB_OBJECT_TERMINATE);
				if ((pace->Mask & JOB_OBJECT_SET_SECURITY_ATTRIBUTES) == JOB_OBJECT_SET_SECURITY_ATTRIBUTES)
					wprintf(L".................... JOB_OBJECT_SET_SECURITY_ATTRIBUTES: 0x%X\n", JOB_OBJECT_SET_SECURITY_ATTRIBUTES);
				if ((pace->Mask & JOB_OBJECT_ALL_ACCESS) == JOB_OBJECT_ALL_ACCESS)
					wprintf(L".................... JOB_OBJECT_ALL_ACCESS: 0x%X\n", JOB_OBJECT_ALL_ACCESS);
				break;

			case 'k': // desktop
				if ((pace->Mask & DESKTOP_READOBJECTS) == DESKTOP_READOBJECTS)
					wprintf(L".................... DESKTOP_READOBJECTS: 0x%X\n", DESKTOP_READOBJECTS);
				if ((pace->Mask & DESKTOP_CREATEWINDOW) == DESKTOP_CREATEWINDOW)
					wprintf(L".................... DESKTOP_CREATEWINDOW: 0x%X\n", DESKTOP_CREATEWINDOW);
				if ((pace->Mask & DESKTOP_CREATEMENU) == DESKTOP_CREATEMENU)
					wprintf(L".................... DESKTOP_CREATEMENU: 0x%X\n", DESKTOP_CREATEMENU);
				if ((pace->Mask & DESKTOP_HOOKCONTROL) == DESKTOP_HOOKCONTROL)
					wprintf(L".................... DESKTOP_HOOKCONTROL: 0x%X\n", DESKTOP_HOOKCONTROL);
				if ((pace->Mask & DESKTOP_JOURNALRECORD) == DESKTOP_JOURNALRECORD)
					wprintf(L".................... DESKTOP_JOURNALRECORD: 0x%X\n", DESKTOP_JOURNALRECORD);
				if ((pace->Mask & DESKTOP_JOURNALPLAYBACK) == DESKTOP_JOURNALPLAYBACK)
					wprintf(L".................... DESKTOP_JOURNALPLAYBACK: 0x%X\n", DESKTOP_JOURNALPLAYBACK);
				if ((pace->Mask & DESKTOP_ENUMERATE) == DESKTOP_ENUMERATE)
					wprintf(L".................... DESKTOP_ENUMERATE: 0x%X\n", DESKTOP_ENUMERATE);
				if ((pace->Mask & DESKTOP_WRITEOBJECTS) == DESKTOP_WRITEOBJECTS)
					wprintf(L".................... DESKTOP_WRITEOBJECTS: 0x%X\n", DESKTOP_WRITEOBJECTS);
				if ((pace->Mask & DESKTOP_SWITCHDESKTOP) == DESKTOP_SWITCHDESKTOP)
					wprintf(L".................... DESKTOP_SWITCHDESKTOP: 0x%X\n", DESKTOP_SWITCHDESKTOP);
				break;

			case 'l': // printer
				if ((pace->Mask & SERVER_ACCESS_ADMINISTER) == SERVER_ACCESS_ADMINISTER)
					wprintf(L".................... SERVER_ACCESS_ADMINISTER: 0x%X\n", SERVER_ACCESS_ADMINISTER);
                if ((pace->Mask & SERVER_ACCESS_ENUMERATE) == SERVER_ACCESS_ENUMERATE)
					wprintf(L".................... SERVER_ACCESS_ENUMERATE: 0x%X\n", SERVER_ACCESS_ENUMERATE);
				if ((pace->Mask & PRINTER_ACCESS_ADMINISTER) == PRINTER_ACCESS_ADMINISTER)
					wprintf(L".................... PRINTER_ACCESS_ADMINISTER: 0x%X\n", PRINTER_ACCESS_ADMINISTER);
				if ((pace->Mask & PRINTER_ACCESS_USE) == PRINTER_ACCESS_USE)
					wprintf(L".................... PRINTER_ACCESS_USE: 0x%X\n", PRINTER_ACCESS_USE);
				if ((pace->Mask & JOB_ACCESS_ADMINISTER) == JOB_ACCESS_ADMINISTER)
					wprintf(L".................... JOB_ACCESS_ADMINISTER: 0x%X\n", JOB_ACCESS_ADMINISTER);
				if ((pace->Mask & PRINTER_ALL_ACCESS) == PRINTER_ALL_ACCESS)
					wprintf(L".................... PRINTER_ALL_ACCESS: 0x%X\n", PRINTER_ALL_ACCESS);
				if ((pace->Mask & PRINTER_READ) == PRINTER_READ)
					wprintf(L".................... PRINTER_READ: 0x%X\n", PRINTER_READ);
				if ((pace->Mask & PRINTER_WRITE) == PRINTER_WRITE)
					wprintf(L".................... PRINTER_WRITE: 0x%X\n", PRINTER_WRITE);
				if ((pace->Mask & PRINTER_EXECUTE) == PRINTER_EXECUTE)
					wprintf(L".................... PRINTER_EXECUTE: 0x%X\n", PRINTER_EXECUTE);
				break;

			case 'm': // mutex
				if ((pace->Mask & MUTANT_QUERY_STATE) == MUTANT_QUERY_STATE)
					wprintf(L".................... MUTANT_QUERY_STATE: 0x%X\n", MUTANT_QUERY_STATE);
				if ((pace->Mask & MUTANT_ALL_ACCESS) == MUTANT_ALL_ACCESS)
					wprintf(L".................... MUTANT_ALL_ACCESS: 0x%X\n", MUTANT_ALL_ACCESS);
				break;

			case 'o': // process access token
				if ((pace->Mask & TOKEN_ASSIGN_PRIMARY) == TOKEN_ASSIGN_PRIMARY)
					wprintf(L".................... TOKEN_ASSIGN_PRIMARY: 0x%X\n", TOKEN_ASSIGN_PRIMARY);
				if ((pace->Mask & TOKEN_DUPLICATE) == TOKEN_DUPLICATE)
					wprintf(L".................... TOKEN_DUPLICATE: 0x%X\n", TOKEN_DUPLICATE);
				if ((pace->Mask & TOKEN_IMPERSONATE) == TOKEN_IMPERSONATE)
					wprintf(L".................... TOKEN_IMPERSONATE: 0x%X\n", TOKEN_IMPERSONATE);
				if ((pace->Mask & TOKEN_QUERY) == TOKEN_QUERY)
					wprintf(L".................... TOKEN_QUERY: 0x%X\n", TOKEN_QUERY);
				if ((pace->Mask & TOKEN_QUERY_SOURCE) == TOKEN_QUERY_SOURCE)
					wprintf(L".................... TOKEN_QUERY_SOURCE: 0x%X\n", TOKEN_QUERY_SOURCE);
				if ((pace->Mask & TOKEN_ADJUST_PRIVILEGES) == TOKEN_ADJUST_PRIVILEGES)
					wprintf(L".................... TOKEN_ADJUST_PRIVILEGES: 0x%X\n", TOKEN_ADJUST_PRIVILEGES);
				if ((pace->Mask & TOKEN_ADJUST_GROUPS) == TOKEN_ADJUST_GROUPS)
					wprintf(L".................... TOKEN_ADJUST_GROUPS: 0x%X\n", TOKEN_ADJUST_GROUPS);
				if ((pace->Mask & TOKEN_ADJUST_DEFAULT) == TOKEN_ADJUST_DEFAULT)
					wprintf(L".................... TOKEN_ADJUST_DEFAULT: 0x%X\n", TOKEN_ADJUST_DEFAULT);
				if ((pace->Mask & TOKEN_ADJUST_SESSIONID) == TOKEN_ADJUST_SESSIONID)
					wprintf(L".................... TOKEN_ADJUST_SESSIONID: 0x%X\n", TOKEN_ADJUST_SESSIONID);
				if ((pace->Mask & TOKEN_ALL_ACCESS) == TOKEN_ALL_ACCESS)
					wprintf(L".................... TOKEN_ALL_ACCESS: 0x%X\n", TOKEN_ALL_ACCESS);
				if ((pace->Mask & TOKEN_READ) == TOKEN_READ)
					wprintf(L".................... TOKEN_READ: 0x%X\n", TOKEN_READ);
				if ((pace->Mask & TOKEN_WRITE) == TOKEN_WRITE)
					wprintf(L".................... TOKEN_WRITE: 0x%X\n", TOKEN_WRITE);
				if ((pace->Mask & TOKEN_EXECUTE) == TOKEN_EXECUTE)
					wprintf(L".................... TOKEN_EXECUTE: 0x%X\n", TOKEN_EXECUTE);
				break;

			case 'p': // process
				if ((pace->Mask & PROCESS_TERMINATE) == PROCESS_TERMINATE)
					wprintf(L".................... PROCESS_TERMINATE: 0x%X\n", PROCESS_TERMINATE);
				if ((pace->Mask & PROCESS_CREATE_THREAD) == PROCESS_CREATE_THREAD)
					wprintf(L".................... PROCESS_CREATE_THREAD: 0x%X\n", PROCESS_CREATE_THREAD);
				if ((pace->Mask & PROCESS_SET_SESSIONID) == PROCESS_SET_SESSIONID)
					wprintf(L".................... PROCESS_SET_SESSIONID: 0x%X\n", PROCESS_SET_SESSIONID);
				if ((pace->Mask & PROCESS_VM_OPERATION) == PROCESS_VM_OPERATION)
					wprintf(L".................... PROCESS_VM_OPERATION: 0x%X\n", PROCESS_VM_OPERATION);
				if ((pace->Mask & PROCESS_VM_READ) == PROCESS_VM_READ)
					wprintf(L".................... PROCESS_VM_READ: 0x%X\n", PROCESS_VM_READ);
				if ((pace->Mask & PROCESS_VM_WRITE) == PROCESS_VM_WRITE)
					wprintf(L".................... PROCESS_VM_WRITE: 0x%X\n", PROCESS_VM_WRITE);
				if ((pace->Mask & PROCESS_DUP_HANDLE) == PROCESS_DUP_HANDLE)
					wprintf(L".................... PROCESS_DUP_HANDLE: 0x%X\n", PROCESS_DUP_HANDLE);
				if ((pace->Mask & PROCESS_CREATE_PROCESS) == PROCESS_CREATE_PROCESS)
					wprintf(L".................... PROCESS_CREATE_PROCESS: 0x%X\n", PROCESS_CREATE_PROCESS);
				if ((pace->Mask & PROCESS_SET_QUOTA) == PROCESS_SET_QUOTA)
					wprintf(L".................... PROCESS_SET_QUOTA: 0x%X\n", PROCESS_SET_QUOTA);
				if ((pace->Mask & PROCESS_SET_INFORMATION) == PROCESS_SET_INFORMATION)
					wprintf(L".................... PROCESS_SET_INFORMATION: 0x%X\n", PROCESS_SET_INFORMATION);
				if ((pace->Mask & PROCESS_QUERY_INFORMATION) == PROCESS_QUERY_INFORMATION)
					wprintf(L".................... PROCESS_QUERY_INFORMATION: 0x%X\n", PROCESS_QUERY_INFORMATION);
				if ((pace->Mask & PROCESS_SUSPEND_RESUME) == PROCESS_SUSPEND_RESUME)
					wprintf(L".................... PROCESS_SUSPEND_RESUME: 0x%X\n", PROCESS_SUSPEND_RESUME);
				if ((pace->Mask & PROCESS_QUERY_LIMITED_INFORMATION) == PROCESS_QUERY_LIMITED_INFORMATION)
					wprintf(L".................... PROCESS_QUERY_LIMITED_INFORMATION: 0x%X\n", PROCESS_QUERY_LIMITED_INFORMATION);
				if ((pace->Mask & PROCESS_QUERY_LIMITED_INFORMATION) == PROCESS_QUERY_LIMITED_INFORMATION)
					wprintf(L".................... PROCESS_QUERY_LIMITED_INFORMATION: 0x%X\n", PROCESS_QUERY_LIMITED_INFORMATION);
				if ((pace->Mask & PROCESS_ALL_ACCESS) == PROCESS_ALL_ACCESS) // TODO: Bits are different for VISTA and beyond
					wprintf(L".................... PROCESS_ALL_ACCESS: 0x%X\n", PROCESS_ALL_ACCESS);
				break;

			case 'x':  // 32 or 64 bit registry depending on OS
			case 'r':  //registry
				if ((pace->Mask & KEY_QUERY_VALUE) == KEY_QUERY_VALUE)
					wprintf(L".................... KEY_QUERY_VALUE: 0x%X\n", KEY_QUERY_VALUE);
				if ((pace->Mask & KEY_SET_VALUE) == KEY_SET_VALUE)
					wprintf(L".................... KEY_SET_VALUE: 0x%X\n", KEY_SET_VALUE);
				if ((pace->Mask & KEY_CREATE_SUB_KEY) == KEY_CREATE_SUB_KEY)
					wprintf(L".................... KEY_CREATE_SUB_KEY: 0x%X\n", KEY_CREATE_SUB_KEY);
				if ((pace->Mask & KEY_ENUMERATE_SUB_KEYS) == KEY_ENUMERATE_SUB_KEYS)
					wprintf(L".................... KEY_ENUMERATE_SUB_KEYS: 0x%X\n", KEY_ENUMERATE_SUB_KEYS);
				if ((pace->Mask & KEY_NOTIFY) == KEY_NOTIFY)
					wprintf(L".................... KEY_NOTIFY: 0x%X\n", KEY_NOTIFY);
				if ((pace->Mask & KEY_CREATE_LINK) == KEY_CREATE_LINK)
					wprintf(L".................... KEY_CREATE_LINK: 0x%X\n", KEY_CREATE_LINK);
				if ((pace->Mask & KEY_WOW64_32KEY) == KEY_WOW64_32KEY)
					wprintf(L".................... KEY_WOW64_32KEY: 0x%X\n", KEY_WOW64_32KEY);
				if ((pace->Mask & KEY_WOW64_64KEY) == KEY_WOW64_64KEY)
					wprintf(L".................... KEY_WOW64_64KEY: 0x%X\n", KEY_WOW64_64KEY);
				if ((pace->Mask & KEY_WOW64_RES) == KEY_WOW64_RES) // what is this?
					wprintf(L".................... KEY_WOW64_RES: 0x%X\n", KEY_WOW64_RES);
				if ((pace->Mask & KEY_READ) == KEY_READ)
					wprintf(L".................... KEY_READ: 0x%X\n", KEY_READ);
				if ((pace->Mask & KEY_WRITE) == KEY_WRITE)
					wprintf(L".................... KEY_WRITE: 0x%X\n", KEY_WRITE);
				if ((pace->Mask & KEY_EXECUTE) == KEY_EXECUTE)
					wprintf(L".................... KEY_EXECUTE: 0x%X\n", KEY_EXECUTE);
				if ((pace->Mask & KEY_ALL_ACCESS) == KEY_ALL_ACCESS)
					wprintf(L".................... KEY_ALL_ACCESS: 0x%X\n", KEY_ALL_ACCESS);
				break;

			case 's': // semaphore
				if ((pace->Mask & SEMAPHORE_MODIFY_STATE) == SEMAPHORE_MODIFY_STATE)
					wprintf(L".................... SEMAPHORE_MODIFY_STATE: 0x%X\n", SEMAPHORE_MODIFY_STATE);
				if ((pace->Mask & SEMAPHORE_ALL_ACCESS) == SEMAPHORE_ALL_ACCESS)
					wprintf(L".................... SEMAPHORE_ALL_ACCESS: 0x%X\n", SEMAPHORE_ALL_ACCESS);
				break;
			
			case 't': // network shares
				if ((pace->Mask & READ) == READ)
					wprintf(L".................... READ\n");
				if ((pace->Mask & CHANGE) == CHANGE)
					wprintf(L".................... CHANGE\n");
				if ((pace->Mask & WRITE) == WRITE)
					wprintf(L".................... WRITE\n");
				break;

			case 'v': // service
				if ((pace->Mask & SERVICE_CHANGE_CONFIG) == SERVICE_CHANGE_CONFIG)
					wprintf(L".................... SERVICE_CHANGE_CONFIG: 0x%X\n", SERVICE_CHANGE_CONFIG);
				if ((pace->Mask & SERVICE_ENUMERATE_DEPENDENTS) == SERVICE_ENUMERATE_DEPENDENTS)
					wprintf(L".................... SERVICE_ENUMERATE_DEPENDENTS: 0x%X\n", SERVICE_ENUMERATE_DEPENDENTS);
				if ((pace->Mask & SERVICE_INTERROGATE) == SERVICE_INTERROGATE)
					wprintf(L".................... SERVICE_INTERROGATE: 0x%X\n", SERVICE_INTERROGATE);
				if ((pace->Mask & SERVICE_PAUSE_CONTINUE) == SERVICE_PAUSE_CONTINUE)
					wprintf(L".................... SERVICE_PAUSE_CONTINUE: 0x%X\n", SERVICE_PAUSE_CONTINUE);
				if ((pace->Mask & SERVICE_QUERY_CONFIG) == SERVICE_QUERY_CONFIG)
					wprintf(L".................... SERVICE_QUERY_CONFIG: 0x%X\n", SERVICE_QUERY_CONFIG);
				if ((pace->Mask & SERVICE_QUERY_STATUS) == SERVICE_QUERY_STATUS)
					wprintf(L".................... SERVICE_QUERY_STATUS: 0x%X\n", SERVICE_QUERY_STATUS);
				if ((pace->Mask & SERVICE_START) == SERVICE_START)
					wprintf(L".................... SERVICE_START: 0x%X\n", SERVICE_START);
				if ((pace->Mask & SERVICE_STOP) == SERVICE_STOP)
					wprintf(L".................... SERVICE_STOP: 0x%X\n", SERVICE_STOP);
				if ((pace->Mask & SERVICE_USER_DEFINED_CONTROL) == SERVICE_USER_DEFINED_CONTROL)
					wprintf(L".................... SERVICE_USER_DEFINED_CONTROL: 0x%X\n", SERVICE_USER_DEFINED_CONTROL);
				if ((pace->Mask & SERVICE_ALL_ACCESS) == SERVICE_ALL_ACCESS)
					wprintf(L".................... SERVICE_ALL_ACCESS: 0x%X\n", SERVICE_ALL_ACCESS);
				break;
			
			case 'w': // windowstation
				if ((pace->Mask & WINSTA_ACCESSCLIPBOARD) == WINSTA_ACCESSCLIPBOARD)
					wprintf(L".................... WINSTA_ACCESSCLIPBOARD: 0x%X\n", WINSTA_ACCESSCLIPBOARD);
				if ((pace->Mask & WINSTA_ACCESSGLOBALATOMS) == WINSTA_ACCESSGLOBALATOMS)
					wprintf(L".................... WINSTA_ACCESSGLOBALATOMS: 0x%X\n", WINSTA_ACCESSGLOBALATOMS);
				if ((pace->Mask & WINSTA_CREATEDESKTOP) == WINSTA_CREATEDESKTOP)
					wprintf(L".................... WINSTA_CREATEDESKTOP: 0x%X\n", WINSTA_CREATEDESKTOP);
				if ((pace->Mask & WINSTA_ENUMDESKTOPS) == WINSTA_ENUMDESKTOPS)
					wprintf(L".................... WINSTA_ENUMDESKTOPS: 0x%X\n", WINSTA_ENUMDESKTOPS);
				if ((pace->Mask & WINSTA_ENUMERATE) == WINSTA_ENUMERATE)
					wprintf(L".................... WINSTA_ENUMERATE: 0x%X\n", WINSTA_ENUMERATE);
				if ((pace->Mask & WINSTA_EXITWINDOWS) == WINSTA_EXITWINDOWS)
					wprintf(L".................... WINSTA_EXITWINDOWS: 0x%X\n", WINSTA_EXITWINDOWS);
				if ((pace->Mask & WINSTA_READATTRIBUTES) == WINSTA_READATTRIBUTES)
					wprintf(L".................... WINSTA_READATTRIBUTES: 0x%X\n", WINSTA_READATTRIBUTES);
				if ((pace->Mask & WINSTA_READSCREEN) == WINSTA_READSCREEN)
					wprintf(L".................... WINSTA_READSCREEN: 0x%X\n", WINSTA_READSCREEN);
				if ((pace->Mask & WINSTA_WRITEATTRIBUTES) == WINSTA_WRITEATTRIBUTES)
					wprintf(L".................... WINSTA_WRITEATTRIBUTES: 0x%X\n", WINSTA_WRITEATTRIBUTES);
				if ((pace->Mask & WINSTA_ALL_ACCESS) == WINSTA_ALL_ACCESS)
					wprintf(L".................... WINSTA_ALL_ACCESS: 0x%X\n", WINSTA_ALL_ACCESS);
				break;

			case 'y': // kernel transaction
				if ((pace->Mask & TRANSACTION_QUERY_INFORMATION) == TRANSACTION_QUERY_INFORMATION)
					wprintf(L".................... TRANSACTION_QUERY_INFORMATION: 0x%X\n", TRANSACTION_QUERY_INFORMATION);
				if ((pace->Mask & TRANSACTION_SET_INFORMATION) == TRANSACTION_SET_INFORMATION)
					wprintf(L".................... TRANSACTION_SET_INFORMATION: 0x%X\n", TRANSACTION_SET_INFORMATION);
				if ((pace->Mask & TRANSACTION_ENLIST) == TRANSACTION_ENLIST)
					wprintf(L".................... TRANSACTION_ENLIST: 0x%X\n", TRANSACTION_ENLIST);
				if ((pace->Mask & TRANSACTION_COMMIT) == TRANSACTION_COMMIT)
					wprintf(L".................... TRANSACTION_COMMIT: 0x%X\n", TRANSACTION_COMMIT);
				if ((pace->Mask & TRANSACTION_ROLLBACK) == TRANSACTION_ROLLBACK)
					wprintf(L".................... TRANSACTION_ROLLBACK: 0x%X\n", TRANSACTION_ROLLBACK);
				if ((pace->Mask & TRANSACTION_PROPAGATE) == TRANSACTION_PROPAGATE)
					wprintf(L".................... TRANSACTION_PROPAGATE: 0x%X\n", TRANSACTION_PROPAGATE);
				if ((pace->Mask & TRANSACTION_RIGHT_RESERVED1) == TRANSACTION_RIGHT_RESERVED1)
					wprintf(L".................... TRANSACTION_RIGHT_RESERVED1: 0x%X\n", TRANSACTION_RIGHT_RESERVED1);
				if ((pace->Mask & TRANSACTION_GENERIC_READ) == TRANSACTION_GENERIC_READ)
					wprintf(L".................... TRANSACTION_GENERIC_READ: 0x%X\n", TRANSACTION_GENERIC_READ);
				if ((pace->Mask & TRANSACTION_GENERIC_WRITE) == TRANSACTION_GENERIC_WRITE)
					wprintf(L".................... TRANSACTION_GENERIC_WRITE: 0x%X\n", TRANSACTION_GENERIC_WRITE);
				if ((pace->Mask & TRANSACTION_GENERIC_EXECUTE) == TRANSACTION_GENERIC_EXECUTE)
					wprintf(L".................... TRANSACTION_GENERIC_EXECUTE: 0x%X\n", TRANSACTION_GENERIC_EXECUTE);
				if ((pace->Mask & TRANSACTION_ALL_ACCESS) == TRANSACTION_ALL_ACCESS)
					wprintf(L".................... TRANSACTION_ALL_ACCESS: 0x%X\n", TRANSACTION_ALL_ACCESS);
				break;

			case 'z': // waitable timer
				if ((pace->Mask & TIMER_QUERY_STATE) == TIMER_QUERY_STATE)
					wprintf(L".................... TIMER_QUERY_STATE: 0x%X\n", TIMER_QUERY_STATE);
				if ((pace->Mask & TIMER_MODIFY_STATE) == TIMER_MODIFY_STATE)
					wprintf(L".................... TIMER_MODIFY_STATE: 0x%X\n", TIMER_MODIFY_STATE);
				if ((pace->Mask & TIMER_ALL_ACCESS) == TIMER_ALL_ACCESS)
					wprintf(L".................... TIMER_ALL_ACCESS: 0x%X\n", TIMER_ALL_ACCESS);
				break;

			default:
				break;
			}
	}
}

//
//   FUNCTION: DumpDacl(PACL, WCHAR, BOOl, BOOL)
//
//   PURPOSE: returns information associated with the DACL or SACL and
//   all ACEs.  NOTE: in order to obtain a SACL for an object, you need
//   ACCESS_SYSTEM_SECURITY access to the securable object plus the process
//   obtaining the SACL needs to have the SE_SECURITY_NAME ("manager auditing
//   and security log") privilege which also needs to be enabled.
//   CppCheckSD.exe
//
//   PARAMETERS:
//   * pacl         - the ACL
//   * c            - type of kernel object
//   * bDacl        - TRUE indicates a DACL versus a SACL
//   * bDaclPresent - TRUE indicates an ACL is present
//
//   RETURN VALUE: none
//
void DumpDacl(PACL pacl, WCHAR c, BOOL bDacl, BOOL bDaclPresent)
{
	ACCESS_ALLOWED_ACE *pace;
    DWORD               dwAceCount;
    DWORD               cbName;
    DWORD               cbReferencedDomainName;
    int                 i;
	LPWSTR              pszStringSid = NULL;
    PSID                psid = NULL;
    SID_NAME_USE        snu;
    WCHAR               szName[UNLEN+1];
    WCHAR               szReferencedDomainName[DNLEN+1];
    WCHAR               szSidType[][17] = {L"User", L"Group", L"Domain", L"Alias", L"Well Known Group", L"Deleted Account", L"Invalid", L"Unknown", L"Computer", L"Label"};

	if (bDaclPresent)
	{

		// Dump the dacl
		if (pacl == NULL)
		{
			if (bDacl)
				wprintf(L"\nDACL ............... NULL\n");
			else
				wprintf(L"\nSACL ............... NULL\n");
		}
		else
		{
			dwAceCount = DumpAclInfo(pacl, bDacl);

			for (i = 0; i < (int)dwAceCount; i++)
			{
				if (!GetAce(pacl, i, (LPVOID *)&pace))
					DisplayError(GetLastError(), L"GetAce");

				wprintf(L"\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
				wprintf(L">>                  ACE #%u                             >>\n", i+1);
				wprintf(L">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n");
				wprintf(L"type ............... ");

				switch(pace->Header.AceType)
				{
					case ACCESS_ALLOWED_ACE_TYPE:
						wprintf(L"ACCESS_ALLOWED_ACE: 0x%X\n", ACCESS_ALLOWED_ACE_TYPE);
						break;
					case ACCESS_DENIED_ACE_TYPE:
						wprintf(L"ACCESS_DENIED_ACE: 0x%X\n", ACCESS_DENIED_ACE_TYPE);
						break;
					case SYSTEM_AUDIT_ACE_TYPE:
						wprintf(L"SYSTEM_AUDIT_ACE, 0x%X\n", SYSTEM_AUDIT_ACE_TYPE);
						break;
					case SYSTEM_MANDATORY_LABEL_ACE_TYPE: // VISTA
						wprintf(L"SYSTEM_MANDATORY_LABEL_ACE: 0x%X\n", SYSTEM_MANDATORY_LABEL_ACE_TYPE);
						break;
				}

				wprintf(L"flags .............. 0x%X\n", pace->Header.AceFlags);

				if ((pace->Header.AceFlags & CONTAINER_INHERIT_ACE) == CONTAINER_INHERIT_ACE)
					wprintf(L".................... CONTAINER_INHERIT_ACE: 0x%X\n", CONTAINER_INHERIT_ACE);

				if ((pace->Header.AceFlags & INHERIT_ONLY_ACE) == INHERIT_ONLY_ACE)
					wprintf(L".................... INHERIT_ONLY_ACE: 0x%X\n", INHERIT_ONLY_ACE);

				if ((pace->Header.AceFlags & NO_PROPAGATE_INHERIT_ACE) == NO_PROPAGATE_INHERIT_ACE)
					wprintf(L".................... NO_PROPAGATE_INHERIT_ACE: 0x%X\n", NO_PROPAGATE_INHERIT_ACE);

				if ((pace->Header.AceFlags & OBJECT_INHERIT_ACE) == OBJECT_INHERIT_ACE)
					wprintf(L".................... OBJECT_INHERIT_ACE: 0x%X\n", OBJECT_INHERIT_ACE);

				if ((pace->Header.AceFlags & FAILED_ACCESS_ACE_FLAG) == FAILED_ACCESS_ACE_FLAG)
					wprintf(L".................... FAILED_ACCESS_ACE_FLAG: 0x%X\n", FAILED_ACCESS_ACE_FLAG);

				if ((pace->Header.AceFlags & SUCCESSFUL_ACCESS_ACE_FLAG) == SUCCESSFUL_ACCESS_ACE_FLAG)
					wprintf(L".................... SUCCESSFUL_ACCESS_ACE_FLAG: 0x%X\n", SUCCESSFUL_ACCESS_ACE_FLAG);

				if ((pace->Header.AceFlags & INHERITED_ACE) == INHERITED_ACE) // NT5.0
					wprintf(L".................... INHERITED_ACE: 0x%X\n", INHERITED_ACE);

				wprintf(L"size ............... %u byte(s)\n", pace->Header.AceSize);
				wprintf(L"mask ............... 0x%X\n", pace->Mask);

				PrintAccessRights(pace, c, bDacl);

				if (bDacl)
				{

					// Object rights
					if ((pace->Mask & READ_CONTROL) == READ_CONTROL)
						wprintf(L".................... READ_CONTROL, 0x%X\n", READ_CONTROL);
					if ((pace->Mask & WRITE_OWNER) == WRITE_OWNER)
						wprintf(L".................... WRITE_OWNER, 0x%X\n", WRITE_OWNER);
					if ((pace->Mask & WRITE_DAC) == WRITE_DAC)
						wprintf(L".................... WRITE_DAC, 0x%X\n", WRITE_DAC);
					if ((pace->Mask & DELETE) == DELETE)
						wprintf(L".................... DELETE, 0x%X\n", DELETE);
					if ((pace->Mask & SYNCHRONIZE) == SYNCHRONIZE)
						wprintf(L".................... SYNCHRONIZE, 0x%X\n", SYNCHRONIZE);
					if ((pace->Mask & ACCESS_SYSTEM_SECURITY) == ACCESS_SYSTEM_SECURITY)
						wprintf(L".................... ACCESS_SYSTEM_SECURITY, 0x%X\n", ACCESS_SYSTEM_SECURITY);

					// GENERIC access rights
					if ((pace->Mask & GENERIC_ALL) == GENERIC_ALL)
						wprintf(L".................... GENERIC_ALL, 0x%X\n", GENERIC_ALL);
					if ((pace->Mask & GENERIC_EXECUTE) == GENERIC_EXECUTE)
						wprintf(L".................... GENERIC_EXECUTE, 0x%X\n", GENERIC_EXECUTE);
					if ((pace->Mask & GENERIC_READ) == GENERIC_READ)
						wprintf(L".................... GENERIC_READ, 0x%X\n", GENERIC_READ);
					if ((pace->Mask & GENERIC_WRITE) == GENERIC_WRITE)
						wprintf(L".................... GENERIC_WRITE, 0x%X\n", GENERIC_WRITE);
				}
				else
				{
					// MANDATORY LABEL
					if ((pace->Mask & SYSTEM_MANDATORY_LABEL_NO_WRITE_UP) == SYSTEM_MANDATORY_LABEL_NO_WRITE_UP)
						wprintf(L".................... SYSTEM_MANDATORY_LABEL_NO_WRITE_UP, 0x%X\n", SYSTEM_MANDATORY_LABEL_NO_WRITE_UP);
					if ((pace->Mask & SYSTEM_MANDATORY_LABEL_NO_READ_UP) == SYSTEM_MANDATORY_LABEL_NO_READ_UP)
						wprintf(L".................... SYSTEM_MANDATORY_LABEL_NO_READ_UP, 0x%X\n", SYSTEM_MANDATORY_LABEL_NO_READ_UP);
					if ((pace->Mask & SYSTEM_MANDATORY_LABEL_NO_EXECUTE_UP) == SYSTEM_MANDATORY_LABEL_NO_EXECUTE_UP)
						wprintf(L".................... SYSTEM_MANDATORY_LABEL_NO_EXECUTE_UP, 0x%X\n", SYSTEM_MANDATORY_LABEL_NO_EXECUTE_UP);
				}

				// Display sid
				cbName = sizeof(szName)/sizeof(TCHAR);

				cbReferencedDomainName = sizeof(szReferencedDomainName)/sizeof(TCHAR);

				ZeroMemory(szName, sizeof(szName));
				ZeroMemory(szReferencedDomainName, sizeof(szReferencedDomainName));

				if (!LookupAccountSid(NULL, &(pace->SidStart), szName, &cbName, szReferencedDomainName, &cbReferencedDomainName, &snu))
				{
					if (GetLastError() == ERROR_NONE_MAPPED)
						LookupAccountOtherSid(&(pace->SidStart), szName, &cbName, szReferencedDomainName, &cbReferencedDomainName, &snu);
					else
						DisplayError(GetLastError(), L"LookupAccountSid");
				}

				wprintf(L"\nuser ............... %s\\%s\n", szReferencedDomainName, szName);

				if (!ConvertSidToStringSid(&(pace->SidStart), &pszStringSid))
					DisplayError(GetLastError(), L"ConvertSidToStringSid");

				wprintf(L"sid ................ %s\n", pszStringSid);
				wprintf(L"sid type ........... %s\n", szSidType[snu-1]);
				wprintf(L"sid size ........... %u bytes\n", GetLengthSid(&(pace->SidStart)));

				if (pszStringSid != NULL)
					LocalFree((HLOCAL)pszStringSid);
			}
		}
	}
	else
	{
		wprintf(L"\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		if (bDacl)
			wprintf(L">>                 NO DACL PRESENT                     >>\n");
		else
			wprintf(L">>                 NO SACL PRESENT                     >>\n");

		wprintf(L">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n");
	}
}

//
//   FUNCTION: DumpOwnerGroup(PSID, BOOL)
//
//   PURPOSE: Returns information on either the owner or primary group 
//   CppCheckSD.exe
//
//   PARAMETERS:
//   * psid   - The SID
//   * bOwner - TRUE indicates and OWNER while FALSE is a Primary Group
//
//   RETURN VALUE: none
//
void DumpOwnerGroup(PSID psid, BOOL bOwner)
{
    WCHAR        szName[UNLEN+1];
    WCHAR        szReferencedDomainName[DNLEN+1];
    DWORD        cbName                 = sizeof(szName)/sizeof(TCHAR);
    DWORD        cbReferencedDomainName = sizeof(szReferencedDomainName)/sizeof(TCHAR);
	HRESULT      hr;
	LPWSTR       pszStringSid = NULL;
	SID_NAME_USE snu;
	WCHAR        szSidType[][17]        = {L"User", L"Group", L"Domain", L"Alias", L"Well Known Group", L"Deleted Account", L"Invalid", L"Unknown", L"Computer", L"Label"};
    WCHAR        szType[6]              = L"";

	if (bOwner)
	{
		hr = StringCbCopy(szType, sizeof(szType), L"owner");
		if (FAILED(hr))
			DisplayError(hr, L"StringCbCopy");
	}
	else
	{
		hr = StringCbCopy(szType, sizeof(szType), L"group");
		if (FAILED(hr))
			DisplayError(hr, L"StringCbCopy");
	}

	if (psid == NULL)
		wprintf(L"%s .............. none\n", szType);
	else
	{
		ZeroMemory(szName, cbName);
		ZeroMemory(szReferencedDomainName, cbReferencedDomainName);

		// Get the owner of the sid.
        if (!LookupAccountSid(NULL, psid, szName, &cbName, szReferencedDomainName, &cbReferencedDomainName, &snu))
		{
			if (GetLastError() != ERROR_NONE_MAPPED)
				DisplayError(GetLastError(), L"LookupAccountSid");
		}

		wprintf(L"\n%s .............. %s\\%s\n", szType, szReferencedDomainName, szName);
		if (!ConvertSidToStringSid(psid, &pszStringSid))
			DisplayError(GetLastError(), L"ConvertSidToStringSid");
          
		wprintf(L"sid ................ %s\n", pszStringSid);
		wprintf(L"sid type ........... %s\n", szSidType[snu-1]);

		if (pszStringSid != NULL)
			LocalFree((HLOCAL)pszStringSid);
     }
}

//
//   FUNCTION: DumpSD(PSECURITY_DESCRIPTOR, WCHAR, BOOL, BOOl, PSID, PSID, PACL, PACL)
//
//   PURPOSE: Bundles up all the other security descriptor calls to one function
//   call 
//   CppCheckSD.exe
//
//   PARAMETERS:
//   * psd      - Security Descriptor (SD)
//   * c        - type of kernel object
//   * bVerbose - TRUE displays full information on a SD while FALSE is SDDL format
//   * bAudit   - SD includes Audit ACEs
//   * pOwner   - SID of owner
//   * pGroup   - SID of primary group
//   * pDacl    - pointer to DACL
//   * pSacl    - pointer to SACL
//
//   RETURN VALUE: none
//
void DumpSD(PSECURITY_DESCRIPTOR psd, WCHAR c, BOOL bVerbose, BOOL bAudit, PSID pOwner, PSID pGroup, PACL pDacl, PACL pSacl)
{
	if (!bVerbose)
	{
		LPWSTR StringSecurityDescriptor    = NULL;
		SECURITY_INFORMATION si            = OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | LABEL_SECURITY_INFORMATION;
		ULONG  StringSecurityDescriptorLen = 0;

		if (!bAudit)
			si = si | SACL_SECURITY_INFORMATION;

		if (!ConvertSecurityDescriptorToStringSecurityDescriptor(psd, SDDL_REVISION_1, si, &StringSecurityDescriptor, &StringSecurityDescriptorLen))
			DisplayError(GetLastError(),L"ConvertSecurityDescriptorToStringSecurityDescriptor");
		else
		{
			wprintf(L"sd ................. %s\n", StringSecurityDescriptor);
			wprintf(L"length ............. %u\n", StringSecurityDescriptorLen);
		}

		if (StringSecurityDescriptor != NULL)
			LocalFree((HLOCAL)StringSecurityDescriptor);
	}
	else
	{
		BOOL  bDaclPresent;
		BOOL  bSaclPresent;
		DWORD dwRet;

		// Dump security descriptor information.
		DumpSDInfo(psd);


		// Dump the control bits
		//
		// 0 = bDacl and bSacl not present
		// 1 = bDacl present and bSacl NOT present
		// 2 = bDacl no present and bSacl present
		// 3 = bDacl and bSacl present
		dwRet = DumpControl(psd);

		if (dwRet == 0)
		{
			bDaclPresent = FALSE;
			bSaclPresent = FALSE;
		}

		if (dwRet == 1)
		{
			bDaclPresent = TRUE;
			bSaclPresent = FALSE;
		}

		if (dwRet == 2)
		{
			bDaclPresent = FALSE;
			bSaclPresent = TRUE;
		}

		if (dwRet == 3)
		{
			bDaclPresent = TRUE;
			bSaclPresent = TRUE;
		}

		// Get the owner.
		DumpOwnerGroup(pOwner, TRUE);

		// Get the group.
		DumpOwnerGroup(pGroup, FALSE);

		// Get the dacl.
		DumpDacl(pDacl, c, TRUE, bDaclPresent);
		DumpDacl(pSacl, c, FALSE, bSaclPresent);
	}
}

//
//   FUNCTION: DumpSDInfo(PSECURITY_DESCRIPTOR)
//
//   PURPOSE: returns whether the security descriptor is valid and it's
//   length
//   CppCheckSD.exe
//
//   PARAMETERS:
//   * psd - security descriptor to display
//
//   RETURN VALUE: none
//
void DumpSDInfo(PSECURITY_DESCRIPTOR psd)
{
     DWORD dwSDLength;

     // Is the security descriptor valid
     wprintf(L"\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
     wprintf(L">>          SECURITY DESCRIPTOR INFORMATION            >>\n");
     wprintf(L">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\nvalid .............. ");

     if (!IsValidSecurityDescriptor(psd))
	 {
          wprintf(L"no\n");
          return;
     }
     else
          wprintf(L"yes\n");

     // Security descriptor size???
     dwSDLength = GetSecurityDescriptorLength(psd);

     wprintf(L"length ............. %u byte(s)\n", dwSDLength);
}