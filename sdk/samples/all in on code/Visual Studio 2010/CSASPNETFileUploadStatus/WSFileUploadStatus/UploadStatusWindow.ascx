<%@ Control Language="C#" AutoEventWireup="true" CodeFile="UploadStatusWindow.ascx.cs"
    Inherits="UploadStatusWindow" %>
<div id="status" class="UploadStatusWindow">
    <table cellpadding="3" cellspacing="0" border="0" align="center">
        <tr>
            <td colspan="4" style="text-align: center; color: Red; width: 550px">
                <span id="spError"></span>
            </td>
        </tr>
        <tr>
            <th>
                Uploaded Content:
            </th>
            <td>
                <span id="spUploaded"></span>
            </td>
            <th>
                Total Content-Length:
            </th>
            <td>
                <span id="spTotle"></span>
            </td>
        </tr>
        <tr>
            <th>
                Upload Speed:
            </th>
            <td>
                <span id="spSpeed"></span>
            </td>
            <th>
                Uploaded Percent:
            </th>
            <td>
                <span id="spPercent"></span>
            </td>
        </tr>
        <tr>
            <th>
                Spent Time:
            </th>
            <td>
                <span id="spSpentTime"></span>
            </td>
            <th>
                Estimated Remain Time:
            </th>
            <td>
                <span id="spRemainTime"></span>
            </td>
        </tr>
        <tr>
            <td colspan="4" id="ProgressBarContainer" style="text-align: center; width: 550px;">
            </td>
        </tr>
        <tr>
            <td colspan="4" style="text-align: right; width: 550px;">
                <input type="button" value="Abort" onclick="AbortUpload()" />
                <input type="button" value="Close" onclick="CloseUploadWindow()" />
            </td>
        </tr>
    </table>
</div>
