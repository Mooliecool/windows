using System;
using System.Web.Mail;
using System.ComponentModel;

namespace AspNetForums.Components {

    /// <summary>
    /// This class contains properties for an EmailTemplate.
    /// </summary>
    public class EmailTemplate {

		int emailTemplateID = 0;			// emailTemplateID - unique identifier
		String subject = "";				// the subject line of the email template
		String message = "";				// the message of the email template
		String fromAddress = "";			// who the email message is sent from
		String description = "";			// a short description of the email template (for use by the administrator)
		MailPriority priority;				// the priority of which to send the email


        // *********************************************************************
        //  EmailTemplateID
        //
        /// <summary>
        /// Returns the EmailTemplateID, which uniquely identifies each email template.
        /// </summary>
        /// 
        // ********************************************************************/
        public int EmailTemplateID {
            get { 
                return emailTemplateID; 
            }

            set { 
                if (value < 0)
                   emailTemplateID = 0;
                else
                    emailTemplateID = value;
            }			
        }

        // *********************************************************************
        //  Subject
        //
        /// <summary>
        /// Specifies the subject of the email template.
        /// </summary>
        /// 
        // ********************************************************************/
        public String Subject {
            get { 
                return subject; 
            }
            set {
                subject = value;
            }
        }

        // *********************************************************************
        //  Body
        //
        /// <summary>
        /// Specifies the body of the email template.
        /// </summary>
        /// 
        // ********************************************************************/
        public String Body {
            get { 
                return message; 
            }
            set { 
                message = value; 
            }			
        }

        // *********************************************************************
        //  From
        //
        /// <summary>
        /// Specifies the email address of the email template sender.
        /// </summary>
        /// 
        // ********************************************************************/
        public String From {
            get { 
                return fromAddress; 
            }
            set { 
                fromAddress = value; 
            }			
        }

        // *********************************************************************
        //  From
        //
        /// <summary>
        /// Provides a short description of the email template.
        /// </summary>
        /// 
        // ********************************************************************/
        public String Description {
            get { 
                return description; 
            }
            set { 
                description = value; 
            }			
        }

        // *********************************************************************
        //  Priority
        //
        /// <summary>
        /// Specifies the priority of which to send the email message.
        /// </summary>
        /// 
        // ********************************************************************/
        public MailPriority Priority {
            get { 
                return priority; 
            }
            set { 
                priority = value; 
            }			
        }
    }
}
