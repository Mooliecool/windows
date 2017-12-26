using System;
using System.Web;

namespace AspNetForums.Components {

    // *********************************************************************
    //  User
    //
    /// <summary>
    /// This class contains the properties for a User.
    /// </summary>
    /// 
    // ********************************************************************/
    public class OnlineUser {
        String username = "";				// the user's Username (unique identifier)
        bool isAdmin = false;               // is the user in the administrators role
        bool isModerator = false;           // is the user a moderator

        /// <summary>
        /// Returns the user's Username.
        /// </summary>
        /// <remarks>The Username is what uniquely identifies each user.</remarks>
        public String Username {
            get { return username; }
            set { username = value; }			
        }

        /// <summary>
        /// Specifies if a user in an administrator or not.
        /// </summary>
        public bool IsAdministrator {
            get { return isAdmin; }
            set { isAdmin = value; }
        }

        /// <summary>
        /// Specifies if a user in a moderator or not.
        /// </summary>
        public bool IsModerator {
            get { return isModerator; }
            set { isModerator = value; }
        }
    }
}
