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

using System;
using System.IO;
using System.Collections;
using System.Collections.Specialized;
using System.Globalization;
using System.Reflection;
using System.Reflection.Emit;
using System.Resources;
using System.Text;
using System.Text.RegularExpressions;
using System.Security.Policy;
using Microsoft.Vsa;
using Microsoft.JScript;
using Microsoft.JScript.Vsa;


class CompilerOptions{
  public bool autoRef;
  public bool fDebug;
  public bool fFast;
  public bool fNoStdlib;
  public PEFileKinds PEFileKind;
  
  // The platform option sets PEKind and MachineTarget.
  public PortableExecutableKinds PEKindFlags;
  public ImageFileMachine PEMachineArchitecture;
  
  public bool fTreatWarningsAsErrors;
  public bool fVersionSafe;
  public bool fPrint;
  public int nWarningLevel;
  public string strOutputFileName;
  public ArrayList SourceFileNames;
  public ArrayList ImportFileNames;
  public Hashtable ManagedResourceFileNames;
  public Hashtable ManagedResources;
  public int codepage;  // Used for converting source files to UNICODE
  public bool fForceCodepage; // Set if user explictly set the codepage
  public Version versionInfo;
  public string libpath;
  public Hashtable Defines;

  public bool autoRefSetExplicitly;
  public bool fTargetSpecified;
  public bool fPrintSetExplicitly;

  public CompilerOptions(){
    this.autoRef = true;
    this.PEFileKind = PEFileKinds.ConsoleApplication;
    this.PEKindFlags = PortableExecutableKinds.ILOnly;
    this.PEMachineArchitecture = ImageFileMachine.I386;
    this.fFast = true;
    this.fPrint = true;
    this.nWarningLevel = 4;
    this.SourceFileNames = new ArrayList();
    this.ImportFileNames = new ArrayList();
    this.ManagedResourceFileNames = new Hashtable(10);
    this.ManagedResources = new Hashtable(10);
    this.Defines = new Hashtable();
    string libpath = System.Environment.GetEnvironmentVariable("LIB");
    if (libpath != null)
      this.libpath = libpath;
    else
      this.libpath  = "";
  }
};

// This class contains the entry point. It simply creates an app-domain, sets fusion
// properties on it and creates an instance of the JScriptCompiler class in the app-domain.
// The latter class actually does the work for hosting the JScript engine.
class Compiler{
  public static int Main(string[] args){
    AppDomainSetup setup = new AppDomainSetup();

    // fusion settings
    setup.PrivateBinPath = "";
    setup.PrivateBinPathProbe = "*";  // disable loading from app base

    try {
      AppDomain appDomain = null;
      Evidence evidence = new Evidence(AppDomain.CurrentDomain.Evidence);
      appDomain = AppDomain.CreateDomain("Compiler", evidence, setup);
      JScriptCompiler jsc = (JScriptCompiler)(appDomain.CreateInstance(
        Assembly.GetAssembly(typeof(JScriptCompiler)).FullName, "JScriptCompiler").Unwrap());
      return jsc.Run(args);
    }
    catch (Exception e) {
      Console.WriteLine(JScriptCompiler.Localize("INTERNAL COMPILER ERROR", e.Message));
      return 10;
    }
    catch {
      Console.WriteLine(JScriptCompiler.Localize("INTERNAL COMPILER ERROR"));
      return 10;
    }
  }
}

class JScriptCompiler : MarshalByRefObject {
  private bool fPrintBanner;
  private bool fBannerPrinted;
  private bool fPrintTargets;
  private int exitCode;
  private bool fHelp;
  private bool fUtf8Output;
  private ArrayList targets;
  private CompilerOptions currentTarget;
  private int codeItemCounter;

  internal const String ContextStringDelimiter = ";;";
  private static int LCID = CultureInfo.CurrentUICulture.LCID;
  private static readonly ResourceManager resourceManager = new ResourceManager("JSC", typeof(JScriptCompiler).Module.Assembly);

  public int Run(string[] args){
    this.exitCode = 0;

    try{
      this.fPrintBanner = true;
      this.fBannerPrinted = false;
      this.fPrintTargets = false;
      this.fHelp = false;
      this.fUtf8Output = false;
      this.targets = new ArrayList();
      this.currentTarget = null;

      if (args.Length == 0){
        PrintUsage();
        return 0;
      }
      // preprocess the entire command line before compilation begins
      this.ParseArguments(args, false);

      // set the encoding option here in case the user wants help in UTF-8
      if (this.fUtf8Output){
        Stream s = Console.OpenStandardOutput();
        StreamWriter sw = new StreamWriter(s, new System.Text.UTF8Encoding(false));
        sw.AutoFlush = true;
        Console.SetOut(TextWriter.Synchronized(sw));
      }
      // do not compile if help is asked for
      if (this.fHelp){
        PrintUsage();
        return 0;
      }
      // final error checking before beginning compilation
      foreach(CompilerOptions target in this.targets){
        if (target.SourceFileNames.Count == 0)
          throw new CmdLineException(CmdLineError.NoInputSourcesSpecified, JScriptCompiler.GetCultureInfo());
      }
      // We will print out the names of the files we are generating if
      // there there are multiple compilations
      if (this.targets.Count > 1)
        this.fPrintTargets = true;

      PrintBanner();

      foreach(CompilerOptions target in this.targets){
        if (!target.fForceCodepage){
          // set the codepage if and only if the codepage hasn't been set explicitly
          target.codepage = JScriptCompiler.GetCultureInfo().TextInfo.ANSICodePage;
        }
        if (target.strOutputFileName == null){
          string outputName = Path.GetFileNameWithoutExtension((string)target.SourceFileNames[0]);
          target.strOutputFileName = outputName + (target.PEFileKind == PEFileKinds.Dll? ".dll" : ".exe");
        }
        if (!Compile(target))
          this.exitCode = 10;
      }
    }catch(CmdLineException e){
      PrintBanner();
      Console.WriteLine(e.Message);
      this.exitCode = 10;
    }catch(Exception e){
      PrintBanner();
      Console.WriteLine("fatal error JS2999: " + e.Message);
      this.exitCode = 10;
    }catch{
      PrintBanner();
      Console.WriteLine("fatal error JS2999: INTERNAL COMPILER ERROR");
      this.exitCode = 10;
    }
    return this.exitCode;
  }

  private void ParseArguments(string[] args, bool fromResponseFile){
    int start = 0;
    while (start < args.Length){
      if (this.currentTarget == null)
        this.currentTarget = new CompilerOptions();
      start = this.ParseOptions(args, start, fromResponseFile);
      start = this.ParseSources(args, start);
      if (!fromResponseFile || (start < args.Length-1)){
        this.targets.Add(this.currentTarget);
        this.currentTarget = null;
      }
    }
  }

  private int ParseOptions(string[] args, int start, bool fromResponseFile){
    // Process per-target options
    int i;
    for (i = start; i < args.Length; i++){
      object argument;
      string option;

      string arg = args[i];
      if (arg == null || arg.Length == 0) continue;

      // Process response file option
      if ('@' == arg[0]){
        if (fromResponseFile)
          throw new CmdLineException(CmdLineError.NestedResponseFiles, JScriptCompiler.GetCultureInfo());
        option = arg.Substring(1);
        if (option.Length == 0)
          throw new CmdLineException(CmdLineError.NoFileName, args[i], JScriptCompiler.GetCultureInfo());
        if (!File.Exists(option))
          throw new CmdLineException(CmdLineError.SourceNotFound, option, JScriptCompiler.GetCultureInfo());
        string[] rgstrFileArgs = ReadResponseFile(option);
        if (rgstrFileArgs != null)
          this.ParseArguments(rgstrFileArgs, true);
        continue;
      }

      // slash ('/') character is a valid filename character on UNIX, so we can't use it as a switch
      if ('-' != arg[0] && ('/' != arg[0] || '/' == Path.DirectorySeparatorChar))
        break;
      option = arg.Substring(1);

      if (option.Length >= 1){
        switch (option[0]){
        case '?':
        case 'h':
        case 'H':
          if (CmdLineOptionParser.IsSimpleOption(option, "?") || CmdLineOptionParser.IsSimpleOption(option, "help")){
            this.fHelp = true;
            continue;
          }
          break;
        case 'a':
        case 'A':
          argument = CmdLineOptionParser.IsBooleanOption(option, "autoref");
          if (argument != null){
            if ((bool)argument){
              this.currentTarget.autoRef = true;
              this.currentTarget.autoRefSetExplicitly = true;
            }else
              this.currentTarget.autoRef = false;
            continue;
          }
          argument = CmdLineOptionParser.IsArgumentOption(option, "a.version", "assembly.version");
          if (argument != null){
            if (((String)argument).Length == 0)
              throw new CmdLineException(CmdLineError.MissingVersionInfo, JScriptCompiler.GetCultureInfo());
            try {
              this.currentTarget.versionInfo = new Version((string)argument);
            }catch{
              throw new CmdLineException(CmdLineError.InvalidVersion, (string)argument, JScriptCompiler.GetCultureInfo());
            }
            continue;
          }
          break;
        case 'c':
        case 'C':
          argument = CmdLineOptionParser.IsArgumentOption(option, "codepage");
          if (argument != null){
            if (((string)argument).Length == 0){
              throw new CmdLineException(CmdLineError.NoCodePage, args[i], JScriptCompiler.GetCultureInfo());
            }else{
              try{
                this.currentTarget.codepage = Int32.Parse((string)argument, CultureInfo.InvariantCulture);
                this.currentTarget.fForceCodepage = true;
              }catch{
                throw new CmdLineException(CmdLineError.InvalidCodePage, (string)argument, JScriptCompiler.GetCultureInfo());
              }
              continue;
            }
          }
          break;
        case 'd':
        case 'D':
          argument = CmdLineOptionParser.IsBooleanOption(option, "debug");
          if (argument != null){
            this.currentTarget.fDebug = (bool)argument;
            continue;                        
          }
          argument = CmdLineOptionParser.IsArgumentOption(option, "d", "define");
          if (argument != null){
            Hashtable newDefines = GetAllDefines((string)argument);
            foreach (DictionaryEntry newDef in newDefines)
              this.currentTarget.Defines[newDef.Key] = newDef.Value;
            continue;
          }
          break;
        case 'f':
        case 'F':
          argument = CmdLineOptionParser.IsBooleanOption(option, "fast");
          if (argument != null){
            this.currentTarget.fFast = (bool)argument;
            continue;
          }
          break;
        case 'l':
        case 'L':
          argument = CmdLineOptionParser.IsArgumentOption(option, "lcid");
          if (argument != null){
            if (((string)argument).Length == 0)
              throw new CmdLineException(CmdLineError.NoLocaleID, args[i], JScriptCompiler.GetCultureInfo());
            try{
              int lcid = Int32.Parse((string)argument, CultureInfo.InvariantCulture);
              CultureInfo culture = new CultureInfo(lcid);
              JScriptCompiler.LCID = lcid;
            }catch{
              throw new CmdLineException(CmdLineError.InvalidLocaleID, (string)argument, JScriptCompiler.GetCultureInfo());
            }
            continue;
          }
          argument = CmdLineOptionParser.IsArgumentOption(option, "lib");
          if (argument != null){
            string newPaths = (string)argument;
            if (newPaths.Length == 0)
              throw new CmdLineException(CmdLineError.MissingLibArgument, JScriptCompiler.GetCultureInfo());
            newPaths = newPaths.Replace(',', Path.PathSeparator);
            this.currentTarget.libpath = newPaths + Path.PathSeparator + this.currentTarget.libpath;
            continue;
          }
          argument = CmdLineOptionParser.IsArgumentOption(option, "linkres", "linkresource");
          if (argument != null){
            ResInfo resinfo = new ResInfo((string)argument, true /* isLinked */);
            this.AddResourceFile(resinfo);
            continue;
          }
          break;
        case 'n':
        case 'N':
          if (CmdLineOptionParser.IsSimpleOption(option, "nologo")){
            this.fPrintBanner = false;
            continue;
          }
          argument = CmdLineOptionParser.IsBooleanOption(option, "nostdlib");
          if (argument != null){
            this.currentTarget.fNoStdlib = (bool)argument;
            if ((bool)argument && !this.currentTarget.autoRefSetExplicitly)
              this.currentTarget.autoRef = false;
            continue;
          }
          break;
        case 'o':
        case 'O':
          argument = CmdLineOptionParser.IsArgumentOption(option, "out");
          if (argument != null){
            try{
              if (Path.GetFileName((string)argument) == "")
                throw new CmdLineException(CmdLineError.NoFileName, args[i], JScriptCompiler.GetCultureInfo());
            }catch(System.ArgumentException){
              throw new CmdLineException(CmdLineError.NoFileName, args[i], JScriptCompiler.GetCultureInfo());
            }
            if (Path.GetExtension((string)argument) == "")
              throw new CmdLineException(CmdLineError.MissingExtension, args[i], JScriptCompiler.GetCultureInfo());
            if (this.currentTarget.strOutputFileName != null)
              throw new CmdLineException(CmdLineError.MultipleOutputNames, JScriptCompiler.GetCultureInfo());
            this.currentTarget.strOutputFileName = (string)argument;
            continue;
          }
          break;
        case 'p':
        case 'P':
          argument = CmdLineOptionParser.IsBooleanOption(option, "print");
          if (argument != null){
            this.currentTarget.fPrint = (bool)argument;
            this.currentTarget.fPrintSetExplicitly = (bool)argument;
            continue;
          }
          argument = CmdLineOptionParser.IsArgumentOption(option, "platform");
          if (argument != null){
            string platform = (string)argument;
            PortableExecutableKinds PEKindFlags;
            ImageFileMachine PEMachineArchitecture;
            if (String.Compare(platform, "x86", StringComparison.OrdinalIgnoreCase) == 0) {
              PEKindFlags = PortableExecutableKinds.ILOnly | PortableExecutableKinds.Required32Bit;
              PEMachineArchitecture = ImageFileMachine.I386;
            } else if (String.Compare(platform, "Itanium", StringComparison.OrdinalIgnoreCase) == 0) {
              PEKindFlags = PortableExecutableKinds.ILOnly | PortableExecutableKinds.PE32Plus;
              PEMachineArchitecture = ImageFileMachine.IA64;
            } else if (String.Compare(platform, "x64", StringComparison.OrdinalIgnoreCase) == 0) {
              PEKindFlags = PortableExecutableKinds.ILOnly | PortableExecutableKinds.PE32Plus;
              PEMachineArchitecture = ImageFileMachine.AMD64;            
            } else if (String.Compare(platform, "anycpu", StringComparison.OrdinalIgnoreCase) == 0) {
              PEKindFlags = PortableExecutableKinds.ILOnly;
              PEMachineArchitecture = ImageFileMachine.I386;
            } else
              throw new CmdLineException(CmdLineError.InvalidPlatform, (string)argument, JScriptCompiler.GetCultureInfo());
            this.currentTarget.PEKindFlags = PEKindFlags;
            this.currentTarget.PEMachineArchitecture = PEMachineArchitecture;
            continue;
          }
          break;
        case 'r':
        case 'R':
          argument = CmdLineOptionParser.IsArgumentOption(option, "r", "reference");
          if (argument != null){
            if (((string)argument).Length == 0)
              throw new CmdLineException(CmdLineError.NoFileName, args[i], JScriptCompiler.GetCultureInfo());
            foreach (string referenceName in GetAllReferences((string)argument)){
              bool duplicate = false;
              foreach (string assemblyName in this.currentTarget.ImportFileNames){
                if (String.Compare(assemblyName, referenceName, StringComparison.OrdinalIgnoreCase) == 0){
                  duplicate = true;
                  break;
                }
              }
              if (!duplicate)
                this.currentTarget.ImportFileNames.Add(referenceName);
            }
            continue;
          }
          argument = CmdLineOptionParser.IsArgumentOption(option, "res", "resource");
          if (argument != null){
            ResInfo resinfo = new ResInfo((string)argument, false /* isLinked */);
            this.AddResourceFile(resinfo);
            continue;
          }
          break;
        case 't':
        case 'T':
          argument = CmdLineOptionParser.IsArgumentOption(option, "t", "target");
          if (argument != null){
            if (this.currentTarget.fTargetSpecified){
              throw new CmdLineException(CmdLineError.MultipleTargets, JScriptCompiler.GetCultureInfo());
            }
            if (String.Compare((string)argument, "exe", StringComparison.OrdinalIgnoreCase) == 0){
              this.currentTarget.fTargetSpecified = true;
              this.currentTarget.PEFileKind = PEFileKinds.ConsoleApplication;
              continue;
            }
            if (String.Compare((string)argument, "winexe", StringComparison.OrdinalIgnoreCase) == 0){
              this.currentTarget.fTargetSpecified = true;
              this.currentTarget.PEFileKind = PEFileKinds.WindowApplication;
              this.currentTarget.fPrint = this.currentTarget.fPrintSetExplicitly;
              continue;
            }
            if (String.Compare((string)argument, "library", StringComparison.OrdinalIgnoreCase) == 0){
              this.currentTarget.fTargetSpecified = true;
              this.currentTarget.PEFileKind = PEFileKinds.Dll;
              continue;
            }
            throw new CmdLineException(CmdLineError.InvalidTarget, (string)argument, JScriptCompiler.GetCultureInfo());
          }
          break;
        case 'u':
        case 'U':
          argument = CmdLineOptionParser.IsBooleanOption(option, "utf8output");
          if (argument != null){
            this.fUtf8Output = (bool)argument;
            continue;
          }
          break;
        case 'v':
        case 'V':
          argument = CmdLineOptionParser.IsBooleanOption(option, "VersionSafe");
          if (argument != null){
            this.currentTarget.fVersionSafe = (bool)argument;
            continue;
          }
          break;
        case 'w':
        case 'W':
          argument = CmdLineOptionParser.IsArgumentOption(option, "w", "warn");
          if (argument != null){
            if (((string)argument).Length == 0){
              throw new CmdLineException(CmdLineError.NoWarningLevel, args[i], JScriptCompiler.GetCultureInfo());
            }
            if (1 == ((string)argument).Length){
              switch (((string)argument)[0]){
              case '0': this.currentTarget.nWarningLevel = 0; continue;
              case '1': this.currentTarget.nWarningLevel = 1; continue;
              case '2': this.currentTarget.nWarningLevel = 2; continue;
              case '3': this.currentTarget.nWarningLevel = 3; continue;
              case '4': this.currentTarget.nWarningLevel = 4; continue;
              }
            }
            throw new CmdLineException(CmdLineError.InvalidWarningLevel, args[i], JScriptCompiler.GetCultureInfo());
          }
          argument = CmdLineOptionParser.IsBooleanOption(option, "warnaserror");
          if (argument != null){
            this.currentTarget.fTreatWarningsAsErrors = (bool)argument;
            continue;
          }
          break;
        default:
          break;
        }
      }
      throw new CmdLineException(CmdLineError.UnknownOption, args[i], JScriptCompiler.GetCultureInfo());
    }
    return i;
  }

  private int ParseSources(string[] args, int start){
    // Read source files
    int i;
    for (i = start; i < args.Length; i++){
      if (args[i].Length == 0)
        continue;
      // slash ('/') character is a valid filename character on UNIX, so we can't break on it 
      // since it might be an absolute path to a file
      if ('-' == args[i][0] || ('/' == args[i][0] && '/' != Path.DirectorySeparatorChar))
        break;
      try{
        if (Path.GetFileName((string)args[i]) == "")
          throw new CmdLineException(CmdLineError.InvalidSourceFile, args[i], JScriptCompiler.GetCultureInfo());
      }catch(System.ArgumentException){
        throw new CmdLineException(CmdLineError.InvalidSourceFile, args[i], JScriptCompiler.GetCultureInfo());
      }
      foreach (string filename in this.currentTarget.ImportFileNames){
        if (String.Compare(filename, (string)args[i], StringComparison.OrdinalIgnoreCase) == 0){
          throw new CmdLineException(CmdLineError.DuplicateFileAsSourceAndAssembly, args[i], JScriptCompiler.GetCultureInfo());
        }
      }
      foreach (string filename in this.currentTarget.SourceFileNames){
        if (String.Compare(filename, (string)args[i], StringComparison.OrdinalIgnoreCase) == 0){
          throw new CmdLineException(CmdLineError.DuplicateSourceFile, args[i], JScriptCompiler.GetCultureInfo());
        }
      }
      this.currentTarget.SourceFileNames.Add(args[i]);
    }
    return i;
  }

  bool Compile(CompilerOptions options){
    if (this.fPrintTargets)
      Console.WriteLine(Localize("Compiling", options.strOutputFileName));

    VsaEngine engine = new Microsoft.JScript.Vsa.VsaEngine();
    if (null == engine)
      throw new CmdLineException(CmdLineError.CannotCreateEngine, JScriptCompiler.GetCultureInfo());
    engine.InitVsaEngine("JSC://Microsoft.JScript.Vsa.VsaEngine", new EngineSite(options));
    engine.LCID = JScriptCompiler.GetCultureInfo().LCID;
    engine.GenerateDebugInfo = options.fDebug;

    engine.SetOption("ReferenceLoaderAPI", "ReflectionOnlyLoadFrom");
    engine.SetOption("AutoRef", options.autoRef);
    engine.SetOption("fast", options.fFast);
    engine.SetOption("output", options.strOutputFileName);
    engine.SetOption("PEFileKind", options.PEFileKind);
    engine.SetOption("PortableExecutableKind", options.PEKindFlags);
    engine.SetOption("ImageFileMachine", options.PEMachineArchitecture);
    engine.SetOption("print", options.fPrint);
    engine.SetOption("libpath", options.libpath);
    if (options.versionInfo != null)
      engine.SetOption("version", options.versionInfo);
    engine.SetOption("VersionSafe", options.fVersionSafe);
    engine.SetOption("defines", options.Defines);
    engine.SetOption("warnaserror", options.fTreatWarningsAsErrors);
    engine.SetOption("WarningLevel", options.nWarningLevel);
    if (options.ManagedResources.Count > 0)
      engine.SetOption("managedResources", options.ManagedResources.Values);

    bool fStdlibAdded = false;
    bool fWinFormsAdded = false;
    foreach (string assemblyName in options.ImportFileNames){
      AddAssemblyReference(engine, assemblyName);
      string filename = Path.GetFileName(assemblyName);
      if (String.Compare(filename, "mscorlib.dll", StringComparison.OrdinalIgnoreCase) == 0)
        fStdlibAdded = true;
      else if (String.Compare(filename, "System.Windows.Forms.dll", StringComparison.OrdinalIgnoreCase) == 0)
        fWinFormsAdded = true;
    }

    // Only add mscorlib if it hasn't already been added.
    if (!options.fNoStdlib && !fStdlibAdded)
      AddAssemblyReference(engine, "mscorlib.dll");
    // add System.Windows.Forms if target is winexe and it hasn't already been added
    if ((options.PEFileKind == PEFileKinds.WindowApplication) && !options.fNoStdlib && !fWinFormsAdded)
      AddAssemblyReference(engine, "System.Windows.Forms.dll");

    for (int j = 0; j < options.SourceFileNames.Count; j++)
      AddSourceFile(engine, (string)options.SourceFileNames[j], options.codepage);

    bool isCompiled = false;
    try{
      isCompiled = engine.Compile();
    }catch(VsaException e){
      // check for expected errors
      if (e.ErrorCode == VsaError.AssemblyExpected){
        if (e.InnerException != null && e.InnerException is System.BadImageFormatException){
          // the reference was not for an assembly
          CmdLineException cmdLineError = new CmdLineException(CmdLineError.InvalidAssembly, e.Message, JScriptCompiler.GetCultureInfo());
          Console.WriteLine(cmdLineError.Message);
        }else if (e.InnerException != null && (e.InnerException is System.IO.FileNotFoundException || e.InnerException is System.IO.FileLoadException)){
          // the referenced file not valid
          CmdLineException cmdLineError = new CmdLineException(CmdLineError.AssemblyNotFound, e.Message, JScriptCompiler.GetCultureInfo());
          Console.WriteLine(cmdLineError.Message);
        }else{
          CmdLineException cmdLineError = new CmdLineException(CmdLineError.InvalidAssembly, JScriptCompiler.GetCultureInfo());
          Console.WriteLine(cmdLineError.Message);
        }
      }else if (e.ErrorCode == VsaError.SaveCompiledStateFailed){
        CmdLineException cmdLineError = new CmdLineException(CmdLineError.ErrorSavingCompiledState, e.Message, JScriptCompiler.GetCultureInfo());
        Console.WriteLine(cmdLineError.Message);
      }else if (e.ErrorCode == VsaError.AssemblyNameInvalid && e.InnerException != null){
        CmdLineException cmdLineError = new CmdLineException(CmdLineError.InvalidCharacters, e.Message, JScriptCompiler.GetCultureInfo());
        Console.WriteLine(cmdLineError.Message);
      }else{
        Console.WriteLine(Localize("INTERNAL COMPILER ERROR"));
        Console.WriteLine(e);
      }
      return false;
    }catch(Exception e){
      Console.WriteLine(Localize("INTERNAL COMPILER ERROR"));
      Console.WriteLine(e);
      return false;
    }catch{
      Console.WriteLine(Localize("INTERNAL COMPILER ERROR"));
      return false;
    }
    return isCompiled;
  }

  private static void AddAssemblyReference(IVsaEngine engine, string fileName){
    IVsaReferenceItem item;
    item = (IVsaReferenceItem)engine.Items.CreateItem(fileName, VsaItemType.Reference, VsaItemFlag.None);
    item.AssemblyName = fileName;
  }

  private void AddSourceFile(IVsaEngine engine, string fileName, int codepage){
    // We don't use the filename as the item name because filenames are allowed to contain
    // things that identifiers aren't (like commas and periods).
    string itemName = "$SourceFile_" + this.codeItemCounter++;
    IVsaCodeItem item = (IVsaCodeItem)engine.Items.CreateItem(itemName, VsaItemType.Code, VsaItemFlag.None);
    item.SetOption("codebase", fileName);
    item.SourceText = ReadFile(fileName, codepage);
  }

  private void AddResourceFile(ResInfo resinfo){
    if (!File.Exists(resinfo.fullpath))
      throw new CmdLineException(CmdLineError.ManagedResourceNotFound, resinfo.filename, JScriptCompiler.GetCultureInfo());
    // duplicate resource references are an error - either linking and embedding the same
    // file, naming two resources the same, or linking or embedding a file more than once
    if (this.currentTarget.ManagedResourceFileNames[resinfo.fullpath] != null)
      throw new CmdLineException(CmdLineError.DuplicateResourceFile, resinfo.filename, JScriptCompiler.GetCultureInfo());
    if (this.currentTarget.ManagedResources[resinfo.name] != null)
      throw new CmdLineException(CmdLineError.DuplicateResourceName, resinfo.name, JScriptCompiler.GetCultureInfo());
    this.currentTarget.ManagedResources[resinfo.name] = resinfo;
    this.currentTarget.ManagedResourceFileNames[resinfo.fullpath] = resinfo;
  }

  // === Helper routines ===

  internal void PrintBanner(){
    PrintBanner(null);
  }

  internal void PrintBanner(CultureInfo culture){
    if (!this.fBannerPrinted && this.fPrintBanner){
      string strJSVersion, strRuntimeVersion;
      strJSVersion = BuildVersionInfo.MajorVersion
                     + "." + BuildVersionInfo.MinorVersion.ToString(CultureInfo.InvariantCulture).PadLeft(2, '0')
                     + "." + BuildVersionInfo.Build.ToString(CultureInfo.InvariantCulture).PadLeft(4, '0');
      Version version = System.Environment.Version;
      strRuntimeVersion = version.Major
                       + "." + version.Minor.ToString(CultureInfo.InvariantCulture)
                       + "." + version.Build.ToString(CultureInfo.InvariantCulture).PadLeft(4, '0');
      this.fBannerPrinted = true;
      Console.WriteLine(String.Format(culture, Localize("Banner line 1", culture), strJSVersion));
      Console.WriteLine(String.Format(culture, Localize("Banner line 2", culture), strRuntimeVersion));
      Console.WriteLine(Localize("Banner line 3", culture) + "\r\n");
    }
  }

  internal void PrintUsage(){

    // if '/' separates dir, use '-' as switch
    string cmdLineSwitch = Path.DirectorySeparatorChar == '/' ? "-" : "/" ;

    CultureInfo culture = JScriptCompiler.GetCultureInfo();
    PrintBanner(culture);
    Console.WriteLine(Localize("Usage format", culture));
    Console.WriteLine();
    Console.Write("                         ");
    Console.WriteLine(Localize("Usage options", culture));
    Console.WriteLine("");
    Console.Write("                           ");
    Console.WriteLine(Localize("Output Files", culture));
    Console.Write("  " + cmdLineSwitch + "out:<file>              ");
    Console.WriteLine(Localize("Usage /out", culture));
    Console.Write("  " + cmdLineSwitch + "t[arget]:exe            ");
    Console.WriteLine(Localize("Usage /t:exe", culture));
    Console.Write("  " + cmdLineSwitch + "t[arget]:winexe         ");
    Console.WriteLine(Localize("Usage /t:winexe", culture));
    Console.Write("  " + cmdLineSwitch + "t[arget]:library        ");
    Console.WriteLine(Localize("Usage /t:library", culture));
    Console.Write("  " + cmdLineSwitch + "platform:<platform>     ");
    Console.WriteLine(Localize("Usage /platform", culture));
    Console.WriteLine();
    Console.Write("                           ");
    Console.WriteLine(Localize("Input Files", culture));
    Console.Write("  " + cmdLineSwitch + "autoref[+|-]            ");
    Console.WriteLine(Localize("Usage /autoref", culture));
    Console.Write("  " + cmdLineSwitch + "lib:<path>              ");
    Console.WriteLine(Localize("Usage /lib", culture));
    Console.Write("  " + cmdLineSwitch + "r[eference]:<file list> ");
    Console.WriteLine(Localize("Usage /reference", culture));
    Console.Write("                           ");
    Console.WriteLine("<file list>: <assembly name>[;<assembly name>...]");
    Console.WriteLine();
    Console.Write("                           ");
    Console.WriteLine(Localize("Resources", culture));
    Console.Write("  " + cmdLineSwitch + "res[ource]:<info>       ");
    Console.WriteLine(Localize("Usage /resource", culture));
    Console.WriteLine("                           <info>: <filename>[,<name>[,public|private]]");
    Console.Write("  " + cmdLineSwitch + "linkres[ource]:<info>   ");
    Console.WriteLine(Localize("Usage /linkresource", culture));
    Console.WriteLine("                           <info>: <filename>[,<name>[,public|private]]");
    Console.WriteLine();
    Console.Write("                           ");
    Console.WriteLine(Localize("Code Generation", culture));
    Console.Write("  " + cmdLineSwitch + "debug[+|-]              ");
    Console.WriteLine(Localize("Usage /debug", culture));
    Console.Write("  " + cmdLineSwitch + "fast[+|-]               ");
    Console.WriteLine(Localize("Usage /fast", culture));
    Console.Write("  " + cmdLineSwitch + "warnaserror[+|-]        ");
    Console.WriteLine(Localize("Usage /warnaserror", culture));
    Console.Write("  " + cmdLineSwitch + "w[arn]:<level>          ");
    Console.WriteLine(Localize("Usage /warn", culture));
    Console.WriteLine();
    Console.Write("                           ");
    Console.WriteLine(Localize("Miscellaneous", culture));
    Console.Write("  @<filename>              ");
    Console.WriteLine(Localize("Usage @file", culture));
    Console.Write("  " + cmdLineSwitch + "?                       ");
    Console.WriteLine(Localize("Usage /?", culture));
    Console.Write("  " + cmdLineSwitch + "help                    ");
    Console.WriteLine(Localize("Usage /?", culture));
    Console.Write("  " + cmdLineSwitch + "d[efine]:<symbols>      ");
    Console.WriteLine(Localize("Usage /define", culture));
    Console.Write("  " + cmdLineSwitch + "nologo                  ");
    Console.WriteLine(Localize("Usage /nologo", culture));
    Console.Write("  " + cmdLineSwitch + "print[+|-]              ");
    Console.WriteLine(Localize("Usage /print", culture));
    Console.WriteLine();
    Console.Write("                           ");
    Console.WriteLine(Localize("Advanced", culture));
    Console.Write("  " + cmdLineSwitch + "codepage:<id>           ");
    Console.WriteLine(Localize("Usage /codepage", culture));
    Console.Write("  " + cmdLineSwitch + "lcid:<id>               ");
    Console.WriteLine(Localize("Usage /lcid", culture));
    Console.Write("  " + cmdLineSwitch + "nostdlib[+|-]           ");
    Console.WriteLine(Localize("Usage /nostdlib", culture));
    Console.Write("  " + cmdLineSwitch + "utf8output[+|-]         ");
    Console.WriteLine(Localize("Usage /utf8output", culture));
    Console.Write("  " + cmdLineSwitch + "versionsafe[+|-]        ");
    Console.WriteLine(Localize("Usage /versionsafe", culture));
    Console.WriteLine();
  }

  internal static void PrintError(string sourceFile, int line, int column, bool fIsWarning, 
          int number, string message){
    string errorNumber = (10000 + (number & 0xFFFF)).ToString(CultureInfo.InvariantCulture).Substring(1);
    if (String.Compare(sourceFile, "no source", StringComparison.Ordinal) != 0)
      Console.Write(sourceFile + "(" + line + "," + column + ") : ");
    Console.WriteLine((fIsWarning ? "warning JS" : "error JS") + errorNumber + ": " + message);
  }

  private static int GetArgumentSeparatorIndex(string argList, int startIndex){
    int commaIndex = argList.IndexOf(",", startIndex);
    int semicolonIndex = argList.IndexOf(";", startIndex);
    if (commaIndex == -1)
      return semicolonIndex;
    if (semicolonIndex == -1)
      return commaIndex;
    if (commaIndex < semicolonIndex)
      return commaIndex;
    return semicolonIndex;
  }

  private static Hashtable GetAllDefines(string definitionList){
    Hashtable newDefines = new Hashtable();
    int startIndex = 0;
    int nextIndex;
    string definition;
    do{
      nextIndex = GetArgumentSeparatorIndex(definitionList, startIndex);
      if (nextIndex == -1)
        definition = definitionList.Substring(startIndex);
      else
        definition = definitionList.Substring(startIndex, nextIndex-startIndex);
      AddDefinition(definition, newDefines);
      startIndex = nextIndex+1;
    }while (nextIndex > -1);
    return newDefines;
  }

  private static StringCollection GetAllReferences(string fileList){
    StringCollection references = new StringCollection();
    int startIndex = 0;
    int nextIndex;
    string referenceName;
    do{
      nextIndex = GetArgumentSeparatorIndex(fileList, startIndex);
      if (nextIndex == -1)
        referenceName = fileList.Substring(startIndex).Trim();
      else
        referenceName = fileList.Substring(startIndex, nextIndex-startIndex).Trim();
      if (referenceName.Length == 0)
        throw new CmdLineException(CmdLineError.MissingReference, fileList, JScriptCompiler.GetCultureInfo());
      references.Add(referenceName);
      startIndex = nextIndex+1;
    }while (nextIndex > -1);
    return references;
  }

  private static void AddDefinition(string def, Hashtable definitions){
    int equalsIndex = def.IndexOf("=");
    string key;
    string strValue;
    object value = null;
    if (equalsIndex == -1){
      key = def.Trim();
      value = true;
    }else{
      key = def.Substring(0, equalsIndex).Trim();
      strValue = def.Substring(equalsIndex + 1).Trim();
      if (String.Compare(strValue, "true", StringComparison.OrdinalIgnoreCase) == 0)
        value = true;
      else if (String.Compare(strValue, "false", StringComparison.OrdinalIgnoreCase) == 0)
        value = false;
      else{
        try{
          value = Int32.Parse(strValue, CultureInfo.InvariantCulture);
        }catch{
          throw new CmdLineException(CmdLineError.InvalidDefinition, key, JScriptCompiler.GetCultureInfo());
        }
      }
    }
    if (key.Length == 0)
      throw new CmdLineException(CmdLineError.MissingDefineArgument, JScriptCompiler.GetCultureInfo());
    if ((key[0] == '_') && (key.CompareTo("_debug") == 0 ||
                           key.CompareTo("_fast") == 0 ||
                           key.CompareTo("_jscript") == 0 ||
                           key.CompareTo("_jscript_build") == 0 ||
                           key.CompareTo("_jscript_version") == 0 ||
                           key.CompareTo("_microsoft") == 0 ||
                           key.CompareTo("_win32") == 0 ||
                           key.CompareTo("_x86") == 0))
      throw new CmdLineException(CmdLineError.CompilerConstant, key, JScriptCompiler.GetCultureInfo());
    definitions[key] = value;
  }

  internal static string ReadFile(string fileName, int codepage){
    // Assert(null != fileName);
    // Assert(0 != codepage);
    FileStream inputStream;
    try{
      inputStream = new FileStream(fileName, FileMode.Open, FileAccess.Read, FileShare.Read);
    }catch(System.ArgumentException){
      throw new CmdLineException(CmdLineError.InvalidCharacters, fileName, JScriptCompiler.GetCultureInfo());
    }catch(System.IO.DirectoryNotFoundException){
      throw new CmdLineException(CmdLineError.SourceNotFound, fileName, JScriptCompiler.GetCultureInfo());
    }catch(System.IO.FileNotFoundException){
      throw new CmdLineException(CmdLineError.SourceNotFound, fileName, JScriptCompiler.GetCultureInfo());
    }
    
    try {
      Int64 size = inputStream.Length;
      if (size == 0)
        return "";
  
      Encoding encoding = null;
      try{
        encoding = Encoding.GetEncoding(codepage);
      }catch(System.ArgumentException){
        throw new CmdLineException(CmdLineError.InvalidCodePage, codepage.ToString(CultureInfo.InvariantCulture), JScriptCompiler.GetCultureInfo());
      }
  
      if (size > Int32.MaxValue)
        throw new CmdLineException(CmdLineError.SourceFileTooBig, JScriptCompiler.GetCultureInfo());
    
      using (StreamReader reader = new StreamReader(inputStream, encoding, true)) {
        return reader.ReadToEnd();
      }
    }finally{
      inputStream.Close();
    }
  }

  private static string[] ReadResponseFile(string strFileName){
    FileStream inputStream = new FileStream(strFileName, FileMode.Open,
                                            FileAccess.Read, FileShare.Read);
    Int64 size = inputStream.Length;
    if (size == 0)
      return null;

    StreamReader reader = new StreamReader(inputStream);

    string curLineArgs = reader.ReadLine();

    // This regular expression is: blank*(nonblanks|stringLiteral)+
    string strReArgs = "\\s*([^\\s\\\"]|(\\\"[^\\\"\\n]*\\\"))+";
    Regex re = new Regex(strReArgs);

    StringCollection args = new StringCollection();

    while (curLineArgs != null){
      if (!curLineArgs.Trim().StartsWith("#", StringComparison.Ordinal)){
        MatchCollection matches = re.Matches(curLineArgs);
        if (matches != null && matches.Count != 0){
          foreach (Match match in matches){
            string arg = match.ToString().Trim();
            int iQuotes = 0;
            while ((iQuotes = arg.IndexOf("\"", iQuotes)) != -1){
              if (iQuotes == 0)
                arg = arg.Substring(1);
              else if (arg[iQuotes-1] == '\\')
                iQuotes += 1;
              else
                arg = arg.Remove(iQuotes, 1);
            }
            args.Add(arg);
          }
        }
      }
      curLineArgs = reader.ReadLine();
    }
    if (args.Count == 0)
      return null;
    string[] argStrings = new string[args.Count];
    args.CopyTo(argStrings, 0);
    return argStrings;
  }

  internal static String Localize(String s){
    return JScriptCompiler.Localize(s, null, null);
  }

  internal static String Localize(String s, String context){
    return JScriptCompiler.Localize(s, context, null);
  }

  internal static String Localize(String s, CultureInfo culture){
    return JScriptCompiler.Localize(s, null, culture);
  }

  internal static String Localize(String s, String context, CultureInfo culture){
    try{
      if (culture == null)
        culture = JScriptCompiler.GetCultureInfo();
      String localizedString = JScriptCompiler.resourceManager.GetString(s, culture);
      if (localizedString != null){
        // find the delimiter at the end of the no-context string
        int splitAt = localizedString.IndexOf(ContextStringDelimiter);
        if (splitAt == -1){
          // there is no context-specific string
          return localizedString;
        }else if (context != null){
          // splitAt is two characters before the beginning of the context string
          return String.Format(localizedString.Substring(splitAt+2), context);
        }else{
          // splitAt is one character past the end of the no-context string
          return localizedString.Substring(0, splitAt);
        }
      }
    }catch(MissingManifestResourceException){
    }
    return s;
  }

  internal static CultureInfo GetCultureInfo(){
    CultureInfo culture = null;
    try{
      culture = new CultureInfo(JScriptCompiler.LCID);
    }catch(ArgumentException){
    }
    if (culture == null)
      culture = CultureInfo.CurrentUICulture;
    
    // If the culture can't be displayed on the console, revert back to English.
    int codePage = System.Console.Out.Encoding.CodePage;
    if (codePage != culture.TextInfo.OEMCodePage && codePage != culture.TextInfo.ANSICodePage)
      culture = new CultureInfo("en-US");
    return culture;
  }
}


class EngineSite : IVsaSite{
  private CompilerOptions options;

  public EngineSite(CompilerOptions options){
    this.options = options;
  }

  // === IVsaSite ===
  public virtual bool OnCompilerError(IVsaError error){
    // Errors have severity 0, warnings have severities 1-4. Setting
    // nWarnLevel to 0 results in all warnings being masked.
    int nSeverity = error.Severity;
    if (nSeverity > this.options.nWarningLevel)
      return true;
    bool isWarning = (0 != nSeverity) && !this.options.fTreatWarningsAsErrors;
    JScriptCompiler.PrintError(error.SourceMoniker, error.Line, error.StartColumn, isWarning, error.Number, error.Description);
    // We want to keep on going as long as we can since it makes debugging easier,
    // thus we will never abort compilation (never return false)
    return true;            
  }

  public virtual object GetItemInfo(string strItemName){
    throw new VsaException(VsaError.CallbackUnexpected);
  }

  public virtual object GetGlobalInstance( string name ){
    throw new VsaException(VsaError.CallbackUnexpected);
  }

  public virtual object GetEventSourceInstance(string ItemName, string EventSourceName){
    throw new VsaException(VsaError.CallbackUnexpected);
  }

  public virtual void Notify(string notification, object value){
    throw new VsaException(VsaError.CallbackUnexpected);
  }

  public virtual void GetCompiledState(out byte[] pe, out byte[] debugInfo){
    pe = null;
    debugInfo = null;
    throw new VsaException(VsaError.CallbackUnexpected);
  }
}
