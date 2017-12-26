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
//  RULE.C -- routines that have to do with inference rules
//
// Purpose:
//  Routines that have to do with inference rules

#include "precomp.h"
#ifdef _MSC_VER
#pragma hdrstop
#endif

#define PUBLIC

extern char * QueryFileInfo(char *, void **);

BOOL   removeDuplicateRules(RULELIST*, RULELIST*);
char * skipPathList(char*);

//
//
//
//
//
//
//
//

RULELIST *
findRule(
    char *name,
    char *target,
    char *ext,
    void *dBuf
    )
{
    RULELIST *r;                    // pointer to rule
    char *s,                        // name of rule
     *ptrToExt;                     // extension
    char *endPath, *ptrToTarg, *ptrToName, *temp;
    int n, m;
    MAKEOBJECT *object = NULL;

    for (r = rules; r; r = r->next) {
        s = r->name;
#ifdef DEBUG_ALL
        printf("* findRule: %s,\n", r->name);
        DumpList(r->buildCommands);
        DumpList(r->buildMacros);
#endif
        ptrToExt = _tcsrchr(s, '.');
        // Compare ignoring enclosing quotes
        if (!strcmpiquote(ptrToExt, ext)) {
            *name = '\0';
            for (ptrToTarg = (s+1); *ptrToTarg && *ptrToTarg != '{';ptrToTarg = _tcsinc(ptrToTarg))
                if (*ptrToTarg == ESCH)
                    ptrToTarg++;
                // If Quotes present skip to end-quote
                else if (*ptrToTarg == '"')
                    for (ptrToTarg++; *ptrToTarg != '"'; ptrToTarg++)
                        ;

            if (*ptrToTarg) {
                for (endPath = ptrToTarg; *endPath && *endPath != '}';endPath = _tcsinc(endPath))
                    if (*endPath == ESCH)
                        endPath++;
                n = (int) (endPath - (ptrToTarg + 1));

                // ignore leading quote on target
                temp = target;
                if (*temp == '"')
                    temp++;

                for (ptrToExt = ptrToTarg+1; n; n -= (int) _tclen(ptrToExt),
                    ptrToExt = _tcsinc(ptrToExt),
                    temp = _tcsinc(temp)) { // compare paths
                    if (IsPathSeparator(*ptrToExt)) {
                        if (!IsPathSeparator(*temp)) {
                            n = -1;
                            break;
                        }
                    } else if (_tcsnicmp(ptrToExt, temp, _tclen(ptrToExt))) {
                        n = -1;
                        break;
                    }
                }

                if (n == -1)
                    continue;           // match failed; do next rule
                ptrToExt = ptrToTarg;
                n = (int) (endPath - (ptrToTarg + 1));

                char *pchLast = _tcsdec(ptrToTarg, endPath);

                ptrToName = target + n + 1;                 // if more path
                if ((temp = FindFirstPathSeparator(ptrToName)) // left in target (we let separator in
                    && (temp != ptrToName                   // target path in rule,
                    || IsPathSeparator(*pchLast)))          // e.g. .c.{\x}.obj same as .c.{\x\}.obj)
                    continue;                               // use dependent's path,
            }                                               // not target's

            if (*s == '{') {
                for (endPath = ++s; *endPath && *endPath != '}'; endPath = _tcsinc (endPath))
                    if (*endPath == ESCH)
                        endPath++;
                n = (int) (endPath - s);

                if (n) {
                    _tcsncpy(name, s, n);
                    s += n + 1;                 // +1 to go past '}'
                    if (!IsPathSeparator(*(s-2)))
                        *(name+n++) = PATH_SEPARATOR_CHAR;
                } else {
                    if (*target == '"')
                        _tcsncpy(name, "\"." PATH_SEPARATOR, n = 3);
                    else
                        _tcsncpy(name, "." PATH_SEPARATOR, n = 2);
                    s += 1;
                }

                ptrToName = FindLastPathSeparator(target);

                if (ptrToName != NULL) {
                    _tcscpy(name+n, ptrToName+1);
                    n += (int) (ext - (ptrToName + 1));
                } else {
                    char *szTargNoQuote = *target == '"' ? target + 1 : target;
                    _tcscpy(name+n, szTargNoQuote);
                    n += (int) (ext - szTargNoQuote);
                }
            } else {
                char *t;

                //if rule has path for target then strip off path part
                if (*ptrToTarg) {

                    t = _tcsrchr(target, '.');

                    while (*t != ':' && !IsPathSeparator(*t) && t > target)
                        t = _tcsdec(target, t);
                    if (*t == ':' || IsPathSeparator(*t))
                        t++;
                } else
                    t = target;
                n = (int) (ext - t);

                // preserve the opening quote on target if stripped off path part
                m = 0;
                if ((t != target) && (*target == '"')) {
                    *name = '"';
                    m = 1;
                }
                _tcsncpy(name + m, t, n);
                n += m;
            }

            m = (int) (ptrToExt - s);
            if (n + m > MAXNAME) {
                makeError(0, NAME_TOO_LONG);
            }

            _tcsncpy(name+n, s, m);    // need to be less
            // If quoted add a quote at the end too
            if (*name == '"' && *(name+n+m-1) != '"') {
                *(name+n+m) = '"';
                m++;
            }
            *(name+n+m) = '\0';         // cryptic w/ error

            // Call QueryFileInfo() instead of DosFindFirst() because we need
            // to circumvent the non-FAPI nature of DosFindFirst()

            if ((object = findTarget(name)) || QueryFileInfo(name, (void **)dBuf)) {
                if (object) {
                    putDateTime((_finddata_t*)dBuf, object->dateTime);
                }

                return(r);
            }
        }
    }

    return(NULL);
}


//  freeRules -- free inference rules
//
// Scope:   Global
//
// Purpose: This function clears the list of inference rules presented to it.
//
// Input:
//  r     -- The list of rules to be freed.
//  fWarn -- Warn if rules is not in .SUFFIXES
//
// Assumes:
//  That the list presented to it is a list of rules which are not needed anymore
//
// Uses Globals:
//  gFlags -- The global actions flag, to find if -p option is specified

void
freeRules(
    RULELIST *r,
    BOOL fWarn
    )
{
    RULELIST *q;

    while ((q = r)) {
        if (fWarn && ON(gFlags, F1_PRINT_INFORMATION))  // if -p option specified
            makeError(0, IGNORING_RULE, r->name);
        FREE(r->name);                  // free name of rule
        freeStringList(r->buildCommands);   // free command list
        freeStringList(r->buildMacros); // free command macros Note: free a Macro List
        r = r->next;
        FREE(q);                        // free rule
    }
}


BOOL
removeDuplicateRules(
    RULELIST *newRule,
    RULELIST *rules
    )
{
    RULELIST *r;
    STRINGLIST *p;

    for (r = rules; r; r = r->next) {
        if (!_tcsicmp(r->name, newRule->name)) {
            FREE(newRule->name);
            while ((p = newRule->buildCommands)) {
                newRule->buildCommands = p->next;
                FREE(p->text);
                FREE_STRINGLIST(p);
            }
            FREE(newRule);
            return(TRUE);
        }
    }
    return(FALSE);
}


//  skipPathList -- skip any path list in string
//
// Scope:   Local
//
// Purpose:
//  This function skips past any path list in an inference rule. A rule can have
//  optionally a path list enclosed in {} before the extensions. skipPathList()
//  checks if any path list is present and if found skips past it.
//
// Input:   s -- rule under consideration
//
// Output:  Returns pointer to the extension past the path list
//
// Assumes: That the inference rule is syntactically correct & its syntax
//
// Notes:   The syntax of a rule is -- {toPathList}.to{fromPathList}.from

char *
skipPathList(
    char *s
    )
{
    if (*s == '{') {
        while (*s != '}') {
            if (*s == ESCH)
                s++;
            s = _tcsinc(s);
        }
        s = _tcsinc(s);
    }
    return(s);
}


//  sortRules -- sorts the list of inference rules on .SUFFIXES order
//
// Scope:   Global
//
// Purpose:
//  This function sorts the inference rules list into an order depending on the
//  order in which the suffixes are listed in '.SUFFIXES'. The inference rules
//  which have their '.toext' part listed earlier in .SUFFIXES are reordered to
//  be earlier in the inference rules list. The inference rules for suffixes that
//  are not in .SUFFIXES are detected here and are ignored.
//
// Modifies Globals:
//  rules -- the list of rules which gets sorted
//
// Uses Globals:
//  dotSuffixList -- the list of valid suffixes for implicit inference rules.
//
// Notes:
//  The syntax of a rule is -- '{toPath}.toExt{fromPath}.fromExt'. This function
//  sorts the rule list into an order. Suffixes are (as of 1.10.016) checked in a
//  case insensitive manner.

PUBLIC void
sortRules(
    void
    )
{
    STRINGLIST *p,                      // suffix under consideration
               *s,
               *macros = NULL;
    RULELIST *oldRules,                 // inference rule list before sort
             *newRules,
             *r;                        // rule under consideration in oldRules
    char *suff, *toExt;
    size_t n;

    oldRules = rules;
    rules = NULL;
    for (p = dotSuffixList; p; p = p->next) {
        n = _tcslen(suff = p->text);
        for (r = oldRules; r;) {
            toExt = skipPathList(r->name);
            if (!_tcsnicmp(suff, toExt, n) &&
                (*(toExt+n) == '.' || *(toExt+n) == '{')
               ) {
                newRules = r;
                if (r->back)
                    r->back->next = r->next;
                else
                    oldRules = r->next;
                if (r->next)
                    r->next->back = r->back;
                r = r->next;
                newRules->next = NULL;
                if (!removeDuplicateRules(newRules, rules)) {
                    for (s = newRules->buildCommands; s; s = s->next) {
                        findMacroValuesInRule(newRules, s->text, &macros);
                    }
                    newRules->buildMacros = macros;
                    macros = NULL;
                    appendItem((STRINGLIST**)&rules, (STRINGLIST*)newRules);
                }
            } else
                r = r->next;
        }
    }
    // forget about rules whose suffixes not in .SUFFIXES
    if (oldRules)
        freeRules(oldRules, TRUE);
}


//  useRule -- applies inference rules for a target (if possible)
//
// Scope:   Local.
//
// Purpose:
//  When no explicit commands are available for a target NMAKE tries to use the
//  available inference rules. useRule() checks if an applicable inference rule
//  is present. If such a rule is found then it attempts a build using this rule
//  and if no applicable rule is present it conveys this to the caller.
//
// Input:
//  object     - object under consideration
//  name       - name of target
//  targetTime - time of target
//  qList      - QuestionList for target
//  sList      - StarStarList for target
//  status     - is dependent available
//  maxDepTime - maximum time of dependent
//  pFirstDep  - first dependent
//
// Output:
//  Returns ... applicable rule

RULELIST *
useRule(
    MAKEOBJECT *object,
    char *name,
    time_t targetTime,
    STRINGLIST **qList,
    STRINGLIST **sList,
    int *status,
    time_t *maxDepTime,
    char **pFirstDep
    )
{
    struct _finddata_t finddata;
    STRINGLIST *temp;
    RULELIST *r;
    time_t tempTime;
    char *t;


    if (!(t = _tcsrchr(object->name, '.')) ||
         (!(r = findRule(name, object->name, t, &finddata)))
       ) {
        return(NULL);                   // there is NO rule applicable
    }
    tempTime = getDateTime(&finddata);
    *pFirstDep = name;
    for (temp = *sList; temp; temp = temp->next) {
        if (!_tcsicmp(temp->text, name)) {
            break;
        }
    }

    if (temp) {
        CLEAR(object->flags2, F2_DISPLAY_FILE_DATES);
    }

    *status += invokeBuild(name, object->flags2, &tempTime, NULL);
   if (ON(object->flags2, F2_FORCE_BUILD) ||
        targetTime < tempTime ||
        (fRebuildOnTie && (targetTime == tempTime))
       ) {
        if (!temp) {
            temp = makeNewStrListElement();
            temp->text = makeString(name);
            appendItem(qList, temp);
            if (!*sList) {              // if this is the only dep found for
                *sList = *qList;        //  the target, $** list is updated
            }
        }

        if (ON(object->flags2, F2_DISPLAY_FILE_DATES) &&
            OFF(object->flags2, F2_FORCE_BUILD)
           ) {
            makeMessage(UPDATE_INFO, name, object->name);
        }
    }

    *maxDepTime = __max(*maxDepTime, tempTime);

    return(r);
}
