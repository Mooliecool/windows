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
using System.Text;
    using System;
    using System.Collections;
    using System.IO;
	using System.Runtime.Serialization;
	using System.Runtime.Serialization.Formatters.Binary;
    public class Co1011Serialize
    {
     public static readonly String s_strActiveBugNums = "";
     public static readonly String s_strDtTmVer       = "";
     public static readonly String s_strLastModCoder  = "";
     public static readonly String s_strOrigCoder     = "";
     public static readonly String s_strComponentBeingTested
                                                   = "PseudoMLFormatter.Serialize(Object,Int32) ,see IFormatter.";
     public static readonly String s_strTFName        = "Co1011Serialize.cs";
     public static readonly String s_strTFAbbrev      = "Co1011";
     public static readonly String s_strTFPath        = "";
     public static Int32 DBGLEV = 2;  
     internal String o_strLoc="Loc_1000wsx";
     internal String o_strBaseLoc;
     internal Int32 iCountErrors = 0;
     internal Int32 iCountTestcases = 0;
     internal Int32 o_nBaseLocCount = 0;
     public virtual bool runTest()
     {
    	Console.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strComponentBeingTested +"  ,Source ver "+ s_strDtTmVer );
      try
      {
      do
      {
    	o_strLoc="Loc_1010wsx";
    	M10_TwoRefsToOneObj();
    	M20_SelfRef();
      } while ( false );
      }
      catch( Exception exc_general )
      {
    	++iCountErrors;
    	Console.WriteLine( "Error Err_8888yyy ("+ s_strTFAbbrev +")!  Unexpected exception thrown sometime after o_strLoc=="+ o_strLoc +" ,exc_general=="+ exc_general );
      }
    	if ( iCountErrors == 0 )
    	{
    		Console.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases=="+ iCountTestcases );
    		return true;
    	}
    	else
    	{
    		Console.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors=="+ iCountErrors +" ,BugNums?: "+ s_strActiveBugNums );
    		return false;
    	}
     }
     public virtual bool M10_TwoRefsToOneObj()
     {
    	Console.WriteLine( "** "+ s_strTFAbbrev +" method: TwoRefsToOneObj()" );
    	Int32 nCntErrorsOrig = iCountErrors;
    	BinaryFormatter pmlformt2;
    	String strPreSerial;
    	String strPostDeserial;
    	ArrayList arlistPreSerial = new ArrayList();
    	ArrayList arlistPostDeserial = new ArrayList();
    	IEnumerator enumer2;
    	IEnumerator enumer3;
    	MemoryStream memstm3 = new MemoryStream( 16 );
    	Int32 nEqualPreSer = -2;
    	Int32 nEqualPostDeser = -3;
    	Int32[] nArrCountsPreSer = new Int32[98];
    	Int32[] nArrCountsPostDeser = new Int32[98];
    	Int32 nCurrSlot2 = -2;
    	Int32 nW = -3;
    	Object objDeserialized;
    	Co1011_root cbrootPreSer;
    	Co1011_root cbrootb1;
    	Co1011_root cbrootc1;
    	Co1011_root cbrootPostDeser;
      try
      {
      do
      {
    	o_strLoc="Loc_1010bik";
    	cbrootPreSer = new Co1011_root();
    	cbrootb1 = new Co1011_root();
    	cbrootc1 = new Co1011_root();
    	cbrootPreSer.m_cbrootb1 = cbrootb1;
    	cbrootPreSer.m_cbrootc1 = cbrootc1;
    	cbrootb1.m_cbrootc1 = cbrootc1;
    	o_strLoc="Loc_1020bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	cbrootPreSer.IncrementPreSerializationCountersRecursively();
    	o_strLoc="Loc_1030bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	arlistPreSerial.Add( (Object)cbrootPreSer );
    	arlistPreSerial.Add( (Object)cbrootPreSer.m_cbrootb1 );
    	arlistPreSerial.Add( (Object)cbrootPreSer.m_cbrootc1 );
    	arlistPreSerial.Add( (Object)cbrootPreSer.m_cbrootb1.m_cbrootc1 );
    	o_strLoc="Loc_1040bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	strPreSerial = cbrootPreSer.GM1SerizalizeToStringRecursively( true );
    	o_strLoc="Loc_1050bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	pmlformt2 = new BinaryFormatter();
    	o_strLoc="Loc_1060bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	pmlformt2.Serialize  
    			(
    			 memstm3
    			,cbrootPreSer
    			,(
    				  null
    			 )
    			);
    	o_strLoc="Loc_1070bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	memstm3.Position = 0;  
    	objDeserialized = pmlformt2.Deserialize(memstm3);  
    	cbrootPostDeser = ( Co1011_root )objDeserialized;
    	o_strLoc="Loc_1080bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	o_strLoc="Loc_1090bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	cbrootPostDeser.IncrementPostDeserializationCountersRecursively();
    	o_strLoc="Loc_1100bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	arlistPostDeserial.Add( (Object)cbrootPostDeser );
    	arlistPostDeserial.Add( (Object)cbrootPostDeser.m_cbrootb1 );
    	arlistPostDeserial.Add( (Object)cbrootPostDeser.m_cbrootc1 );
    	arlistPostDeserial.Add( (Object)cbrootPostDeser.m_cbrootb1.m_cbrootc1 );
    	o_strLoc="Loc_1110bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	strPostDeserial = cbrootPostDeser.GM1SerizalizeToStringRecursively( true );
    	o_strLoc="Loc_1120bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	o_strBaseLoc = o_strLoc + "_"; o_nBaseLocCount=0;
    	nEqualPreSer = 0;
    	enumer2 = arlistPreSerial.GetEnumerator();
    	while ( enumer2.MoveNext() == true )
    	{
    		o_strLoc=o_strBaseLoc+((o_nBaseLocCount++).ToString());
    		enumer3 = arlistPreSerial.GetEnumerator();
    		while ( enumer3.MoveNext() == true )
    		{
    			o_strLoc=o_strBaseLoc+((o_nBaseLocCount++).ToString());
    			if ( enumer2.Current == enumer3.Current )  
    			{
    				++nEqualPreSer;
    			}
    		}
    	}
    	o_strLoc="Loc_1130bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	o_strBaseLoc = o_strLoc + "_"; o_nBaseLocCount=0;
    	nEqualPostDeser = 0;
    	enumer2 = arlistPostDeserial.GetEnumerator();
    	while ( enumer2.MoveNext() == true )
    	{
    		o_strLoc=o_strBaseLoc+((o_nBaseLocCount++).ToString());
    		enumer3 = arlistPostDeserial.GetEnumerator();
    		while ( enumer3.MoveNext() == true )
    		{
    			o_strLoc=o_strBaseLoc+((o_nBaseLocCount++).ToString());
    			if ( enumer2.Current == enumer3.Current )  
    			{
    				++nEqualPostDeser;
    			}
    		}
    	}
    	o_strLoc="Loc_1140bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	++iCountTestcases;
    	if ( nEqualPreSer != nEqualPostDeser )  
    	{
    		++iCountErrors;
    		Console.WriteLine( "Error_1012zeg ("+ s_strTFAbbrev +")!  nEqualPreSer="+ nEqualPreSer +" ,nEqualPostDeser="+ nEqualPostDeser );
    	}
    	o_strLoc="Loc_1150bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	++iCountTestcases;
    	if ( strPreSerial.Equals( strPostDeserial ) != true )  
    	{
    		++iCountErrors;
    		Console.WriteLine();
    		Console.WriteLine( "Error_1033wsx ("+ s_strTFAbbrev +")!" );
    		Console.WriteLine( "   strPreSerial   ="+ strPreSerial );
    		Console.WriteLine( "   strPostDeserial="+ strPostDeserial );
    		Console.WriteLine();
    	}
    	o_strLoc="Loc_1160bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	nArrCountsPreSer = new Int32[98];
    	nCurrSlot2 = 0;
    	cbrootPostDeser.PopulateArrayWithPreSerCountersRecursively( nArrCountsPreSer ,nCurrSlot2 );
    	o_strLoc="Loc_1161bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	nArrCountsPostDeser = new Int32[98];
    	nW = nCurrSlot2;
    	nCurrSlot2 = 0;
    	cbrootPostDeser.PopulateArrayWithPostDeserCountersRecursively( nArrCountsPostDeser ,nCurrSlot2 );
    	++iCountTestcases;
    	if ( nW != nCurrSlot2 )
    	{
    		Console.WriteLine( "Error_1068bik ("+ s_strTFAbbrev +")!  Slots_PreSer="+ nW +" ,Slots_PostDeser="+ nCurrSlot2 );
    	}
    	o_strLoc="Loc_1170bik"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	o_strBaseLoc = o_strLoc + "_"; o_nBaseLocCount=0;
    	for (
    	       int qd=0
    	      ;    qd<nArrCountsPreSer.Length
    	       &&  qd<nArrCountsPostDeser.Length
    	       &&  qd<nCurrSlot2
    	       &&  nW == nCurrSlot2
    	      ;qd++
    	    )
    	{
    		o_strLoc=o_strBaseLoc+((qd).ToString());
    		++iCountTestcases;
    		if ( nArrCountsPreSer[qd] != nArrCountsPostDeser[qd] )
    		{
    			Console.WriteLine( "Error_1571bik ("+ s_strTFAbbrev +")!  qd="+ qd +" ,nArrCountsPreSer[qd]="+ nArrCountsPreSer[qd] +" ,nArrCountsPostDeser[qd]="+ nArrCountsPostDeser[qd] );
    		}
    	}
      } while ( false );
      }
      catch( Exception exc_general )
      {
    	++iCountErrors;
    	Console.WriteLine( "Error Err_8888bik ("+ s_strTFAbbrev +")!  Unexpected exception thrown sometime after o_strLoc=="+ o_strLoc +" ,exc_general=="+ exc_general );
      }
    	if ( ( iCountErrors - nCntErrorsOrig ) == 0 )
    	{
    		return true;
    	}
    	else
    	{
    		return false;
    	}
     }
     public virtual bool M20_SelfRef()
     {
    	Console.WriteLine( "** "+ s_strTFAbbrev +" method: M20_SelfRef()" );
    	Int32 nCntErrorsOrig = iCountErrors;
    	BinaryFormatter pmlformt2;
    	ArrayList arlistPreSerial = new ArrayList();
    	ArrayList arlistPostDeserial = new ArrayList();
    	IEnumerator enumer2;
    	IEnumerator enumer3;
    	MemoryStream memstm3 = new MemoryStream( 16 );
    	Int32 nEqualPreSer = -2;
    	Int32 nEqualPostDeser = -3;
    	Int32[] nArrCountsPreSer = new Int32[98];
    	Int32[] nArrCountsPostDeser = new Int32[98];
    	Object objDeserialized;
    	Co1011_root cbrootPreSer;
    	Co1011_root cbrootPostDeser;
      try
      {
      do
      {
    	o_strLoc="Loc_1010edc";
    	cbrootPreSer = new Co1011_root();
    	cbrootPreSer.m_cbroot_1 = cbrootPreSer;  
    	o_strLoc="Loc_1020edc"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	o_strLoc="Loc_1030edc"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	arlistPreSerial.Add( (Object)cbrootPreSer );
    	arlistPreSerial.Add( (Object)cbrootPreSer.m_cbroot_1 );
    	o_strLoc="Loc_1040edc"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	o_strLoc="Loc_1050edc"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	pmlformt2 = new BinaryFormatter();
    	o_strLoc="Loc_1060edc"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	pmlformt2.Serialize  
    			( memstm3 
    			 ,cbrootPreSer
    			,(
    				  null
    			 )
    			);
    	o_strLoc="Loc_1070edc"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	memstm3.Position = 0;  
    	objDeserialized = pmlformt2.Deserialize(memstm3);  
    	cbrootPostDeser = ( Co1011_root )objDeserialized;
    	o_strLoc="Loc_1080edc"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	o_strLoc="Loc_1090edc"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	o_strLoc="Loc_1100edc"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	arlistPostDeserial.Add( (Object)cbrootPostDeser );
    	arlistPostDeserial.Add( (Object)cbrootPostDeser.m_cbroot_1 );
    	o_strLoc="Loc_1110edc"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	o_strLoc="Loc_1120edc"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	o_strBaseLoc = o_strLoc + "_"; o_nBaseLocCount=0;
    	nEqualPreSer = 0;
    	enumer2 = arlistPreSerial.GetEnumerator();
    	while ( enumer2.MoveNext() == true )
    	{
    		o_strLoc=o_strBaseLoc+((o_nBaseLocCount++).ToString());
    		enumer3 = arlistPreSerial.GetEnumerator();
    		while ( enumer3.MoveNext() == true )
    		{
    			o_strLoc=o_strBaseLoc+((o_nBaseLocCount++).ToString());
    			if ( enumer2.Current == enumer3.Current )  
    			{
    				++nEqualPreSer;
    			}
    		}
    	}
    	o_strLoc="Loc_1130edc"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	o_strBaseLoc = o_strLoc + "_"; o_nBaseLocCount=0;
    	nEqualPostDeser = 0;
    	enumer2 = arlistPostDeserial.GetEnumerator();
    	while ( enumer2.MoveNext() == true )
    	{
    		o_strLoc=o_strBaseLoc+((o_nBaseLocCount++).ToString());
    		enumer3 = arlistPostDeserial.GetEnumerator();
    		while ( enumer3.MoveNext() == true )
    		{
    			o_strLoc=o_strBaseLoc+((o_nBaseLocCount++).ToString());
    			if ( enumer2.Current == enumer3.Current )  
    			{
    				++nEqualPostDeser;
    			}
    		}
    	}
    	o_strLoc="Loc_1140edc"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
    	++iCountTestcases;
    	if ( nEqualPreSer != nEqualPostDeser )
    	{
    		++iCountErrors;
    		Console.WriteLine( "Error_1762beg ("+ s_strTFAbbrev +")!  nEqualPreSer="+ nEqualPreSer +" ,nEqualPostDeser="+ nEqualPostDeser );
    	}
    	o_strLoc="Loc_1150edc"; if(DBGLEV>=2){Console.WriteLine(o_strLoc);}
      } while ( false );
      }
      catch( Exception exc_general )
      {
    	++iCountErrors;
    	Console.WriteLine( "Error Err_8888edc ("+ s_strTFAbbrev +")!  Unexpected exception thrown sometime after o_strLoc=="+ o_strLoc +" ,exc_general=="+ exc_general );
      }
    	if ( ( iCountErrors - nCntErrorsOrig ) == 0 )
    	{
    		return true;
    	}
    	else
    	{
    		return false;
    	}
     }
     public static void Main( String[] args )
     {
    	Environment.ExitCode = 1;  
    	bool bResult = false; 
    	StringBuilder sblMsg = new StringBuilder( 99 );
    	Co1011Serialize cbX = new Co1011Serialize();
    	try
    	{
    		bResult = cbX.runTest();
    	}
    	catch ( Exception exc_main )
    	{
    		bResult = false;
    		Console.WriteLine( "FAiL!  Error Err_9999zzz ("+ s_strTFAbbrev +")!  Uncaught Exception caught fell to Main(), exc_main=="+ exc_main );
    	}
    	if ( ! bResult )
    	{
    		Console.WriteLine( s_strTFPath + s_strTFName );
    		Console.WriteLine( " " );
    		Console.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
    		Console.WriteLine( " " );
    	}
    	if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
     }
    } 
    [Serializable]
	internal class Co1011_extenderOf_root
    		: Co1011_root
    {
     public String m_strExtendedString2 = "m_strExtendedString2 original value";
    }
    [Serializable]
	internal class Co1011_root
    		:
    			 IGM1Serializable
    {
     private Int32 m_nGM1CounterPreSerialization = 0;
     private Int32 m_nGM1CounterPostDeserialization = 0;
     private Guid m_guidThis = new Guid();
     public Int32 m_nA = 1;
     public Int32 m_nB = 2;
     public String m_strA = "Hi Mom";
     public String m_strB = "Low Dad";
     public Co1011_root m_cbroot_1 = null;
     public Co1011_root m_cbroot_2 = null;
     public Co1011_root m_cbrootb1 = null;
     public Co1011_root m_cbrootb2 = null;
     public Co1011_root m_cbrootc1 = null;
     public Co1011_root m_cbrootc2 = null;
     public Co1011_root()  
     {
    	Console.WriteLine( "Co1011_root constructor just started." );
    	m_nB = 102;
    	m_strB = "Down Pop";
     }
     public virtual Int32 GetGM1CounterPreSerialization()
     { return m_nGM1CounterPreSerialization; }
     public virtual void SetGM1CounterPreSerialization( Int32 countPreSer )
     { m_nGM1CounterPreSerialization = countPreSer; }
     public virtual Int32 GetGM1CounterPostDeserialization()
     { return m_nGM1CounterPostDeserialization; }
     public virtual void SetGM1CounterPostDeserialization( Int32 countPostDeser )
     { m_nGM1CounterPostDeserialization = countPostDeser; }
     public virtual String GM1SerizalizeToStringRecursively( Boolean bolNeedComparability )
     {
    	StringBuilder sbl2 = new StringBuilder( 100 );
    	sbl2.Length = 0;
    	sbl2.Append( ';' );
    	sbl2.Append( "m_nGM1CounterPreSerialization=" );
    	sbl2.Append( m_nGM1CounterPreSerialization );
    	sbl2.Append( ';' );
    	sbl2.Append( "m_nA=" );
    	sbl2.Append( m_nA );
    	sbl2.Append( ';' );
    	sbl2.Append( "m_strA=" );
    	sbl2.Append( m_strA );
    	if ( bolNeedComparability == false )
    	{
    		sbl2.Append( ';' );
    		sbl2.Append( "m_guidThis=" );
    		sbl2.Append( m_guidThis );
    		sbl2.Append( ';' );
    		sbl2.Append( "m_nGM1CounterPostDeserialization=" );
    		sbl2.Append( m_nGM1CounterPostDeserialization );
    		sbl2.Append( ';' );
    		sbl2.Append( "m_nB=" );
    		sbl2.Append( m_nB );
    		sbl2.Append( ';' );
    		sbl2.Append( "m_strB=" );
    		sbl2.Append( m_strB );
    	}
    	sbl2.Append( ';' );
    	if ( m_cbroot_1 != null )
    		sbl2.Append( m_cbroot_1.GM1SerizalizeToStringRecursively( true ) );
    	if ( m_cbroot_2 != null )
    		sbl2.Append( m_cbroot_2.GM1SerizalizeToStringRecursively( true ) );
    	if ( m_cbrootb1 != null )
    		sbl2.Append( m_cbrootb1.GM1SerizalizeToStringRecursively( true ) );
    	if ( m_cbrootb2 != null )
    		sbl2.Append( m_cbrootb2.GM1SerizalizeToStringRecursively( true ) );
    	if ( m_cbrootc1 != null )
    		sbl2.Append( m_cbrootc1.GM1SerizalizeToStringRecursively( true ) );
    	if ( m_cbrootc2 != null )
    		sbl2.Append( m_cbrootc2.GM1SerizalizeToStringRecursively( true ) );
    	return sbl2.ToString();
     }
     public virtual void IncrementPreSerializationCountersRecursively()
     {
    	++m_nGM1CounterPreSerialization;
    	if ( m_cbroot_1 != null )
    		m_cbroot_1.IncrementPreSerializationCountersRecursively();
    	if ( m_cbroot_2 != null )
    		m_cbroot_2.IncrementPreSerializationCountersRecursively();
    	if ( m_cbrootb1 != null )
    		m_cbrootb1.IncrementPreSerializationCountersRecursively();
    	if ( m_cbrootb2 != null )
    		m_cbrootb2.IncrementPreSerializationCountersRecursively();
    	if ( m_cbrootc1 != null )
    		m_cbrootc1.IncrementPreSerializationCountersRecursively();
    	if ( m_cbrootc2 != null )
    		m_cbrootc2.IncrementPreSerializationCountersRecursively();
     }
     public virtual void IncrementPostDeserializationCountersRecursively()
     {
    	++m_nGM1CounterPostDeserialization;
    	if ( m_cbroot_1 != null )
    		m_cbroot_1.IncrementPostDeserializationCountersRecursively();
    	if ( m_cbroot_2 != null )
    		m_cbroot_2.IncrementPostDeserializationCountersRecursively();
    	if ( m_cbrootb1 != null )
    		m_cbrootb1.IncrementPostDeserializationCountersRecursively();
    	if ( m_cbrootb2 != null )
    		m_cbrootb2.IncrementPostDeserializationCountersRecursively();
    	if ( m_cbrootc1 != null )
    		m_cbrootc1.IncrementPostDeserializationCountersRecursively();
    	if ( m_cbrootc2 != null )
    		m_cbrootc2.IncrementPostDeserializationCountersRecursively();
     }
     public virtual void PopulateArrayWithPreSerCountersRecursively( Int32[] arrCountPerSlot ,Int32 currSlot )
     {
    	arrCountPerSlot[currSlot++] = GetGM1CounterPreSerialization();
    	if ( m_cbroot_1 != null )
    		m_cbroot_1.PopulateArrayWithPreSerCountersRecursively( arrCountPerSlot ,currSlot );
    	if ( m_cbroot_2 != null )
    		m_cbroot_2.PopulateArrayWithPreSerCountersRecursively( arrCountPerSlot ,currSlot );
    	if ( m_cbrootb1 != null )
    		m_cbrootb1.PopulateArrayWithPreSerCountersRecursively( arrCountPerSlot ,currSlot );
    	if ( m_cbrootb2 != null )
    		m_cbrootb2.PopulateArrayWithPreSerCountersRecursively( arrCountPerSlot ,currSlot );
    	if ( m_cbrootc1 != null )
    		m_cbrootc1.PopulateArrayWithPreSerCountersRecursively( arrCountPerSlot ,currSlot );
    	if ( m_cbrootc2 != null )
    		m_cbrootc2.PopulateArrayWithPreSerCountersRecursively( arrCountPerSlot ,currSlot );
     }
     public virtual void PopulateArrayWithPostDeserCountersRecursively( Int32[] arrCountPerSlot ,Int32 currSlot )
     {
    	arrCountPerSlot[currSlot++] = GetGM1CounterPostDeserialization();
    	if ( m_cbroot_1 != null )
    		m_cbroot_1.PopulateArrayWithPostDeserCountersRecursively( arrCountPerSlot ,currSlot );
    	if ( m_cbroot_2 != null )
    		m_cbroot_2.PopulateArrayWithPostDeserCountersRecursively( arrCountPerSlot ,currSlot );
    	if ( m_cbrootb1 != null )
    		m_cbrootb1.PopulateArrayWithPostDeserCountersRecursively( arrCountPerSlot ,currSlot );
    	if ( m_cbrootb2 != null )
    		m_cbrootb2.PopulateArrayWithPostDeserCountersRecursively( arrCountPerSlot ,currSlot );
    	if ( m_cbrootc1 != null )
    		m_cbrootc1.PopulateArrayWithPostDeserCountersRecursively( arrCountPerSlot ,currSlot );
    	if ( m_cbrootc2 != null )
    		m_cbrootc2.PopulateArrayWithPostDeserCountersRecursively( arrCountPerSlot ,currSlot );
     }
    }
    internal interface IGM1Serializable
    {
     Int32 GetGM1CounterPreSerialization();
     void SetGM1CounterPreSerialization( Int32 countPreSer );
     Int32 GetGM1CounterPostDeserialization();
     void SetGM1CounterPostDeserialization( Int32 countPostDeser );
     String GM1SerizalizeToStringRecursively( Boolean bolNeedComparability );
     void IncrementPreSerializationCountersRecursively();
     void IncrementPostDeserializationCountersRecursively();
     void PopulateArrayWithPreSerCountersRecursively( Int32[] arrCountPerSlot ,Int32 currSlot );
     void PopulateArrayWithPostDeserCountersRecursively( Int32[] arrCountPerSlot ,Int32 currSlot );
    }
    internal class ExceptionCo1011Same
    		: Exception
    {
    }
