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

	void setMethod(QString _method) { method_ = _method; }
	QString getMethod() { return method_; }

	void setTarget(QString _target) { target_ = _target; }
	QString getTarget() { return target_; }

	void setProtocolName(QString _protocolName) { protocolName_ = _protocolName; }
	QString getProtocolName() { return protocolName_; }

	void setProtocolVersion(QString _protocolVersion) { protocolVersion_ = _protocolVersion; }
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
	/*! Appends the provided \a _content to the ProtocolCommand's existing payload, if any.
	    No Content-Length field is added to the ProtocolCommand's fields, nor is an existing
		Content-Length field modified by this method.
	    \param _content a QByteArray containing the content to be added to the payload
	    \return an integer containing the remaining content length needed for a well formed command
		If no Content-Length field is present, this function will return std::numeric_limits<int>::min()
		If the added content causes the stored content to exceed the value specified by the
		Content-Length field, then this function will return a negative value
	*/
	int addToContent(QByteArray _content);
	QByteArray getContent() { return content_; }

	bool isWellFormed();

	void parse(QString commandText);

	int write(QAbstractSocket *socket);

private:
	QString method_, target_, protocolName_, protocolVersion_;
	std::map<QString,QString> fields_;
	QByteArray content_;

	QString parseError, executeError;

	int remainingContentLength();
};

}; //namespace Picto

#endif
