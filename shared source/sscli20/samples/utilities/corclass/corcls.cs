//------------------------------------------------------------------------------
// <copyright file="corcls.cs" company="Microsoft">
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


namespace DefaultNamespace 
{
  using System.ComponentModel;

  using System.Diagnostics;
  using System;
  using System.Reflection;
  using System.Collections;


  /// <summary>
  /// This utility displays the field, ctor, and method membership of a SSCLI class.
  /// </summary>
  public class corcls 
  {

    public static bool  showHelp = false;

    public static bool  dumpModules = false;

    public static bool  generateWeb = false;

    public static bool  showClassNameOnly = false;

    public String moduleName = null;

    private static String[] packagesToAttempt = new String[] {
                                                               "System",
                                                               "System.Reflection",
                                                               "System.Runtime.InteropServices",
                                                               "System.Collections",
                                                               "System.Globalization",
                                                             };

    /// <summary>
    /// FindClass() will locate the classname using various search rules.
    /// </summary>
    /// <param name="className"></param>
    /// <param name="module"></param>
    /// <returns></returns>
    protected static Type FindClass( String className, Assembly module ) 
    {
      Type cls = null;

      if (module == null) 
      {
        // find the class using regular methods
        cls = Type.GetType( className );

        // this failed, so try again with "System." in front
        if (cls == null) 
        {
          for (int i = 0; i < packagesToAttempt.Length; i++) 
          {
            cls = Type.GetType( packagesToAttempt[ i ] + "." + className );
            if (cls != null) 
            {
              break;
            }
          }
        }
      }
      else 
      {
        // find the class using the given module
        cls = module.GetType( className );

        // this failed, so try again with "System." in front
        if (cls == null) 
        {
          for (int i = 0; i < packagesToAttempt.Length; i++) 
          {
            cls = module.GetType( packagesToAttempt[ i ] + "." + className );
            if (cls != null) 
            {
              break;
            }
          }
        }
      }

      return cls;
    }
    
    /// <summary>
    ///  Main does all the real work.  call the arg processor, show the help, and
    ///  run throug the class list on the command line.
    /// </summary>
    /// <param name="args"></param>
    public static void Main (String[] args) 
    {
      corcls lister = new corcls();

      // processArgs() will set the switches and return an array of the non-
      // switch arguments -- the classnames to be displayed.
      String[] classList = lister.processArgs( args );

      // show help if the user asked, or if there were no classname args
      if (showHelp || (classList.Length == 0 && lister.moduleName == null)) 
      {
        Console.WriteLine( "corcls:  Shared Source CLI Runtime class display utility" );

        Console.WriteLine( "\nusage: corcls [options] <classname> [classname...]" );
        Console.WriteLine( "" );
        Console.WriteLine( "   options" );
        Console.WriteLine( "   -a <level>  abbreviation level [ short | medium | long ]" );
        Console.WriteLine( "   -w          generate HTML content" );
        Console.WriteLine( "   -f          force full package qualifiers" );
        Console.WriteLine( "   -m <module> load and search the named CLI assembly" );
        Console.WriteLine( "   -noinherit  don't display inherited methods and fields" );
        Console.WriteLine( "   -o          only show class names within modules (use with -m)" );
        Console.WriteLine( "   -q          be quiet, suppress errors" );
        Console.WriteLine( "   -v          visible (public & protected) classes/methods/fields only" );
        Console.WriteLine( "   -p          public classes, methods, or fields only" );
        Console.WriteLine( "   -h          Show this usage help" );
      }

      Assembly module = null;
      if (lister.moduleName != null)
        module = Assembly.LoadFrom( lister.moduleName );

      // there are no named classes, dump all in the module
      if (classList.Length == 0) 
      {
        lister.showModule( module );
      }
      else 
      {
        // for each classname in the list
        for (int i = 0; i < classList.Length; i++) 
        {
          if (classList[ i ].Equals( "*" ))
            lister.showModule( module );
          else 
          {
            // find the Class
            Type cls = FindClass( classList[ i ], module );

            // if null then there's a problem
            if (cls == null) 
            {
              if (!BaseGenerator.beQuiet)
                Console.WriteLine( "class " + classList[ i ] + " not found." );
            }
            else
              // or, we've got one, so show it.
              lister.showClass( cls );
          }
        }
      }
    }

    /// <summary>
    /// Shows the entire contents of the module.
    /// </summary>
    /// <param name="m"></param>
    internal virtual void showModule( Assembly m ) 
    {
      if (m == null)
        return;

      Type[] classes = m.ManifestModule.GetTypes();
      Array.Sort(classes, TypeComparer.comp);
      for (int j = 0; j < classes.Length; j++) 
      {
        if (BaseGenerator.Show(classes[j])) 
        {
          if (showClassNameOnly)
            Console.WriteLine( classes[ j ].FullName );
          else
            showClass( classes[j] );
        }
      }
    }

    /// <summary>
    /// showClass() will take the given Class instance and display all the
    /// members.
    /// </summary>
    /// <param name="cls"></param>
    internal virtual void showClass( Type cls ) 
    {
      if (generateWeb) 
      {
        Console.WriteLine(new WebGenerator().GetClassCode(cls));
      }
      else 
      {
        Console.WriteLine(new ConsoleGenerator().GetClassCode(cls));
      }
    }
   
    /// <summary>
    /// Process the arguments passed to the program.  Set all the switches, and
    /// return the non-switch arguments (classnames) in an array of strings.
    /// </summary>
    /// <param name="args"></param>
    /// <returns></returns>
    internal virtual String[] processArgs( String[] args ) 
    {
      String   p;
      String[] classList = new String[ 128 ];
      int      count = 0;

      bool nextIsModule = false;
      bool CouldBeFileName;

      for (int idx = 0; idx < args.Length; idx++) 
      {
        CouldBeFileName = true;
        p = args[idx];

        if (p[0] == '-' || p[0] == '/') 
        {
          CouldBeFileName = false;
          if (p.Length == 2)
          {
            switch (Char.ToLower( p[ 1 ] )) 
            {
              case 'a':
                // abbreviation level
                idx++;
                if (idx >= args.Length) 
                {
                  Console.WriteLine("Expected abbreviation level after -a");
                }
                else 
                {
                  string level = args[idx];
                  if (level.Equals("long"))
                    BaseGenerator.defaultAbbreviation = TypeNames.Long;
                  else if (level.Equals("medium"))
                    BaseGenerator.defaultAbbreviation = TypeNames.Medium;
                  else if (level.Equals("short"))
                    BaseGenerator.defaultAbbreviation = TypeNames.Short;
                  else
                    Console.WriteLine("unrecognized abbreviation level '" + level + "'; ignored");
                }
                break;

              case 'd':
                BaseGenerator.showAdvancedness = true;
                break;

                // show full package qualifiers on all type names
              case 'f':
                BaseGenerator.showFullNames = true;
                break;

              case 'm':
                if (moduleName == null)
                  // first module name is OK, moduleName will still be null
                  nextIsModule = true;
                else 
                {
                  Console.WriteLine( "only one module allowed; -m and next arg ignored" );
                  // skip the next arg, presumed to be a module name
                  idx++;
                }
                break;

              case 'n':
                BaseGenerator.showPackage = BaseGenerator.showPackage;
                BaseGenerator.showInherited = false;
                break;

                // show only the class names contained within the module (requires the -m option)
              case 'o':
                showClassNameOnly = true;
                break;

                // Don't show banner and class not found errors or other error output.
              case 'q':
                BaseGenerator.beQuiet = true;
                break;

              case 'p':
                BaseGenerator.showProtected = false;
                goto
                  case 'v';
              case 'v':
                BaseGenerator.showPackage = false;
                BaseGenerator.showPrivate = false;
                break;

              case 'w':
                generateWeb = true;
                break;

              case '?':
              case 'h':
                showHelp = true;
                break;
               
              default:
                Console.WriteLine( "unrecognized option '" + p + "'; ignored" );
                break;
            }
          }
          else if (String.Compare(args[idx], "-noinherit", true) == 0)
          {
            BaseGenerator.showPackage = BaseGenerator.showPackage;
            BaseGenerator.showInherited = false;
          }
#if PLATFORM_UNIX
                    else if (p[0] == '/')
                    {
                        // On UNIX system-based platforms it might be a fully qualified file.
                        CouldBeFileName = true;
                    }
#endif
          else
          {
            Console.WriteLine( "unrecognized option '" + p + "'; ignored" );
          }

        }
        if (CouldBeFileName)
        {
          // not a switch, must be a class module name
          if (nextIsModule) 
          {
            // it's a module
            moduleName = args[idx];
            nextIsModule = false;
          }
          else 
          {
            // it's a class
            String name = args[idx];

            // save off the old allowAbbrev setting so we can turn it on
            bool b = BaseGenerator.allowAbbreviations;
            BaseGenerator.allowAbbreviations = true;

            // leverage the allowAbbreviations setting to expand shortcuts
            name = BaseGenerator.AbbreviateName( name, "S.C", "System.Collections" );
            name = BaseGenerator.AbbreviateName( name, "S.I", "System.Runtime.InteropServices" );
            name = BaseGenerator.AbbreviateName( name, "S.R", "System.Reflection" );
            name = BaseGenerator.AbbreviateName( name, "S.S", "System.Security" );
            name = BaseGenerator.AbbreviateName( name, "S.X", "System.Xml" );
            name = BaseGenerator.AbbreviateName( name, "S", "System" );
            name = BaseGenerator.AbbreviateName( name, "S.R.R", "System.Runtime.Remoting" );
            name = BaseGenerator.AbbreviateName( name, "S.R.S.F.S", "System.Runtime.Serialization.Formatters.Soap" );
            name = BaseGenerator.AbbreviateName( name, "M", "Microsoft" );

            BaseGenerator.allowAbbreviations = b;

            classList[ count++ ] = name;
          }
        }
      }

      if (nextIsModule && moduleName == null)
        Console.WriteLine( "option -m not followed by module name; ignored" );

      // make a right-sized copy of the classlist array
      String[] newList = new String[ count ];
      Array.Copy( classList, 0, newList, 0, count );

      return newList;
    }
    
    
    protected class TypeComparer : IComparer 
    {
     public static readonly TypeComparer comp = new TypeComparer();

      public virtual int Compare(Object x, Object y) 
      {
        if (x == null || y == null) 
        {
          return 0;
        }

        string left = ((Type)x).FullName;
        string right = ((Type)y).FullName;
        int res = String.Compare(left, right);
        return res;
      }
    }
    
  }
}
