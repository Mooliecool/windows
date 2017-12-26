//------------------------------------------------------------------------------
// <copyright file="MailSettingsSection.cs" company="Microsoft Corporation">
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
//------------------------------------------------------------------------------

namespace System.Net.Configuration
{
    using System.Configuration;
    using System.Collections.Generic;
    using System.Globalization;
    using System.IO;    
    public sealed class MailSettingsSectionGroup : ConfigurationSectionGroup
    {
        public MailSettingsSectionGroup() 
        {
        }

        public SmtpSection Smtp
        {
            get { return (SmtpSection)Sections["smtp"]; }
	}
    }

    internal sealed class MailSettingsSectionGroupInternal
    {
        internal MailSettingsSectionGroupInternal()
        {
            this.smtp = SmtpSectionInternal.GetSection();
        }

        internal SmtpSectionInternal Smtp
        {
            get { return this.smtp; }
        }

        static internal MailSettingsSectionGroupInternal GetSection()
        {
            return new MailSettingsSectionGroupInternal();
        }

        SmtpSectionInternal smtp = null;
    }
}
