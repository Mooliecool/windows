// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==

namespace Microsoft.JScript {

    using System;
    using System.Text.RegularExpressions;
    using System.Globalization;
    
    public sealed class RegExpMatch : ArrayObject{
      private bool hydrated;
      private Regex regex;
      private MatchCollection matches;
      private Match match;

      internal RegExpMatch(ArrayPrototype parent, Regex regex, Match match, String input)
        : base(parent, typeof(RegExpMatch)){
        this.hydrated = false;
        this.regex = regex;
        this.matches = null;
        this.match = match;
        base.SetMemberValue("input", input);
        base.SetMemberValue("index", match.Index);
        base.SetMemberValue("lastIndex", match.Length == 0 ? match.Index + 1 : match.Index + match.Length);
        String[] groupNames = regex.GetGroupNames();
        int maxGroupNumber = 0;
        for (int i = 1; i < groupNames.Length; i++) {
          String groupName = groupNames[i];
          int groupNumber = regex.GroupNumberFromName(groupName);
          if (groupName.Equals(groupNumber.ToString(CultureInfo.InvariantCulture))){
            if (groupNumber > maxGroupNumber)
              maxGroupNumber = groupNumber;
          }else{
            Group group = match.Groups[groupName];
            base.SetMemberValue(groupName, group.Success ? group.ToString() : null);
          }
        }
        this.length = maxGroupNumber+1;
      }

      internal RegExpMatch(ArrayPrototype parent, Regex regex, MatchCollection matches, String input)
        : base(parent){
        this.hydrated = false;
        this.length = matches.Count;
        this.regex = regex;
        this.matches = matches;
        this.match = null;
        Match match = matches[matches.Count - 1];
        base.SetMemberValue("input", input);
        base.SetMemberValue("index", match.Index);
        base.SetMemberValue("lastIndex", match.Length == 0 ? match.Index + 1 : match.Index + match.Length);
      }

      internal override void Concat(ArrayObject source){
        if (!this.hydrated)
          this.Hydrate();
        base.Concat(source);
      }

      internal override void Concat(Object value){
        if (!this.hydrated)
          this.Hydrate();
        base.Concat(value);
      }

      internal override bool DeleteValueAtIndex(uint index){
        if (!this.hydrated)
          this.Hydrate();
        return base.DeleteValueAtIndex(index);
      }

      internal override Object GetValueAtIndex(uint index){
        if (!this.hydrated) {
          if (this.matches != null) {
            if (index < (long)this.matches.Count)
              return this.matches[(int)index].ToString();
          }else if (this.match != null) {
            int groupNumber = this.regex.GroupNumberFromName(index.ToString(CultureInfo.InvariantCulture));
            if (groupNumber >= 0) {
              Group group = this.match.Groups[groupNumber];
              return group.Success ? group.ToString() :
                "";
            }
          }
        }
        return base.GetValueAtIndex(index);
      }

      internal override Object GetMemberValue(String name){
        if (!this.hydrated) {
          long index = ArrayObject.Array_index_for(name);
          if (index >= 0)
            return this.GetValueAtIndex((uint)index);
        }
        return base.GetMemberValue(name);
      }

      private void Hydrate(){
        if (this.matches != null)
          for (int i = 0, n = this.matches.Count; i < n; i++)
            base.SetValueAtIndex((uint)i, this.matches[i].ToString());
        else if (this.match != null) {
          String[] groupNames = this.regex.GetGroupNames();
          for (int i = 1, n = groupNames.Length; i < n; i++) {
            String groupName = groupNames[i];
            int groupNumber = this.regex.GroupNumberFromName(groupName);
            Group group = this.match.Groups[groupNumber];
            Object propertyValue = group.Success ? group.ToString() :
              "";
            if (groupName.Equals(groupNumber.ToString(CultureInfo.InvariantCulture)))
              base.SetValueAtIndex((uint)groupNumber, propertyValue);
          }
        }
        this.hydrated = true;
        this.regex = null;
        this.matches = null;
        this.match = null;
      }

      internal override void SetValueAtIndex(uint index, Object value){
        if (!this.hydrated)
          this.Hydrate();
        base.SetValueAtIndex(index, value);
      }

      internal override Object Shift(){
        if (!this.hydrated)
          this.Hydrate();
        return base.Shift();
      }

      internal override void Sort(ScriptFunction compareFn){
        if (!this.hydrated)
          this.Hydrate();
        base.Sort(compareFn);
      }

      internal override void Splice(uint start, uint deleteItems, Object[] args, ArrayObject outArray, uint oldLength, uint newLength){
        if (!this.hydrated)
          this.Hydrate();
        base.Splice(start, deleteItems, args, outArray, oldLength, newLength);
      }

      internal override void SwapValues(uint pi, uint qi){
        if (!this.hydrated)
          this.Hydrate();
        base.SwapValues(pi, qi);
      }

      internal override ArrayObject Unshift(Object[] args){
        if (!this.hydrated)
          this.Hydrate();
        return base.Unshift(args);
      }
    }
}
