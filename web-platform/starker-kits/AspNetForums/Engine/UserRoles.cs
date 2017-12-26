using System;
using System.Collections;
using System.Web;
using System.Web.UI;
using System.Web.Security;
using System.Security.Principal;
using AspNetForums.Components;

namespace AspNetForums {

    // *********************************************************************
    //  UserRoles
    //
    /// <summary>
    /// The user roles class is used to manage user to role mappings.
    /// </summary>
    // ***********************************************************************/
    public class UserRoles {
        const string rolesCookie = "AspNetForumsRoles";

        // *********************************************************************
        //  GetUserRoles
        //
        /// <summary>
        /// Connects to the user role's datasource, retrieves all the roles a given
        /// user belongs to, and add them to the curret IPrincipal. The roles are retrieved
        /// from the datasource or from an encrypted cookie.
        /// </summary>
        // ***********************************************************************/
        public void GetUserRoles() {
            HttpContext Context = HttpContext.Current;
            string[] userRoles = null;
            string formattedUserRoles;

            // Is the request authenticated?
            if (!Context.Request.IsAuthenticated)
                return;

            // Get the roles this user is in
            if ((Context.Request.Cookies[rolesCookie] == null) || (Context.Request.Cookies[rolesCookie].Value == "")) {
                formattedUserRoles = String.Join(";", UserRoles.GetUserRoles(Context.User.Identity.Name));

                // Create authentication ticket
                FormsAuthenticationTicket ticket = new FormsAuthenticationTicket(
                    1,                              // version
                    Context.User.Identity.Name,     // user name
                    DateTime.Now,                   // issue time
                    DateTime.Now.AddHours(1),       // expires every hour
                    false,                          // don't persist cookie
                    formattedUserRoles              // roles
                    );

                // Encrypt the ticket
                String cookieStr = FormsAuthentication.Encrypt(ticket);

                // Send the cookie to the client
                Context.Response.Cookies[rolesCookie].Value = cookieStr;
                Context.Response.Cookies[rolesCookie].Path = Globals.ApplicationVRoot;
                Context.Response.Cookies[rolesCookie].Expires = DateTime.Now.AddMinutes(5);
            } else {

                // Get roles from roles cookie
                //
                FormsAuthenticationTicket ticket = FormsAuthentication.Decrypt(Context.Request.Cookies[rolesCookie].Value);

                if (ticket.Name != Context.User.Identity.Name) {

                    Context.Response.Cookies[rolesCookie].Expires = DateTime.Now;

                    return;

                } else {

                    //convert the string representation of the role data into a string array
                    ArrayList rolesArrayList = new ArrayList();
                    foreach (String role in ticket.UserData.Split( new char[] {';'} )) {
                        if (role.Length > 0)
                            rolesArrayList.Add(role);
                    }

                    userRoles = (string[]) rolesArrayList.ToArray(typeof(string));
                }

            }

            // Add our own custom principal to the request containing the roles in the auth ticket
            Context.User = new GenericPrincipal(Context.User.Identity, userRoles);
        }

        // *********************************************************************
        //  DeleteRole
        //
        /// <summary>
        /// Deletes a security role and any associated forum and user connections
        /// </summary>
        // ***********************************************************************/
        public static void DeleteRole(string role)
        {
            IDataProviderBase dp = DataProvider.Instance();
            dp.DeleteRole(role);
        }

        // *********************************************************************
        //  UpdateRole
        //
        /// <summary>
        /// Updates the description for a given role.
        /// </summary>
        // ***********************************************************************/
        public static void UpdateRole(string role, string description)
        {
            IDataProviderBase dp = DataProvider.Instance();
            dp.UpdateRoleDescription(role, description);
        }

        // *********************************************************************
        //  GetRoleDescription
        //
        /// <summary>
        /// Get the description of a given role
        /// </summary>
        // ***********************************************************************/
        public static string GetRoleDescription(string role)
        {
            IDataProviderBase dp = DataProvider.Instance();
            return dp.GetRoleDescription(role);
        }

        // *********************************************************************
        //  CreateNewRole
        //
        /// <summary>
        /// Creates a new security role
        /// </summary>
        // ***********************************************************************/
        public static void CreateNewRole(string role, string description)
        {
            IDataProviderBase dp = DataProvider.Instance();
            dp.CreateNewRole(role, description);
        }

        // *********************************************************************
        //  AddUserToRole
        //
        /// <summary>
        /// Adds a specified user to a role
        /// </summary>
        // ***********************************************************************/
        public static void AddUserToRole(string username, string role)
        {
            IDataProviderBase dp = DataProvider.Instance();
            dp.AddUserToRole(username, role);
        }

        // *********************************************************************
        //  AddForumToRole
        //
        /// <summary>
        /// Adds a specified user to a role
        /// </summary>
        // ***********************************************************************/
        public static void AddForumToRole(int forumID, string role)
        {
            IDataProviderBase dp = DataProvider.Instance();
            dp.AddForumToRole(forumID, role);
        }

        // *********************************************************************
        //  RemoveUserFromRole
        //
        /// <summary>
        /// Removes the specified user from a role
        /// </summary>
        // ***********************************************************************/
        public static void RemoveUserFromRole(string username, string role)
        {
            IDataProviderBase dp = DataProvider.Instance();
            dp.RemoveUserFromRole(username, role);
        }

        // *********************************************************************
        //  RemoveForumFromRole
        //
        /// <summary>
        /// Removes the specified forum from a role
        /// </summary>
        // ***********************************************************************/
        public static void RemoveForumFromRole(int forumID, string role)
        {
            IDataProviderBase dp = DataProvider.Instance();
            dp.RemoveForumFromRole(forumID, role);
        }

        // *********************************************************************
        //  GetAllRoles
        //
        /// <summary>
        /// All the roles that the system supports
        /// </summary>
        /// <returns>String array of roles</returns>
        // ***********************************************************************/
        public static String[] GetAllRoles() 
        {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetAllRoles();
        }

        // *********************************************************************
        //  GetForumRoles
        //
        /// <summary>
        /// Get all of the roles that a given forum belongs to
        /// </summary>
        /// <returns>String array of roles</returns>
        // ***********************************************************************/
        public static String[] GetForumRoles(int forumID) 
        {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetForumRoles(forumID);
        }

        // *********************************************************************
        //  GetUserRoles
        //
        /// <summary>
        /// All the roles that the named user belongs to
        /// </summary>
        /// <param name="username">Name of user to retrieve roles for</param>
        /// <returns>String array of roles</returns>
        // ***********************************************************************/
        public static String[] GetUserRoles(string username) 
        {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetUserRoles(username);
        }

        // *********************************************************************
        //  SignOut
        //
        /// <summary>
        /// Cleans up cookies used for role management when the user signs out.
        /// </summary>
        // ***********************************************************************/
        public static void SignOut() {
            HttpContext Context = HttpContext.Current;

            // Invalidate roles token
            Context.Response.Cookies[rolesCookie].Value = null;
            Context.Response.Cookies[rolesCookie].Expires = new System.DateTime(1999, 10, 12);
            Context.Response.Cookies[rolesCookie].Path = Globals.ApplicationVRoot;
        }
    }
}