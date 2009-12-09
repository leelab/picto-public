#include "ProtocolCommand.h"

#include <QStringList>
#include <limits>

namespace Picto {

ProtocolCommand::ProtocolCommand()
{
}

ProtocolCommand::ProtocolCommand(QString commandText)
{
	parse(commandText);
}

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

int ProtocolCommand::remainingContentLength()
{
	if(fields_.contains("Content-Length"))
		return fields_["Content-Length"].toInt() - content_.size();
	else
		return std::numeric_limits<int>::min();
}

int ProtocolCommand::addToContent(QByteArray _content)
{
	content_ += _content;

	return remainingContentLength();
}

int ProtocolCommand::setContent(QByteArray _content)
{
	content_ = _content;

	return remainingContentLength();
}

bool ProtocolCommand::hasField(QString field)
{
	return(fields_.contains(field));
}

QString ProtocolCommand::getFieldValue(QString field)
{
	return fields_.value(field,"");
}

void ProtocolCommand::setFieldValue(QString field, QString value)
{
	fields_[field] = value;
}

bool ProtocolCommand::isPendingContent()
{
	return content_.size() < fields_.value("Content-Length", "0").toInt();
}

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

	command = commandHeader.toAscii();

	if(!content_.isEmpty())
		command.append(content_);

	bytesWritten =  socket->write(command);

	socket->flush();

	return bytesWritten;
}

//! Reads a command from the socket, and returns the bytes of content (frequently 0)
int ProtocolCommand::read(QAbstractSocket *socket)
{
	QString currentLine;
	QStringList tokens;

	if(socket->bytesAvailable() <= 0 && !socket->waitForReadyRead(0))
	{
		return -1;
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


