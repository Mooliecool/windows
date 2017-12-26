using System;
using System.Web;
using System.Web.Mail;
using AspNetForums.Components;
using System.ComponentModel;

namespace AspNetForums {


    // *********************************************************************
    //  ForumMessages
    //
    /// <summary>
    /// Class used to manage messages displayed/used in the forums.
    /// </summary>
    /// 
    // ********************************************************************/
    public class ForumMessages {

        // *********************************************************************
        //  GetMessage
        //
        /// <summary>
        /// Gets a message.
        /// </summary>
        /// <param name="messageId">Id of message being requested</param>
        /// 
        // ********************************************************************/
        public static ForumMessage GetMessage(int messageId) {
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetMessage(messageId);
        }

        // *********************************************************************
        //  GetMessageTemplateList
        //
        /// <summary>
        /// This method returns a list of all of the message templates in the messages table.
        /// </summary>
        /// 
        // ********************************************************************/
        public static ForumMessageTemplateCollection GetMessageTemplateList() {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetMessageTemplateList();
        }


        // *********************************************************************
        //  UpdateMessageTemplate
        //
        /// <summary>
        /// This method update a message template.
        /// </summary>
        /// <param name="message">Message to be updated</param>
        /// 
        // ********************************************************************/
        public static void UpdateMessageTemplate(ForumMessage message) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            dp.UpdateMessageTemplate(message);
        }
   }
}