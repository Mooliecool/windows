using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Text.RegularExpressions; //base regular expression functionality found here

namespace MaskEditSample
{

	/// <summary>
	/// The MaskedEditTextBox class is derived from the based TextBox
	/// class.  It accepts or rejects input, based on a supplied regular
	/// expression, in one of two ways: either each keypress is masked (i.e.
	/// all input is validated as it is entered), or the input is checked
	/// all at once when the field is left.  The choice of which mode to use
	/// depends on the regular expression employed (e.g. an expected pattern
	/// of "xxx-xx-xxxx" where "x" is a number can never be matched if only
	/// one character can be entered and evaluated at a time).
	/// </summary>
	public class MaskedEditTextBox : System.Windows.Forms.TextBox
	{
		//used to contain delegates to call when the text is invalid
		private EventHandler textInvalid;

		//property to control event subscription
		public event EventHandler TextInvalid
		{
			add  //add EventHandler delegate (value) to textInvalid
			{
				textInvalid += value;
			}
			remove  //remove EventHandler delegate (value) to textInvalid
			{
				textInvalid -= value;
			}
		}

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		/// <summary>
		/// Private field used to contain a last known good value.
		/// </summary>
		private string lastValidValue = "";

		/// <summary>
		/// Public property used to provide read-only access to lastValidValue
		/// </summary>
		public string LastValidValue
		{
			get
			{
				return lastValidValue;
			}
		}

		/// <summary>
		/// The regular expression that input will be validated against after this
		/// textbox is left.
		/// </summary>
		private Regex expression;

		#region Expression property comments
		/// <summary>
		/// By applying the Editor attribute we can specify a class dervied from
		/// System.Drawing.Design.UITypeEditor; this class will define the user 
		/// interface presented when the property is modified from Visual Studio .NET.
		/// 
		/// The Description attribute is used to provide context help in the 
		/// properties windows of Visual Studio .NET.
		/// 
		/// The DefaultValue attribute can be used to specify a default value for the 
		/// property (in this case, we can use it to specify the default regular 
		/// expression to be used).
		/// 
		/// The Expression property itself is used to store the regular expression 
		/// that will be used to evaluate/mask the input into this control after the control
		/// is left.
		/// </summary>
		#endregion
		[Editor(typeof(System.Web.UI.Design.WebControls.RegexTypeEditor), 
			 typeof(System.Drawing.Design.UITypeEditor)),
		Description( "Use this property to select or specify the regular expression to validate the control." ),
		DefaultValue(" ")]
		public string Expression
		{
			//return the private expression
			get 
			{ 
				return (expression.ToString().Length > 0) ? expression.ToString() : "";
			}
			
			//set the private expression
			set 
			{ 
				expression = new Regex(value); 
			}
		}

		/// <summary>
		/// The regular expression that input will be validated against on a per
		/// character basis as it is typed by the user.
		/// </summary>
		private Regex keyPressExpression;

		/// <summary>
		/// Like Expression above this property is used to provide access to a private
		/// regular expression field, keyPressExpression.  This private field is used to
		/// evalute/mask individual character input.
		/// </summary>
		public string KeyPressExpression
		{
			get
			{ 
				return (keyPressExpression.ToString().Length > 0) ? keyPressExpression.ToString() : "";
			}

			set
			{ 
				keyPressExpression = new Regex(value);
			}
		}

		/// <summary>
		/// Supplies the list of modes to be used by the control.
		/// </summary>
		public enum MaskMode
		{
			KeyPress,
			Leave,
			Both
		}

		/// <summary>
		/// Private field used to contain the mask mode of the control.
		/// </summary>
		private MaskMode mode;

		/// <summary>
		/// Public property used to expose the mask mode of the control.
		/// </summary>
		public MaskMode Mode
		{
			get 
			{
				return mode; 
			}

			set 
			{ 
				mode = value; 
			}
		}

		/// <summary>
		/// Default constructor
		/// </summary>
		public MaskedEditTextBox()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			//initialize mask mode
			this.mode = MaskMode.Both;

			//initialize expression
			this.expression = new Regex("");

			//initialize keyPressExpression
			this.keyPressExpression = new Regex("");
		}

		/// <summary>
		/// Another constructor for the class, this method allows the developer to specify
		/// the mask mode and expressions to be used.
		/// </summary>
		/// <param name="evalMode">The mask mode (i.e. per character, on leave, or both) to be employ</param>
		/// <param name="leaveExpression">The expression used to evalute input on leave</param>
		/// <param name="keystrokeExpression">The expression used to evaluate input as characters are typed</param>
		public MaskedEditTextBox(MaskMode evalMode, string leaveExpression, string keystrokeExpression)
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			//set mask mode
			this.Mode = evalMode;

			//initialize appropriate expressions
			switch ( evalMode )
			{
				case MaskMode.KeyPress :
					this.keyPressExpression = new Regex( keystrokeExpression );
					break;
				case MaskMode.Leave :
					this.expression = new Regex( leaveExpression );
					break;
				case MaskMode.Both :
					this.keyPressExpression = new Regex( keystrokeExpression );
					this.expression = new Regex( leaveExpression );
					break;
			}
		}


		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{

		}
		#endregion

		/// <summary>
		/// Override of the OnKeyPress method of the TextBox base class, this function
		/// will test each character as it is typed to determine if the regular expression
		/// would be satisfied by the resulting value of this.Text.  If so, then the
		/// value is allowed, otherwise the input is ignored.
		/// </summary>
		/// <param name="e">Expected KeyPressEventArgs object provided by the framework</param>
		protected override void OnKeyPress( KeyPressEventArgs e )
		{
			//Evaluate each keystroke?  (i.e. if mode is "Leave" then exit)
			if(this.mode == MaskMode.Leave) 
			{
				//allows further processing (e.g. writing the character) by base class
				e.Handled = false;
				//send all event args to base class, including the above property that
				//indicates to the base class to process the input
				base.OnKeyPress(e);

				return; //exit
			}

			//determine if there is a match
			Match patternMatch = this.keyPressExpression.Match(this.Text + e.KeyChar);

			//if so, then do nothing and call allow input
			if((patternMatch.Success) && (patternMatch.Length == this.Text.Length + 1))
			{
				e.Handled = false;  //allows further processing of input
			}
			//otherwise, prevent input
			else
			{
				e.Handled = true;  //prevents further processing of input
			}
			//send all event args to base class, including the above property that
			//indicates to the base class to process the input
			base.OnKeyPress(e);
		}

		/// <summary>
		/// Override of the OnLeave method of the TextBox base class, this function will test the current
		/// value of the control and determine if it matches the appropriate regular
		/// expression (i.e. this.expression).  If so, then everything proceeds as
		/// normal, otherwise the value is changed back to a previously stored value
		/// (i.e. this.lastValidValue).
		/// </summary>
		/// <param name="e">The expected EventArgs object supplied by the framework.</param>
		protected override void OnLeave( EventArgs e )
		{
			//evalute?  (i.e. if mode is KeyPress then exit)
			if(this.mode == MaskMode.KeyPress) { base.OnLeave( e ); return; }

			//determine if there is a match
			Match patternMatch = this.expression.Match( this.Text );

			//if there is a match containing the whole input string
			if((patternMatch.Success) && (patternMatch.Length == this.Text.Length))
			{
				//store valid value
				lastValidValue = this.Text;

				//call TextBox control's OnLeave
				base.OnLeave( e );
			} 
			//otherwise notify owner
			else 
			{
				//if the owner has subscribed then call textInvalid delegate(s)
				//thus letting owner decide what to do
				if(textInvalid != null)
				{
					textInvalid(this, e);
				}
				//otherwise, just revert to last valid value
				else
				{
					this.Text = lastValidValue;
				}
			}
		}
	}
}
