using System;
using System.Collections;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows.Media.Imaging;
using Microsoft.SDK.Samples.VistaBridge.Interop;

namespace Microsoft.SDK.Samples.VistaBridge.Library.KnownFolders
{
    /// <summary>
    /// Provides access to  
    /// information about a known folder. 
    /// </summary>
    /// <remarks> Obtain instances of this class by calling the
    /// <see cref="Microsoft.SDK.Samples.VistaBridge.Library.KnownFolders.KnownFolders.GetKnownFolder"/> method.</remarks>
    public class KnownFolder
    {
        IKnownFolder _knownFolder;
        FolderProperties properties;

        /// <summary>
        /// Creates a new instance of KnownFolder.
        /// </summary>
        internal KnownFolder(IKnownFolder knownFolder)
        {
            Debug.Assert(knownFolder != null);
            _knownFolder = knownFolder;
            GetFolderProperties();
        }

        /// <summary>
        /// Gets the friendly name of this known folder.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string Name
        {
            get { return properties.name; }
        }

        /// <summary>
        /// Gets the category designation for this known folder.
        /// </summary>
        /// <value>A <see cref="Microsoft.SDK.Samples.VistaBridge.Library.KnownFolders.FolderCategory"/> value.</value>
        public FolderCategory Category
        {
            get { return properties.category; }
        }

        /// <summary>
        /// Gets this known folder's canonical name.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string CanonicalName
        {
            get { return properties.canonicalName; }
        }

        /// <summary>
        /// Gets this known folder's description.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string Description
        {
            get { return properties.description; }
        }

        /// <summary>
        /// Gets the unique identifier for this known folder's parent folder.
        /// </summary>
        /// <value>A <see cref="System.Guid"/> value.</value>
        public Guid ParentId
        {
            get { return properties.parentId; }
        }

        /// <summary>
        /// Gets the name of this known folder's parent folder.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string Parent
        {
            get { return properties.parent; }
        }
        /// <summary>
        /// Gets this known folder's relative path.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string RelativePath
        {
            get { return properties.relativePath; }
        }

        /// <summary>
        /// Gets this known folder's parsing name.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string ParsingName
        {
            get { return properties.parsingName; }
        }
        /// <summary>
        /// Gets this known folder's tool tip text.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string Tooltip
        {
            get { return properties.tooltip; }
        }
        /// <summary>
        /// Gets the resource identifier for this 
        /// known folder's tool tip text.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string TooltipResourceId
        {
            get { return properties.tooltipResourceId; }
        }

        /// <summary>
        /// Gets this known folder's localized name.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string LocalizedName
        {
            get { return properties.localizedName; }
        }
        /// <summary>
        /// Gets the resource identifier for this 
        /// known folder's localized name.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string LocalizedNameResourceId
        {
            get { return properties.localizedNameResourceId; }
        }
        /// <summary>
        /// Gets the icon that is associated with this known folder.
        /// </summary>
        /// <value>A <see cref="System.Windows.Media.Imaging.BitmapSource"/> object.</value>
        public BitmapSource Icon
        {
            get { return properties.icon; }
        }
        /// <summary>
        /// Gets the resource identifier for this known 
        /// folder's icon.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string IconResourceId
        {
            get { return properties.iconResourceId; }
        }

        /// <summary>
        /// Gets this known folder's security attributes.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string Security
        {
            get { return properties.security; }
        }

        /// <summary>
        /// Gets this known folder's file attributes, 
        /// such as "read-only".
        /// </summary>
        /// <value>A <see cref="System.IO.FileAttributes"/> value.</value>
        public System.IO.FileAttributes FileAttributes
        {
            get { return properties.fileAttributes; }
        }

        /// <summary>
        /// Gets an value that describes this known folder's behaviors.
        /// </summary>
        /// <value>A <see cref="Microsoft.SDK.Samples.VistaBridge.Library.KnownFolders.DefinitionOptions"/> value.</value>
        public DefinitionOptions DefinitionOptions
        {
            get { return properties.definitionOptions; }
        }

        /// <summary>
        /// Gets the unique identifier for this known folder's type.
        /// </summary>
        /// <value>A <see cref="System.Guid"/> value.</value>
        public Guid FolderTypeId
        {
            get { return properties.folderTypeId; }
        }

        /// <summary>
        /// Gets a string representation of this known folder's type.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string FolderType
        {
            get { return properties.folderType; }
        }
        /// <summary>
        /// Gets the unique identifier for this known folder.
        /// </summary>
        /// <value>A <see cref="System.Guid"/> value.</value>
        public Guid FolderId
        {
            get { return properties.folderId; }
        }
        /// <summary>
        /// Gets the path for this known folder.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string Path
        {
            get { return properties.path; }
        }

        /// <summary>
        /// Gets a value that indicates whether this known folder's path exists on the 
        /// computer. 
        /// </summary>
        /// <value>A nullable <see cref="System.Boolean"/> value.</value>
        /// <remarks>If this property value is <b>null</b>, 
        /// the folder is a virtual folder.</remarks>
        public bool? PathExists
        {
            get { return properties.pathExists; }
        }

        /// <summary>
        /// Gets a value that states whether this known folder 
        /// can have its path set to a new value, 
        /// including any restrictions on the redirection.
        /// </summary>
        /// <value>A <see cref="Microsoft.SDK.Samples.VistaBridge.Library.KnownFolders.RedirectionCapabilities"/> value.</value>
        public RedirectionCapabilities Redirection
        {
            get { return properties.redirection; }
        }

        /// <summary>
        /// Gets the path of this this known folder.
        /// </summary>
        /// <param name="fileExists">
        /// Returns null if the folder is virtual, or a boolean
        /// value that indicates whether this known folder exists.
        /// </param>
        /// <returns>
        /// A <see cref="System.String"/> containing the path, or <see cref="System.String.Empty"/> if this known folder does not exist.
        /// </returns>
        private string GetPath(out bool? fileExists)
        {
            // Virtual folders do not have path.
            if (_knownFolder.GetCategory() == FolderCategory.Virtual)
            {
                fileExists = null;
                return String.Empty;
            }

            string p;
            fileExists = true;
            try
            {
                p = _knownFolder.GetPath(0);
            }
            catch (System.IO.FileNotFoundException)
            {
                p = String.Empty;
                fileExists = false;
            }
            return p;
        }

        /// <summary>
        /// Populates a structure that contains 
        /// this known folder's properties.
        /// </summary>
        private void GetFolderProperties()
        {
            SafeNativeMethods.NativeFolderDefinition nativeFolderDefinition;
            FolderProperties _definition = 
                new FolderProperties();

            this._knownFolder.GetFolderDefinition(
                out nativeFolderDefinition);

            try
            {
                _definition.category = nativeFolderDefinition.category;
                _definition.canonicalName =
                    Marshal.PtrToStringUni(nativeFolderDefinition.name);
                _definition.description =
                    Marshal.PtrToStringUni(nativeFolderDefinition.description);
                _definition.parentId = nativeFolderDefinition.parentId;
                _definition.relativePath =
                    Marshal.PtrToStringUni(nativeFolderDefinition.relativePath);
                _definition.parsingName =
                    Marshal.PtrToStringUni(nativeFolderDefinition.parsingName);
                _definition.tooltipResourceId =
                    Marshal.PtrToStringUni(nativeFolderDefinition.tooltip);
                _definition.localizedNameResourceId =
                    Marshal.PtrToStringUni(nativeFolderDefinition.localizedName);
                _definition.iconResourceId =
                    Marshal.PtrToStringUni(nativeFolderDefinition.icon);
                _definition.security =
                    Marshal.PtrToStringUni(nativeFolderDefinition.security);
                _definition.fileAttributes =
                    (System.IO.FileAttributes)nativeFolderDefinition.attributes;
                _definition.definitionOptions =
                    nativeFolderDefinition.definitionOptions;

                _definition.folderTypeId = 
                    nativeFolderDefinition.folderTypeId;

                _definition.folderType = FolderTypes.GetFolderType(
                    _definition.folderTypeId);

                bool? pathExists;

                _definition.path = GetPath(out pathExists);
                _definition.pathExists = pathExists;

                _definition.redirection = 
                    _knownFolder.GetRedirectionCapabilities();

                // Turn tooltip, localized name and icon resource IDs 
                // into the actual resources.
                _definition.tooltip = Helpers.GetStringResource(
                    _definition.tooltipResourceId);

                _definition.icon = Helpers.GetIcon(
                    _definition.iconResourceId);

                _definition.localizedName = Helpers.GetStringResource(
                    _definition.localizedNameResourceId);

                _definition.folderId = _knownFolder.GetId();
                _definition.name = FolderIdentifiers.NameForGuid(
                    _definition.folderId);

                if (_definition.parentId != Guid.Empty)
                    _definition.parent = FolderIdentifiers.NameForGuid(
                        _definition.parentId);
                else
                    _definition.parent = String.Empty;

                properties = _definition;
            }
            finally
            {
                // Clean up memory. 
                Marshal.FreeCoTaskMem(nativeFolderDefinition.name);
                Marshal.FreeCoTaskMem(nativeFolderDefinition.description);
                Marshal.FreeCoTaskMem(nativeFolderDefinition.relativePath);
                Marshal.FreeCoTaskMem(nativeFolderDefinition.parsingName);
                Marshal.FreeCoTaskMem(nativeFolderDefinition.tooltip);
                Marshal.FreeCoTaskMem(nativeFolderDefinition.localizedName);
                Marshal.FreeCoTaskMem(nativeFolderDefinition.icon);
                Marshal.FreeCoTaskMem(nativeFolderDefinition.security);
            }
        }
        /// <summary>
        /// Structure used internally to store property values for 
        /// a known folder. This structure holds the information
        /// returned in the FOLDER_DEFINITION structure, and 
        /// resources referenced by fields in NativeFolderDefinition,
        /// such as icon and tool tip.
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        internal struct FolderProperties
        {
            internal string name;
            internal FolderCategory category;
            internal string canonicalName;
            internal string description;
            internal Guid parentId;
            internal string parent;
            internal string relativePath;
            internal string parsingName;
            internal string tooltipResourceId;
            internal string tooltip;
            internal string localizedName;
            internal string localizedNameResourceId;
            internal string iconResourceId;
            internal BitmapSource icon;
            internal DefinitionOptions definitionOptions;
            internal System.IO.FileAttributes fileAttributes;
            internal Guid folderTypeId;
            internal string folderType;
            internal Guid folderId;
            internal string path;
            internal bool? pathExists;
            internal RedirectionCapabilities redirection;
            internal string security;
        }
    }
}
