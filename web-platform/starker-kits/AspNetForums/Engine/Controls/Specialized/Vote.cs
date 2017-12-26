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
using System.Drawing;
using System.Xml;

namespace AspNetForums.Controls.Specialized {

    /// <summary>
    /// The Vote control is allows for users to vote on topics. The results are stored in 
    /// a SQL Server table and a HTML graph is used to display the selections/percentages.
    /// </summary>
    [
    ToolboxData("<{0}:Vote runat=\"server\" />")
    ]
    public class Vote : WebControl, INamingContainer {

        // Internal members
        //
        RadioButtonList radioButtonList;
        VoteDetails voteDetails;
        VoteResultCollection voteResults;
        Color barBackColor;
        Color barForeColor;
        string buttonText = "Vote";
        int postID;

        // *********************************************************************
        //  Vote
        //
        /// <summary>
        // Get Poll Details reads the XML file (or cache)
        // and fills an instance class with details
        /// </summary>
        // ***********************************************************************/
        public Vote(int postID, string postSubject, string postBody) {
            this.postID = postID;
            voteResults = Votes.GetVoteResults(postID);
            voteDetails = new VoteDetails(postID, postSubject, postBody);
        }

        // *********************************************************************
        //  ButtonText
        //
        /// <summary>
        /// Used to control the text on the button. Default is 'Vote'
        /// </summary>
        // ***********************************************************************/        
        [
        DesignerSerializationVisibility(DesignerSerializationVisibility.Content),
        PersistenceMode(PersistenceMode.Attribute)
        ]
        public String ButtonText {
            get {
                return buttonText;
            }
            set {
                buttonText = value;
            }
        }

        // *********************************************************************
        //  BarBackColor
        //
        /// <summary>
        /// Controls the back ground color of the rendered bar(s).
        /// </summary>
        // ***********************************************************************/        
        [
        Category("Style"),
        PersistenceMode(PersistenceMode.Attribute)
        ]
        public Color BarBackColor {
            get {
                return barBackColor;
            }
            set {
                barBackColor = value;
            }
        }

        // *********************************************************************
        //  BarForeColor
        //
        /// <summary>
        /// Controls the foreground color of the rendered bar(s).
        /// </summary>
        // ***********************************************************************/        
        [
        Category("Style"),
        PersistenceMode(PersistenceMode.Attribute)
        ]
        public Color BarForeColor {
            get {
                return barForeColor;
            }
            set {
                barForeColor = value;
            }
        }

        protected override void CreateChildControls() {

            if ((voteDetails.HasVoted) && (voteDetails.AllowMultipleVotes))
                HasNotVoted();
            else if (!voteDetails.HasVoted)
                HasNotVoted();
            else
                HasVoted("Test");

        }

        protected void HasNotVoted() {

            // Member variables
            //
            Table t = new Table();
            t.ID = "HasNotVoted";
            TableRow questionRow = new TableRow();
            TableRow choicesRow = new TableRow();
            TableRow buttonRow = new TableRow();
            TableCell questionCell = new TableCell();
            TableCell choicesCell = new TableCell();
            TableCell buttonCell = new TableCell();
            Button button = new Button();
            radioButtonList = new RadioButtonList();
            radioButtonList.CssClass = "normalTextSmallBold";

            // Wire up the button click event
            //
            button.Click += new EventHandler(VoteButton_Click);
			
            // Format Question
            //
            Label questionText = new Label();
            questionText.CssClass = "forumTitle";
            questionText.Text = "Poll: " + voteDetails.Question;
            questionCell.Controls.Add(questionText);

            // Insert question cell and row into table
            //
            questionRow.Cells.Add(questionCell);
            t.Rows.Add(questionRow);

            // Populate the radioButtonList
            //
            foreach (string key in voteDetails.VoteOptions.Keys)
                radioButtonList.Items.Add(new ListItem((string)voteDetails.VoteOptions[key], key));

            // Insert the radio button into the cell
            //
            choicesCell.Controls.Add(radioButtonList);

            // Add row/cell to table
            //
            choicesRow.Cells.Add(choicesCell);
            t.Rows.Add(choicesRow);

            // Format Button
            //
            button.Text = buttonText;

            // Insert button into cell
            //
            buttonCell.Controls.Add(button);

            // Add button cell/row to table
            //
            buttonRow.Cells.Add(buttonCell);
            t.Rows.Add(buttonRow);

            // Add table to the controls collection
            //
            this.Controls.Add(t);

        }

        protected void HasVoted(string votedFor) {
            // Member Variables
            //
            int sum = 0;
            Table t = new Table();
            TableRow questionRow = new TableRow();
            TableRow resultsRow = new TableRow();
            TableRow footerRow = new TableRow();
            TableCell questionCell = new TableCell();
            TableCell resultsCell = new TableCell();
            TableCell footer = new TableCell();

            // Format the table
            //
            t.BorderWidth = 0;

            // Question
            //
            Label questionText = new Label();
            questionText.CssClass = "forumTitle";
            questionText.Text = voteDetails.Question;
            questionCell.Controls.Add(questionText);
            questionCell.ColumnSpan = 3;

            // Add the question cell/row to the table
            //
            questionRow.Cells.Add(questionCell);
            t.Rows.Add(questionRow);

            // Calculate the sum
            //
            foreach (string key in voteResults.Keys)
                sum = sum + voteResults[key].VoteCount;

            // Calculate percentage
            //
            foreach (string key in voteDetails.VoteOptions.Keys) {

                // Internal variables/instances
                //
                double d = 0;
                int pollValue = 0;
                TableRow row = new TableRow();
                TableCell progressCell = new TableCell();
                TableCell percentageCell = new TableCell();
                TableCell choiceCell = new TableCell();
                ProgressBar bar = new ProgressBar();
				
                // Get the poll value
                //
                if (null != voteResults[key])
                    pollValue = voteResults[key].VoteCount;

                // Percentage for this poll value
                //
                d = ((double)pollValue / (double)sum) * 100;

                // Display the choice in cell and format
                //
                Label voteText = new Label();
                voteText.Text = (string)voteDetails.VoteOptions[key] + ": ";
                voteText.CssClass = "normalTextSmallBold";
                choiceCell.Controls.Add(voteText);
                choiceCell.VerticalAlign = VerticalAlign.Top;
                choiceCell.Wrap = false;

                // Set properties for each bar
                //
                bar.PercentageOfProgress = (int)d;

                // Add the bar and set properties of the cell
                //
                progressCell.Controls.Add(bar);
                progressCell.HorizontalAlign = HorizontalAlign.Right;
                progressCell.VerticalAlign = VerticalAlign.Top;
                progressCell.Width = 100;

                // Special case 0
                //
                Label percentageText = new Label();
                percentageText.CssClass = "normalTextSmallBold";
                if ((double.IsNaN(d)) || (0 == d))
                    percentageText.Text = "(0%)";
                else
                    percentageText.Text = "(" + d.ToString("##.#") + "%)";

                percentageCell.Controls.Add(percentageText);

                // Format percentage cell
                //
                percentageCell.HorizontalAlign = HorizontalAlign.Left;
                percentageCell.VerticalAlign = VerticalAlign.Top;

                // Add the cells to the row
                //
                row.Cells.Add(choiceCell);
                row.Cells.Add(progressCell);
                row.Cells.Add(percentageCell);

                // Add the row to the table
                //
                t.Rows.Add(row);

            }

            // What you voted for
            //
            resultsCell.ColumnSpan = 3;
            resultsCell.HorizontalAlign = HorizontalAlign.Right;

            Label resultsText = new Label();
            resultsText.Text = "Your vote: <b>" + voteDetails.VoteOptions[votedFor] + "</b>";
            resultsText.CssClass = "normalTextSmaller";
            resultsCell.Controls.Add(resultsText);

            // Add results cell/row to table
            //
            resultsRow.Cells.Add(resultsCell);
            t.Rows.Add(resultsRow);
			
            // Set footer properties
            //
            Label totalVotes = new Label();
            totalVotes.Text = "Total votes: <b>" + sum + "</b>";
            totalVotes.CssClass = "normalTextSmaller";
            footer.Controls.Add(totalVotes);
            footer.ColumnSpan = 3;
            footer.HorizontalAlign = HorizontalAlign.Right;

            // Add footer cell and add to table
            //
            footerRow.Cells.Add(footer);
            t.Rows.Add(footerRow);

            this.Controls.Add(t);
        }

        void VoteButton_Click(Object sender, EventArgs e) {
            Votes.Vote(postID, radioButtonList.SelectedItem.Value);
            
            /*
            // Increment our internal poll results
            //
            if (null == voteResults[radioButtonList.SelectedItem.Value])
                pollresults[radioButtonList.SelectedItem.Value] = 1;
            else
                pollresults[radioButtonList.SelectedItem.Value] = (int)pollresults[radioButtonList.SelectedItem.Value] + 1;
            */

            // Clear the controls collection
            //
            Control remove = FindControl("HasNotVoted");
            Controls.Remove(remove);

            // User has voted
            //
            voteDetails.SetHtasVotedCookie(radioButtonList.SelectedItem.Value);

            // Change the display to HasVoted
            //
            HasVoted(radioButtonList.SelectedItem.Value);


        }

    }
}
