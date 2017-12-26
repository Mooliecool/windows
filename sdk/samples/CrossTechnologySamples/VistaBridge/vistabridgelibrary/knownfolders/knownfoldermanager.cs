using System;
using System.Collections;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Reflection;
using Microsoft.SDK.Samples.VistaBridge.Interop;

namespace Microsoft.SDK.Samples.VistaBridge.Library.KnownFolders
{
    /// <summary>
    /// Exposes methods that create known folders 
    /// and return the path for a specified known folder.
    /// </summary>
    internal static class KnownFolderManager
    {
        static IKnownFolderManager _knownFolderManager = (IKnownFolderManager)new KnownFolderManagerClass();

        /// <summary>
        /// Gets the known folder identified by the specified folder Id.
        /// </summary>
        /// <param name="knownFolderId">
        /// A Guid that identifies a known folder.
        /// </param>
        /// <returns>
        /// The known folder for the specified id.
        /// </returns>
        
        internal static KnownFolder GetKnownFolder(Guid knownFolderId)
        {
            IKnownFolder knownFolder;
            _knownFolderManager.GetFolder(knownFolderId, out knownFolder);

            return new KnownFolder(knownFolder);
        }
        
        /// <summary>
        /// Gets the known folder identified by its canonical name.
        /// </summary>
        /// <param name="canonicalName">
        /// A non-localized canonical name for the known folder.
        /// </param>
        /// <returns>
        /// A known folder representing the specified name.
        /// </returns>
        private static KnownFolder GetKnownFolder(string canonicalName)
        {
            IKnownFolder _knownFolder;
            _knownFolderManager.GetFolderByName(
                canonicalName, 
                out _knownFolder);

            return new KnownFolder(_knownFolder);
        }
        internal static KnownFolder GetKnownFolder(
            string name, bool isCanonicalName)
        {
            if (isCanonicalName)
                return GetKnownFolder(name);

            // The name must matche a field name in FolderIdentifiers.
            // The field holds the GUID for the folder.
            // Use reflection to get the GUID.

            Type folderIDs = typeof(FolderIdentifiers);

            FieldInfo fp = folderIDs.GetField(name,
                BindingFlags.NonPublic | BindingFlags.Static);

            if (fp == null)
            {
                throw new ArgumentException("Known folder name is not valid.");
            }
            Guid id = (Guid)fp.GetValue(null);
            KnownFolder kf = GetKnownFolder(id);
            return kf;
        }
        /// <summary>
        /// Returns the path of a known folder
        /// </summary>
        internal static string GetPath(Guid knownFolderId)
        {
            IKnownFolder _knownFolder;
            _knownFolderManager.GetFolder(knownFolderId, out _knownFolder);

            FolderCategory category = _knownFolder.GetCategory();
            if (category == FolderCategory.Virtual)
            {
                return String.Empty;
            }
            return _knownFolder.GetPath(0);
        }
    }
}