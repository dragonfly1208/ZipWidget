#include "utils.h"
#include <QString>
Utils::Utils()
{

}

// 获取文件图标
HICON Utils::fileIcon(std::string extention)
{
    HICON icon = NULL;
    if (extention.length() > 0)
    {
        LPCSTR name = extention.c_str();

        SHFILEINFOA info;
        if (SHGetFileInfoA(name,
            FILE_ATTRIBUTE_NORMAL,
            &info,
            sizeof(info),
            SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES))
        {
            icon = info.hIcon;
        }
    }

    return icon;
}

// 获取文件类型
std::string Utils::fileType(std::string extention)
{
    static std::string simpleType;
    if(extention.length()<=0){
        if(simpleType.length()>0) return simpleType;
        std::string type = fileType("aa.aa");

        size_t index = type.find(' ');
        if(index!=std::string::npos){
            simpleType = type.substr(index);
        }else
            simpleType = "file";
        return simpleType;
    }
    std::string type = "";
    if (extention.length() > 0)
    {
        LPCSTR name = extention.c_str();

        SHFILEINFOA info;
        if (SHGetFileInfoA(name,
            FILE_ATTRIBUTE_NORMAL,
            &info,
            sizeof(info),
            SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES))
        {
            type = info.szTypeName;
        }

    }

    return type;
}

// 获取文件夹图标
HICON Utils::folderIcon()
{
    std::string str = "folder";
    LPCSTR name = str.c_str();

    HICON icon = NULL;

    SHFILEINFOA info;
    if (SHGetFileInfoA(name,
        FILE_ATTRIBUTE_DIRECTORY,
        &info,
        sizeof(info),
        SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES))
    {
        icon = info.hIcon;
    }

    return icon;
}

// 获取文件夹类型
std::string Utils::folderType()
{
    std::string str = "folder";
    LPCSTR name = str.c_str();

    std::string type;

    SHFILEINFOA info;
    if (SHGetFileInfoA(name,
        FILE_ATTRIBUTE_DIRECTORY,
        &info,
        sizeof(info),
        SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES))
    {
        type = info.szTypeName;
    }

    return type;
}


QString & Utils::trimmed(QString &str,QChar c){
    if(str.isEmpty()) return str;
    int beg = 0,end = str.length();
    do{
        --end;
        if(str[end] != c){ ++end;break;}
    }while (end>0);

    while (beg<end) {
        if(str[beg] != c) break;
        ++beg;
    }
    str.remove(end,str.length()-end);
    str.remove(0,beg);
    return str;
}
QString & Utils::trimmedRight(QString &str,QChar c){
    if(str.isEmpty()) return str;
    int end = str.length();
    do{
        --end;
        if(str[end] != c){ ++end;break;}
    }while (end>0);
    str.remove(end,str.length()-end);
    return str;
}
