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
using System;
using System.Threading;
public class ReadWriteLock
{
 public int						OutX;
 public int						ReadX;
 public int						WriteX;
 public static ReaderWriterLock	rwLock	= new ReaderWriterLock( );
 static public int failed = 0;
 ReadWriteLock()
   {
   }
 void ReaderFunc( )
   {	
   int			ReadL			= Interlocked.Increment(ref ReadX);
   LockCookie	upgradeCookie	= new LockCookie( );
   try
     {
     if((rwLock.IsReaderLockHeld)||(rwLock.IsWriterLockHeld))
       lock(this)
	 Console.WriteLine(" {0}, Reader -BAD- No Locks should be held now.",ReadL);
     }
   catch(Exception e)
     {
     lock(this)
       Console.WriteLine(" {0}, Reader -BAD- (IsReaderLockHeld || IsWriterLockHeld) {1}",ReadL,e.ToString());
     failed = 1;
     }
   try
     {
     lock(this)
       Console.WriteLine(" {0}, Reader -fyi- Before AcquireReaderLock.",ReadL);
     rwLock.AcquireReaderLock(10000);
     lock(this)
       Console.WriteLine(" {0}, Reader -fyi- After  AcquireReaderLock.",ReadL);
     }
   catch(Exception e)
     {
     lock(this)
       Console.WriteLine(" {0}, Reader -BAD- (AcquireReaderLock) {1}",ReadL,e.ToString());
     failed = 1;
     }
   Thread.Sleep(10);
   try
     {
     if(!(rwLock.IsReaderLockHeld))
       lock(this)
	 Console.WriteLine(" {0}, Reader -BAD- ReaderLock  FALSE",ReadL);
     lock(this)
       Console.WriteLine(" {0}, Reader -fyi- Before UpgradeToWriterLock.",ReadL);
     upgradeCookie = rwLock.UpgradeToWriterLock(10000);
     lock(this)
       Console.WriteLine(" {0}, Reader -fyi- After  UpgradeToWriterLock.",ReadL);
     }
   catch(Exception e)
     {
     lock(this)
       Console.WriteLine(" {0}, Reader -BAD- (UpgradeToWriterLock) {1}",ReadL,e.ToString());
     failed = 1;
     }
   try
     {
     if(rwLock.IsReaderLockHeld)
       {
       lock(this)
	 Console.WriteLine(" {0}, Reader -BAD- ReaderLock  TRUE",ReadL);
       }
     if(!(rwLock.IsWriterLockHeld))
       {
       lock(this)
	 Console.WriteLine(" {0}, Reader -BAD- WriterLock  FALSE",ReadL);
       }
     lock(this)
       Console.WriteLine(" {0}, Reader -fyi- Before DowngradeFromWriterLock.",ReadL);
     rwLock.DowngradeFromWriterLock(ref upgradeCookie);
     lock(this)
       Console.WriteLine(" {0}, Reader -fyi- After  DowngradeFromWriterLock.",ReadL);
     }
   catch(Exception e)
     {
     lock(this)
       Console.WriteLine(" {0}, Reader -BAD- (DowngradeFromWriterLock) {1}",ReadL,e.ToString());
     failed = 1;
     }
   try
     {
     if(rwLock.IsWriterLockHeld)
       lock(this)
	 Console.WriteLine(" {0}, Reader -BAD- WriterLock  TRUE",ReadL);
     if(!(rwLock.IsReaderLockHeld))
       lock(this)
	 Console.WriteLine(" {0}, Reader -BAD- ReaderLock should be held now.",ReadL);
     lock(this)
       Console.WriteLine(" {0}, Reader -fyi- Before ReleaseReaderLock.",ReadL);
     rwLock.ReleaseReaderLock( );
     lock(this)
       Console.WriteLine(" {0}, Reader -fyi- After  ReleaseReaderLock.",ReadL);
     }
   catch(Exception e)
     {
     lock(this)
       Console.WriteLine(" {0}, Reader -BAD- (ReleaseReaderLock) {1}",ReadL,e.ToString());
     failed = 1;
     }
   try
     {
     if((rwLock.IsReaderLockHeld)||(rwLock.IsWriterLockHeld))
       lock(this)
	 Console.WriteLine(" {0}, Reader -BAD- No Locks should be held now.",ReadL);
     }
   catch(Exception e)
     {
     lock(this)
       Console.WriteLine(" {0}, Reader - {1}",ReadL,e.ToString());
     failed = 1;
     }
   }
 void WriterFunc( )
   {	
   int			WriteL			= Interlocked.Increment(ref WriteX);
   LockCookie	releaseCookie	= new LockCookie( );
   int			iWriterSeqNum;
   if((rwLock.IsReaderLockHeld)||(rwLock.IsWriterLockHeld))
     lock(this)
       Console.WriteLine("{0}, Writer -BAD- No Locks should be held now. (I)",WriteL);
   try
     {
     lock(this)
       Console.WriteLine("{0}, Writer -fyi- Before AcquireWriterLock.",WriteL);
     rwLock.AcquireWriterLock(10000);
     lock(this)
       Console.WriteLine("{0}, Writer -fyi- After  AcquireWriterLock.",WriteL);
     }
   catch(Exception e)
     {
     lock(this)
       Console.WriteLine("{0}, Writer -BAD- (AcquireWriterLock) {1}",WriteL,e.ToString());
     failed = 1;
     }
   Thread.Sleep(10);
   try
     {
     if(!(rwLock.IsWriterLockHeld))
       lock(this)
	 Console.WriteLine("{0}, Writer -BAD- WriterLock should be held now. (I)",WriteL);
     iWriterSeqNum = rwLock.WriterSeqNum;
     lock(this)
       Console.WriteLine("{0}, Writer -fyi- Before ReleaseLock.",WriteL);
     releaseCookie = rwLock.ReleaseLock( );
     lock(this)
       Console.WriteLine("{0}, Writer -fyi- After  ReleaseLock.",WriteL);
     if((rwLock.IsReaderLockHeld)||(rwLock.IsWriterLockHeld))
       lock(this)
	 Console.WriteLine("{0}, Writer -BAD- No Locks should be held now. (II)",WriteL);
     lock(this)
       Console.WriteLine("{0}, Writer -fyi- Before RestoreLock.",WriteL);
     rwLock.RestoreLock(ref releaseCookie);
     lock(this)
       Console.WriteLine("{0}, Writer -fyi- After  RestoreLock.",WriteL);
     if(!(rwLock.IsWriterLockHeld))
       lock(this)
	 Console.WriteLine("{0}, Writer -BAD- WriterLock should be held now. (II)",WriteL);
     lock(this)
       Console.WriteLine("{0}, Writer -fyi- Before ReleaseWriterLock.",WriteL);
     rwLock.ReleaseWriterLock( );
     lock(this)
       Console.WriteLine("{0}, Writer -fyi- After  ReleaseWriterLock.",WriteL);
     bool	boX;
     boX = rwLock.AnyWritersSince(iWriterSeqNum);
     iWriterSeqNum = rwLock.WriterSeqNum;
     boX	= rwLock.AnyWritersSince(iWriterSeqNum);
     }
   catch(Exception e)
     {
     lock(this)
       Console.WriteLine("{0}, Writer - {1}",WriteL,e.ToString());
     failed = 1;
     }
   }
 static public int Main(String[] args)
   {
   const int READER_COUNT = 4;
   const int WRITER_COUNT = 4;
   Thread[]	thrdReaders = new Thread[READER_COUNT];
   Thread[]	thrdWriters = new Thread[WRITER_COUNT];
   ReadWriteLock			XYZ		= new ReadWriteLock( );
   XYZ.OutX = 0;
   XYZ.ReadX = 0;
   XYZ.WriteX = 10;
   ThreadStart				reader	= new ThreadStart(XYZ.ReaderFunc);
   ThreadStart				writer	= new ThreadStart(XYZ.WriterFunc);
   for (int i=0;i<READER_COUNT;i++)
     {
     thrdReaders[i] = new Thread(reader);
     thrdReaders[i].Start( );
     }
   for (int i=0;i<WRITER_COUNT;i++)
     {
     thrdWriters[i] = new Thread(writer);
     thrdWriters[i].Start( );
     }
   for (int i=0;i<READER_COUNT;i++)
     {
     thrdReaders[i].Join();
     }
   for (int i=0;i<WRITER_COUNT;i++)
     {
     thrdWriters[i].Join();
     }
   return failed;
   }
}
