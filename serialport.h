////////////////////////////////////////////////////////////////////////////////
// Name:      serialport.h
// Purpose:   Defines wxSerialPort Class
// Author:    Youcef Kouchkar
// Created:   2018-01-08
// Copyright: (c) 2019 Youcef Kouchkar
// License:   MIT License
////////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SERIALPORT_H_
#define _WX_SERIALPORT_H_

//#define BOOST_ASIO_DISABLE_STD_CHRONO
//#define BOOST_ASIO_HAS_MOVE
//#define BOOST_ASIO_NO_DEPRECATED

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
//#include <boost/chrono/chrono.hpp>
#include <boost/algorithm/string/predicate.hpp>
//#include <boost/regex.hpp>

#define wxTIMEOUT_INFINITE -1

// ===============================================================================================================================

// -------------------------------------------------------------------------------------------------------------------------------
// wxSerialPortBase
// -------------------------------------------------------------------------------------------------------------------------------

class wxSerialPortBase
{
public:
    // Gettable wxSerialPort options
#ifdef __WXMSW__
    typedef enum /*_BaudRate*/ { br_75 = 75, br_110 = 110, br_134_5 = 134/*.5*/, br_150 = 150, br_300 = 300, br_600 = 600,
                                 br_1200 = 1200, br_1800 = 1800, br_2400 = 2400, br_4800 = 4800, br_7200 = 7200, br_9600 = 9600,
                                 br_14400 = 14400, br_19200 = 19200, br_38400 = 38400, br_56000 = 56000, br_57600 = 57600,
                                 br_128000 = 128000, br_115200 = 115200 } BaudRate;
#else // !__WXMSW__
    typedef enum /*_BaudRate*/ { br_0 = 0, br_50 = 50, br_75 = 75, br_110 = 110, br_134 = 134, br_150 = 150, br_200 = 200,
                                 br_300 = 300, br_600 = 600, br_1200 = 1200, br_1800 = 1800, br_2400 = 2400, br_4800 = 4800,
                                 br_9600 = 9600, br_19200 = 19200, br_38400 = 38400, br_57600 = 57600, br_115200 = 115200,
                                 br_230400 = 230400, br_460800 = 460800, br_500000 = 500000, br_576000 = 576000, br_921600 = 921600,
                                 br_1000000 = 1000000, br_1152000 = 1152000, br_1500000 = 1500000, br_2000000 = 2000000,
                                 br_2500000 = 2500000, br_3000000 = 3000000, br_3500000 = 3500000, br_4000000 = 4000000 } BaudRate;
#endif // __WXMSW__
    typedef enum /*_DataBits*/ { db_Five = 5, db_Six = 6, db_Seven = 7, db_Eight = 8,
                                 db_5    = 5, db_6   = 6, db_7     = 7, db_8     = 8 } DataBits;
    typedef enum /*_FlowControl*/ { fc_None, fc_Software, fc_Hardware } FlowControl;
    typedef enum /*_Parity*/ { p_None, p_Odd, p_Even } Parity;
    typedef enum /*_StopBits*/ { sb_One, sb_One_Five, sb_Two, sb_1 = 0, sb_1_5 = 1, sb_2 = 2 } StopBits;
    typedef enum /*_Buffers*/ { buf_In = 1, buf_Out = 2, buf_In_Out = 3 } Buffers;

public:
    wxSerialPortBase();
    ~wxSerialPortBase();

    static std::vector<wxString> GetPortNames();

protected:
    boost::asio::io_context m_io_context;
};

// -------------------------------------------------------------------------------------------------------------------------------
// wxSerialPort
// -------------------------------------------------------------------------------------------------------------------------------

class wxSerialPort : public wxSerialPortBase
{
public:
    wxSerialPort();
    wxSerialPort(const std::string& strDevice);
    wxSerialPort(const wxString& strDevice);
    wxSerialPort(const char *pszDevice);
    wxSerialPort(const boost::asio::serial_port::native_handle_type& native_serial_port);

    // Copy constructor and assignment operator
    wxSerialPort(const wxSerialPort& other);
    wxSerialPort& operator=(const wxSerialPort& other);

    // Move constructor and assignment operator
#ifdef BOOST_ASIO_HAS_MOVE
    wxSerialPort(wxSerialPort&& other);
    wxSerialPort& operator=(wxSerialPort&& other);
#endif // BOOST_ASIO_HAS_MOVE
    virtual ~wxSerialPort();

    boost::asio::serial_port& GetSerialPort(); /*const*/
    BOOST_ASIO_SYNC_OP_VOID Assign(const boost::asio::serial_port::native_handle_type& native_serial_port);

// ===============================================================================================================================

// -------------------------------------------------------------------------------------------------------------------------------
// ReadSome
// -------------------------------------------------------------------------------------------------------------------------------

    std::size_t ReadSome(std::string& strBuffer);
    std::size_t ReadSome(wxCharBuffer& chBuffer);
    std::size_t ReadSome(void *pszBuffer, const std::size_t uiSize);

    template <typename type>
    std::size_t ReadSome(std::vector<type>& vBuffer);

    template <typename type, std::size_t uiSize>
    std::size_t ReadSome(std::array<type, uiSize>& arrBuffer);

// -------------------------------------------------------------------------------------------------------------------------------
// ReadSome with timeout
// -------------------------------------------------------------------------------------------------------------------------------

    std::size_t ReadSome(std::string& strBuffer, const int timeout);
    std::size_t ReadSome(wxCharBuffer& chBuffer, const int timeout);
    std::size_t ReadSome(void *pszBuffer, const std::size_t uiSize, const int timeout);

    template <typename type>
    std::size_t ReadSome(std::vector<type>& vBuffer, const int timeout);

    template <typename type, std::size_t uiSize>
    std::size_t ReadSome(std::array<type, uiSize>& arrBuffer, const int timeout);

// -------------------------------------------------------------------------------------------------------------------------------
// WriteSome
// -------------------------------------------------------------------------------------------------------------------------------

    std::size_t WriteSome(const std::string& strBuffer);
    std::size_t WriteSome(const wxString& strBuffer);
    std::size_t WriteSome(const wxCharBuffer& chBuffer);
    std::size_t WriteSome(const void *pszBuffer, const std::size_t uiSize);

    template <typename type>
    std::size_t WriteSome(const std::vector<type>& vBuffer);

    template <typename type, std::size_t uiSize>
    std::size_t WriteSome(const std::array<type, uiSize>& arrBuffer);

// -------------------------------------------------------------------------------------------------------------------------------
// WriteSome with timeout
// -------------------------------------------------------------------------------------------------------------------------------

    std::size_t WriteSome(const std::string& strBuffer, const int timeout);
    std::size_t WriteSome(const wxString& strBuffer, const int timeout);
    std::size_t WriteSome(const wxCharBuffer& chBuffer, const int timeout);
    std::size_t WriteSome(const void *pszBuffer, const std::size_t uiSize, const int timeout);

    template <typename type>
    std::size_t WriteSome(const std::vector<type>& vBuffer, const int timeout);

    template <typename type, std::size_t uiSize>
    std::size_t WriteSome(const std::array<type, uiSize>& arrBuffer, const int timeout);

// ===============================================================================================================================

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncReadSome
// -------------------------------------------------------------------------------------------------------------------------------

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadSome(std::string& strBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler);

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadSome(wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler);

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadSome(void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(ReadHandler) handler);

    template <typename type, typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadSome(std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler);

    template <typename type, std::size_t uiSize, typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadSome(std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler);

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncReadSome with timeout
// -------------------------------------------------------------------------------------------------------------------------------

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadSome(std::string& strBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout);

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadSome(wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout);

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadSome(void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout);

    template <typename type, typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadSome(std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout);

    template <typename type, std::size_t uiSize, typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadSome(std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout);

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncWriteSome
// -------------------------------------------------------------------------------------------------------------------------------

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWriteSome(const std::string& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWriteSome(const wxString& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWriteSome(const wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWriteSome(const void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(WriteHandler) handler);

    template <typename type, typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWriteSome(const std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler);

    template <typename type, std::size_t uiSize, typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWriteSome(const std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler);

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncWriteSome with timeout
// -------------------------------------------------------------------------------------------------------------------------------

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWriteSome(const std::string& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWriteSome(const wxString& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWriteSome(const wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWriteSome(const void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout);

    template <typename type, typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWriteSome(const std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout);

    template <typename type, std::size_t uiSize, typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWriteSome(const std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout);

// ===============================================================================================================================

// -------------------------------------------------------------------------------------------------------------------------------
// Read
// -------------------------------------------------------------------------------------------------------------------------------

    std::size_t Read(std::string& strBuffer);
    std::size_t Read(wxCharBuffer& chBuffer);
    std::size_t Read(void *pszBuffer, const std::size_t uiSize);

    template <typename type>
    std::size_t Read(std::vector<type>& vBuffer);

    template <typename type, std::size_t uiSize>
    std::size_t Read(std::array<type, uiSize>& arrBuffer);

// -------------------------------------------------------------------------------------------------------------------------------
// Read with timeout
// -------------------------------------------------------------------------------------------------------------------------------

    std::size_t Read(std::string& strBuffer, const int timeout);
    std::size_t Read(wxCharBuffer& chBuffer, const int timeout);
    std::size_t Read(void *pszBuffer, const std::size_t uiSize, const int timeout);

    template <typename type>
    std::size_t Read(std::vector<type>& vBuffer, const int timeout);

    template <typename type, std::size_t uiSize>
    std::size_t Read(std::array<type, uiSize>& arrBuffer, const int timeout);

// -------------------------------------------------------------------------------------------------------------------------------
// Write
// -------------------------------------------------------------------------------------------------------------------------------

    std::size_t Write(const std::string& strBuffer);
    std::size_t Write(const wxString& strBuffer);
    std::size_t Write(const wxCharBuffer& chBuffer);
    std::size_t Write(const void *pszBuffer, const std::size_t uiSize);

    template <typename type>
    std::size_t Write(const std::vector<type>& vBuffer);

    template <typename type, std::size_t uiSize>
    std::size_t Write(const std::array<type, uiSize>& arrBuffer);

// -------------------------------------------------------------------------------------------------------------------------------
// Write with timeout
// -------------------------------------------------------------------------------------------------------------------------------

    std::size_t Write(const std::string& strBuffer, const int timeout);
    std::size_t Write(const wxString& strBuffer, const int timeout);
    std::size_t Write(const wxCharBuffer& chBuffer, const int timeout);
    std::size_t Write(const void *pszBuffer, const std::size_t uiSize, const int timeout);

    template <typename type>
    std::size_t Write(const std::vector<type>& vBuffer, const int timeout);

    template <typename type, std::size_t uiSize>
    std::size_t Write(const std::array<type, uiSize>& arrBuffer, const int timeout);

// ===============================================================================================================================

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncRead
// -------------------------------------------------------------------------------------------------------------------------------

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncRead(std::string& strBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler);

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncRead(wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler);

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncRead(void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(ReadHandler) handler);

    template <typename type, typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncRead(std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler);

    template <typename type, std::size_t uiSize, typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncRead(std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler);

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncRead with timeout
// -------------------------------------------------------------------------------------------------------------------------------

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncRead(std::string& strBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout);

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncRead(wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout);

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncRead(void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout);

    template <typename type, typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncRead(std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout);

    template <typename type, std::size_t uiSize, typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncRead(std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout);

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncWrite
// -------------------------------------------------------------------------------------------------------------------------------

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWrite(const std::string& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWrite(const wxString& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWrite(const wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWrite(const void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(WriteHandler) handler);

    template <typename type, typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWrite(const std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler);

    template <typename type, std::size_t uiSize, typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWrite(const std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler);

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncWrite with timeout
// -------------------------------------------------------------------------------------------------------------------------------

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWrite(const std::string& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWrite(const wxString& strBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWrite(const wxCharBuffer& chBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWrite(const void *pszBuffer, const std::size_t uiSize, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout);

    template <typename type, typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWrite(const std::vector<type>& vBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout);

    template <typename type, std::size_t uiSize, typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncWrite(const std::array<type, uiSize>& arrBuffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout);

// ===============================================================================================================================

// -------------------------------------------------------------------------------------------------------------------------------
// ReadUntil
// -------------------------------------------------------------------------------------------------------------------------------

    std::size_t ReadUntil(std::string& strBuffer, const std::string& strDelim);
    std::size_t ReadUntil(wxCharBuffer& chBuffer, const wxCharBuffer& chDelim);
    std::size_t ReadUntil(void *pszBuffer, const std::size_t uiSize1, const void *pszDelim, const std::size_t uiSize2);

    template <typename type>
    std::size_t ReadUntil(std::vector<type>& vBuffer, const std::vector<type>& vDelim);

    template <typename type, std::size_t uiSize1, std::size_t uiSize2>
    std::size_t ReadUntil(std::array<type, uiSize1>& arrBuffer, const std::array<type, uiSize2>& arrDelim);

// -------------------------------------------------------------------------------------------------------------------------------
// ReadUntil with timeout
// -------------------------------------------------------------------------------------------------------------------------------

    std::size_t ReadUntil(std::string& strBuffer, const std::string& strDelim, const int timeout);
    std::size_t ReadUntil(wxCharBuffer& chBuffer, const wxCharBuffer& chDelim, const int timeout);
    std::size_t ReadUntil(void *pszBuffer, const std::size_t uiSize1, const void *pszDelim, const std::size_t uiSize2, const int timeout);

    template <typename type>
    std::size_t ReadUntil(std::vector<type>& vBuffer, const std::vector<type>& vDelim, const int timeout);

    template <typename type, std::size_t uiSize1, std::size_t uiSize2>
    std::size_t ReadUntil(std::array<type, uiSize1>& arrBuffer, const std::array<type, uiSize2>& arrDelim, const int timeout);

// ===============================================================================================================================

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncReadUntil
// -------------------------------------------------------------------------------------------------------------------------------

    template <typename ReadUntilHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadUntil(std::string& strBuffer, const std::string& strDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler);

    template <typename ReadUntilHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadUntil(wxCharBuffer& chBuffer, const wxCharBuffer& chDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler);

    template <typename ReadUntilHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadUntil(void *pszBuffer, const std::size_t uiSize1, const void *pszDelim, const std::size_t uiSize2, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler);

    template <typename type, typename ReadUntilHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadUntil(std::vector<type>& vBuffer, const std::vector<type>& vDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler);

    template <typename type, std::size_t uiSize1, std::size_t uiSize2, typename ReadUntilHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadUntil(std::array<type, uiSize1>& arrBuffer, const std::array<type, uiSize2>& arrDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler);

// -------------------------------------------------------------------------------------------------------------------------------
// AsyncReadUntil with timeout
// -------------------------------------------------------------------------------------------------------------------------------

    template <typename ReadUntilHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadUntil(std::string& strBuffer, const std::string& strDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler, const int timeout);

    template <typename ReadUntilHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadUntil(wxCharBuffer& chBuffer, const wxCharBuffer& chDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler, const int timeout);

    template <typename ReadUntilHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadUntil(void *pszBuffer, const std::size_t uiSize1, const void *pszDelim, const std::size_t uiSize2, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler, const int timeout);

    template <typename type, typename ReadUntilHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadUntil(std::vector<type>& vBuffer, const std::vector<type>& vDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler, const int timeout);

    template <typename type, std::size_t uiSize1, std::size_t uiSize2, typename ReadUntilHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
    AsyncReadUntil(std::array<type, uiSize1>& arrBuffer, const std::array<type, uiSize2>& arrDelim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler, const int timeout);

// ===============================================================================================================================

    BOOST_ASIO_SYNC_OP_VOID Cancel();
    BOOST_ASIO_SYNC_OP_VOID Close();
    boost::asio::serial_port::executor_type GetExecutor() BOOST_ASIO_NOEXCEPT;

    BOOST_ASIO_SYNC_OP_VOID GetOption(BaudRate& option);
    BOOST_ASIO_SYNC_OP_VOID GetOption(DataBits& option);
    BOOST_ASIO_SYNC_OP_VOID GetOption(FlowControl& option);
    BOOST_ASIO_SYNC_OP_VOID GetOption(Parity& option);
    BOOST_ASIO_SYNC_OP_VOID GetOption(StopBits& option);

    BOOST_ASIO_SYNC_OP_VOID SetOption(const BaudRate option);
    BOOST_ASIO_SYNC_OP_VOID SetOption(const DataBits option);
    BOOST_ASIO_SYNC_OP_VOID SetOption(const FlowControl option);
    BOOST_ASIO_SYNC_OP_VOID SetOption(const Parity option);
    BOOST_ASIO_SYNC_OP_VOID SetOption(const StopBits option);

    BOOST_ASIO_SYNC_OP_VOID GetBaudRate(BaudRate& baudrate);
    BOOST_ASIO_SYNC_OP_VOID GetDataBits(DataBits& databits);
    BOOST_ASIO_SYNC_OP_VOID GetFlowControl(FlowControl& flowcontrol);
    BOOST_ASIO_SYNC_OP_VOID GetParity(Parity& parity);
    BOOST_ASIO_SYNC_OP_VOID GetStopBits(StopBits& stopbits);

    BOOST_ASIO_SYNC_OP_VOID SetBaudRate(const BaudRate baudrate);
    BOOST_ASIO_SYNC_OP_VOID SetDataBits(const DataBits databits);
    BOOST_ASIO_SYNC_OP_VOID SetFlowControl(const FlowControl flowcontrol);
    BOOST_ASIO_SYNC_OP_VOID SetParity(const Parity parity);
    BOOST_ASIO_SYNC_OP_VOID SetStopBits(const StopBits stopbits);

    bool IsOpen() const;
    boost::asio::serial_port::native_handle_type GetNativeHandle();
    BOOST_ASIO_SYNC_OP_VOID Open(const std::string& strDevice);
    BOOST_ASIO_SYNC_OP_VOID Open(const wxString& strDevice);
    BOOST_ASIO_SYNC_OP_VOID Open(const char *pszDevice);
    BOOST_ASIO_SYNC_OP_VOID ReOpen();
    BOOST_ASIO_SYNC_OP_VOID SendBreak();
    BOOST_ASIO_SYNC_OP_VOID Wait(const int timeout);

    template <typename WaitHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WaitHandler, void (boost::system::error_code)) // Return type
    AsyncWait(WaitHandler handler, const int timeout);
    wxString GetPortName() const;
    BOOST_ASIO_SYNC_OP_VOID FlushBuffers(const Buffers buffers);
    BOOST_ASIO_SYNC_OP_VOID WaitForBuffers(const Buffers buffers, const int timeout);
    BOOST_ASIO_SYNC_OP_VOID WaitForInBuffer(const int timeout);
    BOOST_ASIO_SYNC_OP_VOID WaitForOutBuffer(const int timeout);
    void CancelAsyncIO();
    boost::system::error_code GetLastError() const;

protected:
    // These helper functions are intended for internal use by the class itself
    std::size_t DoReadSome(const boost::asio::mutable_buffer& buffer);
    std::size_t DoReadSome(const boost::asio::mutable_buffer& buffer, const int timeout);
    std::size_t DoWriteSome(const boost::asio::const_buffer& buffer);
    std::size_t DoWriteSome(const boost::asio::const_buffer& buffer, const int timeout);

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    DoAsyncReadSome(const boost::asio::mutable_buffer& buffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler);

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    DoAsyncReadSome(const boost::asio::mutable_buffer& buffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    DoAsyncWriteSome(const boost::asio::const_buffer& buffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    DoAsyncWriteSome(const boost::asio::const_buffer& buffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout);

    std::size_t DoRead(const boost::asio::mutable_buffer& buffer);
    std::size_t DoRead(const boost::asio::mutable_buffer& buffer, const int timeout);
    std::size_t DoWrite(const boost::asio::const_buffer& buffer);
    std::size_t DoWrite(const boost::asio::const_buffer& buffer, const int timeout);

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    DoAsyncRead(const boost::asio::mutable_buffer& buffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler);

    template <typename ReadHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void (boost::system::error_code, std::size_t)) // Return type
    DoAsyncRead(const boost::asio::mutable_buffer& buffer, BOOST_ASIO_MOVE_ARG(ReadHandler) handler, const int timeout);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    DoAsyncWrite(const boost::asio::const_buffer& buffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler);

    template <typename WriteHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void (boost::system::error_code, std::size_t)) // Return type
    DoAsyncWrite(const boost::asio::const_buffer& buffer, BOOST_ASIO_MOVE_ARG(WriteHandler) handler, const int timeout);

    std::size_t DoReadUntil(const boost::asio::mutable_buffer& buffer, const boost::asio::const_buffer& delim);
    std::size_t DoReadUntil(const boost::asio::mutable_buffer& buffer, const boost::asio::const_buffer& delim, const int timeout);

    template <typename ReadUntilHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
    DoAsyncReadUntil(const boost::asio::mutable_buffer& buffer, const boost::asio::const_buffer& delim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler);

    template <typename ReadUntilHandler>
    BOOST_ASIO_INITFN_RESULT_TYPE(ReadUntilHandler, void (boost::system::error_code, std::size_t)) // Return type
    DoAsyncReadUntil(const boost::asio::mutable_buffer& buffer, const boost::asio::const_buffer& delim, BOOST_ASIO_MOVE_ARG(ReadUntilHandler) handler, const int timeout);

    void DoSetTimeout(const int timeout);
    BOOST_ASIO_SYNC_OP_VOID DoWait(const int timeout);
    void DoAsyncWaitThenCancelAsyncIO(const int timeout);
    void DoAsyncWaitThenStopAsyncIO(const int timeout);
    void DoSetLastError(const boost::system::error_code& error);

    // Async read, write and wait handlers
    //void AsyncReadHandler(const boost::system::error_code& error, std::size_t bytes_transferred);
    //void AsyncWriteHandler(const boost::system::error_code& error, std::size_t bytes_transferred);
    //void AsyncWaitHandler(const boost::system::error_code& error);

    std::size_t DoCompletionCondition(const boost::asio::mutable_buffer& buffer, const boost::asio::const_buffer& delim,
                                      const boost::system::error_code& error, const std::size_t bytes_transferred) const;

    // Error handler
    virtual void OnError(/*const boost::system::error_code& error*/) {}

private:
    void Init();

private:
    boost::asio::serial_port m_serialPort;
    boost::asio::steady_timer m_timer;
    wxString m_strPortName;
    //std::size_t m_bytes_read;
    //std::size_t m_bytes_written;
    boost::system::error_code m_last_error;
    wxCriticalSection m_csLastError; // Protects m_last_error
    //wxCriticalSection m_csBytesRead; // Protects m_bytes_read
    //wxCriticalSection m_csBytesWritten; // Protects m_bytes_written
};

#endif // _WX_SERIALPORT_H_
