#ifndef UTILS_H
#define UTILS_H
#include <windows.h>
#include <iostream>

class QString;
class QChar;
class Utils
{
public:
    Utils();
    static HICON fileIcon(std::string extention);
    static std::string fileType(std::string extention);
    static HICON folderIcon();
    static std::string folderType();

    static QString & trimmed(QString &str,QChar c);
    static QString & trimmedRight(QString &str,QChar c);
};

#endif // UTILS_H
