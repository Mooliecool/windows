/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:		CSWFLocalService
* Copyright (c) Microsoft Corporation.
* 
* Local service:GuessNumberGameService
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 11/2/2009 8:50 PM Andrew Zhu Created
\***************************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CSWFLocalService
{
	class GuessNumberGameService:IGuessNumberGameService
	{
        public event EventHandler<MessageEventArgs> MessageFromServiceToHostEvent;

        public void MessageFromWFToLocalService(string message)
        {
            MessageEventArgs mea = new MessageEventArgs(new Guid(), message);
            MessageFromServiceToHostEvent(null, mea);
        }
        public event EventHandler<MessageEventArgs> MessageFromHostToWFEvent;

        public void OnMessageFromHostToWFEvent(Guid wfInstanceId, string message)
        {
            MessageEventArgs meg = new MessageEventArgs(wfInstanceId, message);
            MessageFromHostToWFEvent(null, meg);
        }
    }
}
