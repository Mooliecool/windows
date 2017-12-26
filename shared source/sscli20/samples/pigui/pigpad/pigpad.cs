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
using System.IO;
using SharedSourceCLI.TK;

namespace PigPad
{
    /// <summary>
    /// This is a notepad written with a combination of C# and TclTk
    /// </summary>
    class PigPad
    {       
        string cur_filename;

        void cmdHighlight(TclInterp interp, string[]argv)
        {
        }
        
        void cmdNew(TclInterp interp, string[]argv)
        {
            interp.Eval("set query [tk_messageBox -icon question -title \"New\" -message \"Are you sure?\" -type yesno]");
            string result = interp.GetVar("query");

            if(result=="yes")
            {
                interp.Eval(".text delete 1.0 end");

                //Update the titlebar to reflect filename
                interp.Eval("wm title . \"C# Notepad Using TCL - Untitled\"");
            }
        }
        
        void cmdOpen(TclInterp interp, string[]argv)
        {
            //Create a file open box.
            interp.Eval("set types { {\"Text files\" {.txt .doc}} {\"All files\" {*}} }");
            interp.Eval("set file [tk_getOpenFile -filetypes $types]");
            
            string filename = interp.GetVar("file");
            
            if(filename != "")
            {
                cur_filename = filename;
                //Clear the text box.
                interp.Eval(".text delete 1.0 end");

                //Open the file and dump it's contents into our text box.
                TextReader file = File.OpenText(filename);
                string command;
                command = ".text insert end {";
                command += file.ReadToEnd();
                command +="}";

                interp.Eval(command);
                file.Close();

                //Update titlebar to reflect filename changes.
                command = "wm title . \"C# Notepad Using TCL - ";
                command += filename;
                command += "\"";
                interp.Eval(command);
            }
        }
        
        void cmdSave(TclInterp interp, string[]argv)
        {
            //Create a file save box.
            if(cur_filename == null)
            {
                cmdSaveAs(interp, argv);
            }
            else
            {
                //Get text from the text box
                interp.Eval("set data [.text get 1.0 end]");
                string data = interp.GetVar("data");
                
                //Open the file and dump it's contents into our text box.
                TextWriter file = File.CreateText(cur_filename);
                file.Write(data);
                file.Close();
                interp.Eval("set data {}");
            }
        }
        
        void cmdSaveAs(TclInterp interp, string[]argv)
        {
            //Create a file save box.
            interp.Eval("set types { {\"Text files\" {.txt .doc}} {\"All files\" {*}} }");
            interp.Eval("set file [tk_getSaveFile -filetypes $types]");
            
            string filename = interp.GetVar("file");
            
            Console.WriteLine(filename);
            
            if(filename != "")
            {
                //Get text from the text box
                interp.Eval("set data [.text get 1.0 end]");
                string data = interp.GetVar("data");
                
                //Open the file and dump it's contents into our text box.
                TextWriter file = File.CreateText(filename);
                file.Write(data);
                file.Close();
                interp.Eval("set data {}");

                //Change the titlebar to reflect filename change.
                string command = "wm title . \"C# Notepad Using TCL - ";
                command += filename;
                command += "\"";
                interp.Eval(command);
            }
        }
        
        void cmdExit(TclInterp interp, string[]argv)
        {
            interp.Eval("set query [tk_messageBox -icon question -title \"Quit\" -message \"You may have unsaved changes,\nare you sure?\" -type yesno]");
            string result = interp.GetVar("query");

            if(result=="yes")
            {
                interp.Eval("exit");
            }
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
            
            //Setup the main window.
            interp.Eval("wm title . \"C# Notepad Using TCL - Untitled\"");
            
            //Create a text editor with a scrollbar - and pack it so that it always fills the 
            //available space in the window.
            interp.Eval("text .text -yscrollcommand \".scroll set\" -setgrid true -font {Courier 10}");
            interp.Eval("scrollbar .scroll -command \".text yview\"");
            interp.Eval("pack .scroll -side right -fill y");
            interp.Eval("pack .text -expand yes -fill both");

            //Setup the initial menus.
            interp.Eval("menu .menu -tearoff 0");
            interp.Eval("menu .menu.file -tearoff 0");
            interp.Eval("menu .menu.search -tearoff 0");

            //Add items to the 'file' menu and add it to the menubar.
            interp.Eval(".menu add cascade -label {File} -menu .menu.file");
            interp.Eval(".menu add cascade -label {Search} -menu .menu.search");

            interp.Eval(".menu.file add command -label {New} -command {cmdNew}");
            interp.Eval(".menu.file add command -label {Open...} -command {cmdOpen}");
            interp.Eval(".menu.file add command -label {Save...} -command {cmdSave}");
            interp.Eval(".menu.file add command -label {Save As...} -command {cmdSaveAs}");
            interp.Eval(".menu.file add separator");
            interp.Eval(".menu.file add command -label {Exit} -command {cmdExit}");
            
            interp.Eval(".menu.search add command -label {Highlight} -command {cmdHighlight}");

            //Enable menus for the root window.
            interp.Eval(". configure -menu .menu");

            //Setup the menu callbacks.
            interp.CreateCommand("cmdNew", new TclCmdProc(cmdNew));
            interp.CreateCommand("cmdOpen", new TclCmdProc(cmdOpen));
            interp.CreateCommand("cmdSave", new TclCmdProc(cmdSave));
            interp.CreateCommand("cmdSaveAs", new TclCmdProc(cmdSaveAs));
            interp.CreateCommand("cmdExit", new TclCmdProc(cmdExit));
            interp.CreateCommand("cmdHighlight", new TclCmdProc(cmdHighlight));
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
            new PigPad().Run();
        }
    }
}
