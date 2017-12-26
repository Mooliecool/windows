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
    using System.CodeDom;
    using System.CodeDom.Compiler;
    using System.Collections;
    using System.Collections.Specialized;
    using System.Globalization;
    using System.IO;
    using System.Reflection;
    using System.Reflection.Emit;
    using System.Text;
    using System.Text.RegularExpressions;
    using Microsoft.JScript.Vsa;
    using Microsoft.Vsa;

    internal class JSInProcCompiler{
      private int codeItemCounter = 0;
      private string debugCommandLine = null;

      private void AddAssemblyReference(IVsaEngine engine, string filename){
        IVsaReferenceItem item;
        item = (IVsaReferenceItem)engine.Items.CreateItem(filename, VsaItemType.Reference, VsaItemFlag.None);
        item.AssemblyName = filename;
      }

      private void AddDefinition(string def, Hashtable definitions, VsaEngine engine){
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
              throw new CmdLineException(CmdLineError.InvalidDefinition, key, engine.ErrorCultureInfo);
            }
          }
        }
        if (key.Length == 0)
          throw new CmdLineException(CmdLineError.MissingDefineArgument, engine.ErrorCultureInfo);
        definitions[key] = value;
      }

      private void AddResourceFile(ResInfo resinfo, Hashtable resources, Hashtable resourceFiles, VsaEngine engine){
        if (!File.Exists(resinfo.fullpath))
          throw new CmdLineException(CmdLineError.ManagedResourceNotFound, resinfo.filename, engine.ErrorCultureInfo);
        // duplicate resource references are an error - either linking and embedding the same
        // file, naming two resources the same, or linking or embedding a file more than once
        if (resourceFiles[resinfo.fullpath] != null)
          throw new CmdLineException(CmdLineError.DuplicateResourceFile, resinfo.filename, engine.ErrorCultureInfo);
        if (resources[resinfo.name] != null)
          throw new CmdLineException(CmdLineError.DuplicateResourceName, resinfo.name, engine.ErrorCultureInfo);
        resources[resinfo.name] = resinfo;
        resourceFiles[resinfo.fullpath] = resinfo;
      }

      private void AddSourceFile(VsaEngine engine, string filename){
        IVsaCodeItem item;
        string name = "$SourceFile_" + this.codeItemCounter++;
        item = (IVsaCodeItem)engine.Items.CreateItem(name, VsaItemType.Code, VsaItemFlag.None);
        item.SetOption("codebase", filename);
        item.SourceText = this.ReadFile(filename, engine);
      }

      internal int Compile(CompilerParameters options, string partialCmdLine, string[] sourceFiles, string outputFile){
        // write compiler output to a stream with outputFile as the name
        StreamWriter output = null;
        int nativeReturnValue = 0;
        try{
          // Compiler output must be UTF-8 encoded
          output = new StreamWriter(outputFile);
          output.AutoFlush = true;
          if (options.IncludeDebugInformation){
            Debug.Assert(partialCmdLine != null);
            this.PrintOptions(output, options);
            this.debugCommandLine = partialCmdLine;
          }
          VsaEngine engine = null;
          try{
            engine = this.CreateAndInitEngine(options, sourceFiles, outputFile, output);
          }catch(CmdLineException e){
            output.WriteLine(e.Message);
            nativeReturnValue = 10;
          }catch(Exception e){
            output.WriteLine("fatal error JS2999: " + e);
            nativeReturnValue = 10;
          }catch{
            output.WriteLine("fatal error JS2999");
            nativeReturnValue = 10;
          }
          if (engine == null)
            return nativeReturnValue;
          if (options.IncludeDebugInformation){
            // this.debugCommandLine was completed during CreateAndInitEngine (except for filenames)
            StringBuilder fullCmdLine = new StringBuilder(this.debugCommandLine);
            foreach (string sourceFile in sourceFiles){
              fullCmdLine.Append(" \"");
              fullCmdLine.Append(sourceFile);
              fullCmdLine.Append("\"");
            }
            this.debugCommandLine = fullCmdLine.ToString();
            // write the full command line to a response file for debugging
            string responseFileName = options.TempFiles.AddExtension("cmdline");
            StreamWriter responseFile = null;
            try{
              responseFile = new StreamWriter(responseFileName);
              responseFile.WriteLine(this.debugCommandLine);
              responseFile.Flush();
            }finally{
              if (responseFile != null)
                responseFile.Close();
            }
            // show full debug command line in compiler output
            StringBuilder sb = new StringBuilder();
            sb.Append(Environment.NewLine);
            sb.Append(JScriptException.Localize("CmdLine helper", CultureInfo.CurrentUICulture));
            sb.Append(":");
            sb.Append(Environment.NewLine);
            sb.Append("    ");
            sb.Append(options.TempFiles.TempDir);
            sb.Append("> jsc.exe @\"");
            sb.Append(responseFileName);
            sb.Append("\"");
            sb.Append(Environment.NewLine);
            output.WriteLine(sb.ToString());
            this.PrintBanner(engine, output);
          }
          try{
            if (!engine.Compile())
              nativeReturnValue = 10;
            else
              nativeReturnValue = 0;
          }catch(VsaException e){
            // check for expected errors
            if (e.ErrorCode == VsaError.AssemblyExpected){
              if (e.InnerException != null && e.InnerException is System.BadImageFormatException){
                // the reference was not for an assembly
                CmdLineException cmdLineError = new CmdLineException(CmdLineError.InvalidAssembly, e.Message, engine.ErrorCultureInfo);
                output.WriteLine(cmdLineError.Message);
              }else if (e.InnerException != null && e.InnerException is System.IO.FileNotFoundException){
                // the referenced file not valid
                CmdLineException cmdLineError = new CmdLineException(CmdLineError.AssemblyNotFound, e.Message, engine.ErrorCultureInfo);
                output.WriteLine(cmdLineError.Message);
              }else{
                CmdLineException cmdLineError = new CmdLineException(CmdLineError.InvalidAssembly, engine.ErrorCultureInfo);
                output.WriteLine(cmdLineError.Message);
              }
            }else if (e.ErrorCode == VsaError.SaveCompiledStateFailed){
              CmdLineException cmdLineError = new CmdLineException(CmdLineError.ErrorSavingCompiledState, e.Message, engine.ErrorCultureInfo);
              output.WriteLine(cmdLineError.Message);
            }else{
              output.WriteLine(JScriptException.Localize("INTERNAL COMPILER ERROR", engine.ErrorCultureInfo));
              output.WriteLine(e);
            }
            nativeReturnValue = 10;
          }catch(Exception e){
            output.WriteLine(JScriptException.Localize("INTERNAL COMPILER ERROR", engine.ErrorCultureInfo));
            output.WriteLine(e);
            nativeReturnValue = 10;
          }catch{
            output.WriteLine(JScriptException.Localize("INTERNAL COMPILER ERROR", engine.ErrorCultureInfo));
            nativeReturnValue = 10;
          }
        }finally{
          if (output != null)
            output.Close();
        }
        return nativeReturnValue;
      }

      private VsaEngine CreateAndInitEngine(CompilerParameters options, string[] sourceFiles, string outputFile, TextWriter output){
        VsaEngine engine = new VsaEngine(true);
        VsaSite site = new VsaSite(output);
        engine.InitVsaEngine("JSCodeGenerator://Microsoft.JScript.Vsa.VsaEngine", site);

        // Ensure that all options are valid; throw a CmdLineException otherwise
        this.ValidateOptions(options, engine);

        // Set options on the engine (equivalent of cmdline args in out-of-proc scenario)
        engine.GenerateDebugInfo = options.IncludeDebugInformation;
        engine.SetOption("referenceLoaderAPI", "LoadFile");
        engine.SetOption("fast", true);
        engine.SetOption("print", false);
        engine.SetOption("VersionSafe", false);
        engine.SetOption("output", options.OutputAssembly);
        if (options.GenerateExecutable)
          engine.SetOption("PEFileKind", PEFileKinds.ConsoleApplication);
        else
          engine.SetOption("PEFileKind", PEFileKinds.Dll);
        site.treatWarningsAsErrors = options.TreatWarningsAsErrors;
        engine.SetOption("warnaserror", options.TreatWarningsAsErrors);
        site.warningLevel = options.WarningLevel;
        engine.SetOption("WarningLevel", options.WarningLevel);
        bool stdLibAdded = false;
        foreach (string assemblyName in options.ReferencedAssemblies){
          if (String.Compare(Path.GetFileName(assemblyName), "mscorlib.dll", StringComparison.OrdinalIgnoreCase) == 0)
            stdLibAdded = true;
          this.AddAssemblyReference(engine, assemblyName);
        }
        if (!stdLibAdded)
          this.AddAssemblyReference(engine, "mscorlib.dll");

        // Parse any additional compiler options
        StringCollection compilerOptions = this.SplitCmdLineArguments(options.CompilerOptions);
        this.ParseCompilerOptions(engine, compilerOptions, output, options.GenerateExecutable);

        // Add the source files to the engine (as IVsaCodeItems)
        for (int j = 0; j < sourceFiles.Length; j++)
          this.AddSourceFile(engine, sourceFiles[j]);

        return engine;
      }

      private void GetAllDefines(string definitionList, Hashtable defines, VsaEngine engine){
        int startIndex = 0;
        int nextIndex;
        string definition;
        do{
          nextIndex = this.GetArgumentSeparatorIndex(definitionList, startIndex);
          if (nextIndex == -1)
            definition = definitionList.Substring(startIndex);
          else
            definition = definitionList.Substring(startIndex, nextIndex-startIndex);
          this.AddDefinition(definition, defines, engine);
          startIndex = nextIndex+1;
        }while (nextIndex > -1);
      }

      private int GetArgumentSeparatorIndex(string argList, int startIndex){
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

      private void ParseCompilerOptions(VsaEngine engine, StringCollection args, TextWriter output, bool generateExe){
        // Process compiler options and return command line fragment to complete the partial debug command line
        // that was formed by examining the CompilerParameters structure only.
        string libpath = System.Environment.GetEnvironmentVariable("LIB");
        bool generateWinExe = false;
        Hashtable defines = new Hashtable(10);
        Hashtable resources = new Hashtable(10);
        Hashtable resourceFiles = new Hashtable(10);
        bool targetSpecified = false;

        StringBuilder fullCmdLine = null;
        if (this.debugCommandLine != null)
          fullCmdLine = new StringBuilder(this.debugCommandLine);

        // if '/' separates dir, use '-' as switch
        string cmdLineSwitch = Path.DirectorySeparatorChar == '/' ? "-" : "/" ;

        for (int i = 0, n = args.Count; i < n; i++){
          object argument;
          string option;

          string arg = args[i];
          if (arg == null || arg.Length == 0) continue;

          if (arg[0] == '@')
            throw new CmdLineException(CmdLineError.InvalidForCompilerOptions, "@<filename>", engine.ErrorCultureInfo);
         
          // slash ('/') character is a valid filename character on UNIX, so we can't use it as a switch
          if ('-' != arg[0] && ('/' != arg[0] || Path.DirectorySeparatorChar == '/'))
            break;

          option = arg.Substring(1);

          if (option.Length > 0){
            switch (option[0]){
            case '?':
              throw new CmdLineException(CmdLineError.InvalidForCompilerOptions, "/?", engine.ErrorCultureInfo);
            case 'a':
            case 'A':
              argument = CmdLineOptionParser.IsBooleanOption(option, "autoref");
              if (argument != null){
                engine.SetOption("autoref", argument);
                if (fullCmdLine != null){
                  fullCmdLine.Append(arg);
                  fullCmdLine.Append(" ");
                }
                continue;
              }
              break;
            case 'c':
            case 'C':
              argument = CmdLineOptionParser.IsArgumentOption(option, "codepage");
              if (argument != null)
                throw new CmdLineException(CmdLineError.InvalidForCompilerOptions, "/codepage:<id>", engine.ErrorCultureInfo);
              break;
            case 'd':
            case 'D':
              argument = CmdLineOptionParser.IsBooleanOption(option, "debug");
              if (argument != null){
                engine.GenerateDebugInfo = (bool)argument;
                if (fullCmdLine != null){
                  fullCmdLine.Append(arg);
                  fullCmdLine.Append(" ");
                }
                continue;                        
              }
              argument = CmdLineOptionParser.IsArgumentOption(option, "d", "define");
              if (argument != null){
                this.GetAllDefines((string)argument, defines, engine);
                if (fullCmdLine != null){
                  fullCmdLine.Append(cmdLineSwitch + "d:\"");
                  fullCmdLine.Append((string)argument);
                  fullCmdLine.Append("\" ");
                }
                continue;
              }
              break;
            case 'f':
            case 'F':
              argument = CmdLineOptionParser.IsBooleanOption(option, "fast");
              if (argument != null){
                engine.SetOption("fast", argument);
                if (fullCmdLine != null){
                  fullCmdLine.Append(arg);
                  fullCmdLine.Append(" ");
                }
                continue;
              }
              break;
            case 'l':
            case 'L':
              argument = CmdLineOptionParser.IsArgumentOption(option, "lcid");
              if (argument != null){
                if (((string)argument).Length == 0)
                  throw new CmdLineException(CmdLineError.NoLocaleID, arg, engine.ErrorCultureInfo);
                try{
                  engine.LCID = Int32.Parse((string)argument, CultureInfo.InvariantCulture);
                }catch{
                  throw new CmdLineException(CmdLineError.InvalidLocaleID, (string)argument, engine.ErrorCultureInfo);
                }
                continue;
              }
              argument = CmdLineOptionParser.IsArgumentOption(option, "lib");
              if (argument != null){
                string newPaths = (string)argument;
                if (newPaths.Length == 0)
                  throw new CmdLineException(CmdLineError.MissingLibArgument, engine.ErrorCultureInfo);
                newPaths = newPaths.Replace(',', Path.PathSeparator);
                libpath = newPaths + Path.PathSeparator + libpath;
                if (fullCmdLine != null){
                  fullCmdLine.Append(cmdLineSwitch + "lib:\"");
                  fullCmdLine.Append((string)argument);
                  fullCmdLine.Append("\" ");
                }
                continue;
              }
              argument = CmdLineOptionParser.IsArgumentOption(option, "linkres", "linkresource");
              if (argument != null){
                try{
                  ResInfo resinfo = new ResInfo((string)argument, true /* isLinked */);
                  this.AddResourceFile(resinfo, resources, resourceFiles, engine);
                }catch(CmdLineException){
                  throw;
                }catch{
                  throw new CmdLineException(CmdLineError.ManagedResourceNotFound, engine.ErrorCultureInfo);
                }
                continue;
              }
              break;
            case 'n':
            case 'N':
              argument = CmdLineOptionParser.IsBooleanOption(option, "nologo");
              if (argument != null)
                throw new CmdLineException(CmdLineError.InvalidForCompilerOptions, "/nologo[+|-]", engine.ErrorCultureInfo);
              argument = CmdLineOptionParser.IsBooleanOption(option, "nostdlib");
              if (argument != null)
                throw new CmdLineException(CmdLineError.InvalidForCompilerOptions, "/nostdlib[+|-]", engine.ErrorCultureInfo);
              break;
            case 'o':
            case 'O':
              argument = CmdLineOptionParser.IsArgumentOption(option, "out");
              if (argument != null)
                throw new CmdLineException(CmdLineError.InvalidForCompilerOptions, "/out:<filename>", engine.ErrorCultureInfo);
              break;
            case 'p':
            case 'P':
              argument = CmdLineOptionParser.IsBooleanOption(option, "print");
              if (argument != null){
                engine.SetOption("print", argument);
                if (fullCmdLine != null){
                  fullCmdLine.Append(arg);
                  fullCmdLine.Append(" ");
                }
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
                  throw new CmdLineException(CmdLineError.InvalidPlatform, (string)argument, engine.ErrorCultureInfo);
                engine.SetOption("PortableExecutableKind", PEKindFlags);
                engine.SetOption("ImageFileMachine", PEMachineArchitecture);
                if (fullCmdLine != null){
                  fullCmdLine.Append(arg);
                  fullCmdLine.Append(" ");
                }
                continue;
              }
              break;
            case 'r':
            case 'R':
              argument = CmdLineOptionParser.IsArgumentOption(option, "r", "reference");
              if (argument != null)
                throw new CmdLineException(CmdLineError.InvalidForCompilerOptions, "/r[eference]:<file list>", engine.ErrorCultureInfo);
              argument = CmdLineOptionParser.IsArgumentOption(option, "res", "resource");
              if (argument != null){
                try{
                  ResInfo resinfo = new ResInfo((string)argument, false /* isLinked */);
                  this.AddResourceFile(resinfo, resources, resourceFiles, engine);
                }catch(CmdLineException){
                  throw;
                }catch{
                  throw new CmdLineException(CmdLineError.ManagedResourceNotFound, engine.ErrorCultureInfo);
                }
                continue;
              }
              break;
            case 't':
            case 'T':
              argument = CmdLineOptionParser.IsArgumentOption(option, "t", "target");
              if (argument != null){
                if (String.Compare((string)argument, "exe", StringComparison.OrdinalIgnoreCase) == 0){
                  if (!generateExe)
                    throw new CmdLineException(CmdLineError.IncompatibleTargets, arg, engine.ErrorCultureInfo);
                  if (targetSpecified)
                    throw new CmdLineException(CmdLineError.MultipleTargets, engine.ErrorCultureInfo);
                  // no change -- /t:exe is the default when GenerateExecutable is true
                  targetSpecified = true;
                  continue;
                }
                if (String.Compare((string)argument, "winexe", StringComparison.OrdinalIgnoreCase) == 0){
                  if (!generateExe)
                    throw new CmdLineException(CmdLineError.IncompatibleTargets, arg, engine.ErrorCultureInfo);
                  if (targetSpecified)
                    throw new CmdLineException(CmdLineError.MultipleTargets, engine.ErrorCultureInfo);
                  engine.SetOption("PEFileKind", PEFileKinds.WindowApplication);
                  generateWinExe = true;
                  targetSpecified = true;
                  continue;
                }
                if (String.Compare((string)argument, "library", StringComparison.OrdinalIgnoreCase) == 0){
                  if (generateExe)
                    throw new CmdLineException(CmdLineError.IncompatibleTargets, engine.ErrorCultureInfo);
                  if (targetSpecified)
                    throw new CmdLineException(CmdLineError.MultipleTargets, engine.ErrorCultureInfo);
                  // no change -- /t:library is the default when GenerateExecutable is false
                  targetSpecified = true;
                  continue;
                }
                throw new CmdLineException(CmdLineError.InvalidTarget, (string)argument, engine.ErrorCultureInfo);
              }
              break;
            case 'u':
            case 'U':
              argument = CmdLineOptionParser.IsArgumentOption(option, "utf8output");
              if (argument != null)
                throw new CmdLineException(CmdLineError.InvalidForCompilerOptions, "/utf8output[+|-]", engine.ErrorCultureInfo);
              break;
            case 'v':
            case 'V':
              argument = CmdLineOptionParser.IsBooleanOption(option, "VersionSafe");
              if (argument != null){
                engine.SetOption("VersionSafe", argument);
                if (fullCmdLine != null){
                  fullCmdLine.Append(arg);
                  fullCmdLine.Append(" ");
                }
                continue;
              }
              break;
            case 'w':
            case 'W':
              argument = CmdLineOptionParser.IsArgumentOption(option, "w", "warn");
              if (argument != null){
                if (((string)argument).Length == 0){
                  throw new CmdLineException(CmdLineError.NoWarningLevel, arg, engine.ErrorCultureInfo);
                }
                if (((string)argument).Length == 1){
                  if (fullCmdLine != null){
                    fullCmdLine.Append(arg);
                    fullCmdLine.Append(" ");
                  }
                  switch (((string)argument)[0]){
                  case '0': engine.SetOption("WarningLevel", (object)0); continue;
                  case '1': engine.SetOption("WarningLevel", (object)1); continue;
                  case '2': engine.SetOption("WarningLevel", (object)2); continue;
                  case '3': engine.SetOption("WarningLevel", (object)3); continue;
                  case '4': engine.SetOption("WarningLevel", (object)4); continue;
                  }
                }
                throw new CmdLineException(CmdLineError.InvalidWarningLevel, arg, engine.ErrorCultureInfo);
              }
              argument = CmdLineOptionParser.IsBooleanOption(option, "warnaserror");
              if (argument != null){
                engine.SetOption("warnaserror", argument);
                if (fullCmdLine != null){
                  fullCmdLine.Append(arg);
                  fullCmdLine.Append(" ");
                }
                continue;
              }
              break;
            default:
              break;
            }
          }
          throw new CmdLineException(CmdLineError.UnknownOption, arg, engine.ErrorCultureInfo);
        }
        if (fullCmdLine != null){
          // append target type to debug command line
          if (generateExe){
            if (generateWinExe)
              fullCmdLine.Append(cmdLineSwitch + "t:winexe ");
            else
              fullCmdLine.Append(cmdLineSwitch + "t:exe ");
          }else{
            fullCmdLine.Append(cmdLineSwitch + "t:library ");
          }
          this.debugCommandLine = fullCmdLine.ToString();
        }
        // set options on engine that were possibly built from multiple arguments
        engine.SetOption("libpath", libpath);
        engine.SetOption("defines", defines);
      }

      internal void PrintBanner(VsaEngine engine, TextWriter output){
        string jsVersion = BuildVersionInfo.MajorVersion.ToString(CultureInfo.InvariantCulture)
                      + "." + BuildVersionInfo.MinorVersion.ToString(CultureInfo.InvariantCulture).PadLeft(2, '0')
                      + "." + BuildVersionInfo.Build.ToString(CultureInfo.InvariantCulture).PadLeft(4, '0');
        Version version = System.Environment.Version;
        string runtimeVersion = version.Major.ToString(CultureInfo.InvariantCulture)
                        + "." + version.Minor.ToString(CultureInfo.InvariantCulture)
                        + "." + version.Build.ToString(CultureInfo.InvariantCulture).PadLeft(4, '0');
        output.WriteLine(String.Format(engine.ErrorCultureInfo, JScriptException.Localize("Banner line 1", engine.ErrorCultureInfo), jsVersion));
        output.WriteLine(String.Format(engine.ErrorCultureInfo, JScriptException.Localize("Banner line 2", engine.ErrorCultureInfo), runtimeVersion));
        output.WriteLine(JScriptException.Localize("Banner line 3", engine.ErrorCultureInfo) + Environment.NewLine);
      }

      private void PrintOptions(TextWriter output, CompilerParameters options){
        StringBuilder sb = new StringBuilder();
        sb.Append("CompilerParameters.CompilerOptions        : \"");
        sb.Append(options.CompilerOptions);
        sb.Append("\"");
        sb.Append(Environment.NewLine);
        sb.Append("CompilerParameters.GenerateExecutable     : ");
        sb.Append(options.GenerateExecutable.ToString(CultureInfo.InvariantCulture));
        sb.Append(Environment.NewLine);
        sb.Append("CompilerParameters.GenerateInMemory       : ");
        sb.Append(options.GenerateInMemory.ToString(CultureInfo.InvariantCulture));
        sb.Append(Environment.NewLine);
        sb.Append("CompilerParameters.IncludeDebugInformation: ");
        sb.Append(options.IncludeDebugInformation.ToString(CultureInfo.InvariantCulture));
        sb.Append(Environment.NewLine);
        sb.Append("CompilerParameters.MainClass              : \"");
        sb.Append(options.MainClass);
        sb.Append("\"");
        sb.Append(Environment.NewLine);
        sb.Append("CompilerParameters.OutputAssembly         : \"");
        sb.Append(options.OutputAssembly);
        sb.Append("\"");
        sb.Append(Environment.NewLine);
        sb.Append("CompilerParameters.ReferencedAssemblies   : ");
        foreach (string reference in options.ReferencedAssemblies){
          sb.Append(Environment.NewLine);
          sb.Append("        \"");
          sb.Append(reference);
          sb.Append("\"");
        }
        sb.Append(Environment.NewLine);
        sb.Append("CompilerParameters.TreatWarningsAsErrors  : ");
        sb.Append(options.TreatWarningsAsErrors.ToString(CultureInfo.InvariantCulture));
        sb.Append(Environment.NewLine);
        sb.Append("CompilerParameters.WarningLevel           : ");
        sb.Append(options.WarningLevel.ToString(CultureInfo.InvariantCulture));
        sb.Append(Environment.NewLine);
        output.WriteLine(sb.ToString());
      }

      protected string ReadFile(string fileName, VsaEngine engine){
        string s = "";
        FileStream inputStream = null;
        try{
          inputStream = new FileStream(fileName, FileMode.Open, FileAccess.Read, FileShare.Read);
        }catch(System.ArgumentException){
          throw new CmdLineException(CmdLineError.InvalidCharacters, fileName, engine.ErrorCultureInfo);
        }catch(System.IO.FileNotFoundException){
          throw new CmdLineException(CmdLineError.SourceNotFound, fileName, engine.ErrorCultureInfo);
        }
        try {
          if (inputStream.Length != 0) {
            StreamReader reader = new StreamReader(inputStream, true /*detectEncodingFromByteOrderMarks*/);
            try{
              s = reader.ReadToEnd();
            }finally{
              reader.Close();
            }
          }
        }finally{
          inputStream.Close();
        }
        return s;
      }

      private StringCollection SplitCmdLineArguments(string argumentString){
        StringCollection args = new StringCollection();
        if (argumentString == null || argumentString.Length == 0)
          return args;

        // This regular expression is: blank*(nonblanks|stringLiteral)+
        string strReArgs = "\\s*([^\\s\\\"]|(\\\"[^\\\"\\n]*\\\"))+";
        Regex re = new Regex(strReArgs);

        MatchCollection matches = re.Matches(argumentString);
        if (matches != null && matches.Count != 0){
          foreach (Match match in matches){
            string arg = match.ToString().Trim();
            int quotes = 0;
            while ((quotes = arg.IndexOf("\"", quotes)) != -1){
              if (quotes == 0)
                arg = arg.Substring(1);
              else if (arg[quotes-1] == '\\')
                quotes += 1;
              else
                arg = arg.Remove(quotes, 1);
            }
            args.Add(arg);
          }
        }
        return args;
      }

      private void ValidateOptions(CompilerParameters options, VsaEngine engine){
        // make sure we have an output filename with an extension
        string outputName = options.OutputAssembly;
        try{
          if (Path.GetFileName(outputName).Length == 0)
            throw new CmdLineException(CmdLineError.NoFileName, outputName, engine.ErrorCultureInfo);
        }catch(System.ArgumentException){
          throw new CmdLineException(CmdLineError.NoFileName, engine.ErrorCultureInfo);
        }
        if (Path.GetExtension(outputName).Length == 0)
          throw new CmdLineException(CmdLineError.MissingExtension, outputName, engine.ErrorCultureInfo);
        // make sure warninglevel is valid (0-4)
        // Note: ASP+ has -1 as the default value if the warning level is not specified in a page.  We set
        //       any -1 warning level to 4, the JSC default.
        if (options.WarningLevel == -1)
          options.WarningLevel = 4;
        if ((options.WarningLevel < 0) || (options.WarningLevel > 4))
          throw new CmdLineException(CmdLineError.InvalidWarningLevel, options.WarningLevel.ToString(CultureInfo.InvariantCulture), engine.ErrorCultureInfo);
      }
    }

    internal class VsaSite: BaseVsaSite{
      public int warningLevel = 4;
      public bool treatWarningsAsErrors = false;
      public TextWriter output = Console.Out;

      public VsaSite(TextWriter redirectedOutput){
        this.output = redirectedOutput;
      }

      public override bool OnCompilerError(IVsaError error){
        // Errors have severity 0, warnings have severities 1-4.  Setting
        // this.warningLevel to 0 results in all warnings being masked.
        int severity = error.Severity;
        if (severity > this.warningLevel)
          return true;
        bool isWarning = (0 != severity) && !this.treatWarningsAsErrors;
        this.PrintError(error.SourceMoniker, error.Line, error.StartColumn, isWarning, error.Number, error.Description);
        // Report as many errors as possible (never return false)
        return true;            
      }

      private void PrintError(string sourceFile, int line, int column, bool fIsWarning, int number, string message){
        string errorNumber = (10000 + (number & 0xFFFF)).ToString(CultureInfo.InvariantCulture).Substring(1);
        if (String.Compare(sourceFile, "no source", StringComparison.Ordinal) != 0)
          output.Write(sourceFile + "(" + line.ToString(CultureInfo.InvariantCulture) + "," + column.ToString(CultureInfo.InvariantCulture) + ") : ");
        output.WriteLine((fIsWarning ? "warning JS" : "error JS") + errorNumber + ": " + message);
      }
    }
}
