========================================================================
    WINDOWS APPLICATION : CSCustomAuthorization Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The .NET Framework provides an extensible framework for authorizing and 
authenticating users.

This sample demonstrates how to implement custom authentication and 
authorization by using classes that derive from IIdentity and IPrincipal. 
It also demonstrates how to override the application thread's default 
identity, the Windows identity, by setting CurrentPrincipal to an instance of 
the class that derives from IPrincipal. Based on credentials supplied by the 
user, we can provide access to resources based on that role. 

1. To create a class that implements IIdentity. An identity object represents
the user on whose behalf the code is running.

2. To create a class that implements IPrincipal. A principal object represents
the security context of the user on whose behalf the code is running, 
including that user's identity (IIdentity) and any roles to which they belong.

3. Collect user information to authenticate user. Then sets the thread's  
current principal for role-based security) 


/////////////////////////////////////////////////////////////////////////////
References:

Walkthrough: Implementing Custom Authentication and Authorization  
http://msdn.microsoft.com/en-us/library/ms172766.aspx

Authentication and Authorization in the .NET Framework with Visual Basic  
http://msdn.microsoft.com/en-us/library/ms233780.aspx

Role-Based Security
http://msdn.microsoft.com/en-us/library/52kd59t0.aspx


/////////////////////////////////////////////////////////////////////////////
