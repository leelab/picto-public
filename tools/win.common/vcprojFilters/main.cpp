#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QUuid>

#include <iostream>
#include <vector>
#include <map>

/* This code was used with Visual Studio 2008 and before in order to set up our Picto solution with a nice directory 
 * structure.  Now we use "CONFIG -= flat" in the Picto.pro file which takes care of this fairly well on its own.
 * This code does not work correctly with the Visual Studio 2010 file structure, so if you want to start using it again
 * you'll have fix it.
 */
int main(int argc, char *argv[])
{
	if(argc<2) return -1;

	QFile file(argv[1]);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		std::cout << "Could not open vcproj file: " << argv[1] << "." << std::endl;
		return -1;
	}

	QFileInfo fileInfo(file);

	std::cout << "Processing " << fileInfo.fileName().toStdString() << std::endl;
	
	QByteArray xmlData;
	xmlData = file.readAll();
	file.close();

	QXmlStreamReader reader(xmlData);
	QFile outFile(argv[1]);
	if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		std::cout << "Could not open vcproj file: " << argv[1] << "." << std::endl;
		return -1;
	}
	QXmlStreamWriter writer(&outFile);
	writer.setAutoFormatting(true);

	writer.writeStartDocument();

	std::vector<QString> tagHierarchy;
	std::multimap<QString,std::pair<QString,QString> > filterFileMMap;
	std::vector<std::pair<QString,QString> > rootFiles;
	bool bProcessingSourceCode = false;

    while(!reader.atEnd())
	{
        reader.readNext();
        if(reader.isStartElement())
		{
			QString currentTag = reader.name().toString();
			tagHierarchy.push_back(currentTag);

            if(currentTag == "Filter")
			{
                QString filterName = reader.attributes().value("Name").toString();
				if(filterName == "Source Files" || filterName == "Header Files")
				{
					bProcessingSourceCode = true;
				}
			}
			
			if (bProcessingSourceCode && currentTag == "File")
			{
				QString sourceFile = reader.attributes().value("RelativePath").toString();
				QStringList sourceFilePath = sourceFile.split("\\");
				
				QString fileSubXML;
				QXmlStreamWriter fileSubXMLWriter(&fileSubXML);
				fileSubXMLWriter.writeStartDocument();
				fileSubXMLWriter.writeStartElement("vcprojFiltersFileSubXMLContainer");
				
				bool bFoundFileEndElement = false;
				
				do
				{
					reader.readNext();
					if(reader.isStartElement())
					{
						QString subTag = reader.name().toString();
						tagHierarchy.push_back(subTag);
						
						fileSubXMLWriter.writeStartElement(subTag);
						fileSubXMLWriter.writeAttributes(reader.attributes());					
					}
					else if(reader.isEndElement())
					{
						QString subTagBeingClosed = tagHierarchy.back();
						tagHierarchy.pop_back();
						
						if(subTagBeingClosed == "File")
						{
							bFoundFileEndElement = true;
						}
						else
						{
							fileSubXMLWriter.writeEndElement();
						}
					}
					else if(reader.isCharacters())
					{
						QString nodeText = reader.text().toString().trimmed();
						if(nodeText.length()>0)
						{
							fileSubXMLWriter.writeCharacters(nodeText);
						}
					}									
				} while(!bFoundFileEndElement && !reader.atEnd());
				
				fileSubXMLWriter.writeEndElement();
				fileSubXMLWriter.writeEndDocument();

				if(sourceFilePath.size() > 2)
				{
					//std::cout << "Warning - multi-level paths are present; only the last will be used as a project filter" << std::endl;					
					filterFileMMap.insert(std::pair<QString,std::pair<QString,QString> >(sourceFilePath.at(sourceFilePath.size()-2),std::pair<QString,QString>(sourceFile,fileSubXML)));
				}
				else if(sourceFilePath.size() == 2)
				{
					filterFileMMap.insert(std::pair<QString,std::pair<QString,QString> >(sourceFilePath.at(0),std::pair<QString,QString>(sourceFile,fileSubXML)));
				}
				else
				{
					rootFiles.push_back(std::pair<QString,QString>(sourceFile,fileSubXML));
				}			
			}
			else if(!bProcessingSourceCode)
			{
				writer.writeStartElement(currentTag);
				writer.writeAttributes(reader.attributes());
			}
        }
		else if (reader.isEndElement())
		{
			QString tagBeingClosed = tagHierarchy.back();
			tagHierarchy.pop_back();

			if(!bProcessingSourceCode)
			{
				if(tagBeingClosed == "Files")
				{
					writer.writeStartElement("Filter");
					writer.writeAttribute("Name","Project Files");
					writer.writeAttribute("Filter","pro");
					writer.writeAttribute("UniqueIdentifier",QUuid::createUuid().toString());
					writer.writeAttribute("ParseFiles","false");
					writer.writeStartElement("File");
					QDir projectDir = fileInfo.dir();
					QStringList proFilter("*.pro");
					QStringList projectProFiles = projectDir.entryList(proFilter);
					writer.writeAttribute("RelativePath",projectProFiles.at(0));
					writer.writeEndElement();
					writer.writeEndElement();
				
					writer.writeStartElement("Filter");
					writer.writeAttribute("Name","Source Files");
					writer.writeAttribute("Filter","cpp;c;cxx;def;odl;idl;hpj;bat;asm;asmx;h;hpp;hxx;hm;inl;inc;xsd");
					writer.writeAttribute("UniqueIdentifier",QUuid::createUuid().toString());

					for(unsigned int i = 0; i < rootFiles.size(); i++)
					{
						QString subXML = rootFiles[i].second;
						
						writer.writeStartElement("File");
						writer.writeAttribute("RelativePath",rootFiles[i].first);
						if(subXML.length()>0)
						{
							QXmlStreamReader subXMLReader(subXML);
							while(!subXMLReader.atEnd())
							{
								subXMLReader.readNext();
								if(subXMLReader.isStartElement() && subXMLReader.name() != "vcprojFiltersFileSubXMLContainer")
								{								
									writer.writeStartElement(subXMLReader.name().toString());
									writer.writeAttributes(subXMLReader.attributes());					
								}
								else if(subXMLReader.isEndElement() && subXMLReader.name() != "vcprojFiltersFileSubXMLContainer")
								{
									writer.writeEndElement();
								}
								else if(subXMLReader.isCharacters())
								{
									QString subXMLNodeText = reader.text().toString().trimmed();
									if(subXMLNodeText.length()>0)
									{
										writer.writeCharacters(subXMLNodeText);
									}
								}					
							}
						}
						writer.writeEndElement();
					}

					std::multimap<QString,std::pair<QString,QString> >::iterator j;
					std::multimap<QString,std::pair<QString,QString> >::iterator k;

					for(j=filterFileMMap.begin(); j!=filterFileMMap.end();)
					{
						QString currentFilter = j->first;

						k = filterFileMMap.upper_bound(currentFilter);

						writer.writeStartElement("Filter");
						writer.writeAttribute("Name",currentFilter);
						writer.writeAttribute("Filter","cpp;c;cxx;def;odl;idl;hpj;bat;asm;asmx;h;hpp;hxx;hm;inl;inc;xsd");
						writer.writeAttribute("UniqueIdentifier",QUuid::createUuid().toString());
						
						for(; j!=k; j++)
						{
							QString currentFile = j->second.first;
							QString subXML = j->second.second;

							writer.writeStartElement("File");
							writer.writeAttribute("RelativePath",currentFile);
							if(subXML.length()>0)
							{
								QXmlStreamReader subXMLReader(subXML);
								while(!subXMLReader.atEnd())
								{
									subXMLReader.readNext();
									if(subXMLReader.isStartElement() && subXMLReader.name() != "vcprojFiltersFileSubXMLContainer")
									{								
										writer.writeStartElement(subXMLReader.name().toString());
										writer.writeAttributes(subXMLReader.attributes());					
									}
									else if(subXMLReader.isEndElement() && subXMLReader.name() != "vcprojFiltersFileSubXMLContainer")
									{
										writer.writeEndElement();
									}
									else if(subXMLReader.isCharacters())
									{
										QString subXMLNodeText = reader.text().toString().trimmed();
										if(subXMLNodeText.length()>0)
										{
											writer.writeCharacters(subXMLNodeText);
										}
									}					
								}
							}
							writer.writeEndElement();
						}					

						writer.writeEndElement();
					}

					writer.writeEndElement();
				}

				writer.writeEndElement();
			}
			else if(tagBeingClosed == "Filter")
			{
				bProcessingSourceCode = false;
			}

		}
		else if (reader.isCharacters())
		{
			QString nodeText = reader.text().toString().trimmed();
			if(nodeText.length()>0)
			{
				writer.writeCharacters(nodeText);
			}
		}
    }

	writer.writeEndDocument();

    if (reader.error())
	{
		std::cout << "Could not parse vcproj file: " << argv[1] << "." << std::endl;
		outFile.close();
		return -1;
    }

	outFile.close();
	return 0;
}
