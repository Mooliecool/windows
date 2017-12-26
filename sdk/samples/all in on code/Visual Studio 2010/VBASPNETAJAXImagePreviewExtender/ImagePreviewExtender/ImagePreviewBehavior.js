/****************************** Module Header ******************************\
* Module Name:    ImagePreviewBehavior.js
* Project:        VBASPNETImagePreviewExtender
* Copyright (c) Microsoft Corporation
*
* This js file will be generated automatically when we create the project by
* the template,ASP.NET AJAX Server Control Extender, from the Visual Studio.
* 
* In this file, we will define some javascript functions to realize the AJAX
* feature for the extender control.
* 
* Please don't delete the default coding in this file, we just need to modify
* it with some extend codes.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/



/// <reference name="MicrosoftAjax.js"/>

Type.registerNamespace("ImagePreviewExtender");

ImagePreviewExtender.ImagePreviewBehavior = function (element) {
    ImagePreviewExtender.ImagePreviewBehavior.initializeBase(this, [element]);
    this._ThumbnailCssClass = null;
    this._ThumbnailGenericHanler = null;
    this._ThumbnailGenericHanlerPathQueryStringKey = "abc";
}

ImagePreviewExtender.ImagePreviewBehavior.prototype = {

    // Here is the initialize function.
    // We bind events here.
    initialize: function () {
        ImagePreviewExtender.ImagePreviewBehavior.callBaseMethod(this, 'initialize');

        // this.get_element() will return the control with TargetControlID.
        // We set the thumbnailCssClass here and bind a click event for
        // the images to open a popup box.
        var tgName = this.get_element().tagName;
        if (tgName == "DIV") {
            var ctrls = this.get_element().getElementsByTagName("IMG");
            for (var i = 0; i < ctrls.length; i++) {
                ctrls[i].className = this._ThumbnailCssClass;
                $addHandlers(ctrls[i],
                    {
                        'click': this._onclick
                    }, this);
            }
        }
        else if (tgName == "IMG") {
            $addHandlers(this.get_element(),
                    {
                        'click': this._onclick
                    }, this);
            this.get_element().className = this._ThumbnailCssClass;
        }

    },

    // Here is the dispose function.
    // We remove the events here.
    dispose: function () {
        $clearHandlers(this.get_element());
        ImagePreviewExtender.ImagePreviewBehavior.callBaseMethod(this, 'dispose');
    },


    // We create a new function here.
    // We use this event function and bind it to the image control to open
    // a popup box to show the image.
    _onclick: function (e) {
        if (e.target && !e.target.disabled) {
            var bodyElement = document.createElement("IMG");
            bodyElement.style.cssText = "border:1px solid Gray; padding:5px";
            bodyElement.src = e.target.src;
            var dialog = CreateDialog(bodyElement, this.get_element());

        }
    },

    // Here we write the functions to get or set the 
    // ThumbnailCssClass property by javascript.
    get_ThumbnailCssClass: function () {
        return this._ThumbnailCssClass;
    },

    set_ThumbnailCssClass: function (value) {
        if (this._ThumbnailCssClass != value) {
            this._ThumbnailCssClass = value;
            this.raisePropertyChanged('ThumbnailCssClass');
        }
    }


}

// This is a isolated function against the 
// extender behavior(ImagePreviewExtender.ImagePreviewBehavior).
// It will open a popup box.
// The principal is to create an IFRAME object as the background mask and
// a DIV object as the box to hold the popup controls.
// We design their css styles to meet the requirement. 
function CreateDialog(BodyElement, sender) {

    var baseDoc = document.documentElement ? document.documentElement :
                                                 document.outerElement;
    var container = document.createElement("DIV");
    container.style.width = baseDoc.offsetWidth + "px";
    container.style.cssText += ";position:absolute;left:0;top:0;" +
                                "overflow:visible;";
    document.body.appendChild(container);

    var mask = document.createElement("IFRAME");
    mask.style.width = baseDoc.scrollWidth + "px";
    mask.style.height = baseDoc.scrollHeight + "px";
    mask.style.position = "absolute";
    mask.style.zIndex = "10000";
    mask.frameBorder = 0;
    mask.style.left = "0";
    mask.style.top = "0";
    mask.allowTransparency = true;
    mask.style.cssText += ";background-color:gray;-moz-opacity:0.5;" +
        "-khtml-opacity: 0.5;opacity: 0.5;filter:alpha(opacity=50);";
    container.appendChild(mask);

    if (BodyElement != null) {

        var dialog = document.createElement("DIV");
        dialog.appendChild(BodyElement);
        container.appendChild(dialog);
        dialog.style.position = "absolute";
        dialog.style.width = BodyElement.offsetWidth + "px";
        dialog.style.marginLeft = "50%";
        var topPX = baseDoc.clientHeight / 2 - Number(BodyElement.offsetWidth / 2);
        dialog.style.top = (topPX > 0 ? topPX : 15) + "px";
        dialog.style.left = "-" + Number(BodyElement.offsetWidth / 2) + "px";
        if (dialog.offsetLeft < 0) {
            dialog.style.marginLeft = "0";
            dialog.style.left = "15px";
        }
        dialog.style.cssText += ";overflow:visible;background-color:gray; z-index:10001";

        var closeIcon = document.createElement("IMG");
        closeIcon.alt = "Click here or double click the image to close";
        closeIcon.title = "Click here or double click the image to close";
        closeIcon.src = sender.ImagePreviewBehavior.closeImage;
        closeIcon.style.cssText = "position:absolute;left:-15px;top:-15px;cursor:pointer";
        closeIcon.onclick = function () { DisposeDialog(container) }
        dialog.appendChild(closeIcon);

        BodyElement.ondblclick = function () { DisposeDialog(container) }

    }

    document.body.style.overflow = "hidden";

    return container;
}

// This function will remove the popup box we created by CreateDialog().
function DisposeDialog(obj) {
    if (obj != null)
        document.body.removeChild(obj);
    document.body.style.overflow = "auto";
}

// Optional descriptor for JSON serialization.
ImagePreviewExtender.ImagePreviewBehavior.descriptor = {
    properties: [{ name: 'ThumbnailCssClass', type: String }
                    ]
}

ImagePreviewExtender.ImagePreviewBehavior.registerClass(
                'ImagePreviewExtender.ImagePreviewBehavior', Sys.UI.Behavior);

if (typeof (Sys) !== 'undefined') Sys.Application.notifyScriptLoaded();

// SIG // Begin signature block
// SIG // MIIXSgYJKoZIhvcNAQcCoIIXOzCCFzcCAQExCzAJBgUr
// SIG // DgMCGgUAMGcGCisGAQQBgjcCAQSgWTBXMDIGCisGAQQB
// SIG // gjcCAR4wJAIBAQQQEODJBs441BGiowAQS9NQkAIBAAIB
// SIG // AAIBAAIBAAIBADAhMAkGBSsOAwIaBQAEFPcOdhtkQEAR
// SIG // Ym3z+wvIp3SAYRPWoIISMTCCBGAwggNMoAMCAQICCi6r
// SIG // EdxQ/1ydy8AwCQYFKw4DAh0FADBwMSswKQYDVQQLEyJD
// SIG // b3B5cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0IENvcnAu
// SIG // MR4wHAYDVQQLExVNaWNyb3NvZnQgQ29ycG9yYXRpb24x
// SIG // ITAfBgNVBAMTGE1pY3Jvc29mdCBSb290IEF1dGhvcml0
// SIG // eTAeFw0wNzA4MjIyMjMxMDJaFw0xMjA4MjUwNzAwMDBa
// SIG // MHkxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5n
// SIG // dG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVN
// SIG // aWNyb3NvZnQgQ29ycG9yYXRpb24xIzAhBgNVBAMTGk1p
// SIG // Y3Jvc29mdCBDb2RlIFNpZ25pbmcgUENBMIIBIjANBgkq
// SIG // hkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAt3l91l2zRTmo
// SIG // NKwx2vklNUl3wPsfnsdFce/RRujUjMNrTFJi9JkCw03Y
// SIG // SWwvJD5lv84jtwtIt3913UW9qo8OUMUlK/Kg5w0jH9FB
// SIG // JPpimc8ZRaWTSh+ZzbMvIsNKLXxv2RUeO4w5EDndvSn0
// SIG // ZjstATL//idIprVsAYec+7qyY3+C+VyggYSFjrDyuJSj
// SIG // zzimUIUXJ4dO3TD2AD30xvk9gb6G7Ww5py409rQurwp9
// SIG // YpF4ZpyYcw2Gr/LE8yC5TxKNY8ss2TJFGe67SpY7UFMY
// SIG // zmZReaqth8hWPp+CUIhuBbE1wXskvVJmPZlOzCt+M26E
// SIG // RwbRntBKhgJuhgCkwIffUwIDAQABo4H6MIH3MBMGA1Ud
// SIG // JQQMMAoGCCsGAQUFBwMDMIGiBgNVHQEEgZowgZeAEFvQ
// SIG // cO9pcp4jUX4Usk2O/8uhcjBwMSswKQYDVQQLEyJDb3B5
// SIG // cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0IENvcnAuMR4w
// SIG // HAYDVQQLExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xITAf
// SIG // BgNVBAMTGE1pY3Jvc29mdCBSb290IEF1dGhvcml0eYIP
// SIG // AMEAizw8iBHRPvZj7N9AMA8GA1UdEwEB/wQFMAMBAf8w
// SIG // HQYDVR0OBBYEFMwdznYAcFuv8drETppRRC6jRGPwMAsG
// SIG // A1UdDwQEAwIBhjAJBgUrDgMCHQUAA4IBAQB7q65+Siby
// SIG // zrxOdKJYJ3QqdbOG/atMlHgATenK6xjcacUOonzzAkPG
// SIG // yofM+FPMwp+9Vm/wY0SpRADulsia1Ry4C58ZDZTX2h6t
// SIG // KX3v7aZzrI/eOY49mGq8OG3SiK8j/d/p1mkJkYi9/uEA
// SIG // uzTz93z5EBIuBesplpNCayhxtziP4AcNyV1ozb2AQWtm
// SIG // qLu3u440yvIDEHx69dLgQt97/uHhrP7239UNs3DWkuNP
// SIG // tjiifC3UPds0C2I3Ap+BaiOJ9lxjj7BauznXYIxVhBoz
// SIG // 9TuYoIIMol+Lsyy3oaXLq9ogtr8wGYUgFA0qvFL0QeBe
// SIG // MOOSKGmHwXDi86erzoBCcnYOMIIEejCCA2KgAwIBAgIK
// SIG // YQHPPgAAAAAADzANBgkqhkiG9w0BAQUFADB5MQswCQYD
// SIG // VQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4G
// SIG // A1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0
// SIG // IENvcnBvcmF0aW9uMSMwIQYDVQQDExpNaWNyb3NvZnQg
// SIG // Q29kZSBTaWduaW5nIFBDQTAeFw0wOTEyMDcyMjQwMjla
// SIG // Fw0xMTAzMDcyMjQwMjlaMIGDMQswCQYDVQQGEwJVUzET
// SIG // MBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVk
// SIG // bW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0
// SIG // aW9uMQ0wCwYDVQQLEwRNT1BSMR4wHAYDVQQDExVNaWNy
// SIG // b3NvZnQgQ29ycG9yYXRpb24wggEiMA0GCSqGSIb3DQEB
// SIG // AQUAA4IBDwAwggEKAoIBAQC9MIn7RXKoU2ueiU8AI8C+
// SIG // 1B09sVlAOPNzkYIm5pYSAFPZHIIOPM4du733Qo2X1Pw4
// SIG // GuS5+ePs02EDv6DT1nVNXEap7V7w0uJpWxpz6rMcjQTN
// SIG // KUSgZFkvHphdbserGDmCZcSnvKt1iBnqh5cUJrN/Jnak
// SIG // 1Dg5hOOzJtUY+Svp0skWWlQh8peNh4Yp/vRJLOaL+AQ/
// SIG // fc3NlpKGDXED4tD+DEI1/9e4P92ORQp99tdLrVvwdnId
// SIG // dyN9iTXEHF2yUANLR20Hp1WImAaApoGtVE7Ygdb6v0LA
// SIG // Mb5VDZnVU0kSMOvlpYh8XsR6WhSHCLQ3aaDrMiSMCOv5
// SIG // 1BS64PzN6qQVAgMBAAGjgfgwgfUwEwYDVR0lBAwwCgYI
// SIG // KwYBBQUHAwMwHQYDVR0OBBYEFDh4BXPIGzKbX5KGVa+J
// SIG // usaZsXSOMA4GA1UdDwEB/wQEAwIHgDAfBgNVHSMEGDAW
// SIG // gBTMHc52AHBbr/HaxE6aUUQuo0Rj8DBEBgNVHR8EPTA7
// SIG // MDmgN6A1hjNodHRwOi8vY3JsLm1pY3Jvc29mdC5jb20v
// SIG // cGtpL2NybC9wcm9kdWN0cy9DU1BDQS5jcmwwSAYIKwYB
// SIG // BQUHAQEEPDA6MDgGCCsGAQUFBzAChixodHRwOi8vd3d3
// SIG // Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRzL0NTUENBLmNy
// SIG // dDANBgkqhkiG9w0BAQUFAAOCAQEAKAODqxMN8f4Rb0J2
// SIG // 2EOruMZC+iRlNK51sHEwjpa2g/py5P7NN+c6cJhRIA66
// SIG // cbTJ9NXkiugocHPV7eHCe+7xVjRagILrENdyA+oSTuzd
// SIG // DYx7RE8MYXX9bpwH3c4rWhgNObBg/dr/BKoCo9j6jqO7
// SIG // vcFqVDsxX+QsbsvxTSoc8h52e4avxofWsSrtrMwOwOSf
// SIG // f+jP6IRyVIIYbirInpW0Gh7Bb5PbYqbBS2utye09kuOy
// SIG // L6t6dzlnagB7gp0DEN5jlUkmQt6VIsGHC9AUo1/cczJy
// SIG // Nh7/yCnFJFJPZkjJHR2pxSY5aVBOp+zCBmwuchvxIdpt
// SIG // JEiAgRVAfJ/MdDhKTzCCBJ0wggOFoAMCAQICEGoLmU/A
// SIG // ACWrEdtFH1h6Z6IwDQYJKoZIhvcNAQEFBQAwcDErMCkG
// SIG // A1UECxMiQ29weXJpZ2h0IChjKSAxOTk3IE1pY3Jvc29m
// SIG // dCBDb3JwLjEeMBwGA1UECxMVTWljcm9zb2Z0IENvcnBv
// SIG // cmF0aW9uMSEwHwYDVQQDExhNaWNyb3NvZnQgUm9vdCBB
// SIG // dXRob3JpdHkwHhcNMDYwOTE2MDEwNDQ3WhcNMTkwOTE1
// SIG // MDcwMDAwWjB5MQswCQYDVQQGEwJVUzETMBEGA1UECBMK
// SIG // V2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwG
// SIG // A1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSMwIQYD
// SIG // VQQDExpNaWNyb3NvZnQgVGltZXN0YW1waW5nIFBDQTCC
// SIG // ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANw3
// SIG // bvuvyEJKcRjIzkg+U8D6qxS6LDK7Ek9SyIPtPjPZSTGS
// SIG // KLaRZOAfUIS6wkvRfwX473W+i8eo1a5pcGZ4J2botrfv
// SIG // hbnN7qr9EqQLWSIpL89A2VYEG3a1bWRtSlTb3fHev5+D
// SIG // x4Dff0wCN5T1wJ4IVh5oR83ZwHZcL322JQS0VltqHGP/
// SIG // gHw87tUEJU05d3QHXcJc2IY3LHXJDuoeOQl8dv6dbG56
// SIG // 4Ow+j5eecQ5fKk8YYmAyntKDTisiXGhFi94vhBBQsvm1
// SIG // Go1s7iWbE/jLENeFDvSCdnM2xpV6osxgBuwFsIYzt/iU
// SIG // W4RBhFiFlG6wHyxIzG+cQ+Bq6H8mjmsCAwEAAaOCASgw
// SIG // ggEkMBMGA1UdJQQMMAoGCCsGAQUFBwMIMIGiBgNVHQEE
// SIG // gZowgZeAEFvQcO9pcp4jUX4Usk2O/8uhcjBwMSswKQYD
// SIG // VQQLEyJDb3B5cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0
// SIG // IENvcnAuMR4wHAYDVQQLExVNaWNyb3NvZnQgQ29ycG9y
// SIG // YXRpb24xITAfBgNVBAMTGE1pY3Jvc29mdCBSb290IEF1
// SIG // dGhvcml0eYIPAMEAizw8iBHRPvZj7N9AMBAGCSsGAQQB
// SIG // gjcVAQQDAgEAMB0GA1UdDgQWBBRv6E4/l7k0q0uGj7yc
// SIG // 6qw7QUPG0DAZBgkrBgEEAYI3FAIEDB4KAFMAdQBiAEMA
// SIG // QTALBgNVHQ8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAN
// SIG // BgkqhkiG9w0BAQUFAAOCAQEAlE0RMcJ8ULsRjqFhBwEO
// SIG // jHBFje9zVL0/CQUt/7hRU4Uc7TmRt6NWC96Mtjsb0fus
// SIG // p8m3sVEhG28IaX5rA6IiRu1stG18IrhG04TzjQ++B4o2
// SIG // wet+6XBdRZ+S0szO3Y7A4b8qzXzsya4y1Ye5y2PENtEY
// SIG // Ib923juasxtzniGI2LS0ElSM9JzCZUqaKCacYIoPO8cT
// SIG // ZXhIu8+tgzpPsGJY3jDp6Tkd44ny2jmB+RMhjGSAYwYE
// SIG // lvKaAkMve0aIuv8C2WX5St7aA3STswVuDMyd3ChhfEjx
// SIG // F5wRITgCHIesBsWWMrjlQMZTPb2pid7oZjeN9CKWnMyw
// SIG // d1RROtZyRLIj9jCCBKowggOSoAMCAQICCmEFojAAAAAA
// SIG // AAgwDQYJKoZIhvcNAQEFBQAweTELMAkGA1UEBhMCVVMx
// SIG // EzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1Jl
// SIG // ZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3Jh
// SIG // dGlvbjEjMCEGA1UEAxMaTWljcm9zb2Z0IFRpbWVzdGFt
// SIG // cGluZyBQQ0EwHhcNMDgwNzI1MTkwMTE1WhcNMTMwNzI1
// SIG // MTkxMTE1WjCBszELMAkGA1UEBhMCVVMxEzARBgNVBAgT
// SIG // Cldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAc
// SIG // BgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjENMAsG
// SIG // A1UECxMETU9QUjEnMCUGA1UECxMebkNpcGhlciBEU0Ug
// SIG // RVNOOjg1RDMtMzA1Qy01QkNGMSUwIwYDVQQDExxNaWNy
// SIG // b3NvZnQgVGltZS1TdGFtcCBTZXJ2aWNlMIIBIjANBgkq
// SIG // hkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA8AQtspbAGoFn
// SIG // JbEmYrMTS84wusASOPyBZTQHxDayJGj2BwTAB5f0t/F7
// SIG // HmIsRtlLpFE0t9Ns7Vo7tIOhRz0RCC41a0XmwjyMAmYC
// SIG // qRhp60rtJyzuPHdbpNRwmUtXhBDQry34iR3m6im058+e
// SIG // BmKnclTCO8bPP7jhsFgQbOWl18PCdTe99IXhgego2Bvx
// SIG // 8q7xgqPW1wOinxWE+z36q+G2MsigAmTz5v8aJnEIU4oV
// SIG // AvKDJ3ZJgnGn760yeMbXbBZPImWXYk1GL/8jr4XspnC9
// SIG // A8va2DIFxSuQQLae1SyGbLfLEzJ9jcZ+rhcvMvxmux2w
// SIG // RVX4rfotZ4NnKZOE0lqhIwIDAQABo4H4MIH1MB0GA1Ud
// SIG // DgQWBBTol/b374zx5mnjWWhO95iKet2bLjAfBgNVHSME
// SIG // GDAWgBRv6E4/l7k0q0uGj7yc6qw7QUPG0DBEBgNVHR8E
// SIG // PTA7MDmgN6A1hjNodHRwOi8vY3JsLm1pY3Jvc29mdC5j
// SIG // b20vcGtpL2NybC9wcm9kdWN0cy90c3BjYS5jcmwwSAYI
// SIG // KwYBBQUHAQEEPDA6MDgGCCsGAQUFBzAChixodHRwOi8v
// SIG // d3d3Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRzL3RzcGNh
// SIG // LmNydDATBgNVHSUEDDAKBggrBgEFBQcDCDAOBgNVHQ8B
// SIG // Af8EBAMCBsAwDQYJKoZIhvcNAQEFBQADggEBAA0/d1+R
// SIG // PL6lNaTbBQWEH1by75mmxwiNL7PNP3HVhnx3H93rF7K9
// SIG // fOP5mfIKRUitFLtpLPI+Z2JU8u5/JxGSOezO2YdOiPdg
// SIG // RyN7JxVACJ+/DTEEgtg1tgycANOLqnhhxbWIQZ0+NtxY
// SIG // pCebOtq9Bl0UprIPTMGOPIvyYpn4Zu3V8xwosDLbyjEJ
// SIG // vPsiaEZM+tNzIucpjiIA+1a/Bq6BoBW6NPkojh9KYgWh
// SIG // ifWBR+kNkQjXWDuPHmsJaanASHxVgj9fADhDnAbMP9gv
// SIG // v09zCT39ul70x+w3wmRhoE3UPXDMW7ATgcHUozEavWTW
// SIG // ltJ6PypbRlMJPM0D+T9ZAMyJU2ExggSFMIIEgQIBATCB
// SIG // hzB5MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGlu
// SIG // Z3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMV
// SIG // TWljcm9zb2Z0IENvcnBvcmF0aW9uMSMwIQYDVQQDExpN
// SIG // aWNyb3NvZnQgQ29kZSBTaWduaW5nIFBDQQIKYQHPPgAA
// SIG // AAAADzAJBgUrDgMCGgUAoIGwMBkGCSqGSIb3DQEJAzEM
// SIG // BgorBgEEAYI3AgEEMBwGCisGAQQBgjcCAQsxDjAMBgor
// SIG // BgEEAYI3AgEVMCMGCSqGSIb3DQEJBDEWBBRjJcnmSdh5
// SIG // jFoo50E1TAfRueyPpTBQBgorBgEEAYI3AgEMMUIwQKAm
// SIG // gCQAQwBsAGkAZQBuAHQAQgBlAGgAYQB2AGkAbwByADEA
// SIG // LgBqAHOhFoAUaHR0cDovL21pY3Jvc29mdC5jb20wDQYJ
// SIG // KoZIhvcNAQEBBQAEggEAuHW0NXHobiYvXn3hdnY9539x
// SIG // MoT6sEtAVY9ng57TkoQXaEwOxeNN8do0nl5xxW0jXPnq
// SIG // 0rPCALOojM2T6e0GIkAICq7mAvrH4ky2fmTeiBC2akm9
// SIG // qLlso1obi0LZgazz1HggYzdhZ9/Uc1mWEut8L9dN6FFA
// SIG // KZhHWJ83d7pALaYN8UWzWY6VlNTvdiADlCZ8cFJo3yGq
// SIG // 4/vRiHpWgUMCLVuUr4O5i+Tp/bq9xLBBGINVirTife6y
// SIG // P1PZ9PoSBKD4qfE0KbU+cSGhK1Vl8K7v8qPDurpoR2vm
// SIG // mk3KpZcAr+cqUSM4/S/mV+kWZVXwXnypF7wSmKymdNZA
// SIG // 8XwFvjqr5KGCAh8wggIbBgkqhkiG9w0BCQYxggIMMIIC
// SIG // CAIBATCBhzB5MQswCQYDVQQGEwJVUzETMBEGA1UECBMK
// SIG // V2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwG
// SIG // A1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSMwIQYD
// SIG // VQQDExpNaWNyb3NvZnQgVGltZXN0YW1waW5nIFBDQQIK
// SIG // YQWiMAAAAAAACDAHBgUrDgMCGqBdMBgGCSqGSIb3DQEJ
// SIG // AzELBgkqhkiG9w0BBwEwHAYJKoZIhvcNAQkFMQ8XDTEw
// SIG // MDMxOTAzNDA1MFowIwYJKoZIhvcNAQkEMRYEFNFTdseC
// SIG // K66zDGmf0ebqH1Giz0W9MA0GCSqGSIb3DQEBBQUABIIB
// SIG // AMdSAf84fde5dAUcgcGSio6dIyBRlOxroO5qM/dbaeQy
// SIG // qSNArHyGHQH/GWbip+zuLzitG90BI+pSFLmH7mCzvOr0
// SIG // +XMf3BkIIuTvNePml2FrTwOhClg+l1q9GGq9+8qamTp5
// SIG // s8P4owcbBSQz3ydBkvW+8h/HFDFz176RxR+22SK8COZy
// SIG // NWxqMzFvv+2JxE6GMCuHTdtEFBhUkohvUDphwcJkL9rD
// SIG // OpHbf4c33u+69mATMN7INiLJ1cHlRC1Ww/k4wNxb2KGl
// SIG // /3UYyvhHjQZCT8/YSVEfHsA0uYs2VCteWcYmNjZWJxU+
// SIG // uQzub+IiNtIpXlp8OiiCinfFsFcavwVsJgQ=
// SIG // End signature block
