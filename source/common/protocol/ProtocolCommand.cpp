
#include <QStringList>
#include <limits>

#include "ProtocolCommand.h"
#include "../memleakdetect.h"

namespace Picto {

ProtocolCommand::ProtocolCommand()
{
}

/*! \brief Constructs a new Protocol command with the input commandText and parses it into
 *	 this object to initialize all of its data values (using the parse() function).
 */
ProtocolCommand::ProtocolCommand(QString commandText)
{
	parse(commandText);
}

/*! \brief Parses the input commandText into this object to initialize all of its data values.*/
void ProtocolCommand::parse(QString commandText)
{
	QStringList lines = commandText.split(QRegExp("[\r\n]+"));
	QString firstLine = lines.takeFirst();

	QStringList tokens = firstLine.split(QRegExp("[ ][ ]*"));
	if(tokens.count() != 3)
	{
		parseError = "Method Target Protocol/Version header malformed";
	}
	else
	{
		method_ = tokens[0];
		target_ = tokens[1];

		int protocolVersionPosition = tokens[2].indexOf('/');
		if(protocolVersionPosition == -1)
		{
			parseError = "Method Target Protocol/Version header malformed";
		}
		else
		{
			protocolName_ = tokens[2].left(protocolVersionPosition);
			protocolVersion_ = tokens[2].mid(protocolVersionPosition+1);
		}
	}

	foreach(QString line, lines)
	{	
		int fieldEndPosition = line.indexOf(':');

		if(fieldEndPosition == -1)
		{
			if(line!="")
			{
				parseError = "Field: Value header malformed";
				break;
			}
		}
		else
		{
			fields_[line.left(fieldEndPosition)] = line.mid(fieldEndPosition+1).trimmed();
		}
	}
}

/*! \brief Returns the remaining space available for the content field based on the current value of "Content-Length".
 *	\details This is just subtracting the current content size from the "Content-Length" value.
 */
int ProtocolCommand::remainingContentLength()
{
	if(fields_.contains("Content-Length"))
		return fields_["Content-Length"].toInt() - content_.size();
	else
		return std::numeric_limits<int>::min();
}

/*! Appends the provided \a _content to the ProtocolCommand's existing payload, if any.
 *  No Content-Length field is added to the ProtocolCommand's fields, nor is an existing
 *  Content-Length field modified by this method.
 *  \param _content a QByteArray containing the content to be added to the payload
 *  \return an integer containing the remaining content length needed for a well formed command
 *  If no Content-Length field is present, this function will return std::numeric_limits<int>::min()
 *  If the added content causes the stored content to exceed the value specified by the
 *  Content-Length field, then this function will return a negative value
 *	\sa remainingContentLength(), setContent()
 */
int ProtocolCommand::addToContent(QByteArray _content)
{
	content_ += _content;

	return remainingContentLength();
}

/*! Sets the provided \a _content as the ProtocolCommand's payload.
 *  No Content-Length field is added to the ProtocolCommand's fields, nor is an existing
 *  Content-Length field modified by this method.
 *  \param _content a QByteArray containing the content to be set as the payload
 *  \return an integer containing the remaining content length needed for a well formed command
 *  If no Content-Length field is present, this function will return std::numeric_limits<int>::min()
 *  If the set content length exceeds the value specified by the
 *  Content-Length field, then this function will return a negative value
 *	\sa remainingContentLength(), addToContent()
 */
int ProtocolCommand::setContent(QByteArray _content)
{
	content_ = _content;

	return remainingContentLength();
}

/*! \brief Returns true if this ProtocolCommand includes the input field type.*/
bool ProtocolCommand::hasField(QString field)
{
	return(fields_.contains(field));
}

/*! \brief Returns the value of the input field type.*/
QString ProtocolCommand::getFieldValue(QString field)
{
	return fields_.value(field,"");
}

/*! \brief Sets the input value to the input field type.  If this field does not yet exist in the ProtocolCommand
 *	it is added.
 */
void ProtocolCommand::setFieldValue(QString field, QString value)
{
	fields_[field] = value;
}
/*! \brief Returns true if the size of the content payload is smaller than the stored size in the
 *	"Content-Length" field.
 */
bool ProtocolCommand::isPendingContent()
{
	return content_.size() < fields_.value("Content-Length", "0").toInt();
}

/*! \brief Returns true if this ProtocolCommand is valid. 
 *	\details Validity is defined as including no parse errors, having a target, method and version, and
 *	having a "Content-Length" field that matches the size of the content payload.
 */
bool ProtocolCommand::isWellFormed()
{
	if(!parseError.isNull())
	{
		return false;
	}

	if(method_.isEmpty() || target_.isEmpty() || protocolVersion_.isEmpty())
	{
		return false;
	}

	if(isPendingContent())
	{
		return false;
	}

	return true;
}

/*! \brief Writes this ProtocolCommand out over the input QAbstractSocket.
 *	\details This function returns the number of bytes written to the socket if it is succesful.
 *	If the function fails (ie. the ProtocolCommand is not wellFormed, the socket is not connected, etc)
 *	the function returns -1.
 */
int ProtocolCommand::write(QAbstractSocket *socket)
{
	QString commandHeader;
	QByteArray command;

	int bytesWritten;

	if(!isWellFormed())
		return -1;
	if(content_.size() != fields_.value("Content-Length","0").toInt())
		return -1;
	if(socket->state() < QAbstractSocket::ConnectedState)
		return -1;

	commandHeader = method_ + " " + target_ + " " + protocolName_ + "/" + protocolVersion_ + "\r\n";
	
	QMap<QString,QString>::const_iterator fieldIter = fields_.begin();
	while(fieldIter != fields_.end())
	{
		commandHeader += fieldIter.key() + ":" + fieldIter.value() + "\r\n";
		fieldIter++;
	}

	commandHeader += "\r\n";

	command = commandHeader.toLatin1();

	if(!content_.isEmpty())
		command.append(content_);

	bytesWritten =  socket->write(command);

	socket->flush();

	return bytesWritten;
}

//! Reads a command from the socket, and returns the bytes of content (frequently 0)
/*! \brief Reads a ProtocolCommand in over the input socket, waiting a maximum of timeoutMs for it to arrive.
 *	Initializes all of this ProtocolCommands data based on the input text.  
 *
 *	If this function returns a positive value, it is the size of the content payload read in from the socket.  If the
 *	return value is negative, some error occured.
 *		- -1: No data came in over the socket
 *		- -2: The first line of the ProtocolCommand data was misformed
 *		- -4: A field:value pairs was misformed.
 *		- Another negative value: The size of the content payload read in times -1.  This is reported when the content payload
 *			size didn't match the "Content-Length" field.
 */
int ProtocolCommand::read(QAbstractSocket *socket, int timeoutMs)
{
	QString currentLine;
	QStringList tokens;

	if(socket->bytesAvailable() <= 0)
	{
		if(!socket->waitForReadyRead(timeoutMs))
		{
			return -1;
		}
	}

	//read the first line(e.g. "GET /sometarget PICTO/1.0")
	currentLine = socket->readLine();
	currentLine.remove("\r\n");

	tokens = currentLine.split(" ", QString::SkipEmptyParts);
	if(tokens.size() != 3)
		return -2;

	method_ = tokens[0];
	target_ = tokens[1];

	tokens = tokens[2].split("/");
	if(tokens.size() != 2)
		return -2;
	protocolName_ = tokens[0];
	protocolVersion_ = tokens[1];

	//next, clear then populate the fields map
	fields_.clear();
	
	currentLine = socket->readLine();
	while(currentLine != "\r\n")
	{
		currentLine.remove("\r\n");
		QString fieldKey,fieldValue;

		int keyEndPosition = currentLine.indexOf(':');
		if(keyEndPosition < 0)
			return -4;

		fieldKey = currentLine.left(keyEndPosition);
		fieldValue = currentLine.mid(keyEndPosition+2);
		fields_[fieldKey] = fieldValue;

		currentLine = socket->readLine();
	}

	//finally, if there's content, read it
	int contentLength = fields_.value("Content-Length", "0").toInt();
	if(contentLength == 0)
		return 0;

	content_ = socket->read(contentLength);
	while(content_.size() < contentLength && 
		(socket->bytesAvailable() > 0 || socket->waitForReadyRead(500)))
	{
		content_.append(socket->read(contentLength-content_.size()));
	}

	//return length of content read
	if(content_.size() == contentLength)
		return content_.size();
	else
		return 0-content_.size();



}

}; //namespace Picto


