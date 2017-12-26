using System;
using System.Reflection;
using System.Collections.Generic;

namespace Microsoft.SDK.Samples.VistaBridge.Library.KnownFolders
{
    /// <summary>
    /// Provides access to the metadata for known folders. 
    /// This class contains properties to get the path of 
    /// standard known folders.
    /// </summary>
    public static class KnownFolders
    {
        /// <summary>
        /// Returns an object that represents the specified known folder.
        /// </summary>
        /// <param name="name">The name of the folder. 
        /// This can be the friendly name or the canonical name.</param>
        /// <param name="isCanonicalName">A <see cref="T:System.Boolean"/> value that indicates whether
        /// the name parameter is the friendly name or the canonical name.
        /// </param>
        /// <returns>A <see cref="Microsoft.SDK.Samples.VistaBridge.Library.KnownFolders.KnownFolder"/> object.</returns>
        public static KnownFolder GetKnownFolder(
            string name, 
            bool isCanonicalName)
        {
            return KnownFolderManager.GetKnownFolder(
                name, 
                isCanonicalName);
        }

        /// <summary>
        /// Returns a list of name and guid pairs for all known folders. 
        /// </summary>
        /// <returns>A <see cref="T:System.Collections.Generic.SortedList`2"/>.
        /// </returns>
        /// <remarks>The
        /// list returned by this method is sorted by folder name.</remarks>
        public static SortedList<string,Guid>GetAllFolders()
        {
            SortedList<string,Guid> s = FolderIdentifiers.GetAllFolders();
            return s;
        }

        /// <summary>
        /// Gets the path to the computer folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Computer
        {
            get
            {
                return KnownFolderManager.GetPath(
                    FolderIdentifiers.Computer);
            }
        }

        /// <summary>
        /// Gets the path to the Windows Vista Synchronization Manager folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Conflict
        {
            get
            {
                return KnownFolderManager.GetPath(
                    FolderIdentifiers.Conflict);
            }
        }

        /// <summary>
        /// Gets the path to the control panel.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>

        public static string ControlPanel
        {
            get
            {
                return KnownFolderManager.GetPath(
                    FolderIdentifiers.ControlPanel);
            }
        }

        /// <summary>
        /// Gets the path to the desktop folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Desktop
        {
            get
            {
                return KnownFolderManager.GetPath(
                    FolderIdentifiers.Desktop);
            }
        }

        /// <summary>
        /// Gets the path to the internet explorer folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Internet
        {
            get
            {
                return KnownFolderManager.GetPath(
                    FolderIdentifiers.Internet);
            }
        }

        /// <summary>
        /// Gets the path to the My Network Places folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Network
        {
            get
            {
                return KnownFolderManager.GetPath(
                    FolderIdentifiers.Network);
            }
        }

        /// <summary>
        /// Gets the path to the Printers and Faxes folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Printers
        {
            get
            {
                return KnownFolderManager.GetPath(
                    FolderIdentifiers.Printers);
            }
        }

        /// <summary>
        /// Gets the path to the Sync Center folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SyncManager
        {
            get
            {
                return KnownFolderManager.GetPath(
                    FolderIdentifiers.SyncManager);
            }
        }

        /// <summary>
        /// Gets the path to the network connections folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Connections
        {
            get
            {
                return KnownFolderManager.GetPath(
                    FolderIdentifiers.Connections);
            }
        }

        /// <summary>
        /// Gets the path to the Synch Setup folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SyncSetup
        {
            get
            {
                return KnownFolderManager.GetPath(
                    FolderIdentifiers.SyncSetup);
            }
        }

        /// <summary>
        /// Gets the path to the Sync Results folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SyncResults
        {
            get
            {
                return KnownFolderManager.GetPath(
                    FolderIdentifiers.SyncResults);
            }
        }

        /// <summary>
        /// Gets the path to the recycle bin.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string RecycleBin
        {
            get
            {
                return KnownFolderManager.GetPath(
                    FolderIdentifiers.RecycleBin);
            }
        }

        /// <summary>
        /// Gets the path to the fonts folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Fonts
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Fonts);
            }
        }

        /// <summary>
        /// Gets the path to the startup folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Startup
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Startup);
            }
        }

        /// <summary>
        /// Gets the path to the programs folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Programs
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Programs);
            }
        }

        /// <summary>
        /// Gets the path to the per-user start menu folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string StartMenu
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.StartMenu);
            }
        }

        /// <summary>
        /// Gets the path to the per-user My Recent Documents folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Recent
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Recent);
            }
        }

        /// <summary>
        /// Gets the path to the per-user SendTo folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SendTo
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.SendTo);
            }
        }

        /// <summary>
        /// Gets the path to the per-user documents folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Documents
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Documents);
            }
        }

        /// <summary>
        /// Gets the path to the per-user favorites folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Favorites
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Favorites);
            }
        }

        /// <summary>
        /// Gets the path to the network shortcuts folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string NetHood
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.NetHood);
            }
        }

        /// <summary>
        /// Gets the path to the Printer Shortcuts folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string PrintHood
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.PrintHood);
            }
        }

        /// <summary>
        /// Gets the path to the templates folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Templates
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Templates);
            }
        }

        /// <summary>
        /// Gets the path to the common startup folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string CommonStartup
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.CommonStartup);
            }
        }

        /// <summary>
        /// Gets the path to the common programs folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string CommonPrograms
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.CommonPrograms);
            }
        }

        /// <summary>
        /// Gets the path to the common start menu folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string CommonStartMenu
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.CommonStartMenu);
            }
        }

        /// <summary>
        /// Gets the path to the public desktop folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string PublicDesktop
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.PublicDesktop);
            }
        }

        /// <summary>
        /// Gets the path to the Application Data folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string ProgramData
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.ProgramData);
            }
        }

        /// <summary>
        /// Gets the path to the common templates folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string CommonTemplates
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.CommonTemplates);
            }
        }

        /// <summary>
        /// Gets the path to the public documents folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string PublicDocuments
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.PublicDocuments);
            }
        }

        /// <summary>
        /// Gets the path to the roaming application data folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string RoamingAppData
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.RoamingAppData);
            }
        }

        /// <summary>
        /// Gets the path to the per-user 
        /// application data folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string LocalAppData
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.LocalAppData);
            }
        }

        /// <summary>
        /// Gets the path to the LocalAppDataLow folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string LocalAppDataLow
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.LocalAppDataLow);
            }
        }

        /// <summary>
        /// Gets the path to the tempory internet files folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string InternetCache
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.InternetCache);
            }
        }

        /// <summary>
        /// Gets the path to the cookies folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Cookies
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Cookies);
            }
        }

        /// <summary>
        /// Gets the path to the history folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string History
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.History);
            }
        }

        /// <summary>
        /// Gets the path to the System32 folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string System
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.System);
            }
        }

        /// <summary>
        /// Gets the path to the System32 
        /// folder on x86 systems.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SystemX86
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.SystemX86);
            }
        }

        /// <summary>
        /// Gets the path to the windows folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Windows
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Windows);
            }
        }

        /// <summary>
        /// Gets the path to the current user's root folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Profile
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Profile);
            }
        }

        /// <summary>
        /// Gets the path to the per-user My Pictures folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Pictures
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Pictures);
            }
        }

        /// <summary>
        /// Gets the path to the ProgramFiles folder on x86 systems.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string ProgramFilesX86
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.ProgramFilesX86);
            }
        }

        /// <summary>
        /// Gets the path to the common Program files folder on x86 systems.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string ProgramFilesCommonX86
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.ProgramFilesCommonX86);
            }
        }

        /// <summary>
        /// Gets the path to the ProgramsFiles folder on x64 systems.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string ProgramFilesX64
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.ProgramFilesX64);
            }
        }

        /// <summary>
        /// Gets the path to the common Program files folder on x64 systems.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string ProgramFilesCommonX64
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.ProgramFilesCommonX64);
            }
        }

        /// <summary>
        /// Gets the path to the program files folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string ProgramFiles
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.ProgramFiles);
            }
        }

        /// <summary>
        /// Gets the path to the common program files folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string ProgramFilesCommon
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.ProgramFilesCommon);
            }
        }

        /// <summary>
        /// Gets the path to the administrative tools folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string AdminTools
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.AdminTools);
            }
        }

        /// <summary>
        /// Gets the path to the common administrative tools folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string CommonAdminTools
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.CommonAdminTools);
            }
        }

        /// <summary>
        /// Gets the path to the per-user My Music folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Music
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Music);
            }
        }

        /// <summary>
        /// Gets the path to the videos folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Videos
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Videos);
            }
        }

        /// <summary>
        /// Gets the path to the Shared Pictures folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string PublicPictures
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.PublicPictures);
            }
        }

        /// <summary>
        /// Gets the path to the Shared Music folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string PublicMusic
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.PublicMusic);
            }
        }

        /// <summary>
        /// Gets the path to the Shared Videos folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string PublicVideos
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.PublicVideos);
            }
        }

        /// <summary>
        /// Gets the path to the Resources  folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string ResourceDir
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.ResourceDir);
            }
        }

        /// <summary>
        /// Gets the path to the localized resources directory folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string LocalizedResourcesDir
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.LocalizedResourcesDir);
            }
        }

        /// <summary>
        /// Gets the path to the common OEM links folder. 
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string CommonOEMLinks
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.CommonOEMLinks);
            }
        }

        /// <summary>
        /// Gets the path to the folder used to hold files that 
        /// are waiting to be copied onto CD.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string CDBurning
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.CDBurning);
            }
        }

        /// <summary>
        /// Gets the path to the Users folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string UserProfiles
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.UserProfiles);
            }
        }

        /// <summary>
        /// Gets the path to the playlists folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Playlists
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Playlists);
            }
        }

        /// <summary>
        /// Gets the path to the sample playlists folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SamplePlaylists
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.SamplePlaylists);
            }
        }

        /// <summary>
        /// Gets the path to the music samples folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SampleMusic
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.SampleMusic);
            }
        }

        /// <summary>
        /// Gets the path to the picture samples folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SamplePictures
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.SamplePictures);
            }
        }

        /// <summary>
        /// Gets the path to the video samples folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SampleVideos
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.SampleVideos);
            }
        }

        /// <summary>
        /// Gets the path to the Slide Shows folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string PhotoAlbums
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.PhotoAlbums);
            }
        }

        /// <summary>
        /// Gets the path to the public folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Public
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Public);
            }
        }

        /// <summary>
        /// Gets the path to the change/remove programs folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string ChangeRemovePrograms
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.ChangeRemovePrograms);
            }
        }

        /// <summary>
        /// Gets the path to the installed updates folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string AppUpdates
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.AppUpdates);
            }
        }

        /// <summary>
        /// Gets the path to the Programs and Features folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string AddNewPrograms
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.AddNewPrograms);
            }
        }

        /// <summary>
        /// Gets the path to the per-user downloads folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Downloads
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Downloads);
            }
        }

        /// <summary>
        /// Gets the path to the public downloads folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string PublicDownloads
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.PublicDownloads);
            }
        }

        /// <summary>
        /// Gets the path to the per-user saved searches folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SavedSearches
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.SavedSearches);
            }
        }

        /// <summary>
        /// Gets the path to the per-user Quick Launch folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string QuickLaunch
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.QuickLaunch);
            }
        }

        /// <summary>
        /// Gets the path to the contacts folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Contacts
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Contacts);
            }
        }

        /// <summary>
        /// Gets the path to the per-user gadgets folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SidebarParts
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.SidebarParts);
            }
        }

        /// <summary>
        /// Gets the path to the shared Gadgets folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SidebarDefaultParts
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.SidebarDefaultParts);
            }
        }

        /// <summary>
        /// Gets the path to the tree properties folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string TreeProperties
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.TreeProperties);
            }
        }

        /// <summary>
        /// Gets the path to the public game tasks folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string PublicGameTasks
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.PublicGameTasks);
            }
        }

        /// <summary>
        /// Gets the path to the game explorer folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string GameTasks
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.GameTasks);
            }
        }

        /// <summary>
        /// Gets the path to the per-user saved games folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SavedGames
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.SavedGames);
            }
        }

        /// <summary>
        /// Gets the path to the games folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Games
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Games);
            }
        }

        /// <summary>
        /// Gets the path to the recorded tv folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        /// <remarks>This folder is not used.</remarks>
        public static string RecordedTV
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.RecordedTV);
            }
        }

        /// <summary>
        /// Gets the path to the Microsoft Office Outlook folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SearchMapi
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.SearchMapi);
            }
        }

        /// <summary>
        /// Gets the path to the offline files folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SearchCsc
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.SearchCsc);
            }
        }

        /// <summary>
        /// Gets the path to the per-user links folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string Links
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.Links);
            }
        }

        /// <summary>
        /// Gets the path to the user files folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string UsersFiles
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.UsersFiles);
            }
        }

        /// <summary>
        /// Gets the path to the search results folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string SearchHome
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.SearchHome);
            }
        }

        /// <summary>
        /// Gets the path to the original images folder.
        /// </summary>
        /// <value>A <see cref="T:System.String"/> object.</value>
        public static string OriginalImages
        {
            get
            {
                return KnownFolderManager.GetPath(FolderIdentifiers.OriginalImages);
            }
        }
    }
}