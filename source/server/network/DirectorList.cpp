#include "DirectorList.h"

#include <QXmlStreamWriter>

QMap<QString, DirectorList::DirectorInfo> DirectorList::directorMap;

DirectorList::DirectorList()
{
}

/*!	\brief Updates the list with this director
 *
 *	Everytime the DIRECTORUPDATE command is received by the server, this 
 *	function gets called.  The function checks to see if the director instance referenced
 *	in our list, and if not adds it.  If the instance already exists, the function makes
 *	any needed changes.  Note that the IP address is being used as a "primary key" here.
 */
void DirectorList::updateDirector(QHostAddress addr, QString name, DirectorStatus status)
{
	DirectorInfo info ;
	info.address = addr;
	info.name = name;
	info.status = status;

	directorMap[addr.toString()] = info;
}

//! Removes a director instance from the list
void DirectorList::removeDirector(QHostAddress addr)
{
	if(directorMap.contains(addr.toString()))
		directorMap.remove(addr.toString());
}

/*!	\brief Returns a list of director instances as an xml fragment.
 *
 *	The XML fragment will look like this:
 *	<DirectorInstances>
 *		<Director>
 *			<Address>192.168.3.51</Address>
 *			<Name>PictoDirector Room 408</Name>
 *			<Status>Idle</Status>
 *		</Director>
 *		<Director>
 *			<Address>192.168.3.164</Address>
 *			<Name>PictoDirector Room 407</Name>
 *			<Status>Running</Status>
 *		</Director>
 *	</DirectorInstances>
 */
QString DirectorList::getList()
{
	QString xmlFragment;
	QXmlStreamWriter xmlWriter(&xmlFragment);

	xmlWriter.writeStartElement("DirectorInstances");

	foreach(DirectorInfo director, directorMap)
	{
		xmlWriter.writeStartElement("Director");
		xmlWriter.writeTextElement("Address",director.address.toString());
		xmlWriter.writeTextElement("Name", director.name);
		if(director.status == idle)
			xmlWriter.writeTextElement("Status","Idle");
		else if(director.status == running)
			xmlWriter.writeTextElement("Status","Running");
		xmlWriter.writeEndElement(); //Director
	}
	xmlWriter.writeEndElement();	//DirectorInstances

	return xmlFragment;
}