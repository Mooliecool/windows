' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System.Runtime.Serialization

Namespace Microsoft.ServiceModel.Samples

    <DataContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Class Album

        <DataMember()> _
        Public Property Title() As String

            Get

                Return m_title

            End Get
            Set(ByVal value As String)

                m_title = value

            End Set

        End Property

        <DataMember()> _
        Public Property Price() As Double

            Get

                Return m_price

            End Get
            Set(ByVal value As Double)

                m_price = value

            End Set

        End Property

        <DataMember()> _
        Public Property Tracks() As Track()

            Get

                Return m_tracks

            End Get
            Set(ByVal value As Track())

                m_tracks = value

            End Set

        End Property

        Private m_title As String
        Private m_price As Double
        Private m_tracks As Track()

    End Class

    <DataContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Class Track

        <DataMember()> _
        Public Property Name() As String

            Get

                Return m_name

            End Get
            Set(ByVal value As String)

                m_name = value

            End Set

        End Property

        <DataMember()> _
        Public Property Duration() As Integer

            Get

                Return m_duration

            End Get
            Set(ByVal value As Integer)

                m_duration = value

            End Set

        End Property

        Private m_name As String
        Private m_duration As Integer

    End Class

End Namespace
