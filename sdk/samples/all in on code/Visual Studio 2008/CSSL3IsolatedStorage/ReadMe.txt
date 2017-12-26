========================================================================
    SILVERLIGHT APPLICATION : CSSL3IsolatedStorage Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This project create an Isolated Storage Explorer, with the use of explorer,
user could view application IsolatedStorage's virtual file structure, and 
it also provides these file management functions:

    Create dictionary
    Upload local file to isolated storage
    Open media stream stored in isolated storage and play
    Delete dictionary/file
    Increase isolated storeage Quota
    Save isolated storage file to local
    Use IsolatedStorageSettings to store/load config
    
    
/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Tools for Visual Studio 2008 SP1
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en

Silverilght 3 runtime:
http://silverlight.net/getstarted/silverlight3


/////////////////////////////////////////////////////////////////////////////
Demo:

To run the isolatedstorage sample, please try the following steps:
   1. Open CSSL3IsolatedStorage, compile and run it.
   2. In the thrid paragraph on the application's UI, it shows when you opened this application
last time. This information is stored in IsolatedStorageSettings.
   3. Push the "Increase Quota By 10 MB" button, the application's storage quota will be increased.
   4. in treeview control, you may try to add,delete and open file. It will use API
to manipulate the files in application's IsolatedStorage.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1: How does isolated storage treeview viewmodel works?
    1. Define Entities for isolated storage virtual file and directory.
    2. In treeview, use HierarchicalDataTemplate to bind data to treeview node.
    3. when application startup, traverse isolated storage, use defined entity
    to create treeview viewmodel, then set as treeview's itemssource.

2: How to upload file to isolated storage?
    1. Use OpenFileDialog to get a local readable file stream.
    2. Get application's IsolatedStorageFile, then use CreateFile method to get one
       writeable IsolatedStorageStream.
    3. Copy bytes from file stream to isolatedstorage stream.
    4. Close streams.
    
    Note that: copy stream is very time consuming, use BackgroundWorker to run it in
       other thread could get better performance. Details about BackgroundWorker, see
       http://msdn.microsoft.com/en-us/library/cc221403(VS.95).aspx

3. How to open media stream stored in isolated storage and play?
    1. Open isolated storage stream.
       IsolatedStorageFile _isofile = IsolatedStorageFile.GetUserStoreForApplication();
       var stream = _isofile.OpenFile(item.FilePath, FileMode.Open, FileAccess.Read);
       
    2. set to MediaElement, play.
       mePlayer.SetSource(stream);

4. How to use IsolatedStorageSetting to store config?
    IsolatedStorageSettings is a dictionary stored in isolated storage and maintained
    by silverlight. It's a good place to store configuration.
    
    To store data to IsolatedStorageSetting for application usage, use this
        solatedStorageSettings.ApplicationSettings["keyname"] = data;

    And if need store data for whole site usage, use
        solatedStorageSettings.SiteSettings["keyname"] = data;
     
5. In application, why directory depth should be less than 4 ?
	Isolated storage limit the directory name should be less than 248 characters,
	and the full file path should be less than 260 characters. If create deeper
	directory, the path length would exceed the limitation.


/////////////////////////////////////////////////////////////////////////////
References:

Isolated Storage
http://msdn.microsoft.com/en-us/library/bdts8hk0(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////
