// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
namespace System.Diagnostics {
    using System;
    using System.IO;
    using System.Collections;
    
	[Serializable()]
    internal class LogSwitch
    {
    	// ! WARNING ! 
    	// If any fields are added/deleted/modified, perform the 
    	// same in the EE code (debugdebugger.cpp)
    	internal String strName;
    	internal String strDescription;
    	private LogSwitch ParentSwitch;	
    	private LogSwitch[] ChildSwitch;
    	internal LoggingLevels iLevel;
    	internal LoggingLevels iOldLevel;
    	private int iNumChildren;
    	private int iChildArraySize;
    
    	// ! END WARNING !
    
    
    	private LogSwitch ()
    	{
    	}
    
    	// Constructs a LogSwitch.  A LogSwitch is used to categorize log messages.
    	// 
    	// All switches (except for the global LogSwitch) have a parent LogSwitch.
    	//
    	public LogSwitch(String name, String description, LogSwitch parent)
    	{
    		if ((name != null) && (parent != null) )
    		{
    			if (name.Length == 0)
    				throw new ArgumentOutOfRangeException ("Name", Environment.GetResourceString("Argument_StringZeroLength"));
    				
    			strName = name;
    			strDescription = description;
    			iLevel = LoggingLevels.ErrorLevel;
    			iOldLevel = iLevel;
    
    			// update the parent switch to reflect this child switch
    			parent.AddChildSwitch (this);
    
    			ParentSwitch = parent;
    
    			ChildSwitch  = null;
    			iNumChildren = 0;
    			iChildArraySize = 0;
    
    			Log.m_Hashtable.Add (strName, this);			
    
    			// Call into the EE to let it know about the creation of
    			// this switch
    			Log.AddLogSwitch (this);
    
    			// update switch count
    			Log.iNumOfSwitches++;
    		}
    		else
    			throw new ArgumentNullException ((name==null ? "name" : "parent"));
    	}
    
    	internal LogSwitch(String name, String description)
    	{
    		strName = name;
    		strDescription = description;
    		iLevel = LoggingLevels.ErrorLevel;
    		iOldLevel = iLevel;
    		ParentSwitch = null;
    		ChildSwitch  = null;
    		iNumChildren = 0;
    		iChildArraySize = 0;
    
    		Log.m_Hashtable.Add (strName, this);
    
    		// Call into the EE to let it know about the creation of
    		// this switch
    		Log.AddLogSwitch (this);
    
    		// update switch count
    		Log.iNumOfSwitches++;
    	}
    
    
    	// Get property returns the name of the switch
    	public virtual String Name
    	{
    		get { return strName;}
    	}
    
    	// Get property returns the description of the switch
    	public virtual String Description
    	{
    		get {return strDescription;}
    	}
    
    
    	// Get property returns the parent of the switch
    	public virtual LogSwitch Parent
    	{
    		get { return ParentSwitch; }
    	}
    
    
    	// Property to Get/Set the level of log messages which are "on" for the switch.  
    	// 
    	public  virtual LoggingLevels  MinimumLevel
    	{
    		get { return iLevel; }
    		set 
    		{ 
    			iLevel = value; 
    			iOldLevel = value;
    			String strParentName = ParentSwitch!=null ? ParentSwitch.Name : "";
    			if (Debugger.IsAttached)
    				Log.ModifyLogSwitch ((int)iLevel, strName, strParentName);
    	
    			Log.InvokeLogSwitchLevelHandlers (this, iLevel);
    		}
    	}
    
    
    	// Checks if the given level is "on" for this switch or one of its parents.
    	//
    	public virtual bool CheckLevel(LoggingLevels level)
    	{
    		if (iLevel > level)
    		{
    			// recurse through the list till parent is hit. 
    			if (this.ParentSwitch == null)
    				return false;
    			else
    				return this.ParentSwitch.CheckLevel (level);
    		}
    		else
    			return true;
    	}
    
    
    	// Returns a switch with the particular name, if any.  Returns null if no
    	// such switch exists.
    	public static LogSwitch GetSwitch(String name)
    	{
    		return (LogSwitch) Log.m_Hashtable[name];
    	}
    
    	private  void AddChildSwitch (LogSwitch child)
    	{
    		if (iChildArraySize <= iNumChildren)
    		{
    				int iIncreasedSize;
    
    				if (iChildArraySize == 0)
    					iIncreasedSize = 10;
    				else
    					iIncreasedSize = (iChildArraySize * 3) / 2;
    
    				// increase child array size in chunks of 4
    				LogSwitch[] newChildSwitchArray = new LogSwitch [iIncreasedSize];
    
    				// copy the old array objects into the new one.
    	            if (iNumChildren > 0) 
    					Array.Copy(ChildSwitch, newChildSwitchArray, iNumChildren);
    
    				iChildArraySize = iIncreasedSize;
    
    		        ChildSwitch = newChildSwitchArray;
    		}
    
    		ChildSwitch [iNumChildren++] = child;			
    	}
    
    
    }
}
