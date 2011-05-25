//#ifndef NEURALDATACOLLECTOR_H
//#define NEURALDATACOLLECTOR_H
//
//#include <QHostAddress>
//#include <QTimer>
//#include <QTcpSocket>
//#include <QThread>
//#include <QSqlDatabase>
//
//#include "../common/protocol/ProtocolCommand.h"
//#include "../common/protocol/ProtocolResponse.h"
//#include "../common/network/CommandChannel.h"
//#include "alignmenttool.h"
//
///*!	\brief Collects data from a running neural data source.
// *
// *	This object uses a timer to peridocially poll a proxy server for neural data
// *	upon receiving the data, it parses the XML fragment and adds the data to the 
// *	session database.
// *
// *	If you were in the mood to refactor, it would make sense to create a NeuralDataUnit object
// *	and then move the SQL stuff to the SessionInfo object.
// */
//class NeuralDataCollector : public QThread
//{
//	Q_OBJECT
//
//public:
//	NeuralDataCollector(int proxyId, QString sessionDbName, int interval=1000, QObject *parent=0);
//	~NeuralDataCollector() { printf("~NeuralDataCollector\n"); };
//
//	void setCollectionInterval(int interval) { collectionInterval_ = interval;};
//	int getCollectionInterval() { return collectionInterval_; };
//
//	void setAlignmentTool(QSharedPointer<AlignmentTool> a) { align = a; };
//	QSharedPointer<AlignmentTool> getAlignmentTool() { return align; };
//
//	void run();
//	void stop();
//
//private slots:
//	void collectData();
//private:
//	void parseResponse(QSharedPointer<Picto::ProtocolResponse> proxyResponse);
//
//	QSharedPointer<AlignmentTool> align;
//
//	int collectionInterval_;
//	QTimer *pollingTimer_;
//
//	Picto::CommandChannel *cmdChannel_;
//	QHostAddress proxyAddress_;
//	int proxyPort_;
//	//QTcpSocket *proxySocket_;
//
//	QString sessionDbName_;
//	QSqlDatabase db_;
//
//	bool timestampsAligned_;
//};
//
//
//
//
//#endif
//
