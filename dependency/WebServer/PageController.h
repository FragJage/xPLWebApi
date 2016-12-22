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

#ifndef PageController_H
#define PageController_H

#include<string>
#include<vector>
#include<map>
#include<sys/stat.h>
#include "IWebController.h"

class PageController : public IWebController
{
    public:
        PageController();
        PageController(const std::string& rootDir);
        ~PageController();

        void SetRootDir(const std::string& rootDir);
        void AddDefaultPage(const std::string& page);
        bool Controller(Request& request, Response* response);

    private:
        enum TypeResource {isFile, isFolder, notExist};
        TypeResource ResourceExist(const std::string& filePath, struct stat* fileInfo);
        bool SearchDefaultPage(std::string& filePath, struct stat* fileInfo);
        void CompleteFolder(std::string& filePath);
        std::string BuildPath(const std::string& uri);
        std::string m_rootDir;
        std::vector<std::string> m_DefaultPages;
		static void InitStatic();
        static std::map<std::string, std::string> m_TypeMime;
};

#endif // PageController_H
