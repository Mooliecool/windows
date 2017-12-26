//------------------------------------------------------------------------------
// <copyright file="typefinder.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------


using System;
using System.Reflection;
using System.IO;
using System.Collections;
using System.Text;
using System.Runtime.InteropServices;
using System.Security;
using Microsoft.Win32;

class App
{
  public static void Main(string[] args)
  {
    FindType ft = new FindType();
    try
    {
      SetOptions(args, ft);      
      ft.Search();
    }
    catch(SecurityException)
    {
      Console.WriteLine("This sample has failed to run due to a security limitation!");
      Console.WriteLine("Try running the sample from a local drive or using CasPol.exe to turn off security.");
    }
    catch
    {
      PrintUsage();
    }      
  }

  // Prints out usage information to the user
  private static void PrintUsage()
  {
    Console.WriteLine();
    Console.WriteLine("Usage:");
    Console.WriteLine("  typefinder [Location] [MatchOptions] [ShowOptions] [MiscOptions] Name");
    Console.WriteLine();
    Console.WriteLine("  Where Location is:");
    Console.WriteLine("   -d:[Directory] Additional directory to search");

    Console.WriteLine();
    Console.WriteLine("  Where MatchOptions are:");
    Console.WriteLine("    -x   Name = Exact Type Name (including namespace)");
    Console.WriteLine("    -n   Name = Namespace ");
    Console.WriteLine("    -w   Match the name anywhere in the namespace");

    Console.WriteLine();
    Console.WriteLine("  Where ShowOptions are:");                  
    Console.WriteLine("    -i   Show Interfaces");
    Console.WriteLine("    -f   Show Fields");
    Console.WriteLine("    -p   Show Properties");
    Console.WriteLine("    -e   Show Events");
    Console.WriteLine("    -m   Show Methods");
    Console.WriteLine("    -a   Show All Info");
    Console.WriteLine("    -l   Show Module Information");
      
    Console.WriteLine();
    Console.WriteLine("  Where MiscOptions are:");                           
    Console.WriteLine("    -v   Verbose");      
    Console.WriteLine("    -r   For every type find display base type information");      
    Console.WriteLine("    -?   Prints usage information");      
      
    Console.WriteLine();
      
    Console.WriteLine("Examples:");
    Console.WriteLine();
      
    Console.WriteLine(" typefinder String");
    Console.WriteLine("   Finds all types that have 'String' as part of their type name");
    Console.WriteLine();
    Console.WriteLine(" typefinder -r String");
    Console.WriteLine("   Finds all types that have 'String' as part of their name.");
    Console.WriteLine("   and prints out all base classes");
    Console.WriteLine();
    Console.WriteLine(" typefinder -n System.Reflection");
    Console.WriteLine("   Displays all types in the 'System.Reflection' namespace.");
    Console.WriteLine(" typefinder -xipm System.String");
    Console.WriteLine("   Displays the interfaces, properties and methods on the 'System.String' type.");
    Console.WriteLine();
    Console.WriteLine(" typefinder -d:C:\\ -d:\"C:\\test\\Lib\" String");
    Console.WriteLine("   Searches DLLs C:\\ and C:\\Program Files\\Microsoft.NET\\FrameworkSDK\\Lib");
    Console.WriteLine("   as well as in the current directory.");
    Console.WriteLine();
      
    Console.WriteLine();
  }

  // Processes all of the options specified in the arguments   
  private static void SetOptions(string[] args, FindType ft)
  {
    if ( args.Length == 0 )
    {
      PrintUsage();
      return;
    }
      
    ft.VerbosePrint = false;

    char[] backslash = new char[1];
    backslash[0] = '\\';
      
    for( int i = 0; i < args.Length; i++ )
    {
      string curArg = args[i];
         
      if ( curArg[0] == '-' || curArg[0] == '/'  )
      {
        if (Char.ToUpper(curArg[1]) == 'D')
        {
          if (curArg.Length > 2 && curArg[2] == ':')
          {
            String dir = curArg.Substring(3).TrimEnd(backslash).ToUpper();

            if (dir != "")
              ft.DirAdd(dir);
            else
              Console.WriteLine("Directory not specified");
          }
          else
            Console.WriteLine("Directory not specified");
        }
        else
        {
          for ( int j = 1; j < curArg.Length; j++ )
          {
            switch( Char.ToUpper(curArg[j]) )
            {
              case 'X':
                ft.ExactMatchOnly = true;
                break;
              case 'R':
                ft.RecurseTypes = true;
                break;
              case 'V':
                ft.VerbosePrint = true;
                break;
              case 'N':
                ft.MatchOnlyNamespace = true;
                break;
              case 'W':
                ft.WideSearch = true;
                break;
              case 'I':
                ft.ShowInterfaces = true;
                break;
              case 'M':
                ft.ShowMethods = true;
                break;
              case 'F':
                ft.ShowFields = true;
                break;                     
              case 'P':
                ft.ShowProperties = true;
                break;                     
              case 'E':
                ft.ShowEvents = true;
                break;                     
              case 'L':
                ft.ShowModuleInfo = true;
                break;                     
              case 'A':
                ft.ShowAll();
                break;                     
              case '?':
                PrintUsage();
                break;         
              case 'D':
                Console.WriteLine("Directory not specified");
                break;
              default:
                Console.WriteLine("Invald Option[{0}]", curArg[j] );                     
                break;
            }
          }
        }            
      }
      else
      {
        ft.ClassAdd( curArg );
      }
    }
  }   
}

// Utility class that holds methods that can be used to find and 
// display information about types. Use this utility to:
//          
// (1) Find a specific type
// (2) Optionally display information about the type including 
//     interfaces, properties, events and methods
//          
// This utility dumps information directly contained in the type 
// specified. In order to get a complete dump of a type and all 
// base type information you must use the "recurse" mode of the 
// utility.
class FindType
{   
  //   Searches based on settings set in the instance
  public void Search()
  {
    if ( myClassList.Count != 0 )
    {
      for ( int i = 0; i < myClassList.Count; i++ )
      {
        Search( (string)myClassList[i] );
      }
    }      
  }
   
  // Searchs for the passed string in the type names of all
  // assemblies in the specified paths.   
  public void Search(String theSearchString)
  {
    //
    // Search the directory where mscorlib is located.
    //
    myVerboseWriter.WriteLine("Searching BCL");
      
    ArrayList l = new ArrayList();
    BuildDLLFileList( System.Runtime.InteropServices.RuntimeEnvironment.GetRuntimeDirectory() , l );
    for ( int j = 0; j < l.Count; j++ )
    {
      Search(theSearchString, (String)l[j]);
    }

    //
    // Search the current directory
    //
    myVerboseWriter.WriteLine("Searching the current directory...");
    l = new ArrayList();
    BuildDLLFileList( "." , l );
    for ( int j = 0; j < l.Count; j++ )
    {
      Search(theSearchString, (String)l[j]);
    }
      
    //
    // Search the specified directories
    //
    Object[] dir = DirList.ToArray();
    for ( int i = 0; i < dir.Length; i++ )
    {
      myVerboseWriter.WriteLine("Searching directory  {0}...", dir[i]);
         
      l = new ArrayList();
      BuildDLLFileList( (String)dir[i], l );
         
      for ( int j = 0; j < l.Count; j++ )
      {
        Search(theSearchString, (String)l[j]);
      }
    }
  }
   
  // Loads the specified module and searchs through all 
  // the types defined in it for the type name specified.   
  public void Search(String theSearchString, String theModule )
  {
    try
    {
      //
      // Load the module - expect to fail if it is not an assembly or module.
      //

      Assembly a = Assembly.LoadFrom(theModule);
      Module[] m = a.GetModules();

      // Case insensitive
      theSearchString = theSearchString.ToUpper();
         
      for ( int j = 0; j < m.Length; j++ )
      {

        myVerboseWriter.WriteLine("Searching Module {0}", theModule );
               
        if ( m!= null )
        {
          //
          // Get all the types from the module
          //
          Type[] types = m[j].GetTypes();
            
          for (int i=0; i < types.Length; i++)
          {
            Type curType = types[i];

            // Case insensitive
            String name = curType.FullName.ToUpper();
               
            //
            // How has the user indicated they want to search. Note that 
            // even if the user has specified several ways to search we only 
            // respect one of them (from most specific to most general)
            //                  
            if ( ExactMatchOnly )
            {
              if ( name == theSearchString )
              {
                DumpType( curType );
              }                                    
            }
            else if ( MatchOnlyNamespace )
            {
              if ( curType.Namespace != null )
              {
                if ( curType.Namespace.ToUpper() == theSearchString )
                {
                  DumpType( curType );
                }                  
              }                     
            }
            else if ( WideSearch )
            {
              if ( curType.Namespace.ToUpper().IndexOf(theSearchString) != -1 )
              {
                DumpType( curType );
              }                                                      
            }
            else
            {
              //
              // User has not specified a search criteria - so we have some 
              // defaults:
              //   (1) If the search string supplied matches a complete type
              //       name then assume they want to get all information about 
              //       the type. If they have actually set display options then 
              //       respect them.
              //   (2) If we are going to dump all information save the show 
              //       properties so they can be reset after this type (in case the 
              //       user is searching for multiple types).
              //
              if ( name == theSearchString )
              {
                int oldOptions = showOptions;
                     
                if ( showOptions == 0 )
                {
                  ShowAll();
                }
                        
                DumpType( curType );
                        
                showOptions = oldOptions;
              }
              else if ( name.IndexOf(theSearchString) != -1 )
              {
                DumpType( curType );
              }
            }                                    
          }               
        }         
            
      }

    }
    catch(ReflectionTypeLoadException rcle)
    {

      Type[] loadedTypes     = rcle.Types;
      Exception[] exceptions = rcle.LoaderExceptions;
               
      int exceptionCount = 0;
         
      for ( int i =0; i < loadedTypes.Length; i++ )
      {
        if ( loadedTypes[i] == null )
        {
          // The following line would output the TypeLoadException.
          // myWriter.WriteLine("Unable to load a type because {0}", exceptions[exceptionCount] );
          exceptionCount++;      
        }
      }
    }
    catch(FileNotFoundException fnfe)
    {
      myVerboseWriter.WriteLine(fnfe.Message);
    }
    catch{}
  }

  // Do an exact match of passed types - the name passed must 
  // be the same as the fully qualified type name.   
  public bool ExactMatchOnly
  {
    get { return exactMatchOnly; }
    set { exactMatchOnly = value; }
  }

  // Load all base types and display the same information 
  // (methods, properties etc) for the base type.   
  public bool RecurseTypes
  {
    get { return recurseTypes; }
    set { recurseTypes = value; }
  }
               
  // Match only against the namespace component of a type
  // Use this to display all members of a namespace.   
  public bool MatchOnlyNamespace
  {
    get { return matchOnlyNamespace; }
    set { matchOnlyNamespace = value; }
  }

  // Match anywhere in the name (namespace and type name)
  public bool WideSearch
  {
    get { return wideSearch; }
    set { wideSearch = value; }
  }

  // Print verbose information   
  public bool VerbosePrint
  {
    get { return myVerboseWriter.Print; }
    set { myVerboseWriter.Print = value; }
  }
   
  // Show any interfaces implemented by this type   
  public bool ShowInterfaces
  {
    get { return (showOptions & SHOW_INTERFACES) != 0; }
    set 
    { 
      if ( value )
        showOptions |= SHOW_INTERFACES; 
      else
        showOptions &= ~SHOW_INTERFACES; 
    }
  }
   
  // Show any public fields in the type found   
  public bool ShowFields 
  {
    get { return (showOptions & SHOW_FIELDS) != 0; }
    set 
    { 
      if ( value )
        showOptions |= SHOW_FIELDS; 
      else
        showOptions &= ~SHOW_FIELDS; 
    }
  }
   
  // Show any public properties in the type found   
  public bool ShowProperties
  {
    get { return (showOptions & SHOW_PROPERTIES) != 0; }
    set 
    { 
      if ( value )
        showOptions |= SHOW_PROPERTIES; 
      else
        showOptions &= ~SHOW_PROPERTIES; 
    }
  }
   
  // Show any public events in the type found.   
  public bool ShowEvents
  {
    get { return (showOptions & SHOW_EVENTS) != 0; }
    set 
    { 
      if ( value )
        showOptions |= SHOW_EVENTS; 
      else
        showOptions &= ~SHOW_EVENTS; 
    }
  }
   
  // Show any public methods in the type found.   
  public bool ShowMethods
  {
    get { return (showOptions & SHOW_METHODS) != 0 ; }
    set 
    { 
      if ( value )
        showOptions |= SHOW_METHODS; 
      else
        showOptions &= ~SHOW_METHODS; 
    }
  }
   
  // Show the module information for any type found - this is useful when 
  // trying to determine what DLL contains what type.   
  public bool ShowModuleInfo
  {
    get { return (showOptions & SHOW_MODULE_INFO) != 0; }
    set 
    { 
      if ( value )
        showOptions |= SHOW_MODULE_INFO; 
      else
        showOptions &= ~SHOW_MODULE_INFO; 
    }
  }

  // Sets all display options on.   
  public void ShowAll()
  {
    ShowInterfaces = true;
    ShowMethods    = true;
    ShowFields     = true;
    ShowProperties = true;
    ShowEvents     = true;
    ShowModuleInfo = true;
  }   

  // Add a directory to the search set
  public void DirAdd(string dir)
  {
    DirList.Add(dir);      
  }

  // Add a class to the search set
  public void ClassAdd(string newClass)
  {
    myClassList.Add(newClass);      
  }   

  // Given a directory this method appends the fullly qualified path name of 
  // all library assemblies in the directory to the list passed in.   
  private void BuildDLLFileList(String directory, ArrayList list)
  {
    try
    {

      if ( Directory.Exists(directory) )
      {
        String[] e = Directory.GetFiles(directory, "*.dll");
      
        for(int i=0; i < e.Length; i++)
        {
          list.Add(e[i]);
        } 
      }
      else
      {
        myVerboseWriter.WriteLine("Directory [{0}] does not exist!", directory);
      }
   
    }
    catch(Exception Ex)
    {

      //
      // Trap Exception: System.InvalidOperationException  This occurs when the file is an unmanaged dll
      //
      if (Ex.GetType() == typeof(System.InvalidOperationException))
      {
      }               
    }            
  }
   
  // A short description of the type.   
  private String GetTypeDescription(Type aType)
  {
    String str = null;

    if ( aType.IsClass )
    {
      str = "class";
    }                                 
      
    if ( aType.IsInterface )
    {
      str = "interface";
    }                                 
      
    if ( aType.IsValueType )
    {
      str = "struct";
    } 
          
    if ( aType.IsArray )
    {
      str = "array";
    }                                 
      
    return str;
  }
   
  // Dumps information about the specified type.   
  private void DumpType(Type aType)
  {  
    Type baseType = aType.BaseType;

    myWriter.WriteLine("{0,-10} {1}", GetTypeDescription(aType), aType );
      
    if ( ShowModuleInfo )
    {
      myWriter.WriteLine("{0,-10} {1}", "Module:", aType.Module.FullyQualifiedName );
    }
               
    DumpInterfaces( aType );
    DumpFields( aType );
    DumpProperties( aType );
    DumpEvents( aType );         
    DumpMethods( aType );
      
    if ( RecurseTypes )
    {
      myWriter.WriteLine();
    }                                           
                   
    //
    // If recursing then pop the indent on the writers so we 
    // can easily see the nesting for the base type information.
    //                                       
    if ( RecurseTypes && baseType != null )
    {
      myWriter.PushIndent();
      myVerboseWriter.PushIndent();
            
      DumpType( baseType );
            
      myWriter.PopIndent();
      myVerboseWriter.PopIndent();
    }         
  }
     
  // Dumps the interfaces implemented by the specified type.   
  private void DumpInterfaces(Type aType)
  {
    if ( !ShowInterfaces )
      return;
         
    Type[] info = aType.GetInterfaces();
      
    if ( info.Length != 0 )
    {                                      
      myWriter.WriteLine("{0} {1}", "# Interfaces:", 
        info.Length ); 
           
      for ( int i=0; i < info.Length; i++ )
      {
         
        myWriter.PushIndent();
        myWriter.WriteLine("interface {0}", info[i].FullName);
           
        //
        // Only show method information only if requested
        //
        if ( ShowMethods )
        {
          myWriter.PushIndent();
          DumpType( info[i] );
          myWriter.PopIndent();           
        }

        myWriter.PopIndent();
      }
    }
  }

  // Dumps the public properties directly contained in the specified type   
  private void DumpProperties(Type aType)
  {
    if ( !ShowProperties )
      return;
         
    PropertyInfo[] pInfo = aType.GetProperties();
    myWriter.WriteLine("Properties");
      
    bool found = false;
      
    if ( pInfo.Length != 0 )
    {                                                       
      PropertyInfo curInfo = null;
                                   
      for ( int i=0; i < pInfo.Length; i++ )
      {
        curInfo = pInfo[i];
         
        //
        // Only display properties declared in this type.
        //          
        if ( curInfo.DeclaringType != aType )
        {
          continue;
        }
            
        found = true;
            
        String flags = null;
                   
        if ( curInfo.CanRead && curInfo.CanWrite )
        {
          flags = "get; set;";
        }
        else if ( curInfo.CanRead )
        {
          flags = "get";
        }
        else if ( curInfo.CanWrite )
        {
          flags = "set";
        }
           
        myWriter.WriteLine("  {0,-10} '{1}' ", curInfo, 
          flags);
      }
    }
      
    if ( !found )
    {
      myWriter.WriteLine("  (none)");
    }                 
  }
  
  // Dumps the public events directly contained in the specified type   
  private void DumpEvents(Type aType)
  {
    if ( !ShowEvents )
      return;
         
    EventInfo[] eInfo = aType.GetEvents( );
         
    myWriter.WriteLine("Events:");
    bool found = false;
      
    if ( eInfo.Length != 0 )
    {                                                    
      for ( int i=0; i < eInfo.Length; i++ )
      {        
        //
        // Only display events declared in this type.
        //          
        if ( eInfo[i].DeclaringType == aType )
        {
          found = true;
          myWriter.WriteLine("  {0}", eInfo[i]);
        }
      }
    }
      
    if ( !found )
    {
      myWriter.WriteLine("  (none)");
    }    
  }

  // Dumps the public fields directly contained in the specified type   
  private void DumpFields(Type aType)
  {
    if ( !ShowFields )
      return;

    FieldInfo[] info = aType.GetFields( );
         
    myWriter.WriteLine("Fields:");
      
    bool found = false;
      
    if ( info.Length != 0 )
    {
      for ( int i=0; i < info.Length; i++ )
      {        
        //
        // Only display fields declared in this type.
        //          
        if (info[i].DeclaringType == aType )
        {
          myWriter.WriteLine("  {0}", info[i] );
          found = true;
        }
      }
    }          
      
    if ( !found )
    {
      myWriter.WriteLine("  (none)");
    }                          
  }
  
  // Dumps the public methods directly contained in the specified type. 
  // Note "special name" methods are not displayed.   
  private void DumpMethods(Type aType)
  {
    if ( !ShowMethods )
      return;

    MethodInfo[] mInfo = aType.GetMethods();
      
    myWriter.WriteLine("Methods"); 
      
    bool found = false;         
      
    if ( mInfo.Length != 0 )
    {
      for ( int i=0; i < mInfo.Length; i++ )
      {
        //
        // Only display methods declared in this type. Also 
        // filter out any methods with special names - these
        // cannot be generally called by the user (i.e their 
        // functionality is usually exposed in other ways e.g
        // property get/set methods are exposed as properties.
        //          
        if ( mInfo[i].DeclaringType == aType && 
          !mInfo[i].IsSpecialName    
          )
        {      
          found = true;
                
          StringBuilder modifiers = new StringBuilder();
            
          if ( mInfo[i].IsStatic )   { modifiers.Append("static "); }     
          if ( mInfo[i].IsPublic )   { modifiers.Append("public "); }     
          if ( mInfo[i].IsFamily )   { modifiers.Append("protected "); }     
          if ( mInfo[i].IsAssembly ) { modifiers.Append("internal "); }     
          if ( mInfo[i].IsPrivate )  { modifiers.Append("private "); }     
            
          myWriter.WriteLine("  {0} {1}", modifiers ,mInfo[i]);
        }
      }                 
    }                
      
    if ( !found )
    {
      myWriter.WriteLine("  (none)");
    }
  }

  private const int SHOW_INTERFACES  = 0x01;      
  private const int SHOW_FIELDS      = 0x02;
  private const int SHOW_PROPERTIES  = 0x04;
  private const int SHOW_EVENTS      = 0x08;
  private const int SHOW_METHODS     = 0x10;
  private const int SHOW_MODULE_INFO = 0x20;

  private IndentedWriter myVerboseWriter = new IndentedWriter();
  private IndentedWriter myWriter        = new IndentedWriter();
   
  private bool exactMatchOnly      = false;
  private bool recurseTypes        = false;
  private bool matchOnlyNamespace  = false;
  private bool wideSearch          = false;   
         
  private int showOptions = 0;
         
  private ArrayList myClassList    = new ArrayList();
  private ArrayList DirList        = new ArrayList();   
}

// Utility class that performs basic text output operations that can be 
// indented via "push" and "pop". The text writer used to write 
// information out can be changed by setting the appropriate property.
// The default text writer is System.Console.Out
public class IndentedWriter
{
      
  // Controls whether printing is performed or not. This can be useful 
  // for clients that require a "verbose" writer that only displays 
  // information when the verbose mode is set   
  public bool Print
  {
    get { return myPrintFlag; }
    set { myPrintFlag = value; }
  }

  // Sets the TextWriter to use when writing strings out   
  public TextWriter TextWriter
  {
    get { return myTextWriter; }
    set { myTextWriter = value; }
  }
    
  public void PushIndent()
  {
    myIndent += 2;
  }  
   
  public void PopIndent()
  {
    if (myIndent > 0)
    {
      myIndent -= 2;
    }
  }
   
  // Writes out an empty line   
  public void WriteLine()
  {
    WriteLine("");
  }   

  // Writes a formatted string   
  public  void WriteLine(String format, params Object[] arg)
  {
    WriteLine( String.Format(format, arg) );    
  }
   
  // Writes out a string (indenting it as required)   
  public  void WriteLine(string message)
  {
    if (myPrintFlag)
    {
      StringBuilder sb = new StringBuilder();
         
      for (int i = 0; i < myIndent; i++)
      {
        sb.Append(' ');
      }
      sb.Append(message);         
      myTextWriter.WriteLine(sb.ToString());
    }
  }

  private TextWriter myTextWriter = Console.Out;
  private bool       myPrintFlag  = true;
  private int        myIndent     = 0;
}

