#include <QApplication>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QUuid>
#include <QTextStream>

#include <iostream>
#include <vector>
#include <map>

std::map<QString, QString> dependencyGuids;
std::vector<QString> dependencies;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv, false);

	if(argc<3)
	{
		std::cout << "Usage:" << std::endl << std::endl << "vcSlnDependencies filename.sln dependency1 dependency2 dependency3 ..." << std::endl;
		
		return -1;
	}
	else
	{
		for(int i = 2; i < argc; i++)
		{
			dependencies.push_back(argv[i]);
		}
	}

	QFile file(argv[1]);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		std::cout << "Could not open sln file: " << argv[1] << "." << std::endl;
		return -1;
	}

	QFileInfo fileInfo(file);

	std::cout << "Processing " << fileInfo.fileName().toStdString() << std::endl;
	
	QByteArray fileData;
	fileData = file.readAll();
	file.close();

	QFile outFile(argv[1]);
	if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		std::cout << "Could not open sln file: " << argv[1] << "." << std::endl;
		return -1;
	}

	QTextStream reader(fileData);
	QTextStream writer(&outFile);

	//first populate the dependency guids
	while(!reader.atEnd())
	{
		QString currentLine = reader.readLine();

		for(unsigned int i = 0; i < dependencies.size(); i++)
		{
			if(currentLine.contains(dependencies[i]) && currentLine.contains(".vcproj") && currentLine.contains("Project(\"{"))
			{
				dependencyGuids[dependencies[i]] = currentLine.right(39).left(38);
				std::cout << "Setting dependency GUID for " << dependencies[i].toAscii().data() << " to " << currentLine.right(39).left(38).toAscii().data() << "\n";
			}
		}				
	}
	reader.seek(0);

	//now update other projects with those dependencies
    while(!reader.atEnd())
	{
		QString currentLine = reader.readLine();
		
		writer << currentLine << "\n";

		if(currentLine.contains(".vcproj") && currentLine.contains("Project(\"{"))
		{
			QString currentGuid = currentLine.mid(9,38);

			std::vector<QString> currentDependencies;
			
			for(std::map<QString, QString>::const_iterator iter = dependencyGuids.begin(); iter != dependencyGuids.end(); ++iter)
			{
				if(iter->second != currentGuid)
				{
					currentDependencies.push_back(iter->second);
				}
			}
			
			if(currentDependencies.size() > 0)
			{
				writer << "\tProjectSection(ProjectDependencies) = postProject\n";
				
				for(unsigned int i = 0; i < currentDependencies.size(); i++)
				{
					writer << "\t\t" << currentDependencies[i] << " = " << currentDependencies[i] << "\n";					
				}
				
				writer << "\tEndProjectSection\n";
			}
		}		
    }

	writer.flush();
	outFile.close();

	return 0;
}
