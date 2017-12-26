//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    The Credential class models a single user credential.
//
//*****************************************************************************
#include "stdafx.h"
#include "Credential.h"
#include "AutoCredentialBuffer.h"
#include "CredentialNotFoundException.h"
#include "CredentialSetNotAvailableException.h"
#include "AutoPasswordBstr.h"
#include "UserNameInvalidException.h"
#include "DuplicateTargetNameException.h"
#include "Strings.h"

//*****************************************************************************
//
//  Initializes a new instance of the Credential class as a deep copy of the 
//  specified Credential object. The new Credential object has its own copy
//  of the password.
//
//*****************************************************************************
Kerr::Credential::Credential(Credential^ copy) :
    m_disposed(true)
{
    if (nullptr == copy)
    {
        throw gcnew ArgumentNullException("copy");
    }

    Initialize(copy->m_targetName,
               copy->m_type,
               copy->m_userName,
               copy->m_password->Copy(),
               copy->m_secretLength,
               copy->m_persistence,
               copy->m_description,
               copy->m_lastWritten);

    m_disposed = false;
}

//*****************************************************************************
//
//  Initializes a new instance of the Credential class.
//
//*****************************************************************************
Kerr::Credential::Credential(String^ targetName,
                             CredentialType type) :
    m_disposed(true)
{
    if (nullptr == targetName)
    {
        throw gcnew ArgumentNullException("targetName");
    }

    msclr::auto_handle<Security::SecureString> password(gcnew Security::SecureString);

    Initialize(targetName,
               type,
               String::Empty,
               password.get(),
               0, // secret length
               CredentialPersistence::Session,
               String::Empty,
               DateTime::MinValue);

    password.release();

    m_disposed = false;
}

//*****************************************************************************
//
//  Initializes a new instance of the Credential class. The Credential class
//  takes ownership of the password object.
//
//*****************************************************************************
Kerr::Credential::Credential(String^ targetName,
                             CredentialType type,
                             String^ userName,
                             Security::SecureString^ password,
                             CredentialPersistence persistence,
                             String^ description) :
    m_disposed(true)
{
    if (nullptr == targetName)
    {
        throw gcnew ArgumentNullException("targetName");
    }

    if (nullptr == userName)
    {
        throw gcnew ArgumentNullException("userName");
    }

    if (nullptr == password)
    {
        throw gcnew ArgumentNullException("password");
    }

    if (nullptr == description)
    {
        throw gcnew ArgumentNullException("description");
    }

    Initialize(targetName,
               type,
               userName,
               password,
               password->Length * sizeof (wchar_t), // in bytes
               persistence,
               description,
               DateTime::MinValue);

    m_disposed = false;
}

//*****************************************************************************
//
//  Deletes the password stored in the Credential object.
//
//*****************************************************************************
Kerr::Credential::~Credential()
{
    if (!m_disposed)
    {
        delete m_password;
        m_password = nullptr;

        m_disposed = true;
    }

    Debug::Assert(nullptr == m_password);
}

//*****************************************************************************
//
//  Gets the target name associated with the credential.
//
//*****************************************************************************
String^ Kerr::Credential::TargetName::get()
{
    CheckNotDisposed();
    return m_targetName;
}

//*****************************************************************************
//
//  Gets the type of the credential.
//
//*****************************************************************************
Kerr::CredentialType Kerr::Credential::Type::get()
{
    CheckNotDisposed();
    return m_type;
}

//*****************************************************************************
//
//  Gets or sets the user name associated with the credential.
//
//*****************************************************************************
String^ Kerr::Credential::UserName::get()
{
    CheckNotDisposed();
    return m_userName;
}
void Kerr::Credential::UserName::set(String^ value)
{
    CheckNotDisposed();

    if (nullptr == value)
    {
        throw gcnew ArgumentNullException("value");
    }

    m_userName = value;
}

//*****************************************************************************
//
//  Gets or sets the password associated with the credential. The Credential
//  object assumes ownership of the password and will delete it when the 
//  Credential object is destroyed.
//
//*****************************************************************************
Security::SecureString^ Kerr::Credential::Password::get()
{
    CheckNotDisposed();
    return m_password;
}
void Kerr::Credential::Password::set(Security::SecureString^ value)
{
    CheckNotDisposed();

    if (nullptr == value)
    {
        throw gcnew ArgumentNullException("value");
    }

    Debug::Assert(nullptr != m_password);

    if (m_password != value)
    {
        delete m_password;
        m_password = value;

        m_secretLength = m_password->Length * sizeof (wchar_t);
    }
}

//*****************************************************************************
//
//  Gets or sets the persistence defined for the credential.
//
//*****************************************************************************
Kerr::CredentialPersistence Kerr::Credential::Persistence::get()
{
    CheckNotDisposed();
    return m_persistence;
}
void Kerr::Credential::Persistence::set(CredentialPersistence value)
{
    CheckNotDisposed();
    m_persistence = value;
}

//*****************************************************************************
//
//  Gets or sets a string describing the credential.
//
//*****************************************************************************
String^ Kerr::Credential::Description::get()
{
    CheckNotDisposed();
    return m_description;
}
void Kerr::Credential::Description::set(String^ value)
{
    CheckNotDisposed();

    if (nullptr == value)
    {
        throw gcnew ArgumentNullException("value");
    }

    m_description = value;
}

//*****************************************************************************
//
//  Gets the time when the credential was last modified.
//
//*****************************************************************************
DateTime Kerr::Credential::LastWriteTime::get()
{
    CheckNotDisposed();
    return m_lastWritten.ToLocalTime();
}

//*****************************************************************************
//
//  Gets the time, in coordinated universal time (UTC), when the credential was 
//  last modified.
//
//*****************************************************************************
DateTime Kerr::Credential::LastWriteTimeUtc::get()
{
    CheckNotDisposed();
    return m_lastWritten;
}

//*****************************************************************************
//
//  Gets the size, in bytes, of the secret data for the credential. This can be
//  useful for diagnostic purposes if the secret is not a password.
//
//*****************************************************************************
int Kerr::Credential::SecrectLength::get()
{
    CheckNotDisposed();
    return m_secretLength;
}

//*****************************************************************************
//
//  Loads the credential from the user's credential set based on the target
//  name and type.
//
//*****************************************************************************
void Kerr::Credential::Load()
{
    CheckNotDisposed();

    AutoCredentialBuffer<PCREDENTIAL> buffer;

    pin_ptr<const wchar_t> pinnedTargetName = PtrToStringChars(m_targetName);

    if (!::CredRead(pinnedTargetName,
                    static_cast<DWORD>(m_type),
                    0, // reserved
                    &buffer.m_p))
    {
        DWORD result = ::GetLastError();
        ComponentModel::Win32Exception^ innerException = gcnew ComponentModel::Win32Exception;

        switch (result)
        {
            case ERROR_NOT_FOUND:
            {
                throw gcnew CredentialNotFoundException(innerException);
            }
            case ERROR_NO_SUCH_LOGON_SESSION:
            {
                throw gcnew CredentialSetNotAvailableException(innerException);
            }
            default:
            {
                throw innerException;
            }
        }
    }

    Initialize(buffer.m_p);
}

//*****************************************************************************
//
//  Creates a new credential or modifies an existing credential in the user's 
//  credential set.
//
//*****************************************************************************
void Kerr::Credential::Save()
{
    CheckNotDisposed();

    CREDENTIAL credential = { 0 };
    credential.Type = static_cast<DWORD>(m_type);

    pin_ptr<const wchar_t> pinnedTargetName = PtrToStringChars(m_targetName);
    credential.TargetName = const_cast<PWSTR>(pinnedTargetName);

    pin_ptr<const wchar_t> pinnedDescription = PtrToStringChars(m_description);
    credential.Comment = const_cast<PWSTR>(pinnedDescription);

    credential.CredentialBlobSize = m_password->Length * sizeof (wchar_t); // in bytes

    credential.Persist = static_cast<DWORD>(m_persistence);

    pin_ptr<const wchar_t> pinnedUserName = PtrToStringChars(m_userName);
    credential.UserName = const_cast<PWSTR>(pinnedUserName);

    AutoPasswordBstr password;
    password.m_bstr = static_cast<BSTR>(Runtime::InteropServices::Marshal::SecureStringToBSTR(m_password).ToPointer());

    credential.CredentialBlob = reinterpret_cast<PBYTE>(password.m_bstr);

    if (!::CredWrite(&credential,
                     0))
    {
        DWORD result = ::GetLastError();
        ComponentModel::Win32Exception^ innerException = gcnew ComponentModel::Win32Exception;

        switch (result)
        {
            case ERROR_NO_SUCH_LOGON_SESSION:
            {
                throw gcnew CredentialSetNotAvailableException(innerException);
            }
            case ERROR_BAD_USERNAME:
            {
                throw gcnew UserNameInvalidException(innerException);
            }
            default:
            {
                throw innerException;
            }
        }
    }

    m_lastWritten = DateTime::UtcNow;
}

//*****************************************************************************
//
//  Delete a credential from a user's credential set.
//
//*****************************************************************************
void Kerr::Credential::Delete()
{
    CheckNotDisposed();

    Delete(m_targetName,
           m_type);
}

//*****************************************************************************
//
//  Changes the target name for an existing credential in the user's credential
//  set.
//
//*****************************************************************************
void Kerr::Credential::ChangeTargetName(String^ newTargetName)
{
    CheckNotDisposed();

    if (nullptr == newTargetName)
    {
        throw gcnew ArgumentNullException("newTargetName");
    }

    pin_ptr<const wchar_t> oldName = PtrToStringChars(m_targetName);
    pin_ptr<const wchar_t> newName = PtrToStringChars(newTargetName);

    if (!::CredRename(oldName,
                      newName,
                      static_cast<DWORD>(m_type),
                      0)) // reserved
    {
        DWORD result = ::GetLastError();
        ComponentModel::Win32Exception^ innerException = gcnew ComponentModel::Win32Exception;

        switch (result)
        {
            case ERROR_NOT_FOUND:
            {
                throw gcnew CredentialNotFoundException(innerException);
            }
            case ERROR_ALREADY_EXISTS:
            {
                throw gcnew DuplicateTargetNameException(innerException);
            }
            case ERROR_NO_SUCH_LOGON_SESSION:
            {
                throw gcnew CredentialSetNotAvailableException(innerException);
            }
            default:
            {
                throw innerException;
            }
        }
    }

    m_targetName = newTargetName;
    m_lastWritten = DateTime::UtcNow;
}

//*****************************************************************************
//
//  Searches the user's credential set for a matching credential.
//
//*****************************************************************************
bool Kerr::Credential::Exists(String^ targetName,
                              CredentialType type)
{
    if (nullptr == targetName)
    {
        throw gcnew ArgumentNullException("targetName");
    }

    AutoCredentialBuffer<PCREDENTIAL> buffer;

    pin_ptr<const wchar_t> pinnedTargetName = PtrToStringChars(targetName);

    bool exists = 0 != ::CredRead(pinnedTargetName,
                                  static_cast<DWORD>(type),
                                  0, // reserved
                                  &buffer.m_p);
    
    if (exists)
    {
        ::SecureZeroMemory(buffer.m_p->CredentialBlob,
                           buffer.m_p->CredentialBlobSize);
    }

    return exists;
}

//*****************************************************************************
//
//  Delete a credential from a user's credential set.
//
//*****************************************************************************
void Kerr::Credential::Delete(String^ targetName,
                              CredentialType type)
{
    if (nullptr == targetName)
    {
        throw gcnew ArgumentNullException("targetName");
    }

    pin_ptr<const wchar_t> pinnedTargetName = PtrToStringChars(targetName);

    if (!::CredDelete(pinnedTargetName,
                      static_cast<DWORD>(type),
                      0)) // reserved
    {
        DWORD result = ::GetLastError();
        ComponentModel::Win32Exception^ innerException = gcnew ComponentModel::Win32Exception;

        switch (result)
        {
            case ERROR_NOT_FOUND:
            {
                throw gcnew CredentialNotFoundException(innerException);
            }
            case ERROR_NO_SUCH_LOGON_SESSION:
            {
                throw gcnew CredentialSetNotAvailableException(innerException);
            }
            default:
            {
                throw innerException;
            }
        }
    }
}

//*****************************************************************************
//
//  This internal constructor is used by the CredentialSet class to create 
//  Credential objects from the user's credential set.
//
//*****************************************************************************
Kerr::Credential::Credential(PCREDENTIAL credential)
{
    Initialize(credential);
}

//*****************************************************************************
//
//  This private method is used by the internal constructor as well as the 
//  public Load method to initialize the Credential object from a CREDENTIAL
//  structure.
//
//*****************************************************************************
void Kerr::Credential::Initialize(PCREDENTIAL credential)
{
    msclr::auto_handle<Security::SecureString> password;

    if (0 != credential->CredentialBlobSize)
    {
        password.reset(gcnew Security::SecureString(reinterpret_cast<wchar_t*>(credential->CredentialBlob),
                                                    credential->CredentialBlobSize / sizeof (wchar_t)));

        ::SecureZeroMemory(credential->CredentialBlob,
                           credential->CredentialBlobSize);
    }
    else
    {
        password.reset(gcnew Security::SecureString);
    }

    ULARGE_INTEGER fileTime = { 0 };
    fileTime.LowPart = credential->LastWritten.dwLowDateTime;
    fileTime.HighPart = credential->LastWritten.dwHighDateTime;

    Initialize(gcnew String(credential->TargetName),
               static_cast<CredentialType>(credential->Type),
               gcnew String(credential->UserName),
               password.get(),
               credential->CredentialBlobSize,
               static_cast<CredentialPersistence>(credential->Persist),
               gcnew String(credential->Comment),
               DateTime::FromFileTimeUtc(fileTime.QuadPart));

    password.release();
}

//*****************************************************************************
//
//  This private method is used in all the public constructors to initialize 
//  a new Credential objects.
//
//*****************************************************************************
void Kerr::Credential::Initialize(String^ targetName,
                                  CredentialType type,
                                  String^ userName,
                                  Security::SecureString^ password,
                                  int secretLength,
                                  CredentialPersistence persistence,
                                  String^ description,
                                  DateTime lastWritten)
{
    Debug::Assert(nullptr != targetName);
    Debug::Assert(nullptr != userName);
    Debug::Assert(nullptr != password);
    Debug::Assert(nullptr != description);

    if (nullptr != m_password)
    {
        delete m_password;
    }

    m_targetName = targetName;
    m_type = type;
    m_userName = userName;
    m_password = password;
    m_secretLength = secretLength;
    m_persistence = persistence;
    m_description = description;
    m_lastWritten = lastWritten;
}

//*****************************************************************************
//
//  This private method is called by all the public members to check that the 
//  object has not been disposed.
//
//*****************************************************************************
void Kerr::Credential::CheckNotDisposed()
{
    if (m_disposed)
    {
        throw gcnew ObjectDisposedException(String::Empty,
                                            Strings::Get("ObjectDisposedException.Message"));
    }
}
