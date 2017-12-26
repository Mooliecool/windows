using System;
using System.Xml.Serialization;

namespace AspNetForums.Components {
    public class ModerationQueueStatus {

        [XmlElement("count")]
        public int Count;

        [XmlElement("ageInMinutes")]
        public int AgeInMinutes;
    }
}
