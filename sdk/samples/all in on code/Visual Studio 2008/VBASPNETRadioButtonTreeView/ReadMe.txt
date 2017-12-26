========================================================================
             VBASPNETRadioButtonTreeView Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

  The project shows how to simulate a RadioButton Group within the TreeView 
  control to make the user can only select one item from a note tree. Since 
  there is no build-in feature to achieve this, we use two images to imitate
  the RadioButton and set TreeNode's ImageUrl property as the path of these
  images. When user clicks the item in tree, reset the Checked value and the
  ImageUrl value of the treenode to make it looks lick a selected item of a 
  RadioButton appearance. 

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1. Create a Visual Basic ASP.NET Web Application in Visual Studio 2008 /
Visual Web Developer and name it as VBASPNETRadioButtonTreeView.

Step2. Add an TreeView control into the page and change its ID property to
RadioButtonTreeView.

Step3: Add some test TreeNode to the TreeView follow the HTML code.

<asp:TreeView ID="RadioButtonTreeView" runat="server" ShowLines="True">
    <Nodes>
        <asp:TreeNode SelectAction="Expand" Text="Operating System" Value="OS">
            <asp:TreeNode Text="Windows XP SP1" Value="Windows XP SP1" />
            <asp:TreeNode Text="Windows XP SP2" Value="Windows XP SP2" />
            <asp:TreeNode Text="Windows 2003" Value="Windows 2003" />
            <asp:TreeNode Text="Windows Vista" Value="Windows Vista" />
        </asp:TreeNode>
        <asp:TreeNode SelectAction="Expand" Text="Application" Value="App">
            <asp:TreeNode Text="Office XP" Value="Office XP" />
            <asp:TreeNode Text="Office 2003" Value="Office 2003" />
            <asp:TreeNode Text="Office 2007" Value="Office 2007" />
        </asp:TreeNode>
    </Nodes>
</asp:TreeView>

[NOTE] The TreeView exposes custom TreeNodeStyle properties for each node 
type (RootNodeStyle, ParentNodeStyle, and LeafNodeStyle), each of which has
an ImageUrl property used for defining an icon for the node type. These 
images are rendered to the left of the node text, as shown in the figure 
above. Each TreeNode may selectively override the default image for its 
node type using the ImageUrl property on the TreeNode object. 

Step4: Perpare two images. One for the state that the TreeNode isn't been
selected, the other for being selected. There already have been two existed
images doing the role. However, we can also make them ourselves by snipping
from other RadioButton from the screen.

Step5: Set the TreeNode's ImageUrl property to the path of the image standing
for unchecked.

Step7: Open the Code-Behind page and locate the VB.NET code into the
SelectedNodeChanged eventer handler of the TreeView control.

[NOTE] In this step, what the code does is to loop through all the tree nodes 
in the same level of the selected one and set the Checked propert to false as 
well as the image first. Then, change the selected node one to be Checked and
set the ImageUrl of this node to the path of the Checked image.

Step8: Write the code to handle the Click event of the Button to return the 
final selection from the RadioButtonTreeView via CheckedNodes property.

[NOTE] All the things we do is to imitate the feature to TreeView control 
and make it looks like contains a list of RadioButton. So, when user click
the TreeView, the page will post back and get flash somehow. Anyway, it is
a work around that is not difficult and can be used if anyone need to add
radiobuttons to a TreeView control.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: TreeView Class
http://msdn.microsoft.com/en-us/library/system.windows.forms.treeview.aspx

MSDN: TreeView Web Server Control Overview
http://msdn.microsoft.com/en-us/library/e8z5184w.aspx

ASP.NET Quickstart Tutorials: TreeView
http://quickstarts.asp.net/QuickStartv20/aspnet/doc/ctrlref/navigation/treeview.aspx


/////////////////////////////////////////////////////////////////////////////