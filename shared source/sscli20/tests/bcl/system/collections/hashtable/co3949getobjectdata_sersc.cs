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
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
public class Co3949GetObjectData_SerSC
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Hashtable.GetObjectData(SerializationInfo, StreamingContext)";
    public static String s_strTFName        = "Co3949GetObjectData_SerSC.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        Hashtable dic1;
        Int32 iNumberOfItems = 10;
        SerializationInfo ser1;
        Object[] serKeys;
        Object[] serValues;
        Hashtable hsh1;
        Hashtable hsh3;
        Hashtable hsh4;
        DictionaryEntry[] strValueArr;
        MemoryStream ms1;
        try 
        {
            do
            {
                strLoc = "Loc_8345vdfv";
                dic1 = new Hashtable();
                for(int i=0; i<iNumberOfItems; i++)
                {
                    dic1.Add(i, "String_" + i);
                }
                ser1 = new SerializationInfo(typeof(Hashtable), new FormatterConverter());
                dic1.GetObjectData(ser1, new StreamingContext());
                iCountTestcases++;
                if(ser1.GetSingle("LoadFactor") != 0.72f) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_748cdg! Expected value not returned, " + ser1.GetSingle("LoadFactor"));
                }
                if(ser1.GetInt32("Version") != 11) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_01823csdf! Expected value not returned, " + ser1.GetSingle("Version"));
                }
                if(ser1.GetSingle("HashSize") != 23) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_7132fgfg! Expected value not returned, " + ser1.GetSingle("LoadFactor"));
                }
                serKeys = (Object[])ser1.GetValue("Keys", typeof(Object[]));
                    serValues = (Object[])ser1.GetValue("Values", typeof(Object[]));
                        Array.Sort(serKeys);
                Array.Sort(serValues);
                for(int i=0; i<iNumberOfItems; i++)
                {
                    if((Int32)serKeys[i] != i) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_1nd342_" + i + "! Expected value not returned, " + i);
                    }
                    if(!((String)serValues[i]).Equals("String_" + i)) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_7539fdg_" + i + "! Expected value not returned, " + i);
                    }
                }
                try
                {
                    iCountTestcases++;
                    dic1.GetObjectData(null, new StreamingContext());
                    iCountErrors++;
                    Console.WriteLine("Err_7439dg! Exception not thrown");
                }
                catch(ArgumentNullException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_6572fdg! Unexpected exception thrown, " + ex);
                }
                iCountTestcases++;
                hsh1 = new Hashtable();
                for(int i=0; i<10; i++)
                {
                    hsh1.Add("Key_" + i, "Value_" + i);
                }
                BinaryFormatter formatter = new BinaryFormatter();
                ms1 = new MemoryStream();
                formatter.Serialize(ms1, hsh1);
                ms1.Position = 0;
                hsh4 = (Hashtable)formatter.Deserialize(ms1);
                if(hsh4.Count != hsh1.Count) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_072xsf! Expected value not returned, " + hsh4.Count);
                }				
                strValueArr = new DictionaryEntry[hsh4.Count];
                hsh4.CopyTo(strValueArr, 0);
                hsh3 = new Hashtable();
                for(int i=0; i<10; i++)
                {
                    if(!hsh4.Contains("Key_" + i)) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_742ds8f! Expected value not returned, " + hsh4.Contains("Key_" + i));
                    }				
                    if(!hsh4.ContainsKey("Key_" + i)) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_742389dsaf! Expected value not returned, " + hsh4.ContainsKey("Key_" + i));
                    }				
                    if(!hsh4.ContainsValue("Value_" + i)) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_0672esfs! Expected value not returned, " + hsh4.ContainsValue("Value_" + i));
                    }				
                    if(!hsh1.ContainsValue(((DictionaryEntry)strValueArr[i]).Value)) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_87429dsfd! Expected value not returned, " + ((DictionaryEntry)strValueArr[i]).Value);
                    }				
                    try
                    {
                        hsh3.Add(((DictionaryEntry)strValueArr[i]).Value, null);
                    }
                    catch(Exception)
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_74298dsd! Exception thrown for  " + ((DictionaryEntry)strValueArr[i]).Value);
                    }
                }
            } while (false);
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.ToString());
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
            return true;
        }
        else
        {
            Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
            return false;
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co3949GetObjectData_SerSC cbA = new Co3949GetObjectData_SerSC();
        try 
        {
            bResult = cbA.runTest();
        } 
        catch (Exception exc_main)
        {
            bResult = false;
            Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
        }
        if (!bResult)
        {
            Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
            Console.WriteLine( " " );
            Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
            Console.WriteLine( " " );
        }
        if (bResult) Environment.ExitCode=0; else Environment.ExitCode=1;
    }
}
internal class Foo 
{
    internal String strValue;
    internal Foo()
    {
        strValue = "Hello World";
    }
}
