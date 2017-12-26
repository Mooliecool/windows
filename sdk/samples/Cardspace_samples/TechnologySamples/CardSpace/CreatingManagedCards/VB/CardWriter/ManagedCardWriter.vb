Imports System
Imports System.IO
Imports System.Security.Cryptography
Imports System.Security.Cryptography.X509Certificates
Imports Microsoft.VisualBasic

Namespace ManagedCardWriter
    Friend Class ManagedCardWriter
        ' Methods
        Private Shared Sub CreateCard(ByVal inputfilename As String, ByVal outputfilename As String)
            Try
                Dim name1 As StoreName
                Dim location1 As StoreLocation
                Dim textArray1 As String()
                Dim store1 As X509Store
                Dim certificate2 As X509Certificate2
                Dim enumerator1 As X509Certificate2Enumerator
                Console.WriteLine(("Reading card config from " & ChrW(10) & "   " & inputfilename))
                Dim file1 As New IniFile(inputfilename)
                Dim card1 As New ManagedInformationCard(DirectCast(System.Enum.Parse(GetType(CardType), file1.Item("card").Item("type").value), CardType))
                card1.CardName = file1.Item("details").Item("name").value
                card1.CardId = file1.Item("details").Item("id").value
                card1.CardVersion = file1.Item("details").Item("version").value
                Dim info1 As New FileInfo(file1.Item("details").Item("image").value)
                Select Case info1.Extension
                    Case ".jpg"
                        card1.MimeType = "image/jpeg"
                        Exit Select

                    Case ".png"
                        card1.MimeType = "image/png"
                        Exit Select
                    Case ".gif"
                        card1.MimeType = "image/gif"
                        Exit Select
                    Case ".bmp"
                        card1.MimeType = "image/bmp"
                        Exit Select
                    Case ".tiff"
                        card1.MimeType = "image/tiff"
                        Exit Select
                    Case Else
                        Throw New BadImageFormatException(("Image File " & info1.FullName & " image format not supported"))
                End Select
                Try
                    If Not info1.Exists Then
                        Dim info2 As New FileInfo((New FileInfo(inputfilename).Directory.FullName & "\" & file1.Item("details").Item("image").value))
                        If info2.Exists Then
                            info1 = info2
                        End If
                    End If
                    Dim buffer1 As Byte() = New Byte(info1.Length - 1) {}
                    Using stream1 As FileStream = File.OpenRead(info1.FullName)
                        stream1.Read(buffer1, 0, buffer1.Length)
                        card1.CardLogo = buffer1
                    End Using
                Catch exception2 As Exception
                    Throw New FileLoadException(("Could not retrieve the image data from the file:" & info1.FullName))
                End Try
                card1.IssuerId = file1.Item("issuer").Item("address").value
                card1.MexUri = file1.Item("issuer").Item("MexAddress").value
                card1.IssuerName = file1.Item("issuer").Item("name").value
                card1.PrivacyNoticeAt = file1.Item("issuer").Item("privacypolicy").value
                Dim certificate1 As X509Certificate2 = Nothing
                info1 = New FileInfo(file1.Item("issuer").Item("certificate").value)
                If info1.Exists Then
                    Try
                        certificate1 = New X509Certificate2(file1.Item("issuer").Item("certificate").value)
                    Catch exception3 As CryptographicException
                        Try
                            certificate1 = New X509Certificate2(file1.Item("issuer").Item("certificate").value, file1.Item("issuer").Item("certificatepassword").value)
                        Catch exception4 As Exception
                            Throw New Exception(("Could not open the certificate file:" & file1.Item("issuer").Item("certificate").value & ". Make sure the file exists and the password is correct"))
                        End Try
                    End Try
                End If
                If (certificate1 Is Nothing) Then
                    name1 = StoreName.My
                    location1 = StoreLocation.LocalMachine
                    textArray1 = file1.Item("issuer").Item("certificate").value.Split("/".ToCharArray, StringSplitOptions.RemoveEmptyEntries)
                    Try
                        location1 = DirectCast(System.Enum.Parse(GetType(StoreLocation), textArray1(0), True), StoreLocation)
                    Catch exception5 As Exception
                        Throw New Exception(("No Certificate Location: " & textArray1(0)))
                    End Try
                    Try
                        name1 = DirectCast(System.Enum.Parse(GetType(StoreName), textArray1(1), True), StoreName)
                    Catch exception6 As Exception
                        Throw New Exception(("No Certificate Store: " & textArray1(1) & " in " & textArray1(0)))
                    End Try
                    store1 = New X509Store(name1, location1)
                    store1.Open(OpenFlags.MaxAllowed)
                    enumerator1 = store1.Certificates.GetEnumerator
                    Do While enumerator1.MoveNext
                        certificate2 = enumerator1.Current
                        If certificate2.Subject.StartsWith(("CN=" & textArray1(2))) Then
                            certificate1 = certificate2
                            Exit Do
                        End If
                    Loop
                    If (certificate1 Is Nothing) Then
                        Throw New Exception(String.Format("Could not find certificate {0} in {1}:{2}", textArray1(2), textArray1(0), textArray1(1)))
                    End If
                End If
                If (file1.Item("claims").keys.Length = 0) Then
                    Throw New Exception("No claims listed.")
                End If
                Dim key1 As key
                For Each key1 In file1.Item("claims").keys
                    If DefaultValues.Claims.ContainsKey(key1.value.Trim) Then
                        card1.SupportedClaims.Add(DefaultValues.Claims.Item(key1.value.Trim))
                    Else
                        If (file1.Item(key1.value).name.Length = 0) Then
                            Throw New Exception(("Can't find claim specification for [" & key1.value & "]"))
                        End If
                        If (file1.Item(key1.value).Item("display").value.Length = 0) Then
                            Throw New Exception(("Can't find claim display value for claim[" & key1.value & "]"))
                        End If
                        If (file1.Item(key1.value).Item("description").value.Length = 0) Then
                            Throw New Exception(String.Concat(New String() {"Can't find claim description for claim [", key1.value, "] display:[", file1.Item(key1.value).Item("display").value, "]"}))
                        End If
                        card1.SupportedClaims.Add(New ClaimInfo(key1.value, file1.Item(key1.value).Item("display").value, file1.Item(key1.value).Item("description").value))
                    End If
                Next
                If (file1.Item("tokentypes").keys.Length = 0) Then
                    Throw New Exception("No token types listed.")
                End If
                card1.TokenTypes = New String(file1.Item("tokentypes").keys.Length - 1) {}
                Dim num1 As Integer = 0
                Do While (num1 < file1.Item("tokentypes").keys.Length)
                    card1.TokenTypes(num1) = file1.Item("tokentypes").keys(num1).value.Trim
                    num1 += 1
                Loop
                If file1.Item("tokendetails").Item("requiresappliesto").value.Equals("true", StringComparison.CurrentCultureIgnoreCase) Then
                    card1.RequireAppliesTo = True
                End If
                card1.CredentialHint = file1.Item("Credentials").Item("hint").value
                If (card1.CardType = CardType.UserNamePassword) Then
                    card1.CredentialIdentifier = file1.Item("Credentials").Item("value").value
                End If
                If (card1.CardType = CardType.SelfIssuedAuth) Then
                    card1.CredentialIdentifier = file1.Item("Credentials").Item("value").value
                End If
                If (card1.CardType = CardType.SmartCard) Then
                    Dim certificate3 As X509Certificate2 = Nothing
                    info1 = New FileInfo(file1.Item("Credentials").Item("value").value)
                    If info1.Exists Then
                        Try
                            certificate3 = New X509Certificate2(file1.Item("Credentials").Item("value").value)
                        Catch exception7 As CryptographicException
                            Try
                                certificate3 = New X509Certificate2(file1.Item("Credentials").Item("value").value, file1.Item("Credentials").Item("certificatepassword").value)
                            Catch exception8 As Exception
                                Throw New Exception(("Could not open the smartcard certificate file:" & file1.Item("Credentials").Item("value").value & ". Make sure the file exists and the password is correct"))
                            End Try
                        End Try
                    End If
                    name1 = StoreName.My
                    location1 = StoreLocation.CurrentUser
                    If ((certificate3 Is Nothing) AndAlso (file1.Item("Credentials").Item("value").value.Split("/".ToCharArray, StringSplitOptions.RemoveEmptyEntries).Length = 3)) Then
                        textArray1 = file1.Item("Credentials").Item("value").value.Split("/".ToCharArray, StringSplitOptions.RemoveEmptyEntries)
                        Try
                            location1 = DirectCast(System.Enum.Parse(GetType(StoreLocation), textArray1(0), True), StoreLocation)
                        Catch exception9 As Exception
                            Throw New Exception(("No Smartcard Certificate Location: " & textArray1(0)))
                        End Try
                        Try
                            name1 = DirectCast(System.Enum.Parse(GetType(StoreName), textArray1(1), True), StoreName)
                        Catch exception10 As Exception
                            Throw New Exception(("No Smartcard Certificate Store: " & textArray1(1) & " in " & textArray1(0)))
                        End Try
                        store1 = New X509Store(name1, location1)
                        store1.Open(OpenFlags.MaxAllowed)
                        enumerator1 = store1.Certificates.GetEnumerator
                        Do While enumerator1.MoveNext
                            certificate2 = enumerator1.Current
                            If certificate2.Subject.StartsWith(("CN=" & textArray1(2))) Then
                                certificate3 = certificate2
                                Exit Do
                            End If
                        Loop
                    End If
                    If (certificate3 Is Nothing) Then
                        store1 = New X509Store(name1, location1)
                        store1.Open(OpenFlags.MaxAllowed)
                        enumerator1 = store1.Certificates.GetEnumerator
                        Do While enumerator1.MoveNext
                            certificate2 = enumerator1.Current
                            If certificate2.Thumbprint.Equals(file1.Item("Credentials").Item("value").value, StringComparison.CurrentCultureIgnoreCase) Then
                                certificate3 = certificate2
                                Exit Do
                            End If
                        Loop
                        If (certificate3 Is Nothing) Then
                            Console.WriteLine(("Did not find smart card certificate, setting smartcard certificate hash to [" & file1.Item("Credentials").Item("value").value & "]"))
                            card1.CredentialIdentifier = file1.Item("Credentials").Item("value").value
                        Else
                            Console.WriteLine(("Found smart card certificate, setting smartcard certificate hash to [" & Convert.ToBase64String(certificate3.GetCertHash) & "]"))
                            card1.CredentialIdentifier = Convert.ToBase64String(certificate3.GetCertHash)
                        End If
                    Else
                        Console.WriteLine(("Found smart card certificate, setting smartcard certificate hash to [" & Convert.ToBase64String(certificate3.GetCertHash) & "]"))
                        card1.CredentialIdentifier = Convert.ToBase64String(certificate3.GetCertHash)
                    End If
                End If
                card1.SerializeAndSign(outputfilename, certificate1)
                Console.WriteLine(("Card written to " & ChrW(10) & "   " & outputfilename))
            Catch exception1 As Exception
                Console.WriteLine(("Exception:" & ChrW(10) & exception1.Message))
                Console.WriteLine((ChrW(10) & exception1.StackTrace))
            End Try
        End Sub

        <STAThread()> _
        Public Shared Sub Main(ByVal args As String())
            If ((args.Length = 1) OrElse (args.Length = 2)) Then
                Dim text1 As String = args(0).Replace(New FileInfo(args(0)).Extension, ".crd")
                If (args.Length = 2) Then
                    text1 = args(1)
                End If
                text1 = New FileInfo(text1).FullName
                ManagedCardWriter.CreateCard(New FileInfo(args(0)).FullName, text1)
            Else
                Console.Write(ChrW(13) & ChrW(10) & "Usage:" & ChrW(13) & ChrW(10) & "    ManagedCardWriter carddetails.ini [outputfilename.crd]" & ChrW(13) & ChrW(10) & "   " & ChrW(13) & ChrW(10))
            End If
        End Sub

    End Class
End Namespace


