////////////////////////////////////////////////////////////////////////////////
// Name:      serialport.cpp
// Purpose:   Code for wxSerialPort Class
// Author:    Youcef Kouchkar
// Created:   2018-01-08
// Copyright: (c) 2019 Youcef Kouchkar
// License:   MIT License
////////////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "serialport.h"
//#include <boost/bind.hpp>
#include <functional>
//#include <boost/thread.hpp>

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif // __WXMSW__

wxSerialPortBase::wxSerialPortBase() : m_io_context()
{
    // Ctor
    //Init();
}

wxSerialPortBase::~wxSerialPortBase()
{
    // Dtor
    //m_io_context.~io_context();
}

std::vector<wxString> wxSerialPortBase::GetPortNames()
{
    std::vector<wxString> vStrPortNames;
#ifdef __WXMSW__
    wxRegKey regKey(wxRegKey::HKLM, wxS("HARDWARE\\DEVICEMAP\\SERIALCOMM"));

    if (!regKey.Exists())
        return vStrPortNames;

    wxASSERT(regKey.Exists());

    wxString strValueName;
    long lIndex;

    // Enumerate all values
    bool bCont = regKey.GetFirstValue(strValueName, lIndex);

    while (bCont)
    {
        wxRegKey::ValueType valueType = regKey.GetValueType(strValueName);

        if ((valueType == wxRegKey::Type_String || valueType == wxRegKey::Type_Expand_String ||
             valueType == wxRegKey::Type_Multi_String) && !strValueName.empty())
        {
            wxString strValueData;
            regKey.QueryValue(strValueName, strValueData);
            vStrPortNames.push_back(strValueData);
        }

        bCont = regKey.GetNextValue(strValueName, lIndex);
    }
#else // !__WXMSW__
    wxArrayString arrStrFiles;
    wxDir::GetAllFiles(wxS("/dev/"), &arrStrFiles, wxS("ttyS*"), wxDIR_FILES);

    for (wxArrayString::const_iterator it = arrStrFiles.begin(); it != arrStrFiles.end(); ++it)
        vStrPortNames.push_back(*it);
#endif // __WXMSW__

    return vStrPortNames;
}

wxSerialPort::wxSerialPort() : m_serialPort(m_io_context), m_timer(m_io_context)
{
    // Default ctor
    Init();
    //m_strPortName = wxEmptyString;
}

wxSerialPort::wxSerialPort(const char *device)
    : m_serialPort(m_io_context, device), m_timer(m_io_context), m_strPortName(device)
{
    // Ctor
    Init();
}

wxSerialPort::wxSerialPort(const std::string& device)
    : m_serialPort(m_io_context, device), m_timer(m_io_context), m_strPortName(device)
{
    // Ctor
    Init();
}

wxSerialPort::wxSerialPort(const wxString& device)
    : m_serialPort(m_io_context, device), m_timer(m_io_context), m_strPortName(device)
{
    // Ctor
    Init();
}

wxSerialPort::wxSerialPort(const boost::asio::serial_port::native_handle_type& native_serial_port)
    : m_serialPort(m_io_context, native_serial_port), m_timer(m_io_context)
{
    // Ctor
    Init();
    //m_strPortName = wxEmptyString;
}

// Copy constructor
wxSerialPort::wxSerialPort(const wxSerialPort& other)
    /*: m_serialPort(other.m_serialPort)*/ : m_serialPort(m_io_context) /*, m_timer(other.m_timer)*/, m_timer(m_io_context),
    m_strPortName(other.m_strPortName)
{
    // Copy ctor
    Init();
}

// Copy assignment operator
wxSerialPort& wxSerialPort::operator=(const wxSerialPort& other)
{
    //m_serialPort = other.m_serialPort;
    //m_timer = other.m_timer;
    m_strPortName = other.m_strPortName;
    //Init();
    return *this;
}

#ifdef BOOST_ASIO_HAS_MOVE
// Move constructor
wxSerialPort::wxSerialPort(wxSerialPort&& other)
    : m_serialPort(std::move(other.m_serialPort)), m_timer(std::move(other.m_timer)),
    m_strPortName(/*std::move(*/other.m_strPortName/*)*/) //, m_bytes_read(/*std::move(*/other.m_bytes_read/*)*/),
    //m_bytes_written(/*std::move(*/other.m_bytes_written/*)*/)
{
    // Move ctor
}

// Move assignment operator
wxSerialPort& wxSerialPort::operator=(wxSerialPort&& other)
{
    m_serialPort = std::move(other.m_serialPort);
    m_timer = std::move(other.m_timer);
    m_strPortName = /*std::move(*/other.m_strPortName/*)*/;
    //m_bytes_read = /*std::move(*/other.m_bytes_read/*)*/;
    //m_bytes_written = /*std::move(*/other.m_bytes_written/*)*/;
    return *this;
}
#endif // BOOST_ASIO_HAS_MOVE

wxSerialPort::~wxSerialPort()
{
    // Dtor
    m_serialPort.close();
    //m_serialPort.~serial_port();
}

boost::asio::serial_port& wxSerialPort::GetSerialPort() /*const*/
{
    return m_serialPort;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::Assign(const boost::asio::serial_port::native_handle_type& native_serial_port)
{
    boost::system::error_code error;
    m_serialPort.assign(native_serial_port, error);
    DoSetLastError(error);
    return error;
}

// ===============================================================================================================================

// -------------------------------------------------------------------------------------------------------------------------------
// ReadSome
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
std::size_t wxSerialPort::DoReadSome(const boost::asio::mutable_buffer& buffer)
{
    boost::system::error_code error;
    std::size_t ret = m_serialPort.read_some(buffer, error);
    DoSetLastError(error);
    return ret;
}

std::size_t wxSerialPort::ReadSome(std::string& strBuffer)
{
    return DoReadSome(boost::asio::buffer(strBuffer));
}

std::size_t wxSerialPort::ReadSome(wxCharBuffer& chBuffer)
{
    return DoReadSome(boost::asio::buffer(chBuffer.data(), chBuffer.length()));
}

std::size_t wxSerialPort::ReadSome(void *pszBuffer, const std::size_t uiSize)
{
    return DoReadSome(boost::asio::buffer(pszBuffer, uiSize));
}

template <typename type>
std::size_t wxSerialPort::ReadSome(std::vector<type>& vBuffer)
{
    return DoReadSome(boost::asio::buffer(vBuffer));
}

template <typename type, std::size_t uiSize>
std::size_t wxSerialPort::ReadSome(std::array<type, uiSize>& arrBuffer)
{
    return DoReadSome(boost::asio::buffer(arrBuffer));
}

// -------------------------------------------------------------------------------------------------------------------------------
// ReadSome with timeout
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
std::size_t wxSerialPort::DoReadSome(const boost::asio::mutable_buffer& buffer, const int timeout)
{
    std::size_t bytes_read = 0;
    m_io_context.restart();
    DoAsyncWaitThenCancelAsyncIO(timeout);

    m_serialPort.async_read_some(buffer,
    /*std::bind(&wxSerialPort::AsyncReadHandler, this, std::placeholders::_1, std::placeholders::_2)*/
    [&](const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        // Read operation was not aborted
        if (!error) // != boost::asio::error::operation_aborted
            m_timer.cancel();

        bytes_read = bytes_transferred;
        DoSetLastError(error);
    });

    std::thread thread1([this] { m_io_context.run(); });
    thread1.join();
    return bytes_read;
}

std::size_t wxSerialPort::ReadSome(std::string& strBuffer, const int timeout)
{
    return DoReadSome(boost::asio::buffer(strBuffer), timeout);
}

std::size_t wxSerialPort::ReadSome(wxCharBuffer& chBuffer, const int timeout)
{
    return DoReadSome(boost::asio::buffer(chBuffer.data(), chBuffer.length()), timeout);
}

std::size_t wxSerialPort::ReadSome(void *pszBuffer, const std::size_t uiSize, const int timeout)
{
    return DoReadSome(boost::asio::buffer(pszBuffer, uiSize), timeout);
}

template <typename type>
std::size_t wxSerialPort::ReadSome(std::vector<type>& vBuffer, const int timeout)
{
    return DoReadSome(boost::asio::buffer(vBuffer), timeout);
}

template <typename type, std::size_t uiSize>
std::size_t wxSerialPort::ReadSome(std::array<type, uiSize>& arrBuffer, const int timeout)
{
    return DoReadSome(boost::asio::buffer(arrBuffer), timeout);
}

// -------------------------------------------------------------------------------------------------------------------------------
// WriteSome
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
std::size_t wxSerialPort::DoWriteSome(const boost::asio::const_buffer& buffer)
{
    boost::system::error_code error;
    std::size_t ret = m_serialPort.write_some(buffer, error);
    DoSetLastError(error);
    return ret;
}

std::size_t wxSerialPort::WriteSome(const std::string& strBuffer)
{
    return DoWriteSome(boost::asio::buffer(strBuffer));
}

std::size_t wxSerialPort::WriteSome(const wxString& strBuffer)
{
    return DoWriteSome(boost::asio::buffer(strBuffer.data(), strBuffer.size()));
}

std::size_t wxSerialPort::WriteSome(const wxCharBuffer& chBuffer)
{
    return DoWriteSome(boost::asio::buffer(chBuffer.data(), chBuffer.length()));
}

std::size_t wxSerialPort::WriteSome(const void *pszBuffer, const std::size_t uiSize)
{
    return DoWriteSome(boost::asio::buffer(pszBuffer, uiSize));
}

template <typename type>
std::size_t wxSerialPort::WriteSome(const std::vector<type>& vBuffer)
{
    return DoWriteSome(boost::asio::buffer(vBuffer));
}

template <typename type, std::size_t uiSize>
std::size_t wxSerialPort::WriteSome(const std::array<type, uiSize>& arrBuffer)
{
    return DoWriteSome(boost::asio::buffer(arrBuffer));
}

// -------------------------------------------------------------------------------------------------------------------------------
// WriteSome with timeout
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
std::size_t wxSerialPort::DoWriteSome(const boost::asio::const_buffer& buffer, const int timeout)
{
    std::size_t bytes_written = 0;
    m_io_context.restart();
    DoAsyncWaitThenCancelAsyncIO(timeout);

    m_serialPort.async_write_some(buffer,
    /*std::bind(&wxSerialPort::AsyncWriteHandler, this, std::placeholders::_1, std::placeholders::_2)*/
    [&](const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        // Write operation was not aborted
        if (!error) // != boost::asio::error::operation_aborted
            m_timer.cancel();

        bytes_written = bytes_transferred;
        DoSetLastError(error);
    });

    std::thread thread1([this] { m_io_context.run(); });
    thread1.join();
    return bytes_written;
}

std::size_t wxSerialPort::WriteSome(const std::string& strBuffer, const int timeout)
{
    return DoWriteSome(boost::asio::buffer(strBuffer), timeout);
}

std::size_t wxSerialPort::WriteSome(const wxString& strBuffer, const int timeout)
{
    return DoWriteSome(boost::asio::buffer(strBuffer.data(), strBuffer.size()), timeout);
}

std::size_t wxSerialPort::WriteSome(const wxCharBuffer& chBuffer, const int timeout)
{
    return DoWriteSome(boost::asio::buffer(chBuffer.data(), chBuffer.length()), timeout);
}

std::size_t wxSerialPort::WriteSome(const void *pszBuffer, const std::size_t uiSize, const int timeout)
{
    return DoWriteSome(boost::asio::buffer(pszBuffer, uiSize), timeout);
}

template <typename type>
std::size_t wxSerialPort::WriteSome(const std::vector<type>& vBuffer, const int timeout)
{
    return DoWriteSome(boost::asio::buffer(vBuffer), timeout);
}

template <typename type, std::size_t uiSize>
std::size_t wxSerialPort::WriteSome(const std::array<type, uiSize>& arrBuffer, const int timeout)
{
    return DoWriteSome(boost::asio::buffer(arrBuffer), timeout);
}

// ===============================================================================================================================

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncReadSome
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::DoAsyncReadSome(const boost::asio::mutable_buffer& buffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
{
    return m_serialPort.async_read_some(buffer, handler);
}

template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadSome(std::string& strBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
{
    return DoAsyncReadSome(boost::asio::buffer(strBuffer), handler);
}

template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadSome(wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
{
    return DoAsyncReadSome(boost::asio::buffer(chBuffer.data(), chBuffer.length()), handler);
}

template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadSome(void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
{
    return DoAsyncReadSome(boost::asio::buffer(pszBuffer, uiSize), handler);
}

template <typename type, typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadSome(std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
{
    return DoAsyncReadSome(boost::asio::buffer(vBuffer), handler);
}

template <typename type, std::size_t uiSize, typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadSome(std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
{
    return DoAsyncReadSome(boost::asio::buffer(arrBuffer), handler);
}

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncReadSome with timeout
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::DoAsyncReadSome(const boost::asio::mutable_buffer& buffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout)
{
    DoAsyncWaitThenCancelAsyncIO(timeout);
    return m_serialPort.async_read_some(buffer, handler);
}

template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadSome(std::string& strBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout)
{
    return DoAsyncReadSome(boost::asio::buffer(strBuffer), handler, timeout);
}

template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadSome(wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout)
{
    return DoAsyncReadSome(boost::asio::buffer(chBuffer.data(), chBuffer.length()), handler, timeout);
}

template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadSome(void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout)
{
    return DoAsyncReadSome(boost::asio::buffer(pszBuffer, uiSize), handler, timeout);
}

template <typename type, typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadSome(std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout)
{
    return DoAsyncReadSome(boost::asio::buffer(vBuffer), handler, timeout);
}

template <typename type, std::size_t uiSize, typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadSome(std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout)
{
    return DoAsyncReadSome(boost::asio::buffer(arrBuffer), handler, timeout);
}

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncWriteSome
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::DoAsyncWriteSome(const boost::asio::const_buffer& buffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
{
    return m_serialPort.async_write_some(buffer, handler);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWriteSome(const std::string& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
{
    return DoWriteSome(boost::asio::buffer(strBuffer), handler);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWriteSome(const wxString& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
{
    return DoWriteSome(boost::asio::buffer(strBuffer.data(), strBuffer.size()), handler);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWriteSome(const wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
{
    return DoWriteSome(boost::asio::buffer(chBuffer.data(), chBuffer.length()), handler);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWriteSome(const void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
{
    return DoWriteSome(boost::asio::buffer(pszBuffer, uiSize), handler);
}

template <typename type, typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWriteSome(const std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
{
    return DoWriteSome(boost::asio::buffer(vBuffer), handler);
}

template <typename type, std::size_t uiSize, typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWriteSome(const std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
{
    return DoWriteSome(boost::asio::buffer(arrBuffer), handler);
}

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncWriteSome with timeout
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::DoAsyncWriteSome(const boost::asio::const_buffer& buffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout)
{
    DoAsyncWaitThenCancelAsyncIO(timeout);
    return m_serialPort.async_write_some(buffer, handler);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWriteSome(const std::string& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout)
{
    return DoWriteSome(boost::asio::buffer(strBuffer), handler, timeout);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWriteSome(const wxString& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout)
{
    return DoWriteSome(boost::asio::buffer(strBuffer.data(), strBuffer.size()), handler, timeout);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWriteSome(const wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout)
{
    return DoWriteSome(boost::asio::buffer(chBuffer.data(), chBuffer.length()), handler, timeout);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWriteSome(const void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout)
{
    return DoWriteSome(boost::asio::buffer(pszBuffer, uiSize), handler, timeout);
}

template <typename type, typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWriteSome(const std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout)
{
    return DoWriteSome(boost::asio::buffer(vBuffer), handler, timeout);
}

template <typename type, std::size_t uiSize, typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWriteSome(const std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout)
{
    return DoWriteSome(boost::asio::buffer(arrBuffer), handler, timeout);
}

// ===============================================================================================================================

// -------------------------------------------------------------------------------------------------------------------------------
// Read
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
std::size_t wxSerialPort::DoRead(const boost::asio::mutable_buffer& buffer)
{
    boost::system::error_code error;
    std::size_t ret = boost::asio::read(m_serialPort, buffer, error);
    DoSetLastError(error);
    return ret;
}

std::size_t wxSerialPort::Read(std::string& strBuffer)
{
    return DoRead(boost::asio::buffer(strBuffer));
}

std::size_t wxSerialPort::Read(wxCharBuffer& chBuffer)
{
    return DoRead(boost::asio::buffer(chBuffer.data(), chBuffer.length()));
}

std::size_t wxSerialPort::Read(void *pszBuffer, const std::size_t uiSize)
{
    return DoRead(boost::asio::buffer(pszBuffer, uiSize));
}

template <typename type>
std::size_t wxSerialPort::Read(std::vector<type>& vBuffer)
{
    return DoRead(boost::asio::buffer(vBuffer));
}

template <typename type, std::size_t uiSize>
std::size_t wxSerialPort::Read(std::array<type, uiSize>& arrBuffer)
{
    return DoRead(boost::asio::buffer(arrBuffer));
}

// -------------------------------------------------------------------------------------------------------------------------------
// Read with timeout
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
std::size_t wxSerialPort::DoRead(const boost::asio::mutable_buffer& buffer, const int timeout)
{
    std::size_t bytes_read = 0;
    m_io_context.restart();
    DoAsyncWaitThenCancelAsyncIO(timeout);

    boost::asio::async_read(m_serialPort, buffer,
    /*std::bind(&wxSerialPort::AsyncReadHandler, this, std::placeholders::_1, std::placeholders::_2)*/
    [&](const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        // Read operation was not aborted
        if (!error) // != boost::asio::error::operation_aborted
            m_timer.cancel();

        bytes_read = bytes_transferred;
        DoSetLastError(error);
    });

    std::thread thread1([this] { m_io_context.run(); });
    thread1.join();
    return bytes_read;
}

std::size_t wxSerialPort::Read(std::string& strBuffer, const int timeout)
{
    return DoRead(boost::asio::buffer(strBuffer), timeout);
}

std::size_t wxSerialPort::Read(wxCharBuffer& chBuffer, const int timeout)
{
    return DoRead(boost::asio::buffer(chBuffer.data(), chBuffer.length()), timeout);
}

std::size_t wxSerialPort::Read(void *pszBuffer, const std::size_t uiSize, const int timeout)
{
    return DoRead(boost::asio::buffer(pszBuffer, uiSize), timeout);
}

template <typename type>
std::size_t wxSerialPort::Read(std::vector<type>& vBuffer, const int timeout)
{
    return DoRead(boost::asio::buffer(vBuffer), timeout);
}

template <typename type, std::size_t uiSize>
std::size_t wxSerialPort::Read(std::array<type, uiSize>& arrBuffer, const int timeout)
{
    return DoRead(boost::asio::buffer(arrBuffer), timeout);
}

// -------------------------------------------------------------------------------------------------------------------------------
// Write
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
std::size_t wxSerialPort::DoWrite(const boost::asio::const_buffer& buffer)
{
    boost::system::error_code error;
    std::size_t ret = boost::asio::write(m_serialPort, buffer, error);
    DoSetLastError(error);
    return ret;
}

std::size_t wxSerialPort::Write(const std::string& strBuffer)
{
    return DoWrite(boost::asio::buffer(strBuffer));
}

std::size_t wxSerialPort::Write(const wxString& strBuffer)
{
    return DoWrite(boost::asio::buffer(strBuffer.data(), strBuffer.size()));
}

std::size_t wxSerialPort::Write(const wxCharBuffer& chBuffer)
{
    return DoWrite(boost::asio::buffer(chBuffer.data(), chBuffer.length()));
}

std::size_t wxSerialPort::Write(const void *pszBuffer, const std::size_t uiSize)
{
    return DoWrite(boost::asio::buffer(pszBuffer, uiSize));
}

template <typename type>
std::size_t wxSerialPort::Write(const std::vector<type>& vBuffer)
{
    return DoWrite(boost::asio::buffer(vBuffer));
}

template <typename type, std::size_t uiSize>
std::size_t wxSerialPort::Write(const std::array<type, uiSize>& arrBuffer)
{
    return DoWrite(boost::asio::buffer(arrBuffer));
}

// -------------------------------------------------------------------------------------------------------------------------------
// Write with timeout
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
std::size_t wxSerialPort::DoWrite(const boost::asio::const_buffer& buffer, const int timeout)
{
    std::size_t bytes_written = 0;
    m_io_context.restart();
    DoAsyncWaitThenCancelAsyncIO(timeout);

    boost::asio::async_write(m_serialPort, buffer,
    /*std::bind(&wxSerialPort::AsyncWriteHandler, this, std::placeholders::_1, std::placeholders::_2)*/
    [&](const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        // Write operation was not aborted
        if (!error) // != boost::asio::error::operation_aborted
            m_timer.cancel();

        bytes_written = bytes_transferred;
        DoSetLastError(error);
    });

    std::thread thread1([this] { m_io_context.run(); });
    thread1.join();
    return bytes_written;
}

std::size_t wxSerialPort::Write(const std::string& strBuffer, const int timeout)
{
    return DoWrite(boost::asio::buffer(strBuffer), timeout);
}

std::size_t wxSerialPort::Write(const wxString& strBuffer, const int timeout)
{
    return DoWrite(boost::asio::buffer(strBuffer.data(), strBuffer.size()), timeout);
}

std::size_t wxSerialPort::Write(const wxCharBuffer& chBuffer, const int timeout)
{
    return DoWrite(boost::asio::buffer(chBuffer.data(), chBuffer.length()), timeout);
}

std::size_t wxSerialPort::Write(const void *pszBuffer, const std::size_t uiSize, const int timeout)
{
    return DoWrite(boost::asio::buffer(pszBuffer, uiSize), timeout);
}

template <typename type>
std::size_t wxSerialPort::Write(const std::vector<type>& vBuffer, const int timeout)
{
    return DoWrite(boost::asio::buffer(vBuffer), timeout);
}

template <typename type, std::size_t uiSize>
std::size_t wxSerialPort::Write(const std::array<type, uiSize>& arrBuffer, const int timeout)
{
    return DoWrite(boost::asio::buffer(arrBuffer), timeout);
}

// ===============================================================================================================================

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncRead
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::DoAsyncRead(const boost::asio::mutable_buffer& buffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
{
    return boost::asio::async_read(m_serialPort, buffer, handler);
}

template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncRead(std::string& strBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
{
    return DoAsyncRead(boost::asio::buffer(strBuffer), handler);
}

template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncRead(wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
{
    return DoAsyncRead(boost::asio::buffer(chBuffer.data(), chBuffer.length()), handler);
}

template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncRead(void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
{
    return DoAsyncRead(boost::asio::buffer(pszBuffer, uiSize), handler);
}

template <typename type, typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncRead(std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
{
    return DoAsyncRead(boost::asio::buffer(vBuffer), handler);
}

template <typename type, std::size_t uiSize, typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncRead(std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
{
    return DoAsyncRead(boost::asio::buffer(arrBuffer), handler);
}

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncRead with timeout
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::DoAsyncRead(const boost::asio::mutable_buffer& buffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout)
{
    DoAsyncWaitThenCancelAsyncIO(timeout);
    return boost::asio::async_read(m_serialPort, buffer, handler);
}

template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncRead(std::string& strBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout)
{
    return DoAsyncRead(boost::asio::buffer(strBuffer), handler, timeout);
}

template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncRead(wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout)
{
    return DoAsyncRead(boost::asio::buffer(chBuffer.data(), chBuffer.length()), handler, timeout);
}

template <typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncRead(void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout)
{
    return DoAsyncRead(boost::asio::buffer(pszBuffer, uiSize), handler, timeout);
}

template <typename type, typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncRead(std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout)
{
    return DoAsyncRead(boost::asio::buffer(vBuffer), handler, timeout);
}

template <typename type, std::size_t uiSize, typename ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncRead(std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout)
{
    return DoAsyncRead(boost::asio::buffer(arrBuffer), handler, timeout);
}

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncWrite
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::DoAsyncWrite(const boost::asio::const_buffer& buffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
{
    return boost::asio::async_write(m_serialPort, buffer, handler);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWrite(const std::string& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
{
    return DoWrite(boost::asio::buffer(strBuffer), handler);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWrite(const wxString& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
{
    return DoWrite(boost::asio::buffer(strBuffer.data(), strBuffer.size()), handler);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWrite(const wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
{
    return DoWrite(boost::asio::buffer(chBuffer.data(), chBuffer.length()), handler);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWrite(const void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
{
    return DoWrite(boost::asio::buffer(pszBuffer, uiSize), handler);
}

template <typename type, typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWrite(const std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
{
    return DoWrite(boost::asio::buffer(vBuffer), handler);
}

template <typename type, std::size_t uiSize, typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWrite(const std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
{
    return DoWrite(boost::asio::buffer(arrBuffer), handler);
}

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncWrite with timeout
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::DoAsyncWrite(const boost::asio::const_buffer& buffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout)
{
    DoAsyncWaitThenCancelAsyncIO(timeout);
    return boost::asio::async_write(m_serialPort, buffer, handler);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWrite(const std::string& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout)
{
    return DoWrite(boost::asio::buffer(strBuffer), handler, timeout);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWrite(const wxString& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout)
{
    return DoWrite(boost::asio::buffer(strBuffer.data(), strBuffer.size()), handler, timeout);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWrite(const wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout)
{
    return DoWrite(boost::asio::buffer(chBuffer.data(), chBuffer.length()), handler, timeout);
}

template <typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWrite(const void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout)
{
    return DoWrite(boost::asio::buffer(pszBuffer, uiSize), handler, timeout);
}

template <typename type, typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWrite(const std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout)
{
    return DoWrite(boost::asio::buffer(vBuffer), handler, timeout);
}

template <typename type, std::size_t uiSize, typename WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncWrite(const std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout)
{
    return DoWrite(boost::asio::buffer(arrBuffer), handler, timeout);
}

// ===============================================================================================================================

// -------------------------------------------------------------------------------------------------------------------------------
// ReadUntil
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
std::size_t wxSerialPort::DoReadUntil(const boost::asio::mutable_buffer& buffer, const boost::asio::const_buffer& delim)
{
    boost::system::error_code error;
    std::size_t ret = boost::asio::read(m_serialPort, buffer,
    [&](const boost::system::error_code& error, std::size_t bytes_transferred) -> std::size_t
    {
        return DoCompletionCondition(buffer, delim, error, bytes_transferred);
    }, error);
    DoSetLastError(error);
    return ret;
}

std::size_t wxSerialPort::ReadUntil(std::string& strBuffer, const std::string& strDelim)
{
    return DoReadUntil(boost::asio::buffer(strBuffer), boost::asio::buffer(strDelim));
}

std::size_t wxSerialPort::ReadUntil(wxCharBuffer& chBuffer, const wxCharBuffer& chDelim)
{
    return DoReadUntil(boost::asio::buffer(chBuffer.data(), chBuffer.length()), boost::asio::buffer(chDelim.data(), chDelim.length()));
}

std::size_t wxSerialPort::ReadUntil(void *pszBuffer, const std::size_t uiSize1, const void *pszDelim, const std::size_t uiSize2)
{
    return DoReadUntil(boost::asio::buffer(pszBuffer, uiSize1), boost::asio::buffer(pszDelim, uiSize2));
}

template <typename type>
std::size_t wxSerialPort::ReadUntil(std::vector<type>& vBuffer, const std::vector<type>& vDelim)
{
    return DoReadUntil(boost::asio::buffer(vBuffer), boost::asio::buffer(vDelim));
}

template <typename type, std::size_t uiSize1, std::size_t uiSize2>
std::size_t wxSerialPort::ReadUntil(std::array<type, uiSize1>& arrBuffer, const std::array<type, uiSize2>& arrDelim)
{
    return DoReadUntil(boost::asio::buffer(arrBuffer), boost::asio::buffer(arrDelim));
}

// -------------------------------------------------------------------------------------------------------------------------------
// ReadUntil with timeout
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
std::size_t wxSerialPort::DoReadUntil(const boost::asio::mutable_buffer& buffer, const boost::asio::const_buffer& delim, const int timeout)
{
    std::size_t bytes_read = 0;
    m_io_context.restart();
    DoAsyncWaitThenCancelAsyncIO(timeout);

    boost::asio::async_read(m_serialPort, buffer,
    [&](const boost::system::error_code& error, std::size_t bytes_transferred) -> std::size_t
    {
        return DoCompletionCondition(buffer, delim, error, bytes_transferred);
    },
    /*std::bind(&wxSerialPort::AsyncReadUntilHandler, this, std::placeholders::_1, std::placeholders::_2)*/
    [&](const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        // ReadUntil operation was not aborted
        if (!error) // != boost::asio::error::operation_aborted
            m_timer.cancel();

        bytes_read = bytes_transferred;
        DoSetLastError(error);
    });

    std::thread thread1([this] { m_io_context.run(); });
    thread1.join();
    return bytes_read;
}

std::size_t wxSerialPort::ReadUntil(std::string& strBuffer, const std::string& strDelim, const int timeout)
{
    return DoReadUntil(boost::asio::buffer(strBuffer), boost::asio::buffer(strDelim), timeout);
}

std::size_t wxSerialPort::ReadUntil(wxCharBuffer& chBuffer, const wxCharBuffer& chDelim, const int timeout)
{
    return DoReadUntil(boost::asio::buffer(chBuffer.data(), chBuffer.length()), boost::asio::buffer(chDelim.data(), chDelim.length()), timeout);
}

std::size_t wxSerialPort::ReadUntil(void *pszBuffer, const std::size_t uiSize1, const void *pszDelim, const std::size_t uiSize2, const int timeout)
{
    return DoReadUntil(boost::asio::buffer(pszBuffer, uiSize1), boost::asio::buffer(pszDelim, uiSize2), timeout);
}

template <typename type>
std::size_t wxSerialPort::ReadUntil(std::vector<type>& vBuffer, const std::vector<type>& vDelim, const int timeout)
{
    return DoReadUntil(boost::asio::buffer(vBuffer), boost::asio::buffer(vDelim), timeout);
}

template <typename type, std::size_t uiSize1, std::size_t uiSize2>
std::size_t wxSerialPort::ReadUntil(std::array<type, uiSize1>& arrBuffer, const std::array<type, uiSize2>& arrDelim, const int timeout)
{
    return DoReadUntil(boost::asio::buffer(arrBuffer), boost::asio::buffer(arrDelim), timeout);
}

// ===============================================================================================================================

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncReadUntil
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
template <typename ReadUntilHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::DoAsyncReadUntil(const boost::asio::mutable_buffer& buffer, const boost::asio::const_buffer& delim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler)
{
    return boost::asio::async_read(m_serialPort, buffer,
    [&](const boost::system::error_code& error, std::size_t bytes_transferred) -> std::size_t
    {
        return DoCompletionCondition(buffer, delim, error, bytes_transferred);
    }, handler);
}

template <typename ReadUntilHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadUntil(std::string& strBuffer, const std::string& strDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler)
{
    return DoAsyncReadUntil(boost::asio::buffer(strBuffer), boost::asio::buffer(strDelim), handler);
}

template <typename ReadUntilHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadUntil(wxCharBuffer& chBuffer, const wxCharBuffer& chDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler)
{
    return DoAsyncReadUntil(boost::asio::buffer(chBuffer.data(), chBuffer.length()), boost::asio::buffer(chDelim.data(), chDelim.length()), handler);
}

template <typename ReadUntilHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadUntil(void *pszBuffer, const std::size_t uiSize1, const void *pszDelim, const std::size_t uiSize2, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler)
{
    return DoAsyncReadUntil(boost::asio::buffer(pszBuffer, uiSize1), boost::asio::buffer(pszDelim, uiSize2), handler);
}

template <typename type, typename ReadUntilHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadUntil(std::vector<type>& vBuffer, const std::vector<type>& vDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler)
{
    return DoAsyncReadUntil(boost::asio::buffer(vBuffer), boost::asio::buffer(vDelim), handler);
}

template <typename type, std::size_t uiSize1, std::size_t uiSize2, typename ReadUntilHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadUntil(std::array<type, uiSize1>& arrBuffer, const std::array<type, uiSize2>& arrDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler)
{
    return DoAsyncReadUntil(boost::asio::buffer(arrBuffer), boost::asio::buffer(arrDelim), handler);
}

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncReadUntil with timeout
// -------------------------------------------------------------------------------------------------------------------------------

// This helper function is intended for internal use by the class itself
template <typename ReadUntilHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::DoAsyncReadUntil(const boost::asio::mutable_buffer& buffer, const boost::asio::const_buffer& delim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler, const int timeout)
{
    DoAsyncWaitThenCancelAsyncIO(timeout);
    return boost::asio::async_read(m_serialPort, buffer,
    [&](const boost::system::error_code& error, std::size_t bytes_transferred) -> std::size_t
    {
        return DoCompletionCondition(buffer, delim, error, bytes_transferred);
    }, handler);
}

template <typename ReadUntilHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadUntil(std::string& strBuffer, const std::string& strDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler, const int timeout)
{
    return DoAsyncReadUntil(boost::asio::buffer(strBuffer), boost::asio::buffer(strDelim), handler, timeout);
}

template <typename ReadUntilHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadUntil(wxCharBuffer& chBuffer, const wxCharBuffer& chDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler, const int timeout)
{
    return DoAsyncReadUntil(boost::asio::buffer(chBuffer.data(), chBuffer.length()), boost::asio::buffer(chDelim.data(), chDelim.length()), handler, timeout);
}

template <typename ReadUntilHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadUntil(void *pszBuffer, const std::size_t uiSize1, const void *pszDelim, const std::size_t uiSize2, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler, const int timeout)
{
    return DoAsyncReadUntil(boost::asio::buffer(pszBuffer, uiSize1), boost::asio::buffer(pszDelim, uiSize2), handler, timeout);
}

template <typename type, typename ReadUntilHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadUntil(std::vector<type>& vBuffer, const std::vector<type>& vDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler, const int timeout)
{
    return DoAsyncReadUntil(boost::asio::buffer(vBuffer), boost::asio::buffer(vDelim), handler, timeout);
}

template <typename type, std::size_t uiSize1, std::size_t uiSize2, typename ReadUntilHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
wxSerialPort::AsyncReadUntil(std::array<type, uiSize1>& arrBuffer, const std::array<type, uiSize2>& arrDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler, const int timeout)
{
    return DoAsyncReadUntil(boost::asio::buffer(arrBuffer), boost::asio::buffer(arrDelim), handler, timeout);
}

// ===============================================================================================================================

// This helper function is intended for internal use by the class itself
std::size_t wxSerialPort::DoCompletionCondition(const boost::asio::mutable_buffer& buffer, const boost::asio::const_buffer& delim,
                                                const boost::system::error_code& error, const std::size_t bytes_transferred) const
{
    // Look for a match
    auto it = std::search(boost::asio::buffers_begin(buffer), boost::asio::buffers_begin(buffer) + bytes_transferred,
                          boost::asio::buffers_begin(delim), boost::asio::buffers_end(delim));

    return (!!error || it != (boost::asio::buffers_begin(buffer) + bytes_transferred)) ? 0
    : boost::asio::detail::default_max_transfer_size_t::default_max_transfer_size;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::Cancel()
{
    boost::system::error_code error;
    m_serialPort.cancel(error);
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::Close()
{
    boost::system::error_code error;
    m_serialPort.close(error);
    DoSetLastError(error);
    return error;
}

boost::asio::serial_port::executor_type wxSerialPort::GetExecutor() BOOST_ASIO_NOEXCEPT
{
    return m_serialPort.get_executor();
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::GetOption(BaudRate& option)
{
    boost::asio::serial_port_base::baud_rate baudrate;
    boost::system::error_code error;
    m_serialPort.get_option(baudrate, error);
    option = static_cast<BaudRate>(baudrate.value());
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::GetOption(DataBits& option)
{
    boost::asio::serial_port_base::character_size databits;
    boost::system::error_code error;
    m_serialPort.get_option(databits, error);
    option = static_cast<DataBits>(databits.value());
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::GetOption(FlowControl& option)
{
    boost::asio::serial_port_base::flow_control flowcontrol;
    boost::system::error_code error;
    m_serialPort.get_option(flowcontrol, error);
    option = static_cast<FlowControl>(flowcontrol.value());
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::GetOption(Parity& option)
{
    boost::asio::serial_port_base::parity parity;
    boost::system::error_code error;
    m_serialPort.get_option(parity, error);
    option = static_cast<Parity>(parity.value());
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::GetOption(StopBits& option)
{
    boost::asio::serial_port_base::stop_bits stopbits;
    boost::system::error_code error;
    m_serialPort.get_option(stopbits, error);
    option = static_cast<StopBits>(stopbits.value());
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::SetOption(const BaudRate option)
{
    boost::asio::serial_port_base::baud_rate baudrate(option);

    boost::system::error_code error;
    m_serialPort.set_option(baudrate, error);
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::SetOption(const DataBits option)
{
    boost::asio::serial_port_base::character_size databits(option);

    boost::system::error_code error;
    m_serialPort.set_option(databits, error);
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::SetOption(const FlowControl option)
{
    boost::asio::serial_port_base::flow_control flowcontrol(static_cast<boost::asio::serial_port_base::flow_control::type>(option));

    boost::system::error_code error;
    m_serialPort.set_option(flowcontrol, error);
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::SetOption(const Parity option)
{
    boost::asio::serial_port_base::parity parity(static_cast<boost::asio::serial_port_base::parity::type>(option));

    boost::system::error_code error;
    m_serialPort.set_option(parity, error);
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::SetOption(const StopBits option)
{
    boost::asio::serial_port_base::stop_bits stopbits(static_cast<boost::asio::serial_port_base::stop_bits::type>(option));

    boost::system::error_code error;
    m_serialPort.set_option(stopbits, error);
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::GetBaudRate(BaudRate& baudrate)
{
    return GetOption(baudrate);
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::GetDataBits(DataBits& databits)
{
    return GetOption(databits);
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::GetFlowControl(FlowControl& flowcontrol)
{
    return GetOption(flowcontrol);
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::GetParity(Parity& parity)
{
    return GetOption(parity);
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::GetStopBits(StopBits& stopbits)
{
    return GetOption(stopbits);
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::SetBaudRate(const BaudRate baudrate)
{
    return SetOption(baudrate);
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::SetDataBits(const DataBits databits)
{
    return SetOption(databits);
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::SetFlowControl(const FlowControl flowcontrol)
{
    return SetOption(flowcontrol);
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::SetParity(const Parity parity)
{
    return SetOption(parity);
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::SetStopBits(const StopBits stopbits)
{
    return SetOption(stopbits);
}

bool wxSerialPort::IsOpen() const
{
    return m_serialPort.is_open();
}

boost::asio::serial_port::native_handle_type wxSerialPort::GetNativeHandle()
{
    return m_serialPort.native_handle();
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::Open(const std::string& strDevice)
{
    m_strPortName = strDevice;

    boost::system::error_code error;
    m_serialPort.open(strDevice, error);
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::Open(const wxString& strDevice)
{
    return Open(strDevice.ToStdString());
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::Open(const char *pszDevice)
{
    return Open(pszDevice);
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::ReOpen()
{
    return Open(m_strPortName.ToStdString());
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::SendBreak()
{
    boost::system::error_code error;
#ifdef __WXMSW__
    std::thread thread1([&]
    {
    if (!::SetCommBreak(m_serialPort.native_handle()))
    {
        //wxLogError(wxS("SetCommBreak() failed!"));
        error = boost::system::error_code(::GetLastError(), boost::asio::error::get_system_category());
        return;
    }

    //::Sleep(500);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (!::ClearCommBreak(m_serialPort.native_handle()))
    {
        //wxLogError(wxS("ClearCommBreak() failed!"));
        error = boost::system::error_code(::GetLastError(), boost::asio::error::get_system_category());
        //return;
    }
    });
    thread1.join();
#else // !__WXMSW__
    m_serialPort.send_break(error);
#endif // __WXMSW__
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::Wait(const int timeout)
{
    return DoWait(timeout);
}

template <typename WaitHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WaitHandler, void (boost::system::error_code)) // Return type
wxSerialPort::AsyncWait(WaitHandler handler, const int timeout)
{
    DoSetTimeout(timeout);
    m_io_context.restart();

    auto ret = m_timer.async_wait(handler);
    m_io_context.run();
    return ret;
}

wxString wxSerialPort::GetPortName() const
{
    return m_strPortName;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::FlushBuffers(const Buffers buffers)
{
    wxASSERT(buffers >= buf_In && buffers <= buf_In_Out);

    boost::system::error_code error;
    //std::thread thread1([&]
    //{
#ifdef __WXMSW__
    DWORD dwFlags = 0;

    if (buffers & buf_In)
        dwFlags |= PURGE_RXCLEAR;

    if (buffers & buf_Out)
        dwFlags |= PURGE_TXCLEAR;

    if (!::PurgeComm(m_serialPort.native_handle(), dwFlags))
    {
        //wxLogError(wxS("PurgeComm() failed!"));
        error = boost::system::error_code(::GetLastError(), boost::asio::error::get_system_category());
        //return;
    }
#else // !__WXMSW__
    int flags = 0;

    if (buffers == buf_In_Out)
        flags = TCIOFLUSH;
    else if (buffers == buf_In)
        flags = TCIFLUSH;
    else if (buffers == buf_Out)
        flags = TCOFLUSH;

    if (tcflush(m_serialPort.native_handle(), flags) == -1)
    {
        //wxLogError(wxS("tcflush() failed!"));
        error = boost::system::error_code(errno, boost::asio::error::get_system_category());
        //return;
    }
#endif // __WXMSW__
    //});
    //thread1.join();
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::WaitForBuffers(const Buffers buffers, const int timeout)
{
    wxASSERT(buffers == buf_In || buffers == buf_Out);
    DoSetTimeout(timeout);

    boost::system::error_code error;
    m_io_context.restart();

    m_timer.async_wait([&](const boost::system::error_code& error)
    {
        // Timed out
        if (!error) // != boost::asio::error::operation_aborted
        {
            m_io_context.stop();
            DoSetLastError(boost::asio::error::timed_out);
        }
    });

    boost::asio::post(m_io_context, [&]
    {
    for (;;)
    {
        // Have we timed out already?
        if (m_io_context.poll()) // m_io_context.stopped()
            return;

        int nNumByInBuf = 0;
        int nNumByOutBuf = 0;
#ifdef __WXMSW__
        COMSTAT comstat;

        if (!::ClearCommError(m_serialPort.native_handle(), nullptr, &comstat))
        {
            //wxLogError(wxS("ClearCommError() failed!"));
            error = boost::system::error_code(::GetLastError(), boost::asio::error::get_system_category());
            return;
        }

        nNumByInBuf = comstat.cbInQue;
        nNumByOutBuf = comstat.cbOutQue;
#else // !__WXMSW__

        int ret = 0;

        if (buffers == buf_In)
            ret = ioctl(m_serialPort.native_handle(), FIONREAD /*TIOCINQ*/, &nNumByInBuf);
        else if (buffers == buf_Out)
            ret = ioctl(m_serialPort.native_handle(), TIOCOUTQ, &nNumByOutBuf);

        if (ret == -1)
        {
            //wxLogError(wxS("ioctl() failed!"));
            error = boost::system::error_code(errno, boost::asio::error::get_system_category());
            return;
        }
#endif // __WXMSW__

        if (((buffers & buf_In) && nNumByInBuf) || ((buffers & buf_Out) && nNumByOutBuf))
        {
            m_io_context.stop();
            m_timer.cancel();
            return;
        }

        std::this_thread::yield();
    }
    });

    std::thread thread1([this] { m_io_context.run(); });
    thread1.join();
    DoSetLastError(error);
    return error;
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::WaitForInBuffer(const int timeout)
{
    return WaitForBuffers(buf_In, timeout);
}

BOOST_ASIO_SYNC_OP_VOID wxSerialPort::WaitForOutBuffer(const int timeout)
{
    return WaitForBuffers(buf_Out, timeout);
}

void wxSerialPort::CancelAsyncIO()
{
    m_serialPort.cancel();
    m_timer.cancel();
}

boost::system::error_code wxSerialPort::GetLastError() const
{
    return m_last_error;
}

// This helper function is intended for internal use by the class itself
void wxSerialPort::DoSetTimeout(const int timeout)
{
    if (timeout == wxTIMEOUT_INFINITE)
        m_timer.expires_at(std::chrono::steady_clock::time_point::max());
    else
        m_timer.expires_from_now(std::chrono::milliseconds(timeout));
}

// This helper function is intended for internal use by the class itself
BOOST_ASIO_SYNC_OP_VOID wxSerialPort::DoWait(const int timeout)
{
    DoSetTimeout(timeout);

    boost::system::error_code error;
    m_timer.wait(error);
    DoSetLastError(error);
    return error;
}

// This helper function is intended for internal use by the class itself
void wxSerialPort::DoAsyncWaitThenCancelAsyncIO(const int timeout)
{
    DoSetTimeout(timeout);

    m_timer.async_wait(/*std::bind(&wxSerialPort::AsyncWaitHandler, this, std::placeholders::_1)*/
    [this](const boost::system::error_code& error)
    {
        // Timed out
        if (!error) // != boost::asio::error::operation_aborted
        {
            boost::system::error_code error;
            m_serialPort.cancel(error);
            DoSetLastError(boost::asio::error::timed_out);
        }
    });
}

// This helper function is intended for internal use by the class itself
void wxSerialPort::DoAsyncWaitThenStopAsyncIO(const int timeout)
{
    DoSetTimeout(timeout);

    m_timer.async_wait([this](const boost::system::error_code& error)
    {
        // Timed out
        if (!error) // != boost::asio::error::operation_aborted
        {
            m_io_context.stop();
            DoSetLastError(boost::asio::error::timed_out);
        }
    });
}

// This helper function is intended for internal use by the class itself
void wxSerialPort::DoSetLastError(const boost::system::error_code& error)
{
    wxCriticalSectionLocker lock(m_csLastError);
    m_last_error = error;
    OnError();
}

// Async read handler
//void wxSerialPort::AsyncReadHandler(const boost::system::error_code& error, std::size_t bytes_transferred)
//{
//    // Read operation was not aborted
//    if (!error) // != boost::asio::error::operation_aborted
//        m_timer.cancel();
//
//    {
//        wxCriticalSectionLocker lock(m_csBytesRead);
//        m_bytes_read = bytes_transferred;
//    }
//
//    DoSetLastError(error);
//}

// Async write handler
//void wxSerialPort::AsyncWriteHandler(const boost::system::error_code& error, std::size_t bytes_transferred)
//{
//    // Write operation was not aborted
//    if (!error) // != boost::asio::error::operation_aborted
//        m_timer.cancel();
//
//    {
//        wxCriticalSectionLocker lock(m_csBytesWritten);
//        m_bytes_written = bytes_transferred;
//    }
//
//    DoSetLastError(error);
//}

// Async wait handler
//void wxSerialPort::AsyncWaitHandler(const boost::system::error_code& error)
//{
//    // Timed out
//    if (!error) // != boost::asio::error::operation_aborted
//        m_serialPort.cancel();
//
//    DoSetLastError(error);
//}

void wxSerialPort::Init()
{
    //m_bytes_read = 0;
    //m_bytes_written = 0;
    m_last_error = boost::system::error_code(); // 0
}
