//------------------------------------------------------------------------------
// <copyright file="mergeattributes.cs" company="Microsoft">
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

/// <devdoc>
///     This is a small console application that merges the assembly attributes from 
///     two different files.  Because this is part of our build process, you can't
///     link to any Fx dlls!
/// </devdoc>
namespace Microsoft.Tools.Build {
    using System;
    using System.Collections;
    using System.IO;
    using System.Globalization;
    
    /// <include file='doc\mergeattributes.uex' path='docs/doc[@for="MergeAttributes"]/*' />
    public class MergeAttributes {

        /// <devdoc>
        ///     Adds attributes from the given file to the attribute hashtable.
        /// </devdoc>
        private static void AddFileAttributes(StreamReader file, Hashtable attributes) {
            string contents = file.ReadToEnd();
            int startBlock;
            int endBlock;
            LocateAttributeBlock(contents, out startBlock, out endBlock);
            string attributeBlock = contents.Substring(startBlock, endBlock - startBlock);
            ParseFileAttributes(attributeBlock, attributes);
        }
        
        /// <devdoc>
        ///     Writes out the merged file data to a stream.
        /// </devdoc>
        private static void EmitMergedFile(TextWriter stream, string beforeBlob, Hashtable attributes, string afterBlob) {
        
            stream.Write(beforeBlob);
            
            foreach(DictionaryEntry de in attributes) {
                stream.Write((string)de.Key);
                if (de.Value != null) {
                    stream.Write((string)de.Value);
                }
            }
            
            stream.Write(afterBlob);
        }
    
        /// <devdoc>
        ///     Throws a file error
        /// </devdoc>
        private static void ErrorFileNotExist(string fileName) {
            throw new Exception("The file " + fileName + " does not exist.");
        }
        
        /// <devdoc>
        ///     Throws a file error
        /// </devdoc>
        private static void ErrorInvalidFile() {
            throw new Exception("The file does not conform to the proper format.");
        }
        
        /// <devdoc>
        ///     Throws a usage error
        /// </devdoc>
        private static void ErrorUsage() {
        
            throw new Exception("Usage: MergeAttributes <primary file> <merge file> [-exclude <metadata key>] [-out <file>]\r\n" +
                                "       primary file    - The file to merge attributes into\r\n" +
                                "       merge file      - The file that contains attributes to merge\r\n" +
                                "       -exclude        - Zero or more metadata keys to exclude from the merge.  Typically\r\n" +
                                "                         you would exclude the .ver key to preserve the original\r\n" + 
                                "                         assembly's version.\r\n" + 
                                "       -out            - If specified, an output file to route to.  If not\r\n" +
                                "                         specified the output is routed to stdout.\r\n");
        }
        
        /// <devdoc>
        ///     Given a string of text this locates the inner attribute block for the assembly.
        /// </devdoc>
        private static void LocateAttributeBlock(string contents, out int startBlock, out int endBlock) {
            
            
            int startIndex = 0;
            int index;
            string searchString = ".assembly";
            char[] delims = new char[] {' '};
            
            startBlock = -1;    // sentinel so we know if we found it.
            endBlock = -1;
            
            while((index = contents.IndexOf(searchString, startIndex)) != -1) {
            
                // Grab the end of line and then parse the line.
                //
                int endLineIndex = contents.IndexOf('\n', index);
                string line = contents.Substring(index, endLineIndex - index);
                string[] tokens = line.Split(delims);
                
                // If we've found the right block, tokens will have two elements
                // and the second element won't be "extern".
                //
                if (tokens.Length == 2 && !tokens[1].Equals("extern")) {
                    // This is our location.  Advance until we find the starting
                    // "{"
                    for(int i = endLineIndex; i < contents.Length; i++) {
                        if (contents[i] == '{') {
                            while(++i < contents.Length && contents[i] != '\n');
                            startBlock = i + 1;
                            break;
                        }
                    }
                    
                    if (startBlock == -1) {
                        ErrorInvalidFile();
                    }
                    
                    // Now advance until we find the ending point.
                    //
                    endBlock = contents.IndexOf("}", startBlock);
                    
                    if (endBlock == -1) {
                        ErrorInvalidFile();
                    }
                    
                    break;
                }
                
                // Now advance to the next index.
                //
                startIndex = index + searchString.Length;
                if (startIndex >= contents.Length) {
                    break;
                }
            }
        }
    
        /// <include file='doc\mergeattributes.uex' path='docs/doc[@for="MergeAttributes.Main"]/*' />
        /// <devdoc>
        /// Main entry point.
        /// </devdoc>
        public static int Main(string[] args) {
        
            int returnCode = 0;
            
            StreamReader primaryFile = null;
            StreamReader mergeFile = null;
            TextWriter outFile = null;
            bool closeOutFile = false;
            
            try {
                string primaryFileName;
                string mergeFileName;
                string outFileName;
                ArrayList excludeList;
            
                // Parse the command line.
                //
                ParseArguments(args, out primaryFileName, out mergeFileName, out excludeList, out outFileName);
                
                // Open the files.
                //
                primaryFile = new StreamReader(File.OpenRead(primaryFileName));
                mergeFile = new StreamReader(File.OpenRead(mergeFileName));
                if (outFileName != null) {
                    if (File.Exists(outFileName)) {
                        File.Delete(outFileName);
                    }
                    outFile = new StreamWriter(File.OpenWrite(outFileName));
                    closeOutFile = true;
                }
                else {
                    outFile = Console.Out;
                }
                
                // We use a hashtable to map assembly attributes to their assignments.
                // We also keep track of "before" and "after" blobs of text for the primary file
                // so we can easily reconstruct the file.
                //
                string beforeBlob;
                string afterBlob;
                Hashtable attributes = new Hashtable();
                Hashtable excludeAttributes = null;
                
                ParsePrimaryFile(primaryFile, out beforeBlob, attributes, out afterBlob);

                if (excludeList != null) {
                    excludeAttributes = new Hashtable(excludeList.Count);
                    foreach(string s in excludeList) {
                        if (attributes.ContainsKey(s)) {
                            excludeAttributes[s] = attributes[s];
                        }
                    }
                }

                AddFileAttributes(mergeFile, attributes);

                // Now, if we got a set of excluded attributes, re-integrate
                // them.
                //
                if (excludeAttributes != null) {
                    foreach(DictionaryEntry de in excludeAttributes) {
                        attributes[de.Key] = de.Value;
                    }
                }

                EmitMergedFile(outFile, beforeBlob, attributes, afterBlob);
                
                outFile.Flush();
            }
            catch (Exception ex) {
                string message = ex.Message;
                if (message == null) {
                    message = ex.ToString();
                }
                
                Console.Error.WriteLine(message);
                returnCode = -1;
            }
        
            if (primaryFile != null) primaryFile.Close();
            if (mergeFile != null) mergeFile.Close();
            if (closeOutFile) outFile.Close();
            
            return returnCode;
        }
        
        /// <devdoc>
        ///     Parses our command line.
        /// </devdoc>
        private static void ParseArguments(string[] args, out string primaryFile, out string mergeFile, out ArrayList excludeList, out string outFile) {
        
            primaryFile = null;
            mergeFile = null;
            outFile = null;
            excludeList = null;
            
            for(int i = 0; i < args.Length; i++) {
                string arg = args[i];
                
                if (arg[0] == '-' || arg[0] == '/') {
                    string option = arg.Substring(1).ToLower(CultureInfo.InvariantCulture);
                    if (option.Equals("out")) {
                        if (outFile != null || ++i == args.Length) {
                            ErrorUsage();
                        }
                        
                        outFile = args[i];
                    }
                    else if (option.Equals("exclude")) {
                        if (++i == args.Length) {
                            ErrorUsage();
                        }
                        if (excludeList == null) {
                            excludeList = new ArrayList();
                        }
                        excludeList.Add(args[i]);
                    }
                    else {
                        ErrorUsage();
                    }
                }
                else {
                    if (primaryFile == null) {
                        primaryFile = arg;
                    }
                    else if (mergeFile == null) {
                        mergeFile = arg;
                    }
                    else {
                        ErrorUsage();
                    }
                }
            }
            
            // We should have a primary file and a merge file
            //
            if (primaryFile == null || mergeFile == null) {
                ErrorUsage();
            }
            
            // And both input files must exist
            //
            if (!File.Exists(primaryFile)) {
                ErrorFileNotExist(primaryFile);
            }
            
            if (!File.Exists(mergeFile)) {
                ErrorFileNotExist(mergeFile);
            }
        }
        
        /// <devdoc>
        ///     Takes the given block of text and parses attributes from it.
        /// </devdoc>
        private static void ParseFileAttributes(string block, Hashtable attributes) {
        
            // Search for the beginning of an attribute
            for(int idx = 0; idx < block.Length; idx++) {
                if (block[idx] == '.') {
                
                    // Got an attribute.  Now parse it.  If the attribute starts with
                    // "hash" or "ver", read one word and use that as the key.  If not,
                    // then read to an equal sign and use that as the key.
                    
                    int endIdx = block.IndexOf(' ', idx + 1);
                    if (endIdx == -1) {
                        ErrorInvalidFile();
                    }
                    
                    string token = block.Substring(idx, endIdx - idx);
                    if (token.Equals(".hash") || token.Equals(".ver")) {
                    
                        // Special token.  Parse to the end of the line.
                        //
                        int eolIdx = block.IndexOf('\n', endIdx + 1);
                        if (eolIdx == -1) {
                            eolIdx = block.Length - 1;
                        }
                        
                        string value = block.Substring(endIdx, eolIdx - endIdx + 1);
                        attributes[token] = value;
                        idx = eolIdx;
                    }
                    else {
                    
                        // Normal token.  Locate the "=" and then parse to the end of the ")"
                        //
                        int eqIdx = block.IndexOf('=', endIdx + 1);
                        if (eqIdx == -1) {
                            ErrorInvalidFile();
                        }
                        
                        // Normal token is up to the equals
                        //
                        token = block.Substring(idx, eqIdx - idx);
                        
                        // Must be sensitive to end of line comments.
                        //
                        int parenIdx = -1;
                        
                        for(int i = eqIdx + 1; i < block.Length; i++) {
                            if (block[i] == '/' && i + 1 < block.Length && block[i+1] == '/') {
                                // comment.  Skip to the end of the line.
                                while(i < block.Length && block[i] != '\n') {
                                    i++;
                                }
                                i++;
                                if (i >= block.Length) {
                                    break;
                                }
                            }
                            
                            if (block[i] == ')') {
                                parenIdx = i;
                                break;
                            }
                        }
                        
                        if (parenIdx == -1) {
                            ErrorInvalidFile();
                        }
                        
                        int eolIdx = block.IndexOf('\n', parenIdx + 1);
                        if (eolIdx == -1) {
                            ErrorInvalidFile();
                        }
                        
                        string value = block.Substring(eqIdx, eolIdx - eqIdx + 1);
                        attributes[token] = value;
                        idx = eolIdx;
                    }
                    
                }
                else if (block[idx] == '/' && idx < block.Length && block[idx + 1] == '/') {
                    // Advance to the end of the line
                    int eolIdx = block.IndexOf('\n', idx + 1);
                    if (eolIdx != -1) {
                        idx = eolIdx;
                    }
                    else {
                        idx = block.Length;
                    }
                }
            }
        }
        
        /// <devdoc>
        ///     Parses the primary file, adding attributes to the hashtable and
        ///     filling in the beginning and ending blobs.
        /// </devdoc>
        private static void ParsePrimaryFile(StreamReader file, out string beforeBlob, Hashtable attributes, out string afterBlob) {
            string contents = file.ReadToEnd();
            int startBlock;
            int endBlock;
            LocateAttributeBlock(contents, out startBlock, out endBlock);
            string attributeBlock = contents.Substring(startBlock, endBlock - startBlock);
            ParseFileAttributes(attributeBlock, attributes);
            beforeBlob = contents.Substring(0, startBlock);
            afterBlob = contents.Substring(endBlock);
       }
    }
}
