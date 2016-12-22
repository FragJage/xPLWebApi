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
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <sys/types.h>
#include "PageController.h"

using namespace std;

map<string, string> PageController::m_TypeMime;

PageController::PageController()
{
	InitStatic();
}

PageController::PageController(const string& rootDir)
{
	InitStatic();
    SetRootDir(rootDir);
}

void PageController::InitStatic()
{
	if(!m_TypeMime.empty()) return;
	m_TypeMime["html"] = "text/html";
	m_TypeMime["htm"] = "text/html";
	m_TypeMime["shtm"] = "text/html";
	m_TypeMime["shtml"] = "text/html";
	m_TypeMime["css"] = "text/css";
	m_TypeMime["js"] =  "application/x-javascript";
	m_TypeMime["ico"] = "image/x-icon";
	m_TypeMime["gif"] = "image/gif";
	m_TypeMime["jpg"] = "image/jpeg";
	m_TypeMime["jpeg"] = "image/jpeg";
	m_TypeMime["png"] = "image/png";
	m_TypeMime["svg"] = "image/svg+xml";
	m_TypeMime["txt"] = "text/plain";
    m_TypeMime["torrent"] = "application/x-bittorrent";
    m_TypeMime["wav"] = "audio/x-wav";
    m_TypeMime["mp3"] = "audio/x-mp3";
    m_TypeMime["mid"] = "audio/mid";
    m_TypeMime["m3u"] = "audio/x-mpegurl";
    m_TypeMime["ogg"] = "application/ogg";
    m_TypeMime["ram"] = "audio/x-pn-realaudio";
    m_TypeMime["xml"] = "text/xml";
    m_TypeMime["json"] = "application/json";
    m_TypeMime["xslt"] = "application/xml";
    m_TypeMime["xsl"] = "application/xml";
    m_TypeMime["ra"] =  "audio/x-pn-realaudio";
    m_TypeMime["doc"] = "application/msword";
    m_TypeMime["exe"] = "application/octet-stream";
    m_TypeMime["zip"] = "application/x-zip-compressed";
    m_TypeMime["xls"] = "application/excel";
    m_TypeMime["tgz"] = "application/x-tar-gz";
    m_TypeMime["tar"] = "application/x-tar";
    m_TypeMime["gz"] =  "application/x-gunzip";
    m_TypeMime["arj"] = "application/x-arj-compressed";
    m_TypeMime["rar"] = "application/x-rar-compressed";
    m_TypeMime["rtf"] = "application/rtf";
    m_TypeMime["pdf"] = "application/pdf";
    m_TypeMime["swf"] = "application/x-shockwave-flash";
    m_TypeMime["mpg"] = "video/mpeg";
    m_TypeMime["webm"] = "video/webm";
    m_TypeMime["mpeg"] = "video/mpeg";
    m_TypeMime["mov"] = "video/quicktime";
    m_TypeMime["mp4"] = "video/mp4";
    m_TypeMime["m4v"] = "video/x-m4v";
    m_TypeMime["asf"] = "video/x-ms-asf";
    m_TypeMime["avi"] = "video/x-msvideo";
    m_TypeMime["bmp"] = "image/bmp";
    m_TypeMime["ttf"] = "application/x-font-ttf";
}

PageController::~PageController()
{
}

void PageController::SetRootDir(const string& rootDir)
{
    m_rootDir = rootDir;
    CompleteFolder(m_rootDir);
}

void PageController::AddDefaultPage(const string& page)
{
    m_DefaultPages.push_back(page);
}

string PageController::BuildPath(const string& uri)
{
    string filePath = m_rootDir;

    filePath.append(uri);
    if(filePath.back() == '/') filePath.pop_back();
    #ifdef _WIN32
    //replace(filePath.begin(), filePath.end(), '/', '\\');
    #endif

    return filePath;
}

PageController::TypeResource PageController::ResourceExist(const string& filePath, struct stat* fileInfo)
{
    if(stat(filePath.c_str(), fileInfo) != 0) return PageController::notExist;
    if(fileInfo->st_mode & S_IFDIR) return PageController::isFolder;
    if(fileInfo->st_mode & S_IFREG) return PageController::isFile;

    return PageController::notExist;
}

bool PageController::SearchDefaultPage(string& filePath, struct stat* fileInfo)
{
    std::vector<std::string>::iterator it;
    string file;
    string folder = filePath;
    CompleteFolder(folder);

    for(it=m_DefaultPages.begin(); it!=m_DefaultPages.end(); ++it)
    {
        file = folder;
        file.append(*it);
        if(ResourceExist(file, fileInfo)==PageController::isFile)
        {
            filePath = file;
            return true;
        }
    }
    return false;
}

void PageController::CompleteFolder(std::string& filePath)
{
    if(filePath.back() != '/') filePath += '/';
}

bool PageController::Controller(Request& request, Response* response)
{
    struct stat fileInfo;
    string filePath;
    string fileExt;
    string typeMime;
    stringstream stream;
    ifstream fileStream;
    size_t pos;
    PageController::TypeResource typeRes;
    char *buffer;
    string dateMod;


    filePath = BuildPath(request.GetParameter("ALL"));

    typeRes = ResourceExist(filePath, &fileInfo);
    if(typeRes == PageController::notExist) return false;
    if(typeRes == PageController::isFolder)
        if(!SearchDefaultPage(filePath, &fileInfo)) return false;

    pos = filePath.rfind('.');
    if(pos==string::npos) return false;
    fileExt = filePath.substr(pos+1);

    #ifdef DEBUG
        response->SetCache(5);
    #else
        response->SetCache(300);
    #endif
    response->SetHeader("Last-Modified", fileInfo.st_mtime);
    dateMod = request.GetHeader("If-Modified-Since");
    if(dateMod != "")
    {
        if(dateMod == response->GetHeader("Last-Modified"))
        {
            response->SetStatut(304);
            return true;
        }
    }

    fileStream.open(filePath, ios_base::in|ios_base::binary);
    if(!fileStream) return false;
    buffer = new char[fileInfo.st_size];
    fileStream.read (buffer,fileInfo.st_size);
    typeMime = m_TypeMime[fileExt];
    if(typeMime!="") response->SetHeader("Content-Type", typeMime);
    response->SetBuffer(buffer, fileInfo.st_size);
    delete[] buffer;
    fileStream.close();

    return true;
}
