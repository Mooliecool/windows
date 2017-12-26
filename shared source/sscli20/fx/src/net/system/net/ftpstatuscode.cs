// ------------------------------------------------------------------------------
// <copyright file="FtpStatusCode.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright> 
// ------------------------------------------------------------------------------
// 

namespace System.Net {

    public enum FtpStatusCode {
        Undefined = 0,

        //
        // Informational 1xx
        //
        RestartMarker = 110,
        ServiceTemporarilyNotAvailable = 120,
        DataAlreadyOpen = 125,
        OpeningData = 150,

        //
        // Success 2xx
        //

        CommandOK  = 200,
        CommandExtraneous = 202,
        DirectoryStatus = 212, 
        FileStatus = 213, 
        SystemType = 215,
        SendUserCommand = 220,
        ClosingControl = 221,
        ClosingData = 226,
        EnteringPassive = 227,
        LoggedInProceed = 230,
        ServerWantsSecureSession = 234,
        FileActionOK = 250,
        PathnameCreated = 257,

        //
        // Intermeidate 3xx
        //

        SendPasswordCommand  = 331,
        NeedLoginAccount = 332,
        FileCommandPending = 350,

        //
        // Temporary Errors 4xx
        //


        ServiceNotAvailable = 421,
        CantOpenData = 425,
        ConnectionClosed = 426,
        ActionNotTakenFileUnavailableOrBusy = 450,
        ActionAbortedLocalProcessingError = 451,
        ActionNotTakenInsufficientSpace = 452,
        
        
        //
        // Fatal Errors 5xx
        //

        CommandSyntaxError = 500,
        ArgumentSyntaxError = 501,
        CommandNotImplemented = 502,
        BadCommandSequence = 503,
        NotLoggedIn = 530,
        AccountNeeded = 532,
        ActionNotTakenFileUnavailable = 550,
        ActionAbortedUnknownPageType = 551,
        FileActionAborted = 552,
        ActionNotTakenFilenameNotAllowed = 553
    }
}
