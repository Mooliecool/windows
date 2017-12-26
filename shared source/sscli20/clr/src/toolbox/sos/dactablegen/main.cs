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
using System.Globalization;

/******************************************************************************
 *
 *****************************************************************************/

public abstract class SymbolProvider 
{
    public enum SymType
    {
        GlobalData,
        GlobalFunction,
    };
    
    public abstract UInt32 GetGlobalRVA(String symbolName,
                                        SymType symType);
    public abstract UInt32 GetVTableRVA(String symbolName,
                                        String keyBaseName);
}


public class Shell
{
    const String dacSwitch   = "/dac:";
    const String pdbSwitch   = "/pdb:";
    const String mapSwitch   = "/map:";
    const String binSwitch   = "/bin:";
    const String helpSwitch  = "/?";

    public static void Help()
    {
        HelpHdr();
        Console.WriteLine();
        HelpBody();
    }

    public static void HelpHdr()
    {
String helpHdr =

////////////
@"Microsoft (R) CLR External Data Access Data Table Generator Version 0.1
Copyright (C) Microsoft Corp. 2002-2003.  All rights reserved.";
////////////

        Console.WriteLine(helpHdr);
    }

    public static void HelpBody()
    {
  
String helpMsg =

////////////
@"Usage:
  DacTableGen /dac:<file> [/pdb:<file>] [/map:<file>] /bin:<file>
 
Required:
  /dac:   The data access header file containing items to be added.

  /pdb:   The PDB file from which to get details.
  /map:   The MAP file from which to get details.
          In Windows, this file is created by providing /MAP in link.exe. 
          In UNIX, this file is created by the nm utility.

  /bin:   The binary output file.";
////////////

        Console.WriteLine(helpMsg);
    }

    public static bool MatchArg(String arg, String cmd)
    {
        if (arg.Length >= cmd.Length &&
            arg.Substring(0, cmd.Length).ToLower(CultureInfo.InvariantCulture).Equals(cmd.ToLower(CultureInfo.InvariantCulture)))
            return true;

        return false;
    }

    public static void DoMain(String[] args)
    {
        String dacFile    = null;
        String pdbFile    = null;
        String mapFile    = null;
        String binFile    = null;

        for (int i = 0; i < args.Length; i++)
        {
            if (MatchArg(args[i], dacSwitch))
            {
                dacFile = args[i].Substring(dacSwitch.Length);
            }
            else if (MatchArg(args[i], pdbSwitch))
            {
                pdbFile = args[i].Substring(pdbSwitch.Length);
            }
            else if (MatchArg(args[i], mapSwitch))
            {
                mapFile = args[i].Substring(mapSwitch.Length);
            }
            else if (MatchArg(args[i], binSwitch))
            {
                binFile = args[i].Substring(binSwitch.Length);
            }
            else if (MatchArg(args[i], helpSwitch))
            {
                Help();
                return;
            }
        }

        if (dacFile == null ||
            (pdbFile == null && mapFile == null) ||
            binFile == null)
        {
            HelpHdr();
            Console.WriteLine("\nRequired option missing.\n");
            HelpBody();

            return;
        }

        HelpHdr();
        Console.WriteLine();

        ArrayList rvaArray = new ArrayList();
        UInt32 numGlobals;

        if (pdbFile != null)
        {
            throw new InvalidOperationException("Not supported in Rotor.");
        }
        else
        {
            ScanDacFile(dacFile,
                new MapSymbolProvider(mapFile),
                rvaArray,
                out numGlobals);
        }

        UInt32 numVptrs;
        numVptrs = (UInt32)rvaArray.Count - numGlobals;

        FileStream outFile = new FileStream(binFile, FileMode.Create,
                                            FileAccess.Write);
        BinaryWriter binWrite = new BinaryWriter(outFile);

        // Write header information with counts of globals and vptrs.
        binWrite.Write(numGlobals);
        binWrite.Write(numVptrs);
        for (int i = 0; i < numGlobals + numVptrs; i++)
        {
            binWrite.Write((UInt32)rvaArray[i]);
        }

        binWrite.Close();
    }

    public static void Main(string[] args)
    {
        try
        {
            DoMain(args);
        }
        catch(Exception e)
        {
            Console.WriteLine("BUILDMSG: " + e.ToString());
            Environment.ExitCode = 1;
        }
    }
    
    private static void ScanDacFile(String file, 
                                    SymbolProvider sf,
                                    ArrayList rvaArray, 
                                    out UInt32 numGlobals)
    {
        StreamReader strm =
            new StreamReader(file, System.Text.Encoding.ASCII);
        String line;
        Hashtable vtables = new Hashtable(); // hashtable to guarantee uniqueness of entries

        //
        // Scan through the data access header file looking
        // for the globals structure.
        //

        for (;;)
        {
            line = strm.ReadLine();
            if (line == null)
            {
                throw new
                    InvalidOperationException("Invalid dac header format");
            }
            else if (line == "typedef struct _DacGlobals")
            {
                break;
            }
        }
    
        if (strm.ReadLine() != "{")
        {
            throw new InvalidOperationException("Invalid dac header format");
        }
    
        //
        // All the globals come first so pick up each line that
        // begins with ULONG.
        //

        bool fFoundVptrs = false;
        numGlobals = 0;
    
        for (;;)
        {
            line = strm.ReadLine().Trim();

            if (   line.Equals("union {")
                || line.Equals("};")
                || line.Equals("struct {")
                || line.Equals("} _helpers_;")
                || line.Equals("ULONG dac__rgJITHelperRVAs[1];")
                || line.StartsWith("#line "))
            {
                // Ignore.
            }
            else if (line.StartsWith("ULONG "))
            {
                UInt32 rva = 0;
                
                line = line.Remove(0, 6);
                line = line.TrimEnd(";".ToCharArray());

                string vptrSuffixSingle = "__vtAddr";
                string vptrSuffixMulti = "__mvtAddr";
                string vptrSuffix = null;

                if (line.EndsWith(vptrSuffixSingle))
                {
                    vptrSuffix = vptrSuffixSingle;
                }
                else if (line.EndsWith(vptrSuffixMulti))
                {
                    vptrSuffix = vptrSuffixMulti;
                }

                if (vptrSuffix != null)
                {
                    if (!fFoundVptrs)
                    {
                        numGlobals = (UInt32)rvaArray.Count;
                        fFoundVptrs = true;
                    }

                    line = line.Remove(line.Length - vptrSuffix.Length,
                                       vptrSuffix.Length);

                    string keyBaseName = null;
                    string descTail = null;

                    if (vptrSuffix == vptrSuffixMulti)
                    {
                        // line now has the form <class>__<base>, so
                        // split off the base.
                        int basePrefix = line.LastIndexOf("__");
                        if (basePrefix < 0)
                        {
                            throw new InvalidOperationException("VPTR_MULTI_CLASS has no keyBase.");
                        }
                        keyBaseName = line.Substring(basePrefix + 2);
                        line = line.Remove(basePrefix);
                        descTail = " for " + keyBaseName;
                    }

                    rva = sf.GetVTableRVA(line, keyBaseName);
                    
                    if (rva == UInt32.MaxValue)
                    {
                        Console.WriteLine("    " + ToHexNB(rva) +
                                          ", // WARNING: invalid vtable " +
                                          line + descTail);
                    }
                    else
                    {
                        String existing = (String)vtables[rva];
                        if (existing != null)
                        {                       
                            throw new InvalidOperationException(existing + " and " + line + " are at the same offsets." +
                                 " Add VPTR_UNIQUE(<a random unique number here>) to the offending classes to make their vtables unique.");
                        }
                        vtables[rva] = line;       

                        Console.WriteLine("    " + ToHexNB(rva) +
                                          ", // vtable " + line + descTail);
                    }
                }
                else
                {
                    SymbolProvider.SymType symType;
                    
                    if (fFoundVptrs)
                        throw new InvalidOperationException("Invalid dac header format.  Vtable pointers must be last.");
                        
                    string helperPrefix = "dac__JIT_HELPER_FUNCTION__";
                    if (line.StartsWith(helperPrefix))
                    {
                        // Line is of the form "dac__JIT_HELPER_FUNCTION__<helper code>__<function>".
                        // We want the function name.

                        symType = SymbolProvider.SymType.GlobalFunction;

                        int iFunction = line.IndexOf("__", helperPrefix.Length);
                        if (-1 == iFunction)
                            throw new InvalidOperationException("Invalid dac header format");

                        line = line.Remove(0, iFunction + 2);

                        if (line.Equals("NULL"))
                        {
                            rva = UInt32.MaxValue;
                        }
                    }
                    else
                    {
                        if (line.StartsWith("dac__"))
                        {
                            // Global variables, use the prefix.
                            line = line.Remove(0, 5);
                            symType = SymbolProvider.SymType.GlobalData;
                        }
                        else if (line.StartsWith("fn__"))
                        {
                            // Global or static functions, use the prefix.
                            line = line.Remove(0, 4);
                            line = line.Replace("__", "::");
                            symType = SymbolProvider.SymType.GlobalFunction;
                        }
                        else
                        {
                            // Static member variable, use the full name with
                            // namespace replacement.
                            line = line.Replace("__", "::");
                            symType = SymbolProvider.SymType.GlobalData;
                        }
                    }

                    if (0 == rva)
                    {
                        rva = sf.GetGlobalRVA(line, symType);

                        if (rva == UInt32.MaxValue)
                        {
                            Console.WriteLine("    " + ToHexNB(rva) +
                                              ", // WARNING: invalid symbol " +
                                              line);
                        }
                        else
                        {
                            Console.WriteLine("    " + ToHexNB(rva) + ", // " +
                                              line);
                        }
                    }
                }

                rvaArray.Add(rva);

            }
            else if (line == "")
            {
                // Skip blanks.
            }
            else
            {
                // We hit a non-global so we're done.
                if (!line.Equals("} DacGlobals;"))
                {
                    throw new
                        InvalidOperationException("Invalid dac header format at \"" + line + "\"");
                }
                break;
            }
        }

        if (!fFoundVptrs)
            throw new InvalidOperationException("Invalid dac header format.  Vtable pointers not found.");
    }
    
    private static String ToHex(Object o)
    {
        if (o is UInt32 || o is Int32)
            return String.Format("0x{0:x8}", o);
        else if (o is UInt64 || o is Int64)
            return String.Format("0x{0:x16}", o);
        else
            return null;
    }

    private static String ToHexNB(Object o)
    {
        return String.Format("0x{0:x}", o);
    }
}
