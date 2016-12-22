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

#ifndef Request_H
#define Request_H

#include <string>
#include <map>

class Request
{
    public:
        enum HttpMethod {mthGET, mthPOST, mthPUT, mthDELETE, mthPATCH, mthHEAD, mthUNKNOWN};
        Request();
        Request(const std::string& requestString);
        ~Request();

        std::string GetHeader(std::string headerKey);
        std::string GetQuery();
        std::string GetUri();
        std::string GetBody();
        HttpMethod GetMethod();
        bool ExistsParameter(std::string key);
        std::string GetParameter(std::string key);
        template <typename T> T GetParameter(std::string key)
        {
            std::string value = GetParameter(key);
            std::istringstream iss(value);
            T val;
            iss >> val;
            return val;
        }
        void SetParameter(const std::string& key, const std::string& value);
        bool ParseRequest(const std::string& requestString);
        std::string UrlDecode(const std::string& Url);


    private:
        bool ParseQuery(const std::string& queryString);
        void ParseParameters(const std::string& parameters);
        HttpMethod m_Method;
        std::string m_Uri;
        std::string m_UriParameters;
        std::string m_Protocol;
        std::string m_Body;
        std::map<std::string, std::string> m_Header;
        std::map<std::string, std::string> m_Parameters;
        std::string m_Query;
};

#endif
