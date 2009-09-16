#include <QtTest/QtTest>

#include <QHostAddress>

/*! \brief Contains unit tests for basic PICTO and HTTP protocol commands supported by a PictoServer
 *
 * The TestBasicProtocolCommands object contains a set of unit tests for the basic PICTO and HTTP
 * protocol commands supported by a PictoServer.  These tests only test basic responses, and do not
 * set up long running validations.  Those tests are left to other unit test objects.
 *
 * It is worth noting that this object does contain unit tests to validate correct error code responses.
 */
class TestBasicProtocolCommands: public QObject
{
    Q_OBJECT

public:
	TestBasicProtocolCommands(QHostAddress _serverAddress);

private:

	QHostAddress serverAddress_;
	quint16 port_;

	void testQueryResponse(QString query, QString response);

private slots:
	void httpGet_data();
	void httpGet();
	void pictoGet_data();
	void pictoGet();

	void pictoTerminate();

	void malformedRequest_data();
	void malformedRequest();

	void incompleteRequest();

	void unknownRequest_data();
	void unknownRequest();
/*
	void multipleConnectionBenchmark_data();
	void multipleConnectionBenchmark();
*/
};
