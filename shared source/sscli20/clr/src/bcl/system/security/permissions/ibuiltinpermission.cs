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
//  IBuiltInPermission.cs
//

namespace System.Security.Permissions
{
    internal interface IBuiltInPermission
    {
        int GetTokenIndex();
    }

    internal static class BuiltInPermissionIndex
    {
        internal const int NUM_BUILTIN_UNRESTRICTED = 10;
        internal const int NUM_BUILTIN_NORMAL = 7;

        // Unrestricted permissions

        internal const int EnvironmentPermissionIndex = 0;
        internal const int FileDialogPermissionIndex = 1;
        internal const int FileIOPermissionIndex = 2;
        internal const int IsolatedStorageFilePermissionIndex = 3;
        internal const int ReflectionPermissionIndex = 4;
        internal const int SecurityPermissionIndex = 6;
        internal const int UIPermissionIndex = 7;
        internal const int PrincipalPermissionIndex = 8;
        internal const int HostProtectionPermissionIndex = 9;

        // Normal permissions
        internal const int SiteIdentityPermissionIndex = 1 + NUM_BUILTIN_UNRESTRICTED;
        internal const int StrongNameIdentityPermissionIndex = 2 + NUM_BUILTIN_UNRESTRICTED;
        internal const int UrlIdentityPermissionIndex = 3 + NUM_BUILTIN_UNRESTRICTED;
        internal const int ZoneIdentityPermissionIndex = 4 + NUM_BUILTIN_UNRESTRICTED;
        internal const int GacIdentityPermissionIndex = 5 + NUM_BUILTIN_UNRESTRICTED;
    }

	[Serializable]
    internal enum BuiltInPermissionFlag
    {
        // Unrestricted permissions

        EnvironmentPermission = 0x1,
        FileDialogPermission = 0x2,
        FileIOPermission = 0x4,
        IsolatedStorageFilePermission = 0x8,
        ReflectionPermission = 0x10,
        SecurityPermission = 0x40,
        UIPermission = 0x80,
        PrincipalPermission = 0x100,

        // Normal permissions
        SiteIdentityPermission = 0x400,
        StrongNameIdentityPermission = 0x800,
        UrlIdentityPermission = 0x1000,
        ZoneIdentityPermission = 0x2000,
    }
}
