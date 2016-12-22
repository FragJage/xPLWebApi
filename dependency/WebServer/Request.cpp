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
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include "Request.h"
#include "StringTools/StringTools.h"

using namespace std;

Request::Request()
{
}

Request::Request(const std::string& requestString)
{
    if(requestString!="") ParseRequest(requestString);
}

Request::~Request()
{
}

string Request::GetQuery()
{
    return m_Query;
}

string Request::GetBody()
{
    return m_Body;
}

Request::HttpMethod Request::GetMethod()
{
    return m_Method;
}

bool Request::ParseRequest(const string& requestString)
{
    istringstream f(requestString);
    ostringstream body;
    string line, key, value;


    m_Query = requestString;
    m_Body = "";
    m_Header.clear();

    if(!getline(f, line)) return false;
    if(!ParseQuery(line)) return false;

    while (getline(f, line))
    {
        StringTools::ParasitCar(line);
        if(line=="") break;
        if(StringTools::Split(line, ':', &key, &value))
        {
            m_Header[key] = value;
        }
    }

    while (getline(f, line))
    {
        StringTools::ParasitCar(line);
        body << line << endl;
    }
    m_Body = body.str();

    return true;
}

bool Request::ParseQuery(const string& encodedQuery)
{
	size_t pos1, pos2;
	string method;
	string query(encodedQuery);

    StringTools::ParasitCar(query);
    query = UrlDecode(query);
	pos1 = query.find(' ');
	if(pos1==string::npos) return false;
	pos2 = query.find_last_of(' ');
	if(pos2==string::npos) return false;

    method = query.substr(0, pos1);
    m_Uri = query.substr(pos1+1, pos2-pos1-1);
    m_Protocol = query.substr(pos2+1);
    m_UriParameters = "";

	pos1 = m_Uri.find('?');
	if(pos1!=string::npos)
	{
        m_UriParameters = m_Uri.substr(pos1+1);
        m_Uri = m_Uri.substr(0, pos1);
        ParseParameters(m_UriParameters);
	}

    if(method=="GET")
        m_Method = Request::mthGET;
    else if(method=="POST")
        m_Method = Request::mthPOST;
    else if(method=="PUT")
        m_Method = Request::mthPUT;
    else if(method=="DELETE")
        m_Method = Request::mthDELETE;
    else if(method=="PATCH")
        m_Method = Request::mthPATCH;
    else if(method=="HEAD")
        m_Method = Request::mthHEAD;
    else m_Method = Request::mthUNKNOWN;

    return true;
}

void Request::ParseParameters(const string& parameters)
{
    string parameter;
    string remainder;
    string key;
    string value;

    remainder = parameters;

    while(remainder!="")
    {
        if(!StringTools::Split(remainder, '&', &parameter, &remainder))
        {
            parameter = remainder;
            remainder = "";
        }
        if(!StringTools::Split(parameter, '=', &key, &value))
        {
            key = parameter;
            value = "";
        }
        SetParameter(key, value);
    }
}

string Request::GetHeader(string headerKey)
{
    if(m_Header.find(headerKey) == m_Header.end()) return "";
    return m_Header[headerKey];
}

bool Request::ExistsParameter(string key)
{
    return (m_Parameters.find(key) != m_Parameters.end());
}

string Request::GetParameter(string key)
{
    if(m_Parameters.find(key) == m_Parameters.end()) return "";
    return m_Parameters[key];
}

void Request::SetParameter(const string& key, const string& value)
{
    m_Parameters[key] = value;
}

string Request::GetUri()
{
    return m_Uri;
}

string Request::UrlDecode(const string& Url)
{
    ostringstream ret;
	char ch;
	int len, i, j;


    len = Url.length();
	for (i=0; i<len; i++)
    {
		if (Url[i]=='%')
        {
			j = strtoul(Url.substr(i+1,2).c_str(), nullptr, 16);
			ch=static_cast<char>(j);
			ret << ch;
			i=i+2;
		}
		else
        {
			ret << Url[i];
		}
	}
	return ret.str();
}
