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
	std::map<QString,QString>::const_iterator contentLengthIter = fields_.find("Content-Length");
	if(contentLengthIter == fields_.end())
	{
		return std::numeric_limits<int>::min();
	}
	else
	{
		return contentLengthIter->second.toInt() - content_.size();
	}
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

QString ProtocolCommand::getFieldValue(QString field)
{
	QString fieldValue;

	std::map<QString,QString>::const_iterator fieldIter = fields_.find(field);

	if(fieldIter != fields_.end())
	{
		fieldValue = fieldIter->second;
	}

	return fieldValue;
}

void ProtocolCommand::setFieldValue(QString field, QString value)
{
	fields_[field] = value;
}

bool ProtocolCommand::isPendingContent()
{
	std::map<QString,QString>::const_iterator contentLengthIter = fields_.find("Content-Length");
	if(contentLengthIter != fields_.end())
	{
		if(content_.size() < contentLengthIter->second.toInt())
		{
			return true;
		}
	}

	return false;
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

}; //namespace Picto
