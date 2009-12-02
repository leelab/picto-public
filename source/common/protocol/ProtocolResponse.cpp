#include "ProtocolResponse.h"

#include "../globals.h"

#include <QDateTime>
#include <QBuffer>
#include <QtIOCompressor>
#include <QRegExp>
#include <QStringList>


namespace Picto {

ProtocolResponse::ProtocolResponse(QString _serverType,
								   QString _protocol,
								   QString _version,
								   ProtocolResponseType::ProtocolResponseType protocolResponseType) :
	protocolResponseCode(protocolResponseType),
	//serverType(_serverType),
	protocol(_protocol),
	version(_version),
	bShouldTerminateConnection(false),
	bStreamingResponse(false),
	multiPartResponseState(MultiPartResponseType::NotMultiPart),
	multiPartBoundary(""),
	timeout(1000)
	//contentEncodingType(ContentEncodingType::raw),
	//contentType("")
{
	protocolResponseTypeStrings[ProtocolResponseType::NotImplemented] = "Not Implemented";
	protocolResponseTypeStrings[ProtocolResponseType::IncompleteRequest] = "Incomplete Request";
	protocolResponseTypeStrings[ProtocolResponseType::OK] = "OK";
	protocolResponseTypeStrings[ProtocolResponseType::UninitializedResponse] = "Error - Uninitialized Response";
	protocolResponseTypeStrings[ProtocolResponseType::UnknownRequest] = "Unknown Request";
	protocolResponseTypeStrings[ProtocolResponseType::InternalServerError] = "Internal Server Error";
	protocolResponseTypeStrings[ProtocolResponseType::ConnectionTimedOut] = "Request submission time elapsed";
	protocolResponseTypeStrings[ProtocolResponseType::NotFound] = "Resource Not Found";
	protocolResponseTypeStrings[ProtocolResponseType::Unauthorized] = "Unauthorized";
	protocolResponseTypeStrings[ProtocolResponseType::BadRequest] = "Bad Request";

	contentEncodingTypeStrings[ContentEncodingType::raw] = "";
	contentEncodingTypeStrings[ContentEncodingType::gzip] = "gzip";
	contentEncodingTypeStrings[ContentEncodingType::deflate] = "deflate";

	fields["Server"] = _serverType;
}

QString ProtocolResponse::generateHeadersString()
{
	QString headers;

	headers = QString("%1/%2 %3 %4\r\n").arg(protocol)
										.arg(version)
										.arg(protocolResponseCode)
										.arg(protocolResponseTypeStrings[protocolResponseCode]);

	//Print the content length and encoding, but only if this isn't a multipart response
	if(!fields.value("Content-Type").contains("multipart", Qt::CaseInsensitive))
	{
		if(fields.contains("Content-Encoding"))
			headers+= QString("Content-Encoding: %1\r\n").arg(fields.value("Content-Encoding"));
		headers+= QString("Content-Length: %1\r\n").arg(fields.value("Content-Length",0));
	}

	//update (or add) the date field
	fields["Date"] = QString("%1 GMT").arg(QDateTime::currentDateTime().toUTC().toString("ddd, dd MMM yyyy hh:mm:ss"));

	//print everything else that we haven't dealt with yet
	QMap<QString,QString>::const_iterator fieldsIter = fields.begin();
	while(fieldsIter != fields.end())
	{
		if(fieldsIter.key() != "Content-Encoding" &&
			fieldsIter.key() != "Content-Length")
		{
			headers += QString("%1: %2\r\n")
							.arg(fieldsIter.key())
							.arg(fieldsIter.value());
		}
		fieldsIter++;
	}
	return headers;

	///////////////////////////////////////////////////////////
	///////////////
	//OLD VERSION
	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	///////////////
	/////////////////////////////////////////////////////////////

	//headers += QString("Server: %1\r\n").arg(serverType);
	/*if(!contentType.isEmpty())
	{
		headers += QString("Content-Type: %1\r\n").arg(contentType);
	}
	if(contentEncodingType != ContentEncodingType::raw && multiPartResponseState < MultiPartResponseType::MultiPartInitial)
	{
		headers += QString("Content-Encoding: %1\r\n").arg(contentEncodingTypeStrings[contentEncodingType]);
	}
	if(multiPartResponseState < MultiPartResponseType::MultiPartInitial)
	{
		headers += QString("Content-Length: %1\r\n").arg(encodedContent.size());
	}
	
	headers += QString("Date: %1 GMT\r\n").arg(QDateTime::currentDateTime().toUTC().toString("ddd, dd MMM yyyy hh:mm:ss"));

	for(std::map<QString,QString>::const_iterator fieldsIter = fields.begin(); fieldsIter != fields.end(); ++fieldsIter)
	{
		headers += QString("%1: %2\r\n").arg(fieldsIter->first).arg(fieldsIter->second);
	}
	return headers;*/
}

//In the case of a multipart response, after the initial headers are sent, 
//we would send this header with the next piece of content.
QString ProtocolResponse::getMultiPartHeaders()
{
	QString headers;

	headers = QString("%1\r\n").arg(multiPartBoundary);
	headers += QString("Content-Length: %1\r\n").arg(fields.value("Content-Length"));
	headers += QString("Content-Type: %1\r\n").arg(multiPartContentType);
	if(fields.contains("Content-Encoding"))
	{
		headers += QString("Content-Encoding: %1\r\n").arg(fields.value("Content-Encoding"));
	}

	return headers;
}

void ProtocolResponse::setMultiPartContentType(QString multiPartContentTypeString)
{
	multiPartContentType = multiPartContentTypeString;
}

QString ProtocolResponse::getMultiPartContentType()
{
	return multiPartContentType;
}

//Note that this does not affect the Content-Type field.  So, if for example
//you were starting to build a mutlipart response, and this was the first 
//piece of it, you would use the following functions:
//	response->setContentType("multipart/x-mixed-replace; boundary=--pictoboundary");
//	response->setMultiPartBoundary("--pictoboundary");

void ProtocolResponse::setMultiPartBoundary(QString multiPartBoundaryString)
{
	multiPartBoundary = multiPartBoundaryString;
}

QString ProtocolResponse::getMultiPartBoundary()
{
	return multiPartBoundary;
}

int ProtocolResponse::getResponseCode()
{
	return protocolResponseCode;
}

QString ProtocolResponse::getResponseType()
{
	return protocolResponseTypeStrings[protocolResponseCode];
}

QByteArray ProtocolResponse::getContent()
{
	return encodedContent;
}

//Yes, this is identical to getContent, but it makes things a bit clearer
QByteArray ProtocolResponse::getEncodedContent()
{
	return encodedContent;
}

QByteArray ProtocolResponse::getDecodedContent()
{
	return content;
}

void ProtocolResponse::encodeContent()
{
	if(fields.value("Content-Encoding") == "gzip")
	{
		QBuffer contentCompressedBuffer;

		QtIOCompressor compressor(&contentCompressedBuffer);
		compressor.setStreamFormat(QtIOCompressor::GzipFormat);
		compressor.open(QIODevice::WriteOnly);
		compressor.write(content);
		compressor.close();

		encodedContent = contentCompressedBuffer.buffer();	
	}
	else if(fields.value("Content-Encoding") == "deflate")
	{
		QBuffer contentCompressedBuffer;

		QtIOCompressor compressor(&contentCompressedBuffer);
		compressor.setStreamFormat(QtIOCompressor::ZlibFormat);
		compressor.open(QIODevice::WriteOnly);
		compressor.write(content);
		compressor.close();

		encodedContent = contentCompressedBuffer.buffer();
	}
	else
	{
		encodedContent = content;
	}
}

void ProtocolResponse::decodeContent()
{
	if(fields.value("Content-Encoding") == "gzip")
	{
		QBuffer compressedContent(&encodedContent);

		QtIOCompressor compressor(&compressedContent);
		compressor.setStreamFormat(QtIOCompressor::GzipFormat);
		compressor.open(QIODevice::ReadOnly);
		content = compressor.readAll();
		compressor.close();
	}
	else if(fields.value("Content-Encoding") == "deflate")
	{
		QBuffer compressedContent(&encodedContent);

		QtIOCompressor compressor(&compressedContent);
		compressor.setStreamFormat(QtIOCompressor::ZlibFormat);
		compressor.open(QIODevice::ReadOnly);
		content = compressor.readAll();
		compressor.close();
	}
	else
	{
		content = encodedContent;
	}

}

void ProtocolResponse::setContentType(QString contentTypeString)
{
	//contentType = contentTypeString;
	fields["Content-Type"]=contentTypeString;
}

void ProtocolResponse::setContentEncoding(ContentEncodingType::ContentEncodingType contentEncoding)
{
	//contentEncodingType = contentEncoding;
	fields["Content-Encoding"] = contentEncodingTypeStrings[contentEncoding];

	if(!content.isEmpty())
	{
		encodeContent();
	}
}

bool ProtocolResponse::hasContent()
{
	return(!content.isEmpty());
}

void ProtocolResponse::setContent(QByteArray _content)
{
	content = _content;
	if(!content.isEmpty())
		encodeContent();
}

void ProtocolResponse::addField(QString field, QString value)
{
	fields[field] = value;
}

void ProtocolResponse::setShouldTerminateConnection(bool terminateConnection)
{
	bShouldTerminateConnection = terminateConnection;
}

bool ProtocolResponse::shouldTerminateConnection()
{
	return bShouldTerminateConnection;
}

void ProtocolResponse::setShouldStream(bool shouldStream)
{
	bStreamingResponse = shouldStream;
	if(multiPartResponseState == MultiPartResponseType::NotMultiPart)
	{
		multiPartResponseState = MultiPartResponseType::StreamingNonMultiPart;
	}
}

bool ProtocolResponse::shouldStream()
{
	return bStreamingResponse;
}

void ProtocolResponse::setMultiPart(MultiPartResponseType::MultiPartResponseType multiPartState)
{
	multiPartResponseState = multiPartState;
}

MultiPartResponseType::MultiPartResponseType ProtocolResponse::getMultiPart()
{
	return multiPartResponseState;
}

QString ProtocolResponse::getFieldValue(QString field)
{
	QString fieldValue;

	return fields.value(field,"");
}

//! Writes a response to the passed in socket.  Returns total bytes written
int ProtocolResponse::write(QAbstractSocket *socket)
{
	QByteArray response;

	//before doing anything, make sure that Content-Length is accurate
	if(fields.value("Content-Length").toInt() != content.size())
		return -1;

	if(multiPartResponseState <= MultiPartResponseType::MultiPartInitial)
	{
		QString headers = generateHeadersString();
		headers += "\r\n";
		response.append(headers);
	}

	if(multiPartResponseState == MultiPartResponseType::MultiPartPart)
	{
		response.append("\r\n");
	}

	if(multiPartResponseState >= Picto::MultiPartResponseType::MultiPartInitial)
	{
		QString multiPartHeaders = getMultiPartHeaders();
		multiPartHeaders += "\r\n";
		response.append(multiPartHeaders);
	}

	if(hasContent())
	{
		response.append(getContent());
	}
	
	return socket->write(response);
}

//! Reads an incoming response from a socket and returns the bytes of content read

/*!	read() faces some interesting challenges, since it may not be easy to tell what
 *	an incoming response actually is.  For example, if an incoming response is part of
 *	a multi-part response, the response will start with a boundary string, but if we
 *	haven't set the boundary string for the response, we won't know what to 
 *	do with it.  So, the following rules should be followed when reading:
 *		1. If you know anything about the incoming response, tell the object (ie
 *		   if it's part of a multi-part response, set the multi-part response type)
 *		2. You can't assume that write will create an exact copy of the response
 *		   that was sent.  For example, theoriginal response may have had the 
 *		   shouldTerminateConnection value set, but there is no way for us to tell if
 *		   this was the case on the receiving end.
 *	The safest way to use a response that you have just read is to look at the fields 
 *	directly
 *
 */
int ProtocolResponse::read(QAbstractSocket *socket)
{
	QString currentLine;
	int contentLength=0;
	
	if(socket->bytesAvailable() <= 0 && !socket->waitForReadyRead(1000))
	{
		return -1;
	}

	//read the first line, it might be a status line (e.g. ACQ/1.0 200 OK)
	//or it might be a CRLF followed by a multipart boundary string.  This 
	//tells us what type of response we're dealing iwht
	currentLine = socket->readLine();

	//it's a multipart part
	if(currentLine == "\r\n")
	{
		multiPartResponseState = MultiPartResponseType::MultiPartPart;

		//grab the boundary string, and check it
		currentLine = socket->readLine();
		currentLine.remove(QRegExp("[\r\n]"));

		//If the user never set the boundary string, we're just going to assume that 
		//this is a correct boundary.  However, if they did set the string, we
		//need to confirm that it is correct
		if(!multiPartBoundary.isEmpty() &&
			currentLine != multiPartBoundary)
			return -3;

	}
	//it's a multipart initial or, a non multipart
	else
	{
		//if it turns out to be multiPartInitial, this gets changed when the 
		//Content-type field is read.
		multiPartResponseState = MultiPartResponseType::NotMultiPart;

		QStringList tokens = currentLine.split(" ");
		
		if(tokens.count() < 2)
		{
			return -2;
		}
		
		protocolResponseCode = (ProtocolResponseType::ProtocolResponseType)(tokens[1].toInt());

		int protocolVersionPosition = tokens[0].indexOf('/');
		if(protocolVersionPosition == -1)
		{
			return -2;
		}
		else
		{
			protocol = tokens[0].left(protocolVersionPosition);
			version = tokens[0].mid(protocolVersionPosition+1);
		}
	}

	//read headers
	QRegExp newLineRegExp("[\r\n]+");
	currentLine = socket->readLine();
	while(!newLineRegExp.exactMatch(currentLine))
	{
		currentLine.remove(QRegExp("[\r\n]"));
		QString fieldKey,fieldValue;

		int keyEndPosition = currentLine.indexOf(':');
		if(keyEndPosition < 0)
			return -4;

		fieldKey = currentLine.left(keyEndPosition);
		fieldValue = currentLine.mid(keyEndPosition+2);
		fields[fieldKey] = fieldValue;

		currentLine = socket->readLine();
	}

	//Interpret the headers
	contentLength = fields.value("Content-Length",0).toInt();

	if(fields.value("Content-Type").contains("multipart", Qt::CaseInsensitive))
		multiPartResponseState = MultiPartResponseType::MultiPartInitial;



	//read content
	encodedContent = socket->read(contentLength);
	while(encodedContent.size() < contentLength && 
		(socket->bytesAvailable() > 0 || socket->waitForReadyRead(timeout)))
	{
		encodedContent.append(socket->read(contentLength-encodedContent.size()));
	}

	//decode content
	decodeContent();

	//return length of content read
	if(encodedContent.size() == contentLength)
		return encodedContent.size();
	else
		return 0-encodedContent.size();
}

}; //namespace Picto


//Matt's Random Thoughts
/*********************************************************

The protocolResponse is a real pain to deal with from the recipient's end.
The protocolCommand is much easier to deal with.  The difference is that
when you receive a command, all you have to do is add fields and values to 
a map.  However, the response has a whole bunch of special variables that
may need to be changed based on the fields in the response header.  Making matters 
even more complicated, some of these variables don't have setter methods, so creating 
a new responpse from received data must be done from within the response object.
Currently the read method deals with this (although it isn't particularly elegant,
and may well create chaos in the future as new member variables are added).  
I'm starting to run into issues however, as the original read method was designed 
for handling responses coming from the proxy servers, and now I'm trying to extend it
to handle responses that a command channel is reading (the problem here is that the
command channel will have already read the first line of the response, otherwise
I could just reuse the existing read method).

REWRITING THE WHOLE THING
Would it be easier just to use the fields directly?  To do this, we would remove 
almost all of the member variables (e.g.contentEncodingType), and store everyhting
about the response in the fields map.  This would ential rewriting all
 of the get/set methods (increasing their complexity slightly), as well as
rewriting the generate headers function (decreasing it's complexity significantly).
A new constructor could also be added which would convert the header string into
a full blown response.  Since the getters and setters would rely on the field
map, this would actually be a very simple approach.

The advantage of this is that although it will take a while, once it's done, 
we can easily add new features to the response objects.  Also, instead of using
the read method as a "black box", we could kill read, and let everyone handle 
reading on their own.  Or we could leave it in place, since it's a simple
method, but end users would still have the option of simply passing in the headers
to the constructor.

If I took this approach, I would want to set up the response object so that it is 
almost identical to the command object.  (Or at least so that it can be used in 
exactly the same way).  In the long run, it would be nice if both of these objects 
were mirrors of each other, but this may not be feasible...

DEALING WITH IT AS IT IS
We could also leave the response object more or less intact.  The major problem
is that we need to be able to generate a response object after the first line
of the response has already been read from a socket. There are a bunch of ways to
do this:
	-	Add a statusLine argument to the read method.  The argument would be an empty
		string by default, in which case the status line would be read from the socket.
		If the status line is not an empty string, then it's content would be used
		to set the relevant values.
	-	Create a new read method that takes a string containing all of the headers
		We'd have to be careful not to repeat the code from the original read method, 
		so this approach would require some refactoring (probably a parse headers method)
	-	Create a new constructor that takes all of the headers.  This has the same issue 
		as above.


***********************************************************/

/**********************************************************
MORE RANDOM THOUGHTS

Basically, this all comes down to a question of how we want to arrange 
the command and response objects.  In other words, what are they responsible
for?  Here are a couple of possibilities:

OBJECT RESPONSIBLE FOR READING/WRITING FROM/TO NETWORK

In this case each object will have read/write functions that are passed
a socket.  The object will then pull the data from the network, and parse
it.  If this is the case, we'll need to makes sure that the read/write functions 
are robust, and that they can handle a variety of scenarios.

The problem with this approach is that there are a variety of different cases 
in which we need to read a command/response from the network.  Sometimes 
it will be read in full (for example if we know that we are reading a command).
Other times, we'll read the first line ourselves (to determine if it's a 
command or a response), and then want the object to finish reading itself.

This approach also makes creating a command/response from scratch different from 
reading one off the network.  This isn't neccessarily a problem, but there
may be cases where we want to read an object from a source other than the
network (for example out of a file or database).

Advatages:
- Network reading code only written once
- Can hide a lot of the guts of each object

Disadvantages
- Will require adding read function to command, and write function to response
- How will we handle cases where we don't know if the object is a command
  or a response?

END USER RESPONSIBLE FOR READING/WRITING FROM/TO THE NETWORK

With this approach, the end user has to read the data from the network, and then 
build the command/response from the data.  The command object can already do this
easily (read the headers, and pass them into a command constructor, then read the 
content and pass that to setContent or addToContent).  The response object however 
can't do this yet

Advantages
- End user has nearly full control over the object
- End user can create an object from any source of data (network, database
  text file, etc)
- Object can be created from data read in different locations

Disadvantages
- End user has nearly full control over the object
- Network code gets frequently rewritten (although it is simple...)


WHAT ABOUT THE FIELDS?
If every object can have an infinite number of fields, then the 
access to fields will need to be a public function that directly accesses
the field map.  Without this, an object with a "new" field would not be
usuable, since there would be no access to that field.

The other alternative is to limit the fields in any given object to a preset
list.  If this is the case, then there is no need for a public field access method.  
However, there then becomes a need for individual getter and setter methods for 
every field that could possibly exist in a command/response.  This isn't actually 
that bad, but it will lead to long term maintenance issues, and effectively
limit what the protocols are capable of doing.

SUMMARY

There's a real asymmetry between the command and response objects.  
This is almost certainly due to the fact that these objects were developed
with the server (which parses incoming commands, and generates outgoing responses).
Naturally as the code has progressed, we've started needing to handle the other situations
(incomin responses and outgoing commands).  I've just added code as I needed it, but 
things are starting to get messy, so we need to come up with a more unified approach.

Really, it would be easiest, if both the command and response objects behaved in 
the same way.
 **********************************************************/
