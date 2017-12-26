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
/*============================================================
 *
 * Purpose: Administer System.IO.IsolatedStorage
 *
 *                                     
 *                                    
 *
 ===========================================================*/

using System;
using System.Text;
using System.Reflection;
using System.Collections;
using System.IO.IsolatedStorage;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Globalization;
using System.Resources;

class StoreAdm
{
    // Global settings
    private static bool s_LogoPrinted = false;

    // User input Options

    private static bool  s_list    = false;
    private static bool  s_machine = false;
    private static bool  s_roaming = false;
    private static bool  s_remove  = false;
    private static bool  s_quiet   = false;

    private static IsolatedStorageScope s_Scope;
    private static ResourceManager s_resmgr = new ResourceManager("storeadm", Assembly.GetExecutingAssembly());

    private static String g_help0 = s_resmgr.GetString("g_help0");
    private static String g_help1 = s_resmgr.GetString("g_help1");
    private static String g_help2 = s_resmgr.GetString("g_help2");
    private static String g_help3 = s_resmgr.GetString("g_help3");
    private static String g_help4 = s_resmgr.GetString("g_help4");
    private static String g_help5 = s_resmgr.GetString("g_help5");
    private static String g_help6 = s_resmgr.GetString("g_help6");
    
    private static String g_help    = s_resmgr.GetString("g_help");
    private static String g_list    = s_resmgr.GetString("g_list");
    private static String g_machine = s_resmgr.GetString("g_machine");
    private static String g_roaming = s_resmgr.GetString("g_roaming");
    private static String g_remove  = s_resmgr.GetString("g_remove");
    private static String g_quiet   = s_resmgr.GetString("g_quiet");

    private static void Print(String str)
    {
        if (s_quiet)
            return;

        PrintLogo();

        Console.Write(str);
    }

    private static void PrintError(String str)
    {
        PrintLogo();
        Console.Write(str);
    }

    private static void Usage()
    {
        Print(Environment.NewLine);
        Print(s_resmgr.GetString("Usage1") + Environment.NewLine);
        Print(s_resmgr.GetString("Usage2") + Environment.NewLine);
        Print(s_resmgr.GetString("Usage3") + Environment.NewLine);
        Print(s_resmgr.GetString("Usage4") + Environment.NewLine);
        Print(s_resmgr.GetString("Usage5") + Environment.NewLine);
        Print(s_resmgr.GetString("Usage6") + Environment.NewLine);
        Print(s_resmgr.GetString("Usage7") + Environment.NewLine);
    }

    private static void PrintLogo()
    {
        if (s_quiet || s_LogoPrinted)
            return;

        s_LogoPrinted = true;

        StringBuilder sb = new StringBuilder();

        sb.Append(
            s_resmgr.GetString("PAL_Copyright1") +
            " " + ThisAssembly.InformationalVersion + 
            Environment.NewLine +
            CommonResStrings.CopyrightForCmdLine +
            Environment.NewLine
        );

        Console.WriteLine(sb.ToString());
    }

    private static void Main(String[] arg)
    {
        if (arg.Length == 0)
        {
            PrintLogo();
            Usage();
            return;
        }

        for (int i=0; i<arg.Length; ++i)
        {
            if ((String.Compare(g_help,  arg[i], true, CultureInfo.InvariantCulture) == 0) || 
                (String.Compare(g_help0, arg[i], true, CultureInfo.InvariantCulture) == 0) ||
                (String.Compare(g_help1, arg[i], true, CultureInfo.InvariantCulture) == 0) || 
                (String.Compare(g_help2, arg[i], true, CultureInfo.InvariantCulture) == 0) ||
                (String.Compare(g_help3, arg[i], true, CultureInfo.InvariantCulture) == 0) || 
                (String.Compare(g_help4, arg[i], true, CultureInfo.InvariantCulture) == 0) ||
                (String.Compare(g_help5, arg[i], true, CultureInfo.InvariantCulture) == 0)) {
                PrintLogo();
                Usage();
                return;
            } else if (String.Compare(g_quiet, arg[i], true, CultureInfo.InvariantCulture) == 0) {
                s_quiet = true;
            } else if (String.Compare(g_remove, arg[i], true, CultureInfo.InvariantCulture) == 0) {
                s_remove = true;
            } else if (String.Compare(g_list, arg[i], true, CultureInfo.InvariantCulture) == 0) {
                s_list = true;
            } else if (String.Compare(g_machine, arg[i], true, CultureInfo.InvariantCulture) == 0) {
                s_machine = true;
            } else if (String.Compare(g_roaming, arg[i], true, CultureInfo.InvariantCulture) == 0) {
                s_roaming = true;
            } else {
                PrintError(s_resmgr.GetString("Unknown_Option") + " ");
                PrintError(arg[i]);
                PrintError(Environment.NewLine);
                Usage();
                return;
            }
        }

        if (s_machine == true && s_roaming == true) {
            PrintError(s_resmgr.GetString("Invalid_Option_RM"));
            PrintError(Environment.NewLine);
            Usage();
            return;
        }

        try {
            Execute();
        } catch (Exception e) {
            PrintError(s_resmgr.GetString("Unknown_Error"));
            PrintError(Environment.NewLine);
            PrintError(e.ToString());
            PrintError(Environment.NewLine);
        }
        
    }

    private static void Execute()
    {
        if (s_roaming)
            s_Scope = IsolatedStorageScope.User | IsolatedStorageScope.Roaming;
        else if (s_machine)
            s_Scope = IsolatedStorageScope.Machine;
        else
            s_Scope = IsolatedStorageScope.User; 

        if (s_remove)
            Remove();

        if (s_list)
            List();
    }

    private static void Remove()
    {
        try {
            IsolatedStorageFile.Remove(s_Scope);
        } catch {
            PrintError(s_resmgr.GetString("Remove_Error"));
            PrintError(Environment.NewLine);
        }
    }

    private static void List()
    {
        IEnumerator e =
            IsolatedStorageFile.GetEnumerator(s_Scope);
        IsolatedStorageFile isf;
        IsolatedStorageScope scope;
        int i = 0;

        while (e.MoveNext())
        {
            ++i;
            isf = (IsolatedStorageFile) e.Current;
            try
            {
                scope = isf.Scope;

                Print(s_resmgr.GetString("Record_Number"));
                Print(i.ToString(CultureInfo.InvariantCulture));
                Print(Environment.NewLine);

                if ((scope & IsolatedStorageScope.Application) != 0)
                {
                    Print(s_resmgr.GetString("Application") + Environment.NewLine);
                    Print(isf.ApplicationIdentity.ToString());
                    Print(Environment.NewLine);
                }
                if ((scope & IsolatedStorageScope.Domain) != 0)
                {
                    Print(s_resmgr.GetString("Domain") + Environment.NewLine);
                    Print(isf.DomainIdentity.ToString());
                    Print(Environment.NewLine);
                }

                if ((scope & IsolatedStorageScope.Assembly) != 0)
                {
                    Print(s_resmgr.GetString("Assembly") + Environment.NewLine);
                    Print(isf.AssemblyIdentity.ToString());
                    Print(Environment.NewLine);
                }

                if (!s_roaming)
                {
                    Print("\t" + s_resmgr.GetString("Size") + " ");
                    Print(isf.CurrentSize.ToString(CultureInfo.InvariantCulture));
                    Print(Environment.NewLine);
                }
            }
            finally
            {
                isf.Close();
            }
        }
    }
}

