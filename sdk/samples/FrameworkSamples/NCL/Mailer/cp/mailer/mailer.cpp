//---------------------------------------------------------------------
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
//---------------------------------------------------------------------

#include "stdafx.h"

using namespace System;
using namespace System::Net;
using namespace System::Net::Mail;

int main(array<System::String ^> ^args)
{
	if (args->Length < 4)
    {	
        Console::WriteLine(
            "Expected: mailer.exe [from] [to] [subject] [message]");
        return 0;
    }

    enum MailMessagePart {
        From = 0,
        To = 1,
        Subject = 2,
        Message = 3
    };

    // Set mailServerName to be the name of the mail server
    // you wish to use to deliver this message
    String^ mailServerName = "smtphost";
    String^ from = gcnew String(args[From]);
    String^ to = gcnew String(args[To]);
    String^ subject = gcnew String(args[Subject]);
    String^ body = gcnew String(args[Message]);

    SmtpClient^ mailClient;
    try
    {
        // MailMessage is used to represent the e-mail being sent
        MailMessage^ message = gcnew MailMessage(from, to, subject, body);

        // SmtpClient is used to send the e-mail
        mailClient = gcnew SmtpClient(mailServerName);

        // UseDefaultCredentials tells the mail client to use the 
        // Windows credentials of the account (i.e. user account) 
        // being used to run the application
        mailClient->UseDefaultCredentials = true;

        // Send delivers the message to the mail server
        mailClient->Send(message);

        Console::WriteLine("Message sent.");
    }
    catch (FormatException^ ex)
    {
        Console::WriteLine(ex->Message);
    }
    catch (SmtpException^ ex)
    {
        Console::WriteLine(ex->Message);
    }
    finally 
    {
        if (&mailClient != 0) {
            mailClient->~SmtpClient();
        }
    }
    return 0;
}
