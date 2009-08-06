#include "HTTPGetCommandHandler.h"

#include <QDateTime>
#include <QNetworkInterface>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QBuffer>
#include <QWaitCondition>
#include <QMutex>

#include <math.h>

#include "../../common/globals.h"

HTTPGetCommandHandler::HTTPGetCommandHandler()
{
}

QSharedPointer<Picto::ProtocolResponse> HTTPGetCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	/*! \todo implement handler */
	if(command->getTarget() == "/")
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse("HTTP","1.1",Picto::ProtocolResponseType::OK));

		response->setContentType("text/html; charset=\"utf-8\"");

		response->addField("Expires","-1");

		QList<QHostAddress> hostAddresses = QNetworkInterface::allAddresses();

		QString contentString = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
								"<html><head>\r\n"
								"<title>Picto Server Status</title>\r\n"
								"</head><body>\r\n"
								"<h1>Picto Server</h1>\r\n"
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
	else if(command->getTarget() == "/cambozola.jar")
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse("HTTP","1.1",Picto::ProtocolResponseType::OK));
		
		response->setContentType("application/java-archive");
		
		QFile file(":/mjpeg/cambozola.jar");
		file.open(QIODevice::ReadOnly);
		response->setContent(file.readAll());
		file.close();

		return response;
	}
	else if(command->getTarget() == "/testVid")
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse("HTTP","1.1",Picto::ProtocolResponseType::OK));

		response->setContentType("text/html; charset=\"utf-8\"");

		response->addField("Expires","-1");

		QList<QHostAddress> hostAddresses = QNetworkInterface::allAddresses();

		QString contentString = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
								"<html><head>\r\n"
								"<title>Picto Server Status</title>\r\n"
								"</head><body>\r\n";

		contentString += "<SCRIPT LANGUAGE=\"JavaScript\">\r\n"
						 "\r\n"
						 "if(navigator.appName == \"Netscape\")\r\n"
						 "{\r\n"
						 "  document.write(\"<img src=\\\"testMjpegStream\\\" width=320 height=240>\");\r\n"
						 "}\r\n"
						 "else\r\n"
						 "{\r\n"
						 "  document.write(\"<applet code=\\\"com.charliemouse.cambozola.Viewer\\\" archive=\\\"cambozola.jar\\\" width=\\\"320\\\" height=\\\"240\\\">\");\r\n"
						 "  document.write(\"<param name=\\\"url\\\" value=\\\"testMjpegStream\\\"/>\");\r\n"
						 "  document.write(\"<param name=\\\"accessories\\\" value=\\\"none\\\"/>\");\r\n"
						 "  document.write(\"</applet>\");\r\n"
						 "}\r\n"
						 "\r\n"
						 "</SCRIPT>";

		contentString += "</body></html>\r\n";

		response->setContent(contentString.toUtf8());

		return response;
	}
	else if(command->getTarget() == "/testMjpegStream")
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse("HTTP","1.1",Picto::ProtocolResponseType::OK));
		
		response->setContentType("multipart/x-mixed-replace; boundary=--pictoboundary");
		response->setMultiPartContentType("image/jpeg");
		response->setMultiPartBoundary("--pictoboundary");

		int streamState = command->getFieldValue("X-PictoStreamState").toInt();

		response->setShouldStream(true);
		if(streamState)
		{
			QWaitCondition w;
			QMutex sleepmutex;
			sleepmutex.lock();
			w.wait(&sleepmutex, 75);
			sleepmutex.unlock();
			response->setMultiPart(Picto::MultiPartResponseType::MultiPartPart);
		}
		else
		{
			response->setMultiPart(Picto::MultiPartResponseType::MultiPartInitial);
		}
		response->addField("X-PictoStreamState",QString("%1").arg(++streamState));

		QImage testImage(320,240,QImage::Format_RGB32);
		testImage.fill(0);
		
		QPainter p;
		p.begin(&testImage);
		p.setBrush(Qt::cyan);
		p.drawEllipse(QPoint(160,120),(int) (40.0+40.0*sin(((double) streamState)/20.0)),(int) (40.0+40.0*sin(((double) streamState)/20.0)));
		p.end();

		QByteArray ba;
		QBuffer buffer(&ba);
		buffer.open(QIODevice::WriteOnly);
		testImage.save(&buffer, "JPEG");

		response->setContent(ba);

		return response;
	}
	else
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse("HTTP","1.1",Picto::ProtocolResponseType::NotFound));

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
