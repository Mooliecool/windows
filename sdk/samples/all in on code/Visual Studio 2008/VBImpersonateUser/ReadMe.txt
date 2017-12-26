========================================================================
    WINDOWS APPLICATION : VBImpersonateUser Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Windows Impersonation is a powerful feature Windows uses frequently in its 
security model. In general Windows also uses impersonation in its client/
server programming model.Impersonation lets a server to temporarily adopt 
the security profile of a client making a resource request. The server can
then access resources on behalf of the client, and the OS carries out the 
access validations.
A server impersonates a client only within the thread that makes the 
impersonation request. Thread-control data structures contain an optional 
entry for an impersonation token. However, a thread's primary token, which
represents the thread's real security credentials, is always accessible in 
the process's control structure.

After the server thread finishes its task, it reverts to its primary 
security profile. These forms of impersonation are convenient for carrying 
out specific actions at the request of a client and for ensuring that object
accesses are audited correctly.

In this code sample we use the LogonUser API and the WindowsIdentity.
Impersonate method to impersonate the user represented by the specified user
token. Then display the current user via the WindowsIdentity.GetCurrent 
method to show user impersonation. LogonUser can only be used to log onto 
the local machine; it cannot log you onto a remote computer. The account 
that you use in the LogonUser() call must also be known to the local 
machine, either as a local account or as a domain account that is visible
to the local computer. If LogonUser is successful, then it will give you an
access token that specifies the credentials of the user account you chose.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Impersonate the user for calling thread. 

  1.1 Gather the credential information of the impersonated user.
  
  1.2 P/Invoke LogonUser with the credential information to get a token for 
  the user.
  
  1.3 Pass the token to WindowsIdentity.Impersonate and get the impersonation 
  context.

2. Undo impersonation. (WindowsImpersonationContext.Undo)


/////////////////////////////////////////////////////////////////////////////
References:

Safe Impersonation With Whidbey
http://blogs.msdn.com/shawnfa/archive/2005/03/24/401905.aspx

How to Impersonate
http://blogs.msdn.com/shawnfa/archive/2005/03/21/400088.aspx

[Security] - User Impersonation
http://www.codeproject.com/KB/system/UserImpersonation.aspx


/////////////////////////////////////////////////////////////////////////////
