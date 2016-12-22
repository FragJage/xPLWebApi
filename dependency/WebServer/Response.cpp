/*** LICENCE ***************************************************************************************/
/*
  WebServer - Simple class to manage a HTTP web server

  This file is part of WebServer.

    WebServer is free software : you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    WebServer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with WebServer.  If not, see <http://www.gnu.org/licenses/>.
*/
/***************************************************************************************************/

#include <sstream>
#include <cstring>
#include <ctime>
#include "Response.h"

using namespace std;

bool Response::m_StaticInitPerformed = false;
map<int, string> Response::m_MsgStatut;

Response::Response()
{
    m_Buffer=nullptr;
    m_HeaderBuffer=nullptr;
	InitStatic();
	Clear();
}

Response::~Response()
{
    FreeBuffer();
    FreeHeaderBuffer();
}

void Response::InitStatic()
{
    if(m_StaticInitPerformed) return;
    m_StaticInitPerformed = true;
	m_MsgStatut[200] = "OK";
	m_MsgStatut[404] = "Not Found";
	m_MsgStatut[100] = "Continue";
	m_MsgStatut[101] = "Switching Protocols";
	m_MsgStatut[102] = "Processing";
	m_MsgStatut[118] = "Connection timed out";
	m_MsgStatut[200] = "OK";
	m_MsgStatut[201] = "Created";
	m_MsgStatut[202] = "Accepted";
	m_MsgStatut[203] = "Non-Authoritative Information";
	m_MsgStatut[204] = "No Content";
	m_MsgStatut[205] = "Reset Content";
	m_MsgStatut[206] = "Partial Content";
	m_MsgStatut[207] = "Multi-Status";
	m_MsgStatut[210] = "Content Different";
	m_MsgStatut[226] = "IM Used";
	m_MsgStatut[300] = "Multiple Choices";
	m_MsgStatut[301] = "Moved Permanently";
	m_MsgStatut[302] = "Moved Temporarily";
	m_MsgStatut[303] = "See Other";
	m_MsgStatut[304] = "Not Modified";
	m_MsgStatut[305] = "Use Proxy";
	m_MsgStatut[306] = "(none)";
	m_MsgStatut[307] = "Temporary Redirect";
	m_MsgStatut[308] = "Permanent Redirect";
	m_MsgStatut[310] = "Too many Redirects";
	m_MsgStatut[400] = "Bad Request";
	m_MsgStatut[401] = "Unauthorized";
	m_MsgStatut[402] = "Payment Required";
	m_MsgStatut[403] = "Forbidden";
	m_MsgStatut[404] = "Not Found";
	m_MsgStatut[405] = "Method Not Allowed";
	m_MsgStatut[406] = "Not Acceptable";
	m_MsgStatut[407] = "Proxy Authentication Required";
	m_MsgStatut[408] = "Request Time-out";
	m_MsgStatut[409] = "Conflict";
	m_MsgStatut[410] = "Gone";
	m_MsgStatut[411] = "Length Required";
	m_MsgStatut[412] = "Precondition Failed";
	m_MsgStatut[413] = "Request Entity Too Large";
	m_MsgStatut[414] = "Request-URI Too Long";
	m_MsgStatut[415] = "Unsupported Media Type";
	m_MsgStatut[416] = "Requested range unsatisfiable";
	m_MsgStatut[417] = "Expectation failed";
	m_MsgStatut[418] = "I'm a teapot";
	m_MsgStatut[422] = "Unprocessable entity";
	m_MsgStatut[423] = "Locked";
	m_MsgStatut[424] = "Method failure";
	m_MsgStatut[425] = "Unordered Collection";
	m_MsgStatut[426] = "Upgrade Required";
	m_MsgStatut[428] = "Precondition Required";
	m_MsgStatut[429] = "Too Many Requests";
	m_MsgStatut[431] = "Request Header Fields Too Large";
	m_MsgStatut[449] = "Retry With";
	m_MsgStatut[450] = "Blocked by Windows Parental Controls";
	m_MsgStatut[456] = "Unrecoverable Error";
	m_MsgStatut[499] = "client has closed connection";
	m_MsgStatut[500] = "Internal Server Error";
	m_MsgStatut[501] = "Not Implemented";
	m_MsgStatut[502] = "Bad Gateway or Proxy Error";
	m_MsgStatut[503] = "Service Unavailable";
	m_MsgStatut[504] = "Gateway Time-out";
	m_MsgStatut[505] = "HTTP Version not supported";
	m_MsgStatut[506] = "Variant also negociate";
	m_MsgStatut[507] = "Insufficient storage";
	m_MsgStatut[508] = "Loop detected";
	m_MsgStatut[509] = "Bandwidth Limit Exceeded";
	m_MsgStatut[510] = "Not extended";
	m_MsgStatut[520] = "Web server is returning an unknown error";
}

void Response::Clear()
{
    FreeBuffer();
    FreeHeaderBuffer();
    m_Header.clear();
    m_Protocol = "HTTP/1.1";
    m_Statut = 200;
    m_StatutMsg = "OK";
    m_BufferSize = 0;
    m_SendIndex = -1;
}

void Response::FreeBuffer()
{
    if(m_Buffer!=nullptr)
    {
        m_BufferSize = 0;
        delete[] m_Buffer;
        m_Buffer = nullptr;
    }
}

void Response::FreeHeaderBuffer()
{
    if(m_HeaderBuffer!=nullptr)
    {
        delete[] m_HeaderBuffer;
        m_HeaderBuffer = nullptr;
    }
}

void Response::SetStatut(int statut)
{
    m_Statut = statut;
    m_StatutMsg = Response::m_MsgStatut[statut];
}

int Response::GetStatut()
{
    return m_Statut;
}

void Response::SetContent(string content)
{
    FreeBuffer();
    m_BufferSize = content.length();
    m_Buffer = new char[m_BufferSize+1];
    strncpy(m_Buffer, content.c_str(), m_BufferSize);
    m_Buffer[m_BufferSize] = '\0';
}

void Response::SetBuffer(char *buffer, int bufferSize)
{
    FreeBuffer();
    m_BufferSize = bufferSize;
    m_Buffer = new char[m_BufferSize+1];
    memcpy(m_Buffer, buffer, m_BufferSize);
    m_Buffer[m_BufferSize] = '\0';
}

std::string Response::GetHeader(std::string headerKey)
{
    if(m_Header.find(headerKey) == m_Header.end()) return "";
    return m_Header[headerKey];
}

void Response::SetHeader(string key, string value)
{
    m_Header[key] = value;
}

void Response::SetHeader(string key, int value)
{
    ostringstream stream;

    stream << value;
    m_Header[key] = stream.str();
}

void Response::SetHeader(string key, time_t value)
{
    struct tm pDate;
    char mbstr[100];


	#ifdef _MSC_VER
    gmtime_s(&pDate, &value);
	#else
    pDate = *gmtime(&value);
	#endif

    std::strftime(mbstr, 100, "%a, %d %b %Y %H:%M:%S GMT", &pDate);
    m_Header[key] = mbstr;
}

void Response::SetCache(int second)
{
    time_t rawtime;
    ostringstream os;

    os << "max-age=" << second;
    SetHeader("Cache-Control", os.str());

    time(&rawtime);
    SetHeader("Date", rawtime);

    rawtime+=second;
    SetHeader("Expires", rawtime);
}

char* Response::GetSendPointer(int* bufSize)
{
    char* buffer;
    string header;

    if(m_SendIndex==-1)
    {
        ostringstream stream;
        map<std::string, std::string>::iterator it;

        FreeHeaderBuffer();

        stream << m_Protocol <<" "<< m_Statut <<" "<< m_StatutMsg << "\x0D\x0A";
        SetHeader("Content-Length", m_BufferSize);
        for(it=m_Header.begin(); it!=m_Header.end(); ++it)
        {
            stream << it->first <<": "<< it->second << "\x0D\x0A";
        }
        stream << "\x0D\x0A";
        header = stream.str();

        *bufSize = header.length();
        m_HeaderBuffer = new char[*bufSize];
        strncpy(m_HeaderBuffer, header.c_str(), *bufSize);

        m_SendIndex++;
        return m_HeaderBuffer;
    }

    if(m_SendIndex==0)
        FreeHeaderBuffer();

    if(16*1024*m_SendIndex>m_BufferSize)
    {
        m_SendIndex=-1;
        *bufSize = 0;
        return nullptr;
    }

    buffer = m_Buffer+16*1024*m_SendIndex;
    m_SendIndex++;
    if(16*1024*m_SendIndex>m_BufferSize)
        *bufSize = m_BufferSize-(16*1024*(m_SendIndex-1));
    else
        *bufSize = 16*1024;
    return buffer;
}

string Response::ToString()
{
    ostringstream stream;
    map<std::string, std::string>::iterator it;


    stream << m_Protocol <<" "<< m_Statut <<" "<< m_StatutMsg << "\x0D\x0A";
    SetHeader("Content-Length", m_BufferSize);
    for(it=m_Header.begin(); it!=m_Header.end(); ++it)
    {
        stream << it->first <<": "<< it->second << "\x0D\x0A";
    }
    stream << "\x0D\x0A";
    stream << m_Buffer;
    return stream.str();
}

string Response::GetBody()
{
    return m_Buffer;
}
