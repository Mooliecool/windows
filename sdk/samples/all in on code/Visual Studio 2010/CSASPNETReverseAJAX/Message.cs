/****************************** Module Header ******************************\
* Module Name:    Message.cs
* Project:        CSASPNETReverseAJAX
* Copyright (c) Microsoft Corporation
*
* Message class contains all necessary fields in a message package.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

namespace CSASPNETReverseAJAX
{
    /// <summary>
    /// This is a entity class that represents a message item.
    /// </summary>
    public class Message
    {
        /// <summary>
        /// The name who will receive this message.
        /// </summary>
        public string RecipientName { get; set; }

        /// <summary>
        /// The message content.
        /// </summary>
        public string MessageContent { get; set; }
    }
}