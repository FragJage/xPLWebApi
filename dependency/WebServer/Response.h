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

#ifndef Reponse_H
#define Reponse_H

#include <string>
#include <map>

class Response
{
    public:
        Response();
        ~Response();
        void Clear();
        void SetStatut(int statut);
        int GetStatut();
        void SetContent(std::string content);
        void SetBuffer(char *buffer, int bufferSize);
        void SetHeader(std::string key, std::string value);
        void SetHeader(std::string key, int value);
        void SetHeader(std::string key, time_t value);
        std::string GetHeader(std::string key);
        void SetCache(int second);
        char* GetSendPointer(int* bufSize);
        std::string ToString();
        std::string GetBody();

    private:
        void FreeBuffer();
        void FreeHeaderBuffer();
        std::string m_Protocol;
        int m_Statut;
        std::string m_StatutMsg;
        std::map<std::string, std::string> m_Header;
        char *m_Buffer;
        char *m_HeaderBuffer;
        int m_BufferSize;
        int m_SendIndex;
        static bool m_StaticInitPerformed;
		static void InitStatic();
        static std::map<int, std::string> m_MsgStatut;
};

#endif
