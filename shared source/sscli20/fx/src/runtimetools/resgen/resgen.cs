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
namespace System.Tools {
    using System;
    using System.IO; 
    using System.Collections; 
    using System.Collections.Generic; 
    using System.Resources; 
    using System.Text;
    using System.Diagnostics;
    using System.Globalization;
    using System.CodeDom;
    using System.CodeDom.Compiler;
    using System.Xml;
    using System.Reflection;
    using System.Runtime.InteropServices;

    // .NET Development Platform Resource file Generator
    //
    // This program will read in text files or ResX files of name-value pairs and
    // produces a .NET .resources file.  Additionally ResGen can change data from
    // any of these three formats to any of the other formats (though text files
    // only support strings).
    //
    // The text files must have lines of the form name=value, and comments are
    // allowed ('#' at the beginning of the line).
    //
    /// <include file='doc\ResGen.uex' path='docs/doc[@for="ResGen"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class ResGen {
        private const int errorCode = -1;
    
        private static int errors = 0;
        private static int warnings = 0;
        
        // We use a list to preserve the resource ordering (primarily for easier testing),
        // but also use a hash table to check for duplicate names.
        private static ArrayList resources = new ArrayList();
        private static Hashtable resourcesHashTable = new Hashtable(StringComparer.OrdinalIgnoreCase);

        private static List<AssemblyName> assemblyList;

        internal sealed class ResourceClassOptions
        {
            private String _language;
            private String _nameSpace;
            private String _className;
            private String _outputFileName;
            private bool _internalClass;

            internal ResourceClassOptions(String language, String nameSpace, String className, String outputFileName, bool isClassInternal)
            {
                _language = language;
                _nameSpace = nameSpace;
                _className = className;
                _outputFileName = outputFileName;
                _internalClass = isClassInternal;
            }

            internal String Language {
                get { return _language; }
            }

            internal String NameSpace {
                get { return _nameSpace; }
            }

            internal String ClassName {
                get { return _className; }
            }

            internal String OutputFileName {
                get { return _outputFileName; }
            }

            internal bool InternalClass {
                get { return _internalClass; }
                set { _internalClass = value; }
            }
        }

        internal sealed class LineNumberStreamReader : StreamReader {
            // Line numbers start from 1, as well as line position.
            // For better error reporting, set line number to 1 and col to 0.
            int _lineNumber;
            int _col;

            internal LineNumberStreamReader(String fileName, Encoding encoding, bool detectEncoding) : base(fileName, encoding, detectEncoding)
            {
                _lineNumber = 1;
                _col = 0;
            }

            internal LineNumberStreamReader(Stream stream) : base(stream)
            {
                _lineNumber = 1;
                _col = 0;
            }

            public override int Read() {
                int ch = base.Read();
                if (ch != -1) {
                    _col++;
                    if (ch == '\n') {
                        _lineNumber++;
                        _col = 0;
                    }
                }
                return ch;
            }

            public override int Read([In, Out] char[] chars, int index, int count) {
                int r = base.Read(chars, index, count);
                for(int i=0; i<r; i++) {
                    if (chars[i + index] == '\n') {
                        _lineNumber++;
                        _col = 0;
                    }
                    else
                        _col++;
                }
                return r;
            }

            public override String ReadLine()
            {
                String s = base.ReadLine();
                if (s != null) {
                    _lineNumber++;
                    _col = 0;
                }
                return s;
            }

            public override String ReadToEnd()
            {
                throw new NotImplementedException("NYI");
            }

            internal int LineNumber {
                get { return _lineNumber; }
            }

            internal int LinePosition {
                get { return _col; }
            }
        }

        // For flow of control & passing sufficient error context back 
        // from ReadTextResources
        internal sealed class TextFileException : Exception
        {
            private String _fileName;
            private int _lineNumber;
            private int _column;

            internal TextFileException(String message, String fileName, int lineNumber, int linePosition) : base(message)
            {
                _fileName = fileName;
                _lineNumber = lineNumber;
                _column = linePosition;
            }

            internal String FileName {
                get { return _fileName; }
            }

            internal int LineNumber {
                get { return _lineNumber; }
            }

            internal int LinePosition {
                get { return _column; }
            }
        }

        private static void AddResource(string name, object value, String inputFileName, int lineNumber, int linePosition) {
            Entry entry = new Entry(name, value);
    
            if (resourcesHashTable.ContainsKey(name)) {
                Warning(SR.GetString(SR.DuplicateResourceKey, name), inputFileName, lineNumber, linePosition);
                return;
            }
    
            resources.Add(entry);
            resourcesHashTable.Add(name, value);
        }

        private static void AddResource(string name, object value, String inputFileName) {
            Entry entry = new Entry(name, value);
    
            if (resourcesHashTable.ContainsKey(name)) {
                Warning(SR.GetString(SR.DuplicateResourceKey, name), inputFileName);
                return;
            }
    
            resources.Add(entry);
            resourcesHashTable.Add(name, value);
        }
    



        private static void Error(String message) {
            Error(message, 0);
        }

        // Use this for general resgen errors with no specific file info
        private static void Error(String message, int errorNumber) {
            String errorFormat = "ResGen : error RG{1:0000}: {0}";
            Console.Error.WriteLine(errorFormat, message, errorNumber);
            errors++;
        }
        
        // Use this for a general error w.r.t. a file, like a missing file.
        private static void Error(String message, String fileName) {
            Error(message, fileName, 0);
        }

        // Use this for a general error w.r.t. a file, like a missing file.
        private static void Error(String message, String fileName, int errorNumber) {
            String errorFormat = "{0} : error RG{1:0000}: {2}";
            Console.Error.WriteLine(errorFormat, fileName, errorNumber, message);
            errors++;
        }

        // For specific errors about the contents of a file and you know where
        // the error occurred.
        private static void Error(String message, String fileName, int line, int column) {
            Error(message, fileName, line, column, 0);
        }

        // For specific errors about the contents of a file and you know where
        // the error occurred.
        private static void Error(String message, String fileName, int line, int column, int errorNumber) {
            String errorFormat = "{0}({1},{2}): error RG{3:0000}: {4}";
            Console.Error.WriteLine(errorFormat, fileName, line, column, errorNumber, message);
            errors++;
        }

        // General warnings
        private static void Warning(string message) {
            String warningFormat = "ResGen : warning RG0000 : {0}";
            Console.Error.WriteLine(warningFormat, message);
            warnings++;
        }

        // Warnings in a particular file, but we don't have line number info
        private static void Warning(String message, String fileName) {
            Warning(message, fileName, 0);
        }

        // Warnings in a particular file, but we don't have line number info
        private static void Warning(String message, String fileName, int warningNumber) {
            String warningFormat = "{0} : warning RG{1:0000}: {2}";
            Console.Error.WriteLine(warningFormat, fileName, warningNumber, message);
            warnings++;
        }

        // Warnings in a file on a particular line and character
        private static void Warning(String message, String fileName, int line, int column) {
            Warning(message, fileName, line, column, 0);
        }

        // Warnings in a file on a particular line and character
        private static void Warning(String message, String fileName, int line, int column, int warningNumber) {
            String warningFormat = "{0}({1},{2}): warning RG{3:0000}: {4}";
            Console.Error.WriteLine(warningFormat, fileName, line, column, warningNumber, message);
            warnings++;
        }
    
        private static Format GetFormat(string filename) {
            string extension = Path.GetExtension(filename);
            if (String.Compare(extension, ".txt", true, CultureInfo.InvariantCulture) == 0 ||
                String.Compare(extension, ".restext", true, CultureInfo.InvariantCulture) == 0)
                return Format.Text;
            else if (String.Compare(extension, ".resources", true, CultureInfo.InvariantCulture) == 0)
                return Format.Binary;
            else {
                Error(SR.GetString(SR.UnknownFileExtension, extension, filename));
                Environment.Exit(errorCode);
                return Format.Text; // never reached
            }
        }
    
        /// <include file='doc\ResGen.uex' path='docs/doc[@for="ResGen.Main"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static void Main(String[] args) {
            // Tell build we had an error, then set this to 0 if we complete successfully.
            Environment.ExitCode = errorCode;
            if (args.Length < 1 || args[0].Equals("-h") || args[0].Equals("-?") ||
                args[0].Equals("/h") || args[0].Equals("/?")) {
                Usage();
                return;
            }
    
            String[] inFiles = null;
            String[] outFiles = null;
            // Default resource class options for all classes
            ResourceClassOptions resourceClassOptions = null;
            int argIndex = 0;
            bool setSimpleInputFile = false; // For resgen a.resources a.resx
            bool gotOutputFileName = false;  // For resgen a.txt a.resources b.txt
            bool useSourcePath = false;
            bool isClassInternal = true;

            while(argIndex < args.Length && errors == 0) {
                if (args[argIndex].Equals("/compile")) {
                    inFiles = new String[args.Length - argIndex - 1];
                    outFiles = new String[args.Length - argIndex - 1];
                    for(int i=0; i<inFiles.Length; i++) {
                        inFiles[i] = args[argIndex+1];
                        int index = inFiles[i].IndexOf(",");
                        if (index != -1) {
                            String tmp = inFiles[i];
                            inFiles[i] = tmp.Substring(0, index);
                            if (!ValidResourceFileName(inFiles[i])) {
                                Error(SR.GetString(SR.BadFileExtension, inFiles[i]));
                                break;
                            }
                            if (index == tmp.Length-1) {
                                Error(SR.GetString(SR.MalformedCompileString, tmp));
                                inFiles = new String[0];
                                break;
                            }
                            outFiles[i] = tmp.Substring(index+1);
                            if (!ValidResourceFileName(outFiles[i])) {
                                Error(SR.GetString(SR.BadFileExtension, outFiles[i]));
                                break;
                            }
                        }
                        else {
                            if (!ValidResourceFileName(inFiles[i])) {
                                if (inFiles[i][0] == '/' || inFiles[i][0] == '-') {
                                    Error(SR.GetString(SR.InvalidCommandLineSyntax, "/compile", inFiles[i]));
                                }
                                else
                                    Error(SR.GetString(SR.BadFileExtension, inFiles[i]));
                                break;
                            }

                            string resourceFileName = GetResourceFileName(inFiles[i]);
                            Debug.Assert(resourceFileName != null, "Unexpected null file name!");
                            outFiles[i] = resourceFileName;
                        }
                        argIndex++;
                    }
                }
                else if (args[argIndex].StartsWith("/r:") || args[argIndex].StartsWith("-r:")) {
                    // assembly names syntax /r:c:\system\System.Drawing.dll
                    String s = args[argIndex];
                    s = s.Substring(3);  // Skip over "/r:"
                    if(assemblyList == null) {
                        assemblyList = new List<AssemblyName>();
                    }
                    try {
                        assemblyList.Add(AssemblyName.GetAssemblyName(s));
                    }
                    catch(Exception e) {
                        Error(SR.GetString(SR.CantLoadAssembly, s, e.GetType().Name, e.Message));
                    }
                }
                else if (args[argIndex].ToLower(CultureInfo.InvariantCulture).Equals("/usesourcepath") || 
                         args[argIndex].ToLower(CultureInfo.InvariantCulture).Equals("-usesourcepath")) {
                    useSourcePath = true;
                } else if (args[argIndex].ToLower(CultureInfo.InvariantCulture).Equals("/publicclass") || 
                           args[argIndex].ToLower(CultureInfo.InvariantCulture).Equals("-publicclass")) {
                    isClassInternal = false;
                } else {
                    if (ValidResourceFileName(args[argIndex])) {
                        if (!setSimpleInputFile) {
                            inFiles = new String[1];
                            inFiles[0] = args[argIndex];
                            outFiles = new String[1];
                            outFiles[0] = GetResourceFileName(inFiles[0]);
                            setSimpleInputFile = true;
                        }
                        else {
                            if (!gotOutputFileName) {
                                outFiles[0] = args[argIndex];
                                gotOutputFileName = true;
                            }
                            else {
                                Error(SR.GetString(SR.InvalidCommandLineSyntax, "<none>", args[argIndex]));
                                break;
                            }
                        }
                    }
                    else {
                        if (args[argIndex][0] == '/' || args[argIndex][0] == '-') {
                            Error(SR.GetString(SR.BadCommandLineOption, args[argIndex]));
                        }
                        else
                            Error(SR.GetString(SR.BadFileExtension, args[argIndex]));
                        return;
                    }
                }
                argIndex++;
            }

            if ((inFiles == null || inFiles.Length == 0) && errors == 0) {
                Usage();
                return;
            }

            if (resourceClassOptions != null) {
                resourceClassOptions.InternalClass = isClassInternal;
            
                // Verify we don't produce two identically named resource classes, 
                // or write different classes to the same file when using the 
                // /compile option.
                if (inFiles.Length > 1) {
                    if (resourceClassOptions.ClassName != null || resourceClassOptions.OutputFileName != null) {
                        Error(SR.GetString(SR.CompileAndSTRDontMix));
                    }
                }
            }

            // Do all the work.
            if (errors == 0) {
                for(int i=0; i<inFiles.Length; i++) {
                    ProcessFile(inFiles[i], outFiles[i], resourceClassOptions, useSourcePath);
                }
            }
    
            // Quit & report errors, if necessary.
            if (warnings != 0)
                Console.Error.WriteLine(SR.GetString(SR.WarningCount, warnings));

            if (errors != 0) {
                Console.Error.WriteLine(SR.GetString(SR.ErrorCount, errors));
                Debug.Assert(Environment.ExitCode != 0);
                // Now delete all the output files, ensuring the build won't
                // continue using half-generated output files.  This is a 
                // backstop for other errors up above.
                if (outFiles != null) {
                    foreach(String outFile in outFiles) {
                        if (File.Exists(outFile)) {
                            try {
                                File.Delete(outFile);
                            }
                            catch {}
                        }
                    }
                }
            }
            else {
                // Tell build we succeeded.
                Environment.ExitCode = 0;
            }
        }
    
        private static String GetResourceFileName(String inFile) {
            if (inFile == null) {
                return null;
            }
        
            // Note that the naming scheme is basename.[en-US.]resources
            int end = inFile.LastIndexOf('.');
            if (end == -1) {
                return null;
            }
            return inFile.Substring(0, end) + ".resources";
        }

        private static bool ValidResourceFileName(String inFile) {
            if (inFile == null)
                return false;

            CompareInfo comp = CultureInfo.InvariantCulture.CompareInfo;
            if (comp.IsSuffix(inFile, ".resx", CompareOptions.IgnoreCase) ||
                comp.IsSuffix(inFile, ".txt", CompareOptions.IgnoreCase) ||
                comp.IsSuffix(inFile, ".restext", CompareOptions.IgnoreCase) ||
                comp.IsSuffix(inFile, ".resources", CompareOptions.IgnoreCase))
                return true;
            return false;
        }

    
        private static void ProcessFile(String inFile, String outFile, ResourceClassOptions resourceClassOptions, bool useSourcePath) {
            //Console.WriteLine("Processing {0} --> {1}", inFile, outFile);
            // Reset state
            resources.Clear();
            resourcesHashTable.Clear();
    
            try {
                // Explicitly handle missing input files here - don't catch a 
                // FileNotFoundException since we can get them from the loader
                // if we try loading an assembly version we can't find.
                if (!File.Exists(inFile)) {
                    Error(SR.GetString(SR.FileNotFound, inFile));
                    return;
                }

                ReadResources(inFile, useSourcePath);
            }
            catch (ArgumentException ae) {
                if (ae.InnerException is XmlException) {
                    XmlException xe = (XmlException) ae.InnerException;
                    Error(xe.Message, inFile, xe.LineNumber, xe.LinePosition);
                }
                else {
                    Error(ae.Message, inFile);
                }
                return;
            }
            catch (TextFileException tfe) {
                // Used to pass back error context from ReadTextResources to here.
                Error(tfe.Message, tfe.FileName, tfe.LineNumber, tfe.LinePosition);
                return;
            }
            catch (Exception e) {
                Error(e.Message, inFile);
                // We need to give meaningful error messages to the user. 
                // Note that ResXResourceReader wraps any exception it gets
                // in an ArgumentException with the message "Invalid ResX input."
                // If you don't look at the InnerException, you have to attach
                // a debugger to find the problem.
                if (e.InnerException != null) {
                    Exception inner = e.InnerException;
                    StringBuilder sb = new StringBuilder(200);
                    sb.Append(e.Message);
                    while (inner != null) {
                        sb.Append(" ---> ");
                        sb.Append(inner.GetType().Name);
                        sb.Append(": ");
                        sb.Append(inner.Message);
                        inner = inner.InnerException;
                    }
                    Error(SR.GetString(SR.SpecificError, e.InnerException.GetType().Name, sb.ToString()), inFile);
                }
                return;
            }
    
            try {
                WriteResources(outFile);
            }
            catch (IOException io) {
                Error(SR.GetString(SR.WriteError, outFile), outFile);
                if (io.Message != null)
                    Error(SR.GetString(SR.SpecificError, io.GetType().Name, io.Message), outFile);
                if (File.Exists(outFile)) {
                    Error(SR.GetString(SR.CorruptOutput, outFile));
                    try {
                        File.Delete(outFile);
                    }
                    catch (Exception) {
                        Error(SR.GetString(SR.DeleteOutputFileFailed, outFile));
                    }
                }
                return;
            }
            catch (Exception e) {
                Error(SR.GetString(SR.GenericWriteError, outFile));
                if (e.Message != null)
                    Error(SR.GetString(SR.SpecificError, e.GetType().Name, e.Message));
            }
        }



        // <doc>
        // <desc>
        //     Reads the resources out of the specified file and populates the
        //     resources hashtable.
        // </desc>
        // <param term='filename'>
        //     Filename to load.
        // </param>
        // </doc>   
        private static void ReadResources(String filename, bool useSourcePath) {
            Format format = GetFormat(filename);
            switch (format) {
                case Format.Text:
                    ReadTextResources(filename);
                    break;
    
    
                case Format.Binary:
                    ReadResources(new ResourceReader(filename), filename); // closes reader for us
                    break;
    
                default:
                    Debug.Fail("Unknown format " + format.ToString());
                    break;
            }
            Console.WriteLine(SR.GetString(SR.ReadIn, resources.Count, filename));
        }
    
        // closes reader when done.  File name is for error reporting.
        private static void ReadResources(IResourceReader reader, String fileName) {
            using(reader) {
                IDictionaryEnumerator resEnum = reader.GetEnumerator();
                while (resEnum.MoveNext()) {
                    string name = (string)resEnum.Key;
                    object value = resEnum.Value;
                    AddResource(name, value, fileName);
                }
            }
        }
    
        private static void ReadTextResources(String fileName) {
            // Check for byte order marks in the beginning of the input file, but
            // default to UTF-8.
            using(LineNumberStreamReader sr = new LineNumberStreamReader(fileName, new UTF8Encoding(true), true))
            {
                StringBuilder name = new StringBuilder(255);
                StringBuilder value = new StringBuilder(2048);

                int ch = sr.Read();
                while (ch != -1) {
                    if (ch == '\n' || ch == '\r') {
                        ch = sr.Read();
                        continue;
                    }
    
                    // Skip over commented lines or ones starting with whitespace.
                    // Support LocStudio INF format's comment char, ';'
                    if (ch == '#' || ch == '\t' || ch == ' ' || ch == ';') {
                        // comment char (or blank line) - skip line.
                        sr.ReadLine();
                        ch = sr.Read();
                        continue;
                    }
                    // Note that in Beta we recommended users should put a [strings] 
                    // section in their file.  Now it's completely unnecessary and can 
                    // only cause bugs.  We will not parse anything using '[' stuff now
                    // and we should give a warning about seeing [strings] stuff.
                    // In V1.1 or V2, we can rip this out completely, I hope.
                    if (ch == '[') {
                        String skip = sr.ReadLine();
                        if (skip.Equals("strings]"))
                            Warning(SR.GetString(SR.StringsTagObsolete), fileName, sr.LineNumber - 1, 1);
                        else
                            throw new TextFileException(SR.GetString(SR.INFFileBracket, skip), fileName, sr.LineNumber - 1, 1);
                        ch = sr.Read();
                        continue;
                    }
    
                    // Read in name
                    name.Length = 0;
                    while (ch != '=') {
                        if (ch == '\r' || ch == '\n')
                            throw new TextFileException(SR.GetString(SR.NoEqualsWithNewLine, name.Length, name), fileName, sr.LineNumber, sr.LinePosition);
                        
                        name.Append((char)ch);
                        ch = sr.Read();
                        if (ch == -1)
                            break;
                    }
                    if (name.Length == 0)
                        throw new TextFileException(SR.GetString(SR.NoEquals), fileName, sr.LineNumber, sr.LinePosition);
    
                    // For the INF file, we must allow a space on both sides of the equals
                    // sign.  Deal with it.
                    if (name[name.Length-1] == ' ') {
                        name.Length = name.Length - 1;
                    }
                    ch = sr.Read(); // move past =
                    // If it exists, move past the first space after the equals sign.
                    if (ch == ' ')
                        ch = sr.Read();
    
                    // Read in value
                    value.Length = 0;

                    while(ch != -1) {
                        // Did we read @"\r" or @"\n"?
                        bool quotedNewLine = false;
                        if (ch == '\\') {
                            ch = sr.Read();
                            switch (ch) {
                            case '\\':
                                // nothing needed
                                break;
                            case 'n':
                                ch = '\n';
                                quotedNewLine = true;
                                break;
                            case 'r':
                                ch = '\r';
                                quotedNewLine = true;
                                break;
                            case 't':
                                ch = '\t';
                                break;
                            case '"':
                                ch = '\"';
                                break;
                            case 'u':
                                char[] hex = new char[4];
                                int numChars = 4;
                                int index = 0;
                                while(numChars > 0) {
                                    int n = sr.Read(hex, index, numChars);
                                    if (n == 0)
                                        throw new TextFileException(SR.GetString(SR.BadEscape, (char) ch, name.ToString()), fileName, sr.LineNumber, sr.LinePosition);
                                    index += n;
                                    numChars -= n;
                                }
                                ch = (char) UInt16.Parse(new String(hex), NumberStyles.HexNumber, CultureInfo.InvariantCulture);
                                quotedNewLine = (ch == '\n' || ch == '\r');
                                break;

                            default:
                                throw new TextFileException(SR.GetString(SR.BadEscape, (char) ch, name.ToString()), fileName, sr.LineNumber, sr.LinePosition);
                            }
                        }

                        // Consume endline...
                        //   Endline can be \r\n or \n.  But do not treat a 
                        //   quoted newline (ie, @"\r" or @"\n" in text) as a
                        //   real new line.  They aren't the end of a line.
                        if (!quotedNewLine) {
                            if (ch == '\r') {
                                ch = sr.Read();
                                if (ch == -1) {
                                    break;
                                }
                                else if (ch == '\n') {
                                    ch = sr.Read();
                                    break;
                                }
                            }
                            else if (ch == '\n') {
                                ch = sr.Read();
                                break;
                            }
                        }

                        value.Append((char)ch);
                        ch = sr.Read();
                    }
                    
                    // Note that value can be an empty string

                    AddResource(name.ToString(), value.ToString(), fileName, sr.LineNumber, sr.LinePosition);
                }
            }
        }
    
        private static void WriteResources(String filename) {
            Format format = GetFormat(filename);
            switch (format) {
                case Format.Text:
                    WriteTextResources(filename);
                    break;

    
                case Format.Binary:
                    WriteResources(new ResourceWriter(filename)); // closes writer for us
                    break;
    
                default:
                    Debug.Fail("Unknown format " + format.ToString());
                    break;
            }
        }
    
        // closes writer automatically
        private static void WriteResources(IResourceWriter writer) {
            foreach (Entry entry in resources) {
                string key = entry.name;
                object value = entry.value;
                writer.AddResource(key, value);
            }
            Console.Write(SR.GetString(SR.BeginWriting));
            writer.Close();
            Console.WriteLine(SR.GetString(SR.DoneDot));
        }
    
        private static void WriteTextResources(String fileName) {
            using(StreamWriter writer = new StreamWriter(fileName, false, Encoding.UTF8)) {
                foreach (Entry entry in resources) {
                    String key = entry.name;
                    Object v = entry.value;
                    String value = v as String;
                    if (value == null) {
                        Error(SR.GetString(SR.OnlyString, key, v.GetType().FullName), fileName);
                    }

                    // Escape any special characters in the String.
                    value = value.Replace("\\", "\\\\");
                    value = value.Replace("\n", "\\n");
                    value = value.Replace("\r", "\\r");
                    value = value.Replace("\t", "\\t");

                    writer.WriteLine("{0}={1}", key, value);
                }
            }
        }
    
        private static void Usage() {
            Console.WriteLine(SR.GetString(SR.Usage, Environment.Version, CommonResStrings.CopyrightForCmdLine));

            Console.WriteLine(SR.GetString(SR.ValidLanguages));

            CompilerInfo[] compilerInfos = CodeDomProvider.GetAllCompilerInfo();
            for (int i=0; i<compilerInfos.Length; i++) {
                string[] languages = compilerInfos[i].GetLanguages();
                if (i != 0)
                    Console.Write(", ");    

                for (int j=0; j<languages.Length; j++) {
                    if (j != 0)
                        Console.Write(", ");    
                    Console.Write(languages[j]);
                }
            }
            Console.WriteLine();
        }
    
        // Text files are just name/value pairs.  ResText is the same format
        // with a unique extension to work around some ambiguities with MSBuild
        // ResX is our existing XML format from V1.
        private enum Format {
            Text, // .txt or .restext
            Binary, // .resources
        }
    
        // name/value pair
        private class Entry {
            public Entry(string name, object value) {
                this.name = name;
                this.value = value;
            }
    
            public string name;
            public object value;
        }
    }
}
