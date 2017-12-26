//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    The PromptForCredential class provides a configurable 
//                  dialog box to accept credential information from a user.
//
//*****************************************************************************

#include "stdafx.h"
#include "PromptForCredential.h"
#include "AutoGdiObject.h"
#include "AutoPasswordBstr.h"
#include "AutoPasswordBuffer.h"
#include "Strings.h"

//*****************************************************************************
//
//  Initializes a new instance of the PromptForCredential class.
//
//*****************************************************************************
Kerr::PromptForCredential::PromptForCredential() :
    m_targetName(String::Empty),
    m_userName(String::Empty)
{
    // Do nothing
}

//*****************************************************************************
//
//  Deletes the resources held by the object, optionally confirming the 
//  credential with the operating system.
//
//*****************************************************************************
Kerr::PromptForCredential::~PromptForCredential()
{
    if (!m_disposed)
    {
        delete m_password;
        m_password = nullptr;

        delete m_banner;
        m_banner = nullptr;

        if (SaveChecked && ExpectConfirmation && !m_confirmed)
        {
            pin_ptr<const wchar_t> pinnedTargetName = PtrToStringChars(m_targetName);

            ::CredUIConfirmCredentials(pinnedTargetName,
                                       FALSE);
        }

        m_disposed = true;
    }

    Debug::Assert(nullptr == m_password);
}

//*****************************************************************************
//
//  Gets or sets the target name that is used to identify the credential when 
//  storing and retrieving it. It is also used as part of the title and message 
//  text on the dialog box if these are not overridden.
//
//*****************************************************************************
String^ Kerr::PromptForCredential::TargetName::get()
{
    CheckNotDisposed();
    return m_targetName;
}
void Kerr::PromptForCredential::TargetName::set(String^ value)
{
    CheckNotDisposed();

    if (nullptr == value)
    {
        throw gcnew ArgumentNullException("value");
    }

    m_targetName = value;
}

//*****************************************************************************
//
//  Gets or sets the error code to allow the dialog box to accommodate certain 
//  errors.
//
//*****************************************************************************
int Kerr::PromptForCredential::ErrorCode::get()
{
    CheckNotDisposed();
    return m_errorCode;
}
void Kerr::PromptForCredential::ErrorCode::set(int value)
{
    CheckNotDisposed();
    m_errorCode = value;
}

//*****************************************************************************
//
//  Gets or sets the user name entered by the user. The dialog box will be 
//  prefilled with the initial value.
//
//*****************************************************************************
String^ Kerr::PromptForCredential::UserName::get()
{
    CheckNotDisposed();
    return m_userName;
}
void Kerr::PromptForCredential::UserName::set(String^ value)
{
    CheckNotDisposed();

    if (nullptr == value)
    {
        throw gcnew ArgumentNullException("value");
    }

    if (CREDUI_MAX_USERNAME_LENGTH < value->Length)
    {
        throw gcnew ArgumentOutOfRangeException("value");
    }

    m_userName = value;
}

//*****************************************************************************
//
//  Gets or sets the password entered by the user. The dialog box will be 
//  prefilled with the initial value.
//
//*****************************************************************************
Security::SecureString^ Kerr::PromptForCredential::Password::get()
{
    CheckNotDisposed();

    if (nullptr == m_password)
    {
        m_password = gcnew Security::SecureString;
    }

    return m_password;
}
void Kerr::PromptForCredential::Password::set(Security::SecureString^ value)
{
    CheckNotDisposed();

    if (nullptr == value)
    {
        throw gcnew ArgumentNullException("value");
    }

    if (m_password != value)
    {
        delete m_password;
        m_password = value;
    }
}

//*****************************************************************************
//
//  Gets or sets a value indicating whether the save check box is checked. This 
//  value is ignored unless the ShowSaveCheckBox property is set to true.
//
//*****************************************************************************
bool Kerr::PromptForCredential::SaveChecked::get()
{
    CheckNotDisposed();
    return m_saveChecked;
}
void Kerr::PromptForCredential::SaveChecked::set(bool value)
{
    CheckNotDisposed();
    m_saveChecked = value;
}

//*****************************************************************************
//
//  Gets or sets the message displayed on the dialog box. If the message is an 
//  empty string, the dialog box contains a default message including the 
//  target name.
//
//*****************************************************************************
String^ Kerr::PromptForCredential::Message::get()
{
    CheckNotDisposed();
    return m_message ? m_message : String::Empty;
}
void Kerr::PromptForCredential::Message::set(String^ value)
{
    CheckNotDisposed();

    if (nullptr == value)
    {
        throw gcnew ArgumentNullException("value");
    }

    if (CREDUI_MAX_MESSAGE_LENGTH < value->Length)
    {
        throw gcnew ArgumentOutOfRangeException("value");
    }

    m_message = value;
}

//*****************************************************************************
//
//  Gets or sets the dialog box title. If the title is an empty string, the 
//  dialog box uses a default title including the target name.
//
//*****************************************************************************
String^ Kerr::PromptForCredential::Title::get()
{
    CheckNotDisposed();
    return m_title ? m_title : String::Empty;
}
void Kerr::PromptForCredential::Title::set(String^ value)
{
    CheckNotDisposed();

    if (nullptr == value)
    {
        throw gcnew ArgumentNullException("value");
    }

    if (CREDUI_MAX_CAPTION_LENGTH < value->Length)
    {
        throw gcnew ArgumentOutOfRangeException("value");
    }

    m_title = value;
}

//*****************************************************************************
//
//  Gets or sets a banner bitmap to display in the dialog box. If a bitmap is 
//  not provided, the dialog box displays a default bitmap. The bitmap size is 
//  limited to 320 x 60 pixels.
//
//*****************************************************************************
Drawing::Bitmap^ Kerr::PromptForCredential::Banner::get()
{
    CheckNotDisposed();
    return m_banner;
}
void Kerr::PromptForCredential::Banner::set(Drawing::Bitmap^ value)
{
    CheckNotDisposed();

    if (value != m_banner)
    {
        delete m_banner;
        m_banner = value;
    }
}

//*****************************************************************************
//
//  The dialog box should be displayed even if a matching credential exists in 
//  the user’s credential set.
//
//*****************************************************************************
bool Kerr::PromptForCredential::AlwaysShowUI::get()
{
    CheckNotDisposed();
    return 0 != (CREDUI_FLAGS_ALWAYS_SHOW_UI & m_flags);
}
void Kerr::PromptForCredential::AlwaysShowUI::set(bool value)
{
    CheckNotDisposed();
    Flag(value, CREDUI_FLAGS_ALWAYS_SHOW_UI);
}

//*****************************************************************************
//
//  The dialog box will automatically add the target name as the authority in 
//  the user name if the user doesn’t specify an authority. This property is 
//  only used with generic credentials as user name completion is always used 
//  for Windows credentials.
//
//*****************************************************************************
bool Kerr::PromptForCredential::CompleteUserName::get()
{
    CheckNotDisposed();
    return 0 != (CREDUI_FLAGS_COMPLETE_USERNAME & m_flags);
}
void Kerr::PromptForCredential::CompleteUserName::set(bool value)
{
    CheckNotDisposed();
    Flag(value, CREDUI_FLAGS_COMPLETE_USERNAME);
}

//*****************************************************************************
//
//  The dialog box should not store the credential in the user’s credential 
//  set. The Save check box is not displayed unless the ShowSaveCheckBox 
//  property is set to true.
//
//*****************************************************************************
bool Kerr::PromptForCredential::DoNotPersist::get()
{
    CheckNotDisposed();
    return 0 != (CREDUI_FLAGS_DO_NOT_PERSIST & m_flags);
}
void Kerr::PromptForCredential::DoNotPersist::set(bool value)
{
    CheckNotDisposed();
    Flag(value, CREDUI_FLAGS_DO_NOT_PERSIST);
}

//*****************************************************************************
//
//  Certificate or smart card credentials will not be displayed in the User 
//  name combo box. Only generic and password credentials will be present.
//
//*****************************************************************************
bool Kerr::PromptForCredential::ExcludeCertificates::get()
{
    CheckNotDisposed();
    return 0 != (CREDUI_FLAGS_EXCLUDE_CERTIFICATES & m_flags);
}
void Kerr::PromptForCredential::ExcludeCertificates::set(bool value)
{
    CheckNotDisposed();
    Flag(value, CREDUI_FLAGS_EXCLUDE_CERTIFICATES);
}

//*****************************************************************************
//
//  The credential manager expects that you will validate the credentials 
//  before it stores them. This avoids invalid credentials from being added to 
//  the user’s credential set. 
//
//*****************************************************************************
bool Kerr::PromptForCredential::ExpectConfirmation::get()
{
    CheckNotDisposed();
    return 0 != (CREDUI_FLAGS_EXPECT_CONFIRMATION & m_flags);
}
void Kerr::PromptForCredential::ExpectConfirmation::set(bool value)
{
    CheckNotDisposed();
    Flag(value, CREDUI_FLAGS_EXPECT_CONFIRMATION);
}

//*****************************************************************************
//
//  The entered credentials are considered application-specific.
//
//*****************************************************************************
bool Kerr::PromptForCredential::GenericCredentials::get()
{
    CheckNotDisposed();
    return 0 != (CREDUI_FLAGS_GENERIC_CREDENTIALS & m_flags);
}
void Kerr::PromptForCredential::GenericCredentials::set(bool value)
{
    CheckNotDisposed();
    Flag(value, CREDUI_FLAGS_GENERIC_CREDENTIALS);
}

//*****************************************************************************
//
//  The dialog box displays a balloon tip indicating that a logon attempt was 
//  unsuccessful, suggesting that the password may be incorrect.
//
//*****************************************************************************
bool Kerr::PromptForCredential::IncorrectPassword::get()
{
    CheckNotDisposed();
    return 0 != (CREDUI_FLAGS_INCORRECT_PASSWORD & m_flags);
}
void Kerr::PromptForCredential::IncorrectPassword::set(bool value)
{
    CheckNotDisposed();
    Flag(value, CREDUI_FLAGS_INCORRECT_PASSWORD);
}

//*****************************************************************************
//
//  The dialog box will not display the Save check box but will behave as 
//  though it were shown and checked.
//
//*****************************************************************************
bool Kerr::PromptForCredential::Persist::get()
{
    CheckNotDisposed();
    return 0 != (CREDUI_FLAGS_PERSIST & m_flags);
}
void Kerr::PromptForCredential::Persist::set(bool value)
{
    CheckNotDisposed();
    Flag(value, CREDUI_FLAGS_PERSIST);
}

//*****************************************************************************
//
//  The user name combo box is populated with the names of the local 
//  administrator accounts. If this property is not set, the dialog box 
//  populates the combo box with the user names from the user’s credential set.
//
//*****************************************************************************
bool Kerr::PromptForCredential::RequestAdministrator::get()
{
    CheckNotDisposed();
    return 0 != (CREDUI_FLAGS_REQUEST_ADMINISTRATOR & m_flags);
}
void Kerr::PromptForCredential::RequestAdministrator::set(bool value)
{
    CheckNotDisposed();
    Flag(value, CREDUI_FLAGS_REQUEST_ADMINISTRATOR);
}

//*****************************************************************************
//
//  The user name combo box is populated with available certificates and the 
//  user is not able to enter a user name.
//
//*****************************************************************************
bool Kerr::PromptForCredential::RequireCertificate::get()
{
    CheckNotDisposed();
    return 0 != (CREDUI_FLAGS_REQUIRE_CERTIFICATE & m_flags);
}
void Kerr::PromptForCredential::RequireCertificate::set(bool value)
{
    CheckNotDisposed();
    Flag(value, CREDUI_FLAGS_REQUIRE_CERTIFICATE);
}

//*****************************************************************************
//
//  The user name combo box is populated with available smart cards and the 
//  user is not able to enter a user name.
//
//*****************************************************************************
bool Kerr::PromptForCredential::RequireSmartCard::get()
{
    CheckNotDisposed();
    return 0 != (CREDUI_FLAGS_REQUIRE_SMARTCARD & m_flags);
}
void Kerr::PromptForCredential::RequireSmartCard::set(bool value)
{
    CheckNotDisposed();
    Flag(value, CREDUI_FLAGS_REQUIRE_SMARTCARD);
}


//*****************************************************************************
//
//  The dialog box will display the check box despite the fact that it will not 
//  actually persist the credential. This is useful for applications that need 
//  to manage credential storage manually.
//
//*****************************************************************************
bool Kerr::PromptForCredential::ShowSaveCheckBox::get()
{
    CheckNotDisposed();
    return 0 != (CREDUI_FLAGS_SHOW_SAVE_CHECK_BOX & m_flags);
}
void Kerr::PromptForCredential::ShowSaveCheckBox::set(bool value)
{
    CheckNotDisposed();
    Flag(value, CREDUI_FLAGS_SHOW_SAVE_CHECK_BOX);
}

//*****************************************************************************
//
//  The user name field is read-only, allowing only a password to be entered.
//
//*****************************************************************************
bool Kerr::PromptForCredential::UserNameReadOnly::get()
{
    CheckNotDisposed();
    return 0 != (CREDUI_FLAGS_KEEP_USERNAME & m_flags);
}
void Kerr::PromptForCredential::UserNameReadOnly::set(bool value)
{
    CheckNotDisposed();
    Flag(value, CREDUI_FLAGS_KEEP_USERNAME);
}

//*****************************************************************************
//
//  The dialog box will ensure that the entered user name uses a valid format. 
//  This property is only used with generic credentials as user name validation 
//  is always used for Windows credentials.
//
//*****************************************************************************
bool Kerr::PromptForCredential::ValidateUserName::get()
{
    CheckNotDisposed();
    return 0 != (CREDUI_FLAGS_VALIDATE_USERNAME & m_flags);
}
void Kerr::PromptForCredential::ValidateUserName::set(bool value)
{
    CheckNotDisposed();
    Flag(value, CREDUI_FLAGS_VALIDATE_USERNAME);
}

//*****************************************************************************
//
//  Shows the dialog box.
//
//*****************************************************************************
Windows::Forms::DialogResult Kerr::PromptForCredential::ShowDialog()
{
    CheckNotDisposed();
    return ShowDialog(nullptr);
}

//*****************************************************************************
//
//  Shows the dialog box using the specified owner window.
//
//*****************************************************************************
Windows::Forms::DialogResult Kerr::PromptForCredential::ShowDialog(Windows::Forms::IWin32Window^ owner)
{
    CheckNotDisposed();
    CREDUI_INFO info = { sizeof (CREDUI_INFO) };

    if (nullptr != owner)
    {
        info.hwndParent = static_cast<HWND>(owner->Handle.ToPointer());
    }

    AutoGdiObject<HBITMAP> bitmap;

    if (nullptr != m_banner)
    {
        bitmap.m_handle = static_cast<HBITMAP>(m_banner->GetHbitmap().ToPointer());
    }

    info.hbmBanner = bitmap.m_handle;

    pin_ptr<const wchar_t> pinnedMessage = nullptr;

    if (nullptr != m_message &&
        0 < m_message->Length)
    {
        pinnedMessage = PtrToStringChars(m_message);
        info.pszMessageText = pinnedMessage;
    }

    pin_ptr<const wchar_t> pinnedTitle = nullptr;

    if (nullptr != m_title &&
        0 < m_title->Length)
    {
        pinnedTitle = PtrToStringChars(m_title);
        info.pszCaptionText = pinnedTitle;
    }

    pin_ptr<const wchar_t> pinnedTargetName = PtrToStringChars(m_targetName);

    wchar_t userName[CREDUI_MAX_USERNAME_LENGTH + 1] = { 0 };
    int index = 0;

    for each (wchar_t element in m_userName)
    {
        userName[index++] = element;
    }

    AutoPasswordBuffer<CREDUI_MAX_PASSWORD_LENGTH + 1> password;

    if (nullptr != m_password &&
        0 < m_password->Length)
    {
        AutoPasswordBstr bstrPassword;
        bstrPassword.m_bstr = static_cast<BSTR>(Runtime::InteropServices::Marshal::SecureStringToBSTR(m_password).ToPointer());

        wcscpy_s(password.m_buffer,
                 CREDUI_MAX_PASSWORD_LENGTH,
                 bstrPassword.m_bstr);
    }

    BOOL saveChecked = m_saveChecked;

    DWORD result = ::CredUIPromptForCredentials(&info,
                                                pinnedTargetName,
                                                0, // reserved
                                                m_errorCode,
                                                userName,
                                                CREDUI_MAX_USERNAME_LENGTH + 1,
                                                password.m_buffer,
                                                CREDUI_MAX_PASSWORD_LENGTH + 1,
                                                &saveChecked,
                                                m_flags);

    Windows::Forms::DialogResult dialogResult = Windows::Forms::DialogResult::None;

    switch (result)
    {
        case NO_ERROR:
        {
            m_userName = Runtime::InteropServices::Marshal::PtrToStringUni(IntPtr(userName));

            m_password = gcnew Security::SecureString(password.m_buffer,
                                                      static_cast<int>(wcslen(password.m_buffer)));

            m_saveChecked = 0 != saveChecked;

            dialogResult = Windows::Forms::DialogResult::OK;
            break;
        }
        case ERROR_CANCELLED:
        {
            dialogResult = Windows::Forms::DialogResult::Cancel;
            break;
        }
        default:
        {
            throw gcnew ComponentModel::Win32Exception(result);
        }
    }

    return dialogResult;
}

//*****************************************************************************
//
//  Confirms the validity of the previously collected credential.
//
//*****************************************************************************
void Kerr::PromptForCredential::ConfirmCredentials()
{
    CheckNotDisposed();
    pin_ptr<const wchar_t> pinnedTargetName = PtrToStringChars(m_targetName);

    DWORD result = ::CredUIConfirmCredentials(pinnedTargetName,
                                              TRUE);

    m_confirmed = true;

    if (NO_ERROR != result)
    {
        throw gcnew ComponentModel::Win32Exception(result);
    }
}

//*****************************************************************************
//
//  This private method is used to set or clear the bitmask of flags.
//
//*****************************************************************************
void Kerr::PromptForCredential::Flag(bool add,
                                     DWORD flag)
{
    if (add)
    {
        m_flags |= flag;
    }
    else
    {
        m_flags &= ~flag;
    }
}

//*****************************************************************************
//
//  This private method is called by all the public members to check that the 
//  object has not been disposed.
//
//*****************************************************************************
void Kerr::PromptForCredential::CheckNotDisposed()
{
    if (m_disposed)
    {
        throw gcnew ObjectDisposedException(String::Empty,
                                            Strings::Get("ObjectDisposedException.Message"));
    }
}
