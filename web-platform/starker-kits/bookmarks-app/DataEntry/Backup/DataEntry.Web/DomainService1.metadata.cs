
namespace DataEntry.Web
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.ComponentModel.DataAnnotations;
    using System.Linq;
    using System.ServiceModel.DomainServices.Hosting;
    using System.ServiceModel.DomainServices.Server;


    // The MetadataTypeAttribute identifies SiteMetadata as the class
    // that carries additional metadata for the Site class.
    [MetadataTypeAttribute(typeof(Site.SiteMetadata))]
    public partial class Site
    {

        // This class allows you to attach custom attributes to properties
        // of the Site class.
        //
        // For example, the following marks the Xyz property as a
        // required property and specifies the format for valid values:
        //    [Required]
        //    [RegularExpression("[A-Z][A-Za-z0-9]*")]
        //    [StringLength(32)]
        //    public string Xyz { get; set; }
        internal sealed class SiteMetadata
        {

            // Metadata classes are not meant to be instantiated.
            private SiteMetadata()
            {
            }

            public int Category { get; set; }

            public string City { get; set; }

            public string Country { get; set; }

            public string Name { get; set; }

            public int SiteID { get; set; }

            public string States { get; set; }

            public string URL { get; set; }
        }
    }
}
