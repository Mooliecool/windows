using System;
using System.Collections;

namespace AspNetForums.Components {
    public class EmailTemplateCollection : ArrayList {
        // default constructor
        public EmailTemplateCollection() : base() {}
        public EmailTemplateCollection(ICollection c) : base(c) {}
    }
}
