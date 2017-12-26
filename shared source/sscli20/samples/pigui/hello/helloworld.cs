//------------------------------------------------------------------------------
// <copyright file="PigPad.cs" company="Microsoft">
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
using SharedSourceCLI.TK;

namespace TkHelloWorld
{
    /// <summary>
    /// Summary description for HelloWorld.
    /// </summary>
    class HelloWorld
    {       
        void doItProc(TclInterp interp, string[] s)
        {
            System.Console.Write("Hello World!! [{0}] [{1}]\n", s.Length, s[0]);
            interp.Eval(".b configure -bg PeachPuff1");
        }

        void InitProc(TclInterp interp)
        {

            /*
             * Initialize TCL - basically this function runs the init.tcl file which 
             * should be in your path environment variable.
             */
            interp.TclInit();
    
            /*
             * Initialize the TK libs - basically loads all of the .tcl files that define
             * the widgets - these should be in the tcl\lib path (which should also be in
             * your path environment variable).
             */
            interp.TkInit();

            /*
             * Evaluate a script - this creates a button, called .b with a label "Hello, World!".
             * When the button is pressed it writes "Hello, world" to the console.
             */
            
            //Create the main window.
            interp.Eval("button .b -text {Hello, World!} -command {doIt}; pack .b");
            interp.CreateCommand("doIt", new TclCmdProc(doItProc));
        }
                    
        void Run()
        {                   
            //
            // Create the interpreter - and start the application.
            //
            TclInterp interp = new TclInterp();
            interp.TkMain(new TclAppInitProc(InitProc));
        }       
        
        static void Main()
        {           
            new HelloWorld().Run();
        }       
    }
}
