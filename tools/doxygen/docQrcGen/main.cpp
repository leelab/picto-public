#include <QFile>
#include <QDir>
#include <QFileInfo>

#include <iostream>

int main(int argc, char *argv[])
{
	if(argc<4)
	{
        std::cout << "Usage: " << std::endl << std::endl
                  << "docQrcGen generatedQrcFileName.qrc directoryContainingFiles relativePathToPrefixWithTrailingSlash" << std::endl;
        return -1;
    }

	QFile file(argv[1]);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		std::cout << "Could not create qrc file: " << argv[1] << "." << std::endl;
		return -1;
	}
  
    QDir dir(argv[2]);
    if(!dir.exists())
    {
		std::cout << "Could not open directory: " << argv[2] << "." << std::endl;
		return -1;
    }

    file.write("<!DOCTYPE RCC>\n<RCC version=\"1.0\">\n <qresource prefix=\"/documentation\">\n");

    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    for(int i=0; i<list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        file.write("   <file alias=\"");
        file.write(fileInfo.fileName().toUtf8().data());
        file.write("\">");
        file.write(argv[3]);
        file.write(fileInfo.fileName().toUtf8().data());
        file.write("</file>\n");
    }

    file.write(" </qresource>\n</RCC>\n");
    file.close();

	return 0;
}
