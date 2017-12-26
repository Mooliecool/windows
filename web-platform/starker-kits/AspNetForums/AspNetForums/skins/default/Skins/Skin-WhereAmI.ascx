<%@ Control Language="C#" %>
<table cellPadding="0" cellSpacing="0" width="100%">
    <tr>
        <td valign="top" align="left" width="1px">
            <nobr>
                <asp:HyperLink CssClass="linkMenuSink" runat="server" id="LinkHome" />
            </nobr>
        </td>
        <td class="popupMenuSink" valign="top" align="left" runat="server" id="ForumGroupMenu" width="1px">
            <nobr>
                <span runat="server" class="normalTextSmallBold" id="ForumGroupSeparator">&nbsp;&gt;</span>
                <asp:HyperLink CssClass="linkMenuSink" runat="server" id="LinkForumGroup" />
            </nobr>
        </td>
        <td class="popupMenuSink" valign="top" align="left" runat="server" id="ForumMenu" width="1px">
            <nobr>
                <span runat="server" class="normalTextSmallBold" id="ForumSeparator">&nbsp;&gt;</span>
                <asp:HyperLink CssClass="linkMenuSink" runat="server" id="LinkForum" />
            </nobr>
        </td>
        <td class="popupMenuSink" valign="top" align="left" runat="server" id="PostMenu" width="1px">
            <nobr>
                <span runat="server" class="normalTextSmallBold" id="PostSeparator">&nbsp;&gt;</span>
                <asp:HyperLink CssClass="linkMenuSink" runat="server" id="LinkPost" />
            </nobr>
        </td>
        <td valign="top" align="left" width="*">&nbsp;</td>
    </tr>
</table>

<span id="MenuScript" runat="server">
    <script language='javascript'>
    var timeoutCookie;
    var menuPopupCookie;

    function menuOver(id)
    {
        if ( menuPopupCookie != null )
        {
            window.clearTimeout(menuPopupCookie);
            menuPopupCookie = null;
        }
        
        if ( event.srcElement.className != 'linkMenuSink' )
        {
            return;
        }
        
        var menuX = (event.clientX - (event.x + 2)) + document.body.scrollLeft;
        var menuY = (event.clientY - event.y) + document.body.scrollTop + 16;
        
        event.srcElement.onmouseleave = menuPopCancel;
        menuPopupCookie = window.setTimeout('menuPop(\'' + id + '\', ' + menuX + ', ' + menuY + ');', 1000);
    }
    
    function menuPopCancel()
    {
        if ( menuPopupCookie != null )
        {
            window.clearTimeout(menuPopupCookie);
            menuPopupCookie = null;
        }
    }
    
    function menuPop(id, x, y)
    {
        menuPopupCookie = null;
        var menu = document.all.item(id);
        
        if ( menu != null )
        {
            menu.style.left = x;
            menu.style.top = y;
            menu.style.zIndex = 100;
            menu.style.display = 'block';
            menu.onmouseleave = menuOut;
            menu.onmouseenter = menuEnter;
        }
    }

    function menuEnter()
    {
        if ( timeoutCookie != null )
        {
            window.clearTimeout(timeoutCookie);
            timeoutCookie = null;
        }
    }

    function menuSourceLeave(id)
    {
        timeoutCookie = window.setTimeout('menuClose(\'' + id + '\');', 100);
    }

    function menuClose(id)
    {
        var menu = document.all.item(id);
        if ( menu != null )
        {
            menu.style.zIndex = -1;
            menu.style.display = 'none';
        }
    }

    function menuOut()
    {
        var menu = event.srcElement;
        if ( menu != null )
        {
            menu.style.zIndex = -1;
            menu.style.display = 'none';
        }
    }
    </script>
</span>