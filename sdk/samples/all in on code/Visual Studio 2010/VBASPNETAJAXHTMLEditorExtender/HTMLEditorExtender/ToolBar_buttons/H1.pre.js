Type.registerNamespace("Sys.Extended.UI.HTMLEditor.ToolbarButton");

Sys.Extended.UI.HTMLEditor.ToolbarButton.H1 = function (element) {
    Sys.Extended.UI.HTMLEditor.ToolbarButton.H1.initializeBase(this, [element]);
}

Sys.Extended.UI.HTMLEditor.ToolbarButton.H1.prototype = {
    callMethod: function () {

        var editor = this._designPanel;
        var sel = editor._getSelection();
        var range = editor._createRange(sel);
        var parent = Sys.Extended.UI.HTMLEditor.getSelParent(editor);

        var state = this._get_state();
        if (state) {
            if (parent.tagName.toLowerCase() != "h1") {
                parent = parent.parentNode;
            }
            // remove the H1 tag.
            if (Sys.Extended.UI.HTMLEditor.isIE) {
                parent.outerHTML = parent.innerHTML;
            } else {
                var r = sel.getRangeAt(0);
                r.setStartBefore(parent);
                var df = r.createContextualFragment(parent.innerHTML);
                parent.parentNode.replaceChild(df, parent);
            }
            return true;
        } else {
            // add the H1 tag.
            if (Sys.Extended.UI.HTMLEditor.isIE) {
                var original = range.htmlText;
                if (original != "")
                    range.pasteHTML("<h1>" + original + "</h1>");
            }
            else {
                var edittext = sel.getRangeAt(0);
                if (edittext.toString() != "") {
                    var h = document.createElement("H1");
                    edittext.surroundContents(h);
                }
            }

            return false;
        }

        if (!Sys.Extended.UI.HTMLEditor.ToolbarButton.H1.callBaseMethod(this, "callMethod")) return false;
    },

    checkState: function () {
        if (!Sys.Extended.UI.HTMLEditor.ToolbarButton.H1.callBaseMethod(this, "checkState")) return false;
        return this._get_state();
    },

    _get_state: function () {
        var editor = this._designPanel;
        var sel = editor._getSelection();
        var range = editor._createRange(sel);
        var parent = Sys.Extended.UI.HTMLEditor.getSelParent(editor);

        if (parent.nodeType == 3) {
            parent = parent.parentNode;
        }

        while (parent && Sys.Extended.UI.HTMLEditor.isStyleTag(parent.tagName) && parent.tagName.toUpperCase() != "H1") {
            parent = parent.parentNode;
        }

        if (parent && parent.tagName.toUpperCase() == "H1") {
            return true;
        } else {
            return false;
        }
    }

}

Sys.Extended.UI.HTMLEditor.ToolbarButton.H1.registerClass("Sys.Extended.UI.HTMLEditor.ToolbarButton.H1", Sys.Extended.UI.HTMLEditor.ToolbarButton.EditorToggleButton);

