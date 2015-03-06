#ifndef _PROTOCOLCOMMAND_H_
#define _PROTOCOLCOMMAND_H_

#include <QString>
#include <QSharedPointer>
#include <QAbstractSocket>

#include "../common.h"

#include <map>

namespace Picto {

/*! \brief Represents a command in either the PICTO or HTTP protocols
 *
 * The ProtocolCommand object is used to parse and execute commands received via the PICTO protocol.
 * Since the PICTO protocol shares the same semantics as the HTTP protocol, a ProtocolCommand object
 * can be used for both purposes.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
struct PICTOLIB_API ProtocolCommand
#else
struct ProtocolCommand
#endif
{
public:
	ProtocolCommand();
	ProtocolCommand(QString commandText);

	/*! \brief Sets the method for this command.  The method is the overall action that the Protocol will perform (ie. PUT or GET in http).*/
	void setMethod(QString _method) { method_ = _method; }
	/*! \brief Returns the method for this command.
	 *	\sa setMethod()
	 */
	QString getMethod() { return method_; }

	/*! \brief Sets the target for this command.*/
	void setTarget(QString _target) { target_ = _target; }
	/*! \brief Gets the target for this command.*/
	QString getTarget() { return target_; }

	/*! \brief Sets the name of the Protocol in which this ProtocolCommand is defined.*/
	void setProtocolName(QString _protocolName) { protocolName_ = _protocolName; }
	/*! \brief Gets the name of the Protocol in which this ProtocolCommand is defined.*/
	QString getProtocolName() { return protocolName_; }

	/*! \brief Sets the version of the Protocol in which this ProtocolCommand is defined.*/
	void setProtocolVersion(QString _protocolVersion) { protocolVersion_ = _protocolVersion; }
	/*! \brief Gets the version of the Protocol in which this ProtocolCommand is defined.*/
	QString getProtocolVersion() { return protocolVersion_; }

	bool hasField(QString field);
	QString getFieldValue(QString field);
	/*! Modifies the headers so that the field \a field contains the value \a value.
	    \param field a QString containing the field name
		\param value a QString containing the value which should be assigned to the field \a field
		\return none
	 */
	void setFieldValue(QString field, QString value);

	bool isPendingContent();

	int setContent(QByteArray _content);

	int addToContent(QByteArray _content);
	/*! \brief Returns the content of this ProtocolCommand.
	 *	\sa setContent(), addToContents()
	 */
	QByteArray getContent() { return content_; }

	bool isWellFormed();

	void parse(QString commandText);

	int write(QAbstractSocket *socket);
	int read(QAbstractSocket *socket, int timeoutMs=0);

private:
	QString method_, target_, protocolName_, protocolVersion_;
	QMap<QString,QString> fields_;
	QByteArray content_;

	QString parseError, executeError;

	int remainingContentLength();
};

}; //namespace Picto

#endif
