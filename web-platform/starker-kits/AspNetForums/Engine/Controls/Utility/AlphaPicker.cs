using System;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Configuration;
using System.Web.UI;
using System.Web.UI.WebControls;
using AspNetForums;
using AspNetForums.Components;
using System.ComponentModel;
using System.IO;

namespace AspNetForums.Controls {
    [
    ParseChildren(true)
    ]

    public class AlphaPicker : WebControl, INamingContainer {

        string currentLetter = "All";

        // *********************************************************************
        //  CreateChildControls
        //
        /// <summary>
        /// Renders the Alpha picker options
        /// </summary>
        /// 
        // ********************************************************************/ 
        protected override void CreateChildControls() {
            Label label;

            // Add the series of linkbuttons
            char chrStart = 'A';
            char chrStop = 'Z';

            // Loop through all the characters
            for (int iLoop = chrStart; iLoop <= chrStop; iLoop++) {
                Controls.Add(CreateLetteredLinkButton(((char) iLoop).ToString()));

                label = new Label();
                label.CssClass = "normalTextSmallBold";
                label.Text = " | ";
                Controls.Add(label);
            }

            Controls.Add(CreateLetteredLinkButton("All"));


        }

        // *********************************************************************
        //  CreateLetteredLinkButton
        //
        /// <summary>
        /// Creates link buttons
        /// </summary>
        /// 
        // ********************************************************************/ 
        private LinkButton CreateLetteredLinkButton(String buttonText) {

            // Add a new link button
            LinkButton btnTmp = new LinkButton();
            btnTmp.Text = buttonText;
            btnTmp.CssClass = "linkSmallBold";
            btnTmp.CommandArgument = buttonText;
            btnTmp.Click += new System.EventHandler(Letter_Clicked);

            return btnTmp;
        }

        // *********************************************************************
        //  Letter_Clicked
        //
        /// <summary>
        /// Event raised when a letter has been clicked by the end user
        /// </summary>
        /// 
        // ********************************************************************/
        public event System.EventHandler Letter_Changed;

        // *********************************************************************
        //  Letter_Clicked
        //
        /// <summary>
        /// Event raised when a letter is clicked upon.
        /// </summary>
        /// 
        // ********************************************************************/
        private void Letter_Clicked(Object sender, EventArgs e) {

            SelectedLetter = ((LinkButton) sender).CommandArgument;

            if (null != Letter_Changed)
                Letter_Changed(sender, e);

        }

        // *********************************************************************
        //  SelectedLetter
        //
        /// <summary>
        /// Property that returns the currently selected Letter
        /// </summary>
        /// 
        // ********************************************************************/
        public string SelectedLetter {
            get {
                if (ViewState["SelectedLetter"] == null)
                    return currentLetter;

                return (string) ViewState["SelectedLetter"];
            }
            set {
                ViewState["SelectedLetter"] = value;
            }
        }

    }
}
