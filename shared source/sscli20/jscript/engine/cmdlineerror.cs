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

namespace Microsoft.JScript{

    using System;

    // The values assigned in this enumeration correspond to JSC errors JS2000-JS3000.  The values
    // of existing tags should not be changed since documentation for the error code would then
    // also require a change.

    // To add an error:
    //  1. add the enumeration here
    //  2. edit CmdLineException.ResourceKey to return the key for the string resource
    //  3. edit Microsoft.JScript.txt to add the string resource
    //  4. update LAST to the largest error code you assigned so that the names can be
    //     kept in alphabetical order without making it too easy to assign a duplicate

    public enum CmdLineError{
      NoError = 0,
      
      //2000-3000 JSC command line errors
      AssemblyNotFound = 2001, // Assembly not found;;Referenced assembly '{0}' could not be found
      CannotCreateEngine = 2002, // Cannot create JScript engine = Cannot create JScript engine
      CompilerConstant = 2003, // Compiler constant = Cannot change value of compiler constant;;Cannot change value of compiler constant '{0}'
      DuplicateFileAsSourceAndAssembly = 2004, // Duplicate file as source and assembly = File name is listed as both a source file and an imported assembly;;Duplicate file name: '{0}' is listed as both a source file and an imported assembly
      DuplicateResourceFile = 2005, // Duplicate resource file = Duplicate resource file reference;;Resource file '{0}' referenced more than once
      DuplicateResourceName = 2006, // Duplicate resource name = Duplicate resource name;;Resource name '{0}' is not unique
      DuplicateSourceFile = 2007, // Duplicate source file = File name is listed as a source file more than once;;Duplicate file name: '{0}' is listed as a source file more than once
      ErrorSavingCompiledState = 2008, // Error saving compiled state = Error: Could not save compiled state;;Error saving compiled state: {0}
      IncompatibleTargets = 2038, // Incompatible targets = CompilerParameters.GenerateExecutable and CompilerParameters.CompilerOptions specify incompatible target types;;The value of CompilerParameters.GenerateExecutable is not compatible with '{0}'
      InvalidAssembly = 2009, // Invalid assembly = Invalid assembly reference;;'{0}' is not a valid assembly
      InvalidCharacters = 2036, // Invalid characters = File name contains invalid characters (wildcard characters are not supported);;File name '{0}' contains invalid characters (wildcard characters are not supported)
      InvalidCodePage = 2010, // Invalid code page = Code page is invalid or not installed;;Code page '{0}' is invalid or not installed
      InvalidDefinition = 2011, // Invalid definition = Definitions must be assigned a boolean or integral value;;Definition '{0}' must be assigned a boolean or integral value
      InvalidForCompilerOptions = 2037, // Invalid for CompilerOptions = Invalid option in CompilerOptions field;;'{0}' is not allowed in the CompilerOptions field
      InvalidLocaleID = 2012, // Invalid Locale ID = Locale ID is invalid or not installed;;Locale ID '{0}' is invalid or not installed
      InvalidPlatform = 2039, // Invalid platform = Platform is invalid. Specify 'x86', 'Itanium', 'x64', or 'anycpu';;Platform '{0}' is invalid. Specify 'x86', 'Itanium', 'x64', or 'anycpu'
      InvalidTarget = 2013, // Invalid target = Target type is invalid;;Target '{0}' is invalid.  Specify 'exe' or 'library'
      InvalidSourceFile = 2014, // Invalid source file = Invalid source file;;'{0}' is not a valid source file
      InvalidVersion = 2031, // Invalid version = Invalid version argument;;'{0}' is not a valid version string
      InvalidWarningLevel = 2015, // Invalid warning level = Invalid warning level;;Invalid warning level specified with option '{0}'
      ManagedResourceNotFound = 2022, // Managed resource not found = Managed resource not found;;Managed resource '{0}' was not found
      MissingDefineArgument = 2018, // Missing define argument = Missing identifier for '-define:' command line option
      MissingExtension = 2019, // Missing extension = Missing file extension;; Missing file extension for '{0}'
      MissingLibArgument = 2020, // Missing lib argument = Missing file specification for '-lib:' command line option
      MissingReference = 2034, // Missing reference = Missing reference in argument list;;Missing reference in argument list '{0}'
      MissingVersionInfo = 2021, // Missing version info = Missing version info in -assembly.version option
      MultipleOutputNames = 2016, // Multiple output filenames = Multiple output filenames specified
      MultipleTargets = 2017, // Multiple targets = Multiple targets specified
      MultipleWin32Resources = 2033, // Multiple win32resources = Cannot specify more than one Win32 resource file
      NestedResponseFiles = 2023, // Nested response files = Cannot process nested response files
      NoCodePage = 2024, // No code page = No code page is specified;;No code page is specified with option '{0}'
      NoFileName = 2025, // No filename = No filename specified;;No filename specified with option '{0}'
      NoInputSourcesSpecified = 2026, // No input sources specified = No input sources specified
      NoLocaleID = 2027, // No Locale ID = No Locale ID is specified;;No Locale ID is specified with option '{0}'
      NoWarningLevel = 2028, // No warning level = No warning level specified;;No warning level specified with option '{0}'
      ResourceNotFound = 2029, // Resource not found = Win32 resource not found;;Win32 resource '{0}' was not found
      SourceFileTooBig = 2032, // Source file too big
      SourceNotFound = 2035, // Source not found = Source file not found;;Could not find file '{0}'
      UnknownOption = 2030, // Unknown option = Unknown option;;Unknown option '{0}'
      Unspecified = 2999, // Unspecified error; see message text for more details

      // LAST should be the updated to the value of the last assigned enumeration value
      LAST = InvalidPlatform // 2038
    }
}
