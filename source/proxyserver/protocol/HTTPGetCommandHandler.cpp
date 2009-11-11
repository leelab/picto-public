#include "HTTPGetCommandHandler.h"

#include <QDateTime>
#include <QNetworkInterface>
#include <QRegExp>

#include "../../common/globals.h"

#include <windows.h>
#include "plexon.h"

#include "ACQGetCommandHandler.h"

#define MAX_MAP_EVENTS_PER_READ 5000

HTTPGetCommandHandler::HTTPGetCommandHandler(QObject *acqPlugin)
{
	acgPlugin = acqPlugin;
}

QSharedPointer<Picto::ProtocolResponse> HTTPGetCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	/*! \todo implement handler */
	if(command->getTarget() == "/")
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->proxyServerAppName,"HTTP","1.1",Picto::ProtocolResponseType::OK));

		response->setContentType("text/html; charset=\"utf-8\"");

		response->addField("Expires","-1");

		QList<QHostAddress> hostAddresses = QNetworkInterface::allAddresses();

		QString contentString = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
								"<html><head>\r\n"
								"<title>Picto Proxy Server Status</title>\r\n"
								"</head><body>\r\n"
								"<h1>Picto Proxy Server</h1>\r\n"
								+ QString("Status as of: %1 GMT\r\n").arg(QDateTime::currentDateTime().toUTC().toString("ddd, dd MMM yyyy hh:mm:ss")) +
								"<br>\r\n"
								"Listening on address: ";

		if(!hostAddresses.empty())
		{
			contentString += hostAddresses[0].toString();
		}
		else
		{
			contentString += "ERROR retrieving address";
		}
		contentString += "</body></html>\r\n";

		response->setContent(contentString.toUtf8());

		return response;
	}
	else if(command->getTarget() == "/data")
	{
		QSharedPointer<Picto::ProtocolResponse> xmlResponse(new Picto::ProtocolResponse(Picto::Names->proxyServerAppName,"HTTP","1.1",Picto::ProtocolResponseType::OK));
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->proxyServerAppName,"HTTP","1.1",Picto::ProtocolResponseType::OK));
		QSharedPointer<ACQGetCommandHandler> ACQGetCommandHandler(new ACQGetCommandHandler(acgPlugin));

		//grab the XML
		xmlResponse = ACQGetCommandHandler->processCommand(command);
		QString content = xmlResponse->getContent();

		//replace the < and > with &lt and &gt
		content.replace(QRegExp("<"),"&lt;");
		content.replace(QRegExp(">"),"&gt;");
		content.replace(QRegExp("\n"),"<br>");

		//wrap it in HTML
		response->setContentType("text/html; charset=\"utf-8\"");
		response->addField("Expires","-1");

		
		content.prepend(QString("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
								"<html><head>\r\n"
								"<title>Picto Proxy Server Status</title>\r\n"
								"</head><body>\r\n"
								"<P>This is the result of<BR> \"GET /data ACQ/1.0</P>\r\n"));
		content.append(QString("</body></html>\r\n"));
		
		response->setContent(content.toUtf8());
		return response;
	}

	else
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->proxyServerAppName,"HTTP","1.1",Picto::ProtocolResponseType::NotFound));

		/*! \todo these should probably be moved to files in the Qt resource system.  This would require having some
		 *        rudimentary server side scripting support to replace basic dynamic values such as the URL that was
		 *        not found below.
		 */

		response->setContent(QString("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
									 "<html><head>\r\n"
									 "<title>404 Not Found</title>\r\n"
									 "</head><body>\r\n"
									 "<h1>Not Found</h1>\r\n"
									 "<p>The requested URL %1 was not found on this server.</p>\r\n"
									 "</body></html>\r\n").arg(command->getTarget()).toUtf8());

		return response;
	}
}
