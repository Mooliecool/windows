Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Runtime.InteropServices

Namespace ManagedCardWriter
    Public NotInheritable Class ClaimInfo
        ' Methods
        Public Sub New(ByVal id As String, ByVal tag As String, ByVal des As String)
            Me.m_id = id
            Me.m_displayTag = tag
            Me.m_description = des
        End Sub


        ' Properties
        Public Property Description() As String
            Get
                Return Me.m_description
            End Get
            Set(ByVal value As String)
                Me.m_description = value
            End Set
        End Property

        Public Property DisplayTag() As String
            Get
                Return Me.m_displayTag
            End Get
            Set(ByVal value As String)
                Me.m_displayTag = value
            End Set
        End Property

        Public Property Id() As String
            Get
                Return Me.m_id
            End Get
            Set(ByVal value As String)
                Me.m_id = value
            End Set
        End Property


        ' Fields
        Private m_description As String
        Private m_displayTag As String
        Private m_id As String
    End Class
End Namespace


