#region Using Directive
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Security;
using System.Security.Principal;
using Microsoft.VisualBasic.ApplicationServices;
using Microsoft.VisualBasic;
using System.Security.Cryptography;
#endregion


/// <summary>
/// Implements the IIdentity Interface which defines the basic functionality
/// of an identity object
/// </summary>
public class SampleIIdentity : IIdentity
{
    // Fields

    // Add private fields to store the user name and a value that indicates 
    // if the user is authenticated
    private bool authenticatedValue;
    private string nameValue;
    private BuiltInRole roleValue;

    // Methods

    /// <summary>
    /// Initializes the class by authenticating the user and then setting the 
    /// user name and role, based on a name and a password
    /// </summary>
    /// <param name="name"></param>
    /// <param name="password"></param>
    public SampleIIdentity(string name, string password)
    {
        if (this.IsValidNameAndPassword(name, password))
        {
            this.nameValue = name;
            this.authenticatedValue = true;
            this.roleValue = BuiltInRole.Administrator;
        }
        else
        {
            this.nameValue = "anonymouse";
            this.authenticatedValue = false;
            this.roleValue = BuiltInRole.Guest;
        }
    }

    private string GetHashedPassword(string username)
    {
        if (Strings.Trim(username).ToLower() == "testuser")
        {
            return "ZFFzgfsGjgtmExzWBRmZI5S4w6o=";
        }
        return "";
    }

    private string GetSalt(string username)
    {
        if (Strings.Trim(username).ToLower() == "testuser")
        {
            return "Should be a different random value for each user";
        }
        return "";
    }

    /// <summary>
    /// Determines if a user name and password combination is valid. You should
    /// not store user passwords in your system. You could store the hash of each 
    /// user's password. Read the following link for best practice. This sample
    /// is just for demonstration. 
    /// http://msdn.microsoft.com/en-us/library/ms172766.aspx 
    /// </summary>
    /// <param name="username"></param>
    /// <param name="password"></param>
    /// <returns></returns>
    private bool IsValidNameAndPassword(string username, string password)
    {
        string storedHashedPW = this.GetHashedPassword(username);
        string rawSalted = this.GetSalt(username) + Strings.Trim(password);
        byte[] saltedPwBytes = Encoding.Unicode.GetBytes(rawSalted);
        SHA1CryptoServiceProvider sha1 = new SHA1CryptoServiceProvider();
        return (Convert.ToBase64String(sha1.ComputeHash(saltedPwBytes)) == storedHashedPW);
    }

    // Properties

    /// <summary>
    /// Returns a string that indicates the current authentication mechanism
    /// </summary>
    public string AuthenticationType
    {
        get
        {
            return "Custom Authentication";
        }
    }

    /// <summary>
    /// Returns a value that indicates whether the user has been authenticated
    /// </summary>
    public bool IsAuthenticated
    {
        get
        {
            return this.authenticatedValue;
        }
    }

    /// <summary>
    /// Returns the user's name
    /// </summary>
    public string Name
    {
        get
        {
            return this.nameValue;
        }
    }

    /// <summary>
    /// Returns the user's role
    /// </summary>
    public BuiltInRole Role
    {
        get
        {
            return this.roleValue;
        }
    }

    /// <summary>
    /// Gets the type of authentication used
    /// </summary>
    string System.Security.Principal.IIdentity.AuthenticationType
    {
        get
        {
            return "Custom Authentication";
        }
    }

    /// <summary>
    /// Gets a value that indicates whether the user has been authenticated
    /// </summary>
    bool System.Security.Principal.IIdentity.IsAuthenticated
    {
        get
        {
            return this.authenticatedValue;
        }
    }

    // Gets the name of the current user
    string System.Security.Principal.IIdentity.Name
    {
        get
        {
            return this.nameValue;
        }
    }
}

 
 
