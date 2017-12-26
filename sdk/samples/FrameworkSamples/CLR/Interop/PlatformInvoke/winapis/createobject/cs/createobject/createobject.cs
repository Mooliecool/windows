//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//-----------------------------------------------------------------------
// CreateObject.cs

using System;
using System.Threading;
using System.Runtime.InteropServices;

public class LibWrap 
{
	// STDAPI CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, 
	//		DWORD dwClsContext, REFIID riid, LPVOID * ppv);	
	
	[ DllImport( "Ole32.dll" )] 
    public static extern uint CoCreateInstance(
    	[ In ] ref Guid clsid,
		[ MarshalAs( UnmanagedType.IUnknown )] Object punkOuter,
		int context,
		[ In ] ref Guid iid,
		[ MarshalAs( UnmanagedType.IUnknown )] out Object o );
                                                 
	[ DllImport( "Ole32.dll", PreserveSig=false, EntryPoint="CoCreateInstance" )]
	[ return:MarshalAs( UnmanagedType.IUnknown )] 
    public static extern Object CoCreateInstance2(
    	[ In ] ref Guid clsid,
		[ MarshalAs( UnmanagedType.IUnknown )] Object punkOuter,
		int context,
		[ In ] ref Guid iid );
		
	public const int INPROC_SERVER = 1;
	public const int LOCAL_SERVER = 4;
}

public class App
{
	public static void Main()
	{
		uint hr = 0;
		Object syncMgr = null;
        Guid CLSID_BadID = new Guid( "6295DF27-35EE-11d1-8707-00C04FD93326" );
        Guid CLSID_SyncMgr = new Guid( "6295DF27-35EE-11d1-8707-00C04FD93327" );
		Guid IID_ISyncMgrReg = 
			new Guid( "6295DF42-35EE-11d1-8707-00C04FD93327" );

		// create the real SyncMgr object using CoCreateInstance 
		// that returns an HR

        Console.WriteLine( "\nCreating Sync Mgr with CoCreateInstance" );
        
        hr = LibWrap.CoCreateInstance( ref CLSID_SyncMgr, null, 
        	LibWrap.INPROC_SERVER | LibWrap.LOCAL_SERVER, 
        	ref IID_ISyncMgrReg, out syncMgr );
        	
        Console.WriteLine( "CoCreateInstance return HRESULT = {0:x}", hr );

		// create the real SyncMgr object using CoCreateInstance 
		// that returns an Object and throws exception

        Console.WriteLine( "\nCreating Sync Mgr with CoCreateInstance2" );
        
        try{
        	syncMgr = LibWrap.CoCreateInstance2( ref CLSID_SyncMgr, null,
        		LibWrap.INPROC_SERVER | LibWrap.LOCAL_SERVER, 
        		ref IID_ISyncMgrReg );

        	Console.WriteLine( syncMgr.ToString() );

	   }
	   catch
	    {
	    }
	
		// try to create an object that doesn't exist.  
		// HRESULT returned should be 0x80040154

        Console.WriteLine( "\nCreating Sync Mgr with CoCreateInstance" );
        
        hr = LibWrap.CoCreateInstance( ref CLSID_BadID, null, 
        	LibWrap.INPROC_SERVER | LibWrap.LOCAL_SERVER,  
        	ref IID_ISyncMgrReg, out syncMgr );
        	
        if( hr != 0 ) 
        {
	        if( hr == 0x80040154 )
	        {
	        	Console.WriteLine( "CoCreateInstance failed as expected" );
	        }
	        else
	        {
	        	Console.WriteLine( "CoCreateInstance failed unexpectedly with HRESULT = {0:x}", hr );
	        }
		}        
        
		// try to create an object that doesn't exist. COMException is thrown.
	
        Console.WriteLine( "\nCreating Sync Mgr with CoCreateInstance2" );
        try 
        {
        	syncMgr = LibWrap.CoCreateInstance2( ref CLSID_BadID, null, 
        		LibWrap.INPROC_SERVER | LibWrap.LOCAL_SERVER, 
        		ref IID_ISyncMgrReg );
        } 
        catch( COMException e )
        {
			if( (uint)e.ErrorCode == 0x80040154 )
			{
        		Console.WriteLine( "CoCreateInstance failed as expected" );
        	}
        	else
        	{
	        	Console.WriteLine( "CoCreateInstance failed unexpectedly with HRESULT = {0:x}", e.ErrorCode );
	        }
        }
    }
}

