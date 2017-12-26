//------------------------------------------------------------------------------
// <copyright file="ResourcePermissionBaseEntry.cs" company="Microsoft">
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

namespace System.Security.Permissions {
    
    [
    Serializable()
    ]
    public class ResourcePermissionBaseEntry { 
        private string[] accessPath;
        private int permissionAccess;

        public ResourcePermissionBaseEntry() {
           this.permissionAccess = 0;
           this.accessPath = new string[0]; 
        }

        public ResourcePermissionBaseEntry(int permissionAccess, string[] permissionAccessPath) {
            if (permissionAccessPath == null)  
                throw new ArgumentNullException("permissionAccessPath");
                    
            this.permissionAccess = permissionAccess;
            this.accessPath = permissionAccessPath;
        }

        public int PermissionAccess {
            get {
                return this.permissionAccess;
            }                       
        }
        
        public string[] PermissionAccessPath {
            get {
                return this.accessPath;
            }            
        }    
    }
}  


