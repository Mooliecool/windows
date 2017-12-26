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
//
// The enumeration constants used in CultureInfo.GetCultures().
//
namespace System.Globalization {    


    [Flags, Serializable]
    [System.Runtime.InteropServices.ComVisible(true)]
    public enum CultureTypes 
    {
        NeutralCultures             = 0x0001,                               // Neutral cultures are cultures like "en", "de", "zh", etc.
        SpecificCultures            = 0x0002,                               // Non-netural cultuers.  Examples are "en-us", "zh-tw", etc.
        InstalledWin32Cultures      = 0x0004,                               // Win32 installed cultures in the system and exists in the framework too.
        
        AllCultures                 = NeutralCultures | SpecificCultures | InstalledWin32Cultures,
        
        UserCustomCulture           = 0x0008,               // User defined custom culture
        ReplacementCultures         = 0x0010,               // User defined replacement custom culture.
        WindowsOnlyCultures         = 0x0020,               // Culture exist in Win32 but not in the Framework.
        FrameworkCultures           = 0x0040,               // the language tag match a culture that ships with the .NET framework
    }
    
}    
