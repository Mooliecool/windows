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
/* ------------------------------------------------------------------------- *
 * debug\shell.h: generic shell class
 * ------------------------------------------------------------------------- */

#ifndef SHELL_H_
#define SHELL_H_

#include <string.h>
#include <specstrings.h>

//---------------------------------------------------------------------------
// Simple debugger string class. This is basically a trivial holder
// for the memory used by a string.
// Each instance has its own copy of the string (we don't try to be smart about
// sharing), which is internally stored as unicode.
// When assigning from ansi or retrieving as ansi, this encapsulates the
// conversion routines.
//---------------------------------------------------------------------------
class DebuggerString
{
private:
    // Forbid copy-ctor and assignment operator
    DebuggerString(const DebuggerString & );
    void operator=(const DebuggerString & );

public:
    DebuggerString() { m_szData = NULL; }
    ~DebuggerString() { Clear(); }

    void Clear()
    {
        delete [] m_szData;
        m_szData = NULL;
    }

    // Return true if successfully copied, false on failure (eg, oom).
    bool CopyFrom(const WCHAR * szData)
    {
        if (szData == NULL)
        {
            Clear();
            return true;
        }
        if (!Allocate(wcslen (szData)))
        {
            return false;
        }

        wcscpy(m_szData, szData);
        return true;
    }

    // Copy len chars (not including the null).
    // Useful in extracting a substring.
    bool CopyNFrom(const WCHAR * szData, int len)
    {
        if (szData == NULL)
        {
            Clear();
            return true;
        }

        if (!Allocate(len))
        {
            return false;
        }

        wcsncpy(m_szData, szData, len);
        m_szData[len] = 0;
        return true;

    }

    // pre-allocate length, not including the null.
    // Return true if successfully allocated, false on failure (eg, oom).
    bool Allocate(size_t len)
    {
        _ASSERTE(len >= 0);

        Clear();
        m_szData = new WCHAR [len + 1];
        if (m_szData == NULL)
        {
            return false;
        }
        m_szData[len] = 0;
        return true;
    }

    // Need to be able to retrieve as CHAR array.
    // This will convert to Ascii and copy to szBuffer.
    // Returns true on success, false on failure (eg, buffer is too small)
    bool GetAsChar(__inout_ecount(cChars) char * szBuffer, SIZE_T cChars);

    // Initialize from an ansi string. We'll convert it to unicode and then store it.
    bool CopyFromAscii(__in_z char * szBuffer);

    // This gets an L-value, meaning we can read & write the data.
    WCHAR & operator [] (int idx)
    {
        _ASSERTE(m_szData != NULL);
        _ASSERTE(idx >= 0);

        return m_szData[idx];
    }

    // Get the length, not including the null terminator.
    SIZE_T Length() const
    {
        return wcslen(m_szData);
    }

    bool IsEmpty() const
    {
        return (m_szData == NULL) || (Length() == 0);
    }

    // GetData() gets a writable version of the data.
    // The implicit operator WCHAR* gets a read-only version of the data.
    WCHAR * GetData() { return m_szData; }
    operator const WCHAR * () const { return m_szData; }

private:
    // This instances owns the memory. It isn't shared.
    WCHAR * m_szData;
};


/* ------------------------------------------------------------------------- *
 * Class forward declations
 * ------------------------------------------------------------------------- */

class Shell;
class ShellCommand;
class ShellCommandTable;
class HelpShellCommand;

/* ------------------------------------------------------------------------- *
 * Abstract ShellCommand class
 *
 * All commands that the shell will support must be derived from this class.
 * ------------------------------------------------------------------------- */

class ShellCommand
{
protected:

    const WCHAR *m_pName;       // full command name
    WCHAR m_pShortcutName[64];  // shortcut syntax

    // The minimum subset of name that must be typed in
    int m_minMatchLength;

    // does this command have a shortcut?
    BOOL m_bHasShortcut;

public:
    ShellCommand(const WCHAR *name, int MatchLength = 0);

    virtual ~ShellCommand()
    {
    }

    /*********************************************************************
     * Methods
     */

    /*
     * Executes the shell command
     */
    virtual void Do(Shell *shell, const WCHAR *args) = 0;

    /*
     * Displays a help message to the user
     */
    virtual void Help(Shell *shell);

    /*
     * Returns a short help message for the user
     */
    virtual const WCHAR *ShortHelp(Shell *shell)
    {
        // Name is a good default.
        return m_pName;
    }

    /*
     * Returns the name of the command
     */
    const WCHAR *GetName()
    {
        return m_pName;
    }

    /*
     * Returns the shortcut name of the command
     */
    const WCHAR *GetShortcutName()
    {
        return m_pShortcutName;
    }

    /*
     * Returns the minimum match length
     */
    int GetMinMatchLength()
    {
        return m_minMatchLength;
    }

    /*
     * Returns whether the name has a shortcut
     */
    BOOL HasShortcut()
    {
        return m_bHasShortcut;
    }

};

/* ------------------------------------------------------------------------- *
 * Abstract Shell class
 *
 * A basic outline of a command shell, used by the debugger.
 * ------------------------------------------------------------------------- */

const int BUFFER_SIZE = 1024;

class Shell
{
private:
    // The collection of the available commands
    ShellCommandTable *m_pCommands;

    // The last command executed
    DebuggerString m_lastCommand;

    // A buffer for reading input
    WCHAR m_buffer[BUFFER_SIZE];

protected:
    // The input prompt
    WCHAR *m_pPrompt;

public:
    Shell();
    ~Shell();

    /*********************************************************************
     * Shell I/O routines
     */

    /*
     * Read a line of input from the user, getting a maximum of maxCount chars
     */
    virtual bool ReadLine(WCHAR *buffer, int maxCount) = 0;

    /*
     * Write a line of output to the shell
     */
    virtual HRESULT Write(const WCHAR *buffer, ...) = 0;

    /*
     * Write an error to the shell
     */
    virtual void Error(const WCHAR *buffer, ...) = 0;

    void ReportError(long res);
    void SystemError();

    /*********************************************************************
     * Shell functionality
     */

    /*
     * This will add a command to the collection of available commands
     */
    void AddCommand(ShellCommand *command);

    /*
     * This will get a command from the available commands by matching the
     * command name with string.
     */
    ShellCommand *GetCommand(const WCHAR *string, size_t length);

    /*
     * This will get a command from the available commands by matching the
     * command name with string.
     */
    void PutCommand(FILE *f);

    /*
     * This will read a command from the user
     */
    void ReadCommand();

    /*
     * This will attempt to match string with a command and execute it with
     * the arguments following the command string.
     */
    void DoCommand(const WCHAR *string);

    /*
     * This will call DoCommand once for each thread in the process.
     */
    virtual void DoCommandForAllThreads(const WCHAR *string) = 0;

    /*
     * This will provide a listing of the commands available to the shell.
     */
    void Help();

    // utility methods:
    bool GetStringArg(__in_z __deref_inout WCHAR * &string, __deref_inout WCHAR * &result);
    bool GetStringArg(const WCHAR * &string, __deref_inout WCHAR * &result);
    bool GetStringArg(__in_z __deref_inout WCHAR * &string, const WCHAR * &result);
    bool GetStringArg(const WCHAR * &string, const WCHAR * &result);
    bool GetIntArg(const WCHAR * &string, int &result);
    bool GetInt64Arg(const WCHAR * &string, unsigned __int64 &result);

    size_t GetArgArray(__in_z WCHAR *argString, const WCHAR **argArray, size_t argMax);

    const WCHAR *GetPrompt()
    {
        return m_pPrompt;
    }

    void SetPrompt(const WCHAR *prompt)
    {
        m_pPrompt = new WCHAR[wcslen(prompt)];
        wcscpy(m_pPrompt, prompt);
    }
};

/* ------------------------------------------------------------------------- *
 * Predefined command classes
 * ------------------------------------------------------------------------- */

class HelpShellCommand : public ShellCommand
{
public:
    HelpShellCommand(const WCHAR *name, int minMatchLength = 0)
    : ShellCommand(name, minMatchLength)
    {

    }

    /*
     * This will display help for the command given in args, or help on the
     * help command if args is empty.
     */
    void Do(Shell *shell, const WCHAR *args);

    /*
     * This will provide help for the help command.
     */
    void Help(Shell *shell);

    const WCHAR *ShortHelp(Shell *shell);
};


/* ------------------------------------------------------------------------- *
 * Allows comments in the cordbg session
 * ------------------------------------------------------------------------- */
class CommentShellCommand : public ShellCommand
{
public:
    CommentShellCommand(const WCHAR *name, int minMatchLength = 0)
    : ShellCommand(name, minMatchLength)
    {

    }

    /*
     * This will display help for the command given in args, or help on the
     * help command if args is empty.
     */
    void Do(Shell *shell, const WCHAR *args)
    {
        return;
    }

    /*
     * This will provide help for the help command.
     */
    void Help(Shell *shell)
    {
        shell->Write(L"# Comment\n");
        shell->Write(L"Allows comments in the cordbg session\n");

    }

    const WCHAR *ShortHelp(Shell *shell)
    {
        return L"Allows comments in the cordbg session";
    }

};

#endif /* SHELL_H_ */
