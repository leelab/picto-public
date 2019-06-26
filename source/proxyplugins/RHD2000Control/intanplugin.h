#ifndef IntanPLUGIN_H
#define IntanPLUGIN_H

#include <QObject>

#include "mdaio.h"

#define BEHAV_MATCHED_COLUMN 3
#define NEURAL_MATCHED_COLUMN 2
#define BEHAV_TIMESTAMP_COLUMN 0
#define NEURAL_TIMESTAMP_COLUMN 0
#define BEHAV_EVENT_CODE_COLUMN 1
#define NEURAL_EVENT_CODE_COLUMN 1


#include "../../proxyserver/NeuralDataAcqInterface.h"
#include "intanalignmenttool.h"
#include "SmartDataStream.h"
#include "QProgressBar.h"

/*! \brief A plugin allowing the proxy server to interface with a Intan system
*          used for Neural Data Aquisition : align all neural data recorded with the Intan system to the Picto behavioral time
*          Input: 
*				- convert mode: 
*								the .rhd file containing the neural data recorded with the RHD2000 Eval Board.
*				- align mode: 
*								the .rhd file containing the neural data recorded with the RHD2000 Eval Board.
*								the .mda sorted files coming from the Mountain Sort software.
*								the .sql database containing the behavioral alignment info saved in the Picto Server.
*								
*          Output: 
*				- convert mode: .rhd file is converted to a .mda file (data is stored as uint16_t in both files)
*								The neural channels and timepoints are extracted from the .rhd file and saved in a .mda file. 
*								This mda file can then be used as input in the MountainSort pipeline.
*				- align mode: 
*								The alignment info is calculated based on the .rhd and .sql files and saved in the .sql file. 
*								Then the neural data is extracted from the sorted .mda file and saved in the .sql file. 
*
*	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
*	\date 2009-2018
*/
class IntanPlugin : public QObject, public NeuralDataAcqInterface
{

	Q_OBJECT
	Q_INTERFACES(NeuralDataAcqInterface)
	Q_PLUGIN_METADATA(IID "org.blockdesign.Picto.ProxyServer.NeuralDataAcqInterface/1.0")

public:
	IntanPlugin();
	virtual ~IntanPlugin();

	QString device() const;
	void deviceSelected();
	NeuralDataAcqInterface::deviceStatus startDevice();
	NeuralDataAcqInterface::deviceStatus stopDevice();
	NeuralDataAcqInterface::deviceStatus getDeviceStatus();
	float samplingRate();
	QList<QSharedPointer<Picto::DataUnit>> dumpData();
	bool acqDataAfterNow();

	void alignSysTimes(QString rhdFile, QString saveFolder, QProgressBar* bar, QString sqlFile, QString mdaFile, bool convert);
	
private:

	QList<QVariantList> readSQLData(QString path, QProgressBar* bar); //picto
	QList<QVariantList> readRhdData(QString path, QString saveFolder, QProgressBar* bar, bool convertToMda = false); //intan rhd2000
	QVector<double> readRhdSynthData(QString path, QString saveFolder); //testing
	QList<QVariantList> readMdaData(QString path, QProgressBar* bar); //mountainsort
	void readRhdHeader();
	void updateAlignInfo(QList<QVariantList> behavAlignEvents, QList<QVariantList> neuralAlignEvents, QProgressBar* bar);

	void writeData(int dataType, QVariantList data);		
	bool startDataWrite(QString connName);
	bool endDataWrite(QString connName);

	QString fread_QString(FILE* file);
	void allocateIntArray2D(QVector<QVector<int> > &array2D, int xSize, int ySize);
	QString formatSuffix(QString filename);

	FILE* infile;
	float_t sample_rate;

	int16_t num_samples_per_data_block;
	int16_t num_temp_sensor_channels;
	int16_t data_file_main_version_number;
	int16_t data_file_secondary_version_number;
	QVector<int> event_codes;
	int16_t num_amplifier_channels;
	int num_data_blocks;

	int numTimeStampsLH[32]; //Array that stores how many low-to-high transitions occur for each digital input and output channel
	int numTimeStampsHL[32]; //Array that stores how many high-to-low transitions occur for each digital input and output channel
	QString namePrefix; //QString containing the prefix for the channel name (native channel name before channel number)
	QString nameSuffix; //QString containing the suffix for the channel name (custom channel name after channel number)
	QString name; //QString containing the complete channel name
	QString nameNumber; //QString containing the channel number
	qint64 commentPos; //Variable that stores the position of the "Comment" field within the output file, so that it after reading the digital inputs the initial values can be written into the field

	SmartDataStream *smartStream; //Buffered data stream that allows bulk writing to output file through a compile-time-determined buffer size
	
	NeuralDataAcqInterface::deviceStatus deviceStatus_;
	QSharedPointer<IntanAlignmentTool> alignmentTool_;
	QSqlDatabase newSession_;
	QSharedPointer<QSqlQuery> query_;	
};


#endif