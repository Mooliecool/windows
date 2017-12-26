// PackWebRequest SDK Sample - Simulator.cs
// Copyright (c) Microsoft Corporation. All Rights Reserved.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Threading;
using System.Windows;

namespace PackWebRequestSample
{
    /// <summary>
    /// This application simulates following sample scenario.
    /// Scenario overview:
    ///     Retail Server handles the interaction between clients and photo 
    ///     archive servers. Photo archive servers hold package files with a 
    ///     number of photos. Retail server holds the snapshot packages of the 
    ///     archive servers with thumbnails and information about the location 
    ///     of photos. 
    ///          Thumbnails are exposed to and can be selected by a client. 
    ///     Retail server retrieves photos on archive servers that correspond 
    ///     to the selected thumbnails and sends them to the client.
    /// 
    /// Scenario steps:
    /// 1.  Photo archive server 1 creates a photo package for its photos. 
    ///     It also creates a snapshot package with thumbnails and information 
    ///     of the photos. It sends the snapshot package to retail server.
    /// 2.  Photo archive server 2 does the same thing as in the step 1 for 
    ///     its photos.
    /// 3.  Client sends a query to retail server to request for photo 
    ///     thumbnails with keyword, say, 'dog'.
    /// 4.  Retail server processes the query and extracts thumbnails with 
    ///     related keywords from snapshot packages. The thumbnails are packed 
    ///     into a thumbnail package and sent to client.
    /// 5.  Client receives the thumbnail package, views and selects thumbnails 
    ///     interested. Client then sends a query with the IDs of selected 
    ///     thumbnails to retail server.
    /// 6.  Retail server uses the selected thumbnail IDs and information in 
    ///     the snapshot packages to retrieve (using PackWebRequest) selected 
    ///     photos from the photo archive servers. It creates an XPS document 
    ///     with retrieved photos and sends it to the client.
    /// 
    /// The three components in this scenario are simulated by three 
    /// applications: ArchiveServer, RetailServer and Client. They are 
    /// simulated on the local machine without any networking traffic. 
    /// Each component has its own folder. The communication between 
    /// components are simulated by reading/writing files in the subfolders 
    /// of each component.
    /// </summary>
    class Simulator
    {
        // Relative directory of the PackWebRequestSample VS solution
        private static string _root = @"..\..\..\";

        private static string _clientFolder = "Client";
        private static string _retailFolder = "RetailServer";
        private static string _archiveFolder = "ArchiveServer";

        private static string _clientExe = _clientFolder + @"\bin\Debug\Client.exe";
        private static string _retailExe =
            _retailFolder + @"\bin\Debug\RetailServer.exe";
        private static string _archiveExe =
            _archiveFolder + @"\bin\Debug\ArchiveServer.exe";

        // Folder on client that holds downloaded files from retail server
        private static string _clientDownloadFolder = _clientFolder + @"\Download";

        // Folder on retail server that holds snapshot packages from 
        // photo archive server
        private static string _retailArchiveFolder = _retailFolder + @"\ArchiveInfo";

        // Folder on retail server that holds query XML file from client
        private static string _retailQueryFolder = _retailFolder + @"\QueryBuffer";

        // Folder on photo archive server 1 that holds photos
        private static string _archivePhotoFolder1 =
            _archiveFolder + @"\Server1\Photos";

        // Folder on photo archive server 2 that holds photos
        private static string _archivePhotoFolder2 =
            _archiveFolder + @"\Server2\Photos";

        private static void Main()
        {
            // Set current directory to root VS solution directory.
            Environment.CurrentDirectory = _root;

            GetAbsolutePaths();

            // Step 1. Run archive server 1 to create its photo package and 
            //         snapshot package.
            //         It then sends the snapshot package to retail server.
            Process archiveServerProc = CreateProcess(_archiveExe);

            // For folder name argument, we should quote it with "" so 
            // folder name with space will still be treated as one argument.
            // (For example: "C:\My Folder 1").
            archiveServerProc.StartInfo.Arguments =
                "\"" + _archivePhotoFolder1 + "\" \""
                + _retailArchiveFolder + "\"";

            Console.WriteLine("==========Step 1===========");
            Console.WriteLine("[Archive Server 1]\tRunning...");
            archiveServerProc.Start();
            archiveServerProc.WaitForExit();

            if (!CheckExitCode(archiveServerProc))
                return;

            Console.WriteLine("[Archive Server 1]\tDone.");

            // Step 2. Run archive server 2 to create its photo package and 
            //         snapshot package.
            //         It then sends the snapshot package to retail server.
            archiveServerProc.StartInfo.Arguments =
                "\"" + _archivePhotoFolder2 + "\" \""
                + _retailArchiveFolder + "\"";

            Console.WriteLine("\n==========Step 2===========");
            Console.WriteLine("[Archive Server 2]\tRunning...");
            archiveServerProc.Start();
            archiveServerProc.WaitForExit();

            if (!CheckExitCode(archiveServerProc))
                return;

            Console.WriteLine("[Archive Server 2]\tDone.");

            // Step 3. Run client to send query to retail server requesting 
            //         for photo thumbnails with keyword "dog" for preview and
            //         selection.
            Process clientProc = CreateProcess(_clientExe);
            clientProc.StartInfo.Arguments =
                "-k dog \"" + _retailQueryFolder + "\" \""
                + _clientDownloadFolder + "\"";

            Console.WriteLine("\n==========Step 3===========");
            Console.WriteLine("[Client]\tRunning...");
            clientProc.Start();
            clientProc.WaitForExit();

            if (!CheckExitCode(clientProc))
                return;

            Console.WriteLine("[Client]\tDone.");

            // Step 4. Run retail server to process the query from client.
            //         In this case, it will extract thumbnails with keyword 
            //         'dog' from the snapshot package, pack them into a 
            //         thumbnail package and send it to the client.
            Process retailProc = CreateProcess(_retailExe);
            retailProc.StartInfo.Arguments =
                "\"" + _retailQueryFolder + "\" \""
                + _retailArchiveFolder + "\"";

            Console.WriteLine("\n==========Step 4===========");
            Console.WriteLine("[Retail Server]\tRunning...");

            retailProc.Start();
            retailProc.WaitForExit();

            if (!CheckExitCode(retailProc))
                return;

            Console.WriteLine("[Retail Server]\tDone.");

            // Step 5. Run client to select thumbnails from the thumbnail 
            //         package and send a request for selected photos to 
            //         the retail server.
            clientProc.StartInfo.Arguments =
                "-s \"" + _retailQueryFolder + "\" \""
                + _clientDownloadFolder + "\"";

            Console.WriteLine("\n==========Step 5===========");
            Console.WriteLine("[Client]\tRunning...");

            clientProc.Start();
            clientProc.WaitForExit();

            Console.WriteLine("[Client]\tDone.");

            if (!CheckExitCode(clientProc))
                return;

            // Step 6. Run retail server to process the query from client.
            //         In this case, it will retrieve selected photos from 
            //         archive server, create an XPS document with the photos, 
            //         and send it to the client.
            Console.WriteLine("\n==========Step 6===========");
            Console.WriteLine("[Retail Server]\tRunning...");

            retailProc.Start();
            retailProc.WaitForExit();

            Console.WriteLine("[Retail Server]\tDone.");

            if (!CheckExitCode(retailProc))
                return;

            Console.WriteLine("[Simulation Done.]");

            // Pause to show the console output.
            MessageBox.Show("Normal end-of-program.  Click OK to exit.",
                            "PackWebRequest Sample", MessageBoxButton.OK);
        }

        private static void GetAbsolutePaths()
        {
            // Get the absolute path of the root, our VS solution folder.
            _root = Environment.CurrentDirectory;

            // Once we get the absolute path of the root, we are able to 
            // resolve the absolute paths of other folders.
            _root += "\\";

            _clientExe = _root + _clientExe;
            _retailExe = _root + _retailExe;
            _archiveExe = _root + _archiveExe;

            _clientDownloadFolder = _root + _clientDownloadFolder;
            _retailArchiveFolder = _root + _retailArchiveFolder;
            _retailQueryFolder = _root + _retailQueryFolder;
            _archivePhotoFolder1 = _root + _archivePhotoFolder1;
            _archivePhotoFolder2 = _root + _archivePhotoFolder2;
        }

        private static Process CreateProcess(string exeFullPath)
        {
            Process proc = new Process();

            proc.StartInfo.FileName = exeFullPath;

            // We want to execute the process in the same shell as the 
            // current simulator process.
            proc.StartInfo.UseShellExecute = false;

            return proc;
        }

        private static bool CheckExitCode(Process p)
        {
            if (p.ExitCode != 0)
            {
                Console.WriteLine("The process failed. Return code: "
                    + p.ExitCode + ".");
                Console.WriteLine("Cannot continue simulation. Abort.");
                return false;
            }

            return true;
        }
    }
}
