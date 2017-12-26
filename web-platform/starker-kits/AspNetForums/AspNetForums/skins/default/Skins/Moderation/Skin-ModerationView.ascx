<%@ Control Language="C#" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Register TagPrefix="AspNetForumsModeration" Namespace="AspNetForums.Controls.Moderation" Assembly="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Import Namespace="AspNetForums" %>

<table cellPadding="0" width="100%">
  <tr>
    <td colspan="2" align="left"><AspNetForums:WhereAmI EnableLinks="false" id="Whereami1" runat="server" NAME="Whereami1" /></td>
  </tr>
  <tr>
    <td>
      &nbsp;
    </td>
  </tr>
  <tr>
    <td>
      <span class="normalTextSmallBold">Forum Description: </span><asp:Label CssClass="normalTextSmall" id="ForumDescription" runat="server"/>
    </td>
  </tr>
  <tr>
    <td vAlign="top" colSpan="2">

      <AspNetForums:ThreadList CssClass="tableBorder" ExtractTemplateRows="true" id="ThreadList" Width="100%" CellPadding="3" Cellspacing="1" runat="server">
         <HeaderTemplate>
           <asp:Table runat="server">
             <asp:TableRow>
               <asp:TableHeaderCell Align="Left" CssClass="tableHeaderText">
                 &nbsp; Posts requiring moderation
               </asp:TableHeaderCell> 
             </asp:TableRow>
           </asp:Table>
         </HeaderTemplate>

         <ItemTemplate>
           <asp:Table runat="server">
             <asp:TableRow>
               <asp:TableCell CssClass="forumRow">
                 <AspNetForumsModeration:ModerationMenu UsernamePostedBy='<%# DataBinder.Eval(Container.DataItem, "Username") %>' PostID='<%# DataBinder.Eval(Container.DataItem, "PostID") %>'  ThreadID='<%# DataBinder.Eval(Container.DataItem, "ThreadID") %>' runat="Server" />
                 <table width="100%" cellpadding="3" cellspacing="0">
                   <tr> 
                     <td class="forumRow">
                       <span class="normalTextSmallBold">Subject: </span><a target="_new" href='<%# Globals.UrlShowPost + DataBinder.Eval(Container.DataItem, "PostID") %>' class="linkSmallBold"><%# DataBinder.Eval(Container.DataItem, "Subject") %></a>
                       <br><span class="normalTextSmallBold">Posted By: </span><a class="linkSmallBold" href='<%# Globals.UrlUserProfile + DataBinder.Eval(Container.DataItem, "Username")%>'><%# DataBinder.Eval(Container.DataItem, "Username") %></a>
                     </td>
                   </tr>
                   <tr> 
                     <td class="forumRow">
                     </td>
                   </tr>
                   <tr> 
                     <td class="forumRow">
                       <span class="normalTextSmall"><%# Globals.FormatPostBody(DataBinder.Eval(Container.DataItem, "Body").ToString()) %></span>
                     </td>
                   </tr>
                 </table>
               </asp:TableCell> 

             </asp:TableRow>

             <asp:TableRow>
               <asp:TableCell ColumnSpan="2">
               </asp:TableCell>
             </asp:TableRow>
           </asp:Table>
         </ItemTemplate>

      </AspNetForums:ThreadList>
    </td>
  </tr>
  <tr>
    <td colspan="2">
      &nbsp;
    </td>
  </tr>
  <tr>
    <td colSpan="2">&nbsp;
    </td>
  </tr>
</table>


