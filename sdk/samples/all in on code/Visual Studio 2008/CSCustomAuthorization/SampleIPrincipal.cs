#region Using Directive
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Security;
using System.Security.Principal;
#endregion


/// <summary>
/// Implements the IPrincipal nterface which defines the basic functionality
/// of a principal object
/// </summary>
public class SampleIPrincipal : IPrincipal
{
    // Fields

    /// <summary>
    /// To store the identity associated with this principal
    /// </summary>
    private SampleIIdentity identityValue;

    // Methods

    /// <summary>
    /// Initializes the class with a new instance of SampleIIdentity given a 
    /// user name and password
    /// </summary>
    /// <param name="name"></param>
    /// <param name="password"></param>
    public SampleIPrincipal(string name, string password)
    {
        this.identityValue = new SampleIIdentity(name, password);
    }

    /// <summary>
    /// Determines whether the current principal belongs to the specified role
    /// </summary>
    /// <param name="role"></param>
    /// <returns></returns>
    public bool IsInRole(string role)
    {
        return (role == this.identityValue.Role.ToString());
    }

    // Properties

    /// <summary>
    /// Returns the user identity of the current principal
    /// </summary>
    public System.Security.Principal.IIdentity Identity
    {
        get
        {
            return this.identityValue;
        }
    }
}