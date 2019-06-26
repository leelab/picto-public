#include "intanplugin.h"

#include "QSqlQuery.h"
#include "QFileInfo.h"

/*! \brief Constructs a IntanPlugin object.*/
IntanPlugin::IntanPlugin()
{
	deviceStatus_ = started;
	sample_rate = 0.;
}

IntanPlugin::~IntanPlugin()
{

}
QString IntanPlugin::device() const
{
	return "Intan";
}

void IntanPlugin::deviceSelected()
{

}
bool IntanPlugin::acqDataAfterNow()
{
	startDevice();
	return true;
}
NeuralDataAcqInterface::deviceStatus IntanPlugin::startDevice()
{
	deviceStatus_ = started;
	return deviceStatus_;
}

NeuralDataAcqInterface::deviceStatus IntanPlugin::stopDevice()
{
	deviceStatus_ = notStarted;
	return deviceStatus_;
}

NeuralDataAcqInterface::deviceStatus IntanPlugin::getDeviceStatus()
{
	deviceStatus_ = hasData;
	return deviceStatus_;
}


float IntanPlugin::samplingRate()
{
	return 0.;
}

QList<QSharedPointer<Picto::DataUnit>> IntanPlugin::dumpData()
{
	QList<QSharedPointer<Picto::DataUnit>> returnList;

	return returnList;
}
void IntanPlugin::alignSysTimes(QString rhdFile, QString saveFolder, QProgressBar* bar, QString sqlFile, QString mdaFile, bool convert)
{
	if (convert)
	{
		bar->setMaximum(30);

		// Convert mode
		QList<QVariantList> neuralAlignEvents = readRhdData(rhdFile, saveFolder, bar, true); //intan to mda
	}
	else
	{
		bar->setMaximum(100);

		// Align mode
		newSession_ = QSqlDatabase::addDatabase("QSQLITE", "IntanConnection");
		newSession_.setDatabaseName(sqlFile);
		newSession_.open();

		startDataWrite("IntanConnection");

		//sql file: retrieve align codes from picto behavioral file
		QList<QVariantList> behavAlignEvents = readSQLData(sqlFile,bar); //picto		
		
		// rhd file: retrieve align codes from intan neural file		
		QList<QVariantList> neuralAlignEvents = readRhdData(rhdFile, "", bar); //intan		

		// Calculate the alignment coefficients (linear regression) and update the sql file
		updateAlignInfo(behavAlignEvents, neuralAlignEvents,bar);

		// mda file: retrieve sorted neural data (spikes, lfp) from mountain sort file and update the sql file
		QList<QVariantList> neuralData = readMdaData(mdaFile, bar); //mountain sort (sample rate given by the rhd file)
	
		endDataWrite("IntanConnection");
	}
}

bool IntanPlugin::startDataWrite(QString connName)
{
	QSqlDatabase sessionDb;
	if (QSqlDatabase::contains(connName))
	{
		sessionDb = QSqlDatabase::database(connName);
	}
	if (!query_.isNull())
	{
		Q_ASSERT(query_.isNull());
	}
	query_ = QSharedPointer<QSqlQuery>(new QSqlQuery(sessionDb));
	bool success = sessionDb.transaction();
	if (!success)
	{	
		return false;
	}
	return true;
}
bool IntanPlugin::endDataWrite(QString connName)
{
	QSqlDatabase sessionDb;
	if (QSqlDatabase::contains(connName))
	{
		sessionDb = QSqlDatabase::database(connName);
	}
	bool success = sessionDb.commit();
	query_.clear();
	if (!success)
	{
		sessionDb.rollback();
		return false;
	}
	return true;
}
QList<QVariantList> IntanPlugin::readSQLData(QString path, QProgressBar* bar)
{
	QList<QVariantList> returnVal;

	QString querysql = "SELECT timestamp,aligncode,aligneventnumber FROM behavioralalignevents order by timestamp";
	bool success = query_->exec(querysql);
	if (!success)
		return returnVal;

	while (query_->next())
	{
		QVariantList returnRow;
		returnRow.append(query_->value(0)); //timestamp
		returnRow.append(query_->value(1));	//align code
		returnRow.append(query_->value(2)); //aligneventnumber
		returnRow.append(0);				//matched

		returnVal.append(returnRow);
	}
	int Barval = 10 + bar->value();
	bar->setValue(Barval);
	return returnVal;
}
QList<QVariantList> IntanPlugin::readMdaData(QString path, QProgressBar* bar)
{
	// Mda file (Mountain Sort output with sorted neural data)		
	QList<QVariantList> returnVal;	

	if (sample_rate == 0.)
		return returnVal;

	QByteArray ba = path.toLatin1();
	const char *fName = ba.data();
	FILE* infile = fopen(fName, "rb");
	struct MDAIO_HEADER H;
	bigint M, N;
	
	if (!infile)
		return returnVal;

	//read the header
	mda_read_header(&H, infile);

	//if the data type is zero then there was a problem reading
	if (!H.data_type) {
		fclose(infile);
		return returnVal;
	}

	//get the dimensions and allocate the in/out arrays
	M = H.dims[0];
	N = H.dims[1];

	double* mda_data;
	mda_data = (double*)malloc(sizeof(double) * M * N); //if code -7:double
	//mda_read_byte(mda_data, &H, M * N, infile); //any type
	mda_read_float64(mda_data, &H, M * N, infile);
	

	//Update sql with spikes
	QString tableName = "spikes";
	QString colNames = "dataid, channel, timestamp, unit";
	QStringList placeHolderList = colNames.split(",", QString::SkipEmptyParts);
	for (int i = 0; i < placeHolderList.size(); i++)
	{
		placeHolderList[i] = "?";
	}
	QString placeHolderString = placeHolderList.join(","); 

	QString queryString = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tableName).arg(colNames).arg(placeHolderString);

	//mda_data: ch1,time1,unit1,ampl1,ch2,time2,unit2,ampl2, ... where (ch1,t1,unit1,ampl1) is one single sample (a column)
	bigint d_id = 0;
	query_->finish();
	for (int j = 0; j < N; j++)
	{
		int columnNb = M * j;
		QVariantList returnCol;

		query_->prepare(queryString);
		d_id++;
		QVariant dataid(d_id);
		query_->addBindValue(dataid); //dataid

		int val = 0;
		int index = columnNb;
		val = (int)(mda_data[index]);
		QVariant channel(val);
		returnCol.append(channel);
		query_->addBindValue(channel); //channel

		index = columnNb + 1;

		double Dval = 0.00000;
		Dval = (double)(mda_data[index]);
		double val2 = 0.000000000000;
		val2 = Dval / sample_rate;
		QVariant timestamp(val2);
		returnCol.append(timestamp);
		query_->addBindValue(timestamp); //timestamp

		index = columnNb + 2;
		val = (int)(mda_data[index]);
		QVariant unit(val);
		returnCol.append(unit);
		query_->addBindValue(unit); //unit

		returnVal.append(returnCol);

		bool success = query_->exec();
		if (!success)
		{
			return returnVal;
		}
	}
	
	int Barval = 15 + bar->value();
	bar->setValue(Barval);

	//update DataSource key in sessioninfo (resolution)
	QString val;
	double resolution = 0.00000;
	resolution = (double)(1 / sample_rate);
	QSharedPointer<QXmlStreamWriter> xmlStreamWriter(new QXmlStreamWriter(&val));
	if (xmlStreamWriter)
	{
		xmlStreamWriter->writeStartElement("DataSourceInfo");
		xmlStreamWriter->writeAttribute("name", "spikes");
		xmlStreamWriter->writeAttribute("tableName", "spikes");
		xmlStreamWriter->writeAttribute("resolution", QString("%1").arg(resolution));
		xmlStreamWriter->writeEndElement();
	}

	
	{
		QString tableName = "sessioninfo";
		QString colNames = "dataid,key,value";
		QStringList placeHolderList = colNames.split(",", QString::SkipEmptyParts);
		for (int i = 0; i < placeHolderList.size(); i++)
		{
			placeHolderList[i] = "?";
		}
		QString placeHolderString = placeHolderList.join(","); //empty array to bind values to 
		queryString = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tableName).arg(colNames).arg(placeHolderString);
		query_->finish();
		query_->prepare(queryString);
		QString keyVal = "DataSource";
		int dataid = 100;
		query_->addBindValue(QVariant(dataid));
		query_->addBindValue(QVariant(keyVal));
		query_->addBindValue(QVariant(val));

		bool success = query_->exec();
		if (!success)
		{
			return returnVal;
		}
	}
	
	free(mda_data);
	fclose(infile);

	Barval = 15 + bar->value();
	bar->setValue(Barval);

	return returnVal;
}

QList<QVariantList> IntanPlugin::readRhdData(QString path, QString saveFolder, QProgressBar* bar, bool convertToMda)
{
	//Intan Rhd file (with neural data: extract alignment info: code and timestamps)

	QList<QVariantList> returnVal;

	//Set up the filename for the output file (identical to input file, but with a .nex suffix instead of .rhd)
	QFileInfo inFileInfo(path);
	QString inFileDirPath = inFileInfo.dir().absolutePath();
	QString outFilename = formatSuffix(inFileInfo.fileName());

	//Open input file, and abort if input file cannot be opened
	QFile inFile(path);
	if (!inFile.open(QIODevice::ReadOnly)) {
		qDebug() << "Unable to open input file.";
		return returnVal;
	}

	//Store the size of the input file
	int filesize = inFile.size();

	//Set up input stream
	QDataStream inStream(&inFile);
	inStream.setVersion(QDataStream::Qt_4_8);
	inStream.setByteOrder(QDataStream::LittleEndian);
	inStream.setFloatingPointPrecision(QDataStream::SinglePrecision);

	//Set up temporary variables
	qint16 tempQint16;
	quint16 tempQuint16;
	quint32 tempQuint32;
	float tempsingle;
	QString tempQString;


	//Read magic number, and abort if this file does not have an .rhd magic number
	inStream >> tempQuint32;
	long magic_number = (long)tempQuint32;
	if (magic_number != 0xc6912702) {
		qDebug() << "Unrecognized file type";
		return returnVal;
	}

	//Read version number
	inStream >> tempQint16;
	int data_file_main_version_number = tempQint16;
	inStream >> tempQint16;
	int data_file_secondary_version_number = tempQint16;

	//Print satus message
	QString version_number = QString::number(data_file_main_version_number) + "." + QString::number(data_file_secondary_version_number);
	qDebug() << "\nReading Intan Technologies RHD2000 Data File, Version" << version_number << "\n";

	//Get correct number of samples per data block
	num_samples_per_data_block = 0;
	if (data_file_main_version_number == 1)
		num_samples_per_data_block = 60;
	else
		num_samples_per_data_block = 128;

	//Read information of sampling rate and amplifier frequency settings
	inStream >> tempsingle;
	sample_rate = tempsingle;

	//Skip 36 bytes (DSP, actual and desired bandwidths, notch filter mode, and impedance test frequency) because they are not used in this conversion
	inStream.skipRawData(36);

	//Read 3 note QStrings (which are currently unused in this conversion)
	inStream >> tempQString;
	QString note1 = tempQString;
	inStream >> tempQString;
	QString note2 = tempQString;
	inStream >> tempQString;
	QString note3 = tempQString;

	//If data file is from GUI v1.1 or later, see if temperature sensor data was saved
	int num_temp_sensor_channels = 0;
	if ((data_file_main_version_number == 1 && data_file_secondary_version_number >= 1) || (data_file_main_version_number > 1)) {
		inStream >> tempQint16;
		num_temp_sensor_channels = tempQint16;
	}

	//If data file is from GUI v1.3 or later, load eval board mode.
	int eval_board_mode = 0;
	if ((data_file_main_version_number == 1 && data_file_secondary_version_number >= 3) || (data_file_main_version_number > 1)) {
		inStream >> tempQint16;
		eval_board_mode = tempQint16;
	}

	//If data file is from v2.0 or later (Intan Recording Controller), load name of digital reference channel
	QString reference_channel = "";
	if (data_file_main_version_number > 1) {
		inStream >> tempQString;
		reference_channel = tempQString;
	}

	//Define data structure for spike trigger settings (which are currently unused in this conversion)
	typedef struct spike_trigger_structure {
		qint16 voltage_trigger_mode;
		qint16 voltage_threshold;
		qint16 digital_trigger_channel;
		qint16 digital_edge_polarity;
	} spike_trigger_struct;

	spike_trigger_struct new_trigger_channel;
	QVector <spike_trigger_struct> spike_triggers(0);

	//Define data structure for data channels
	typedef struct channel_structure {
		QString native_channel_name;
		QString custom_channel_name;
		qint16 native_order;
		qint16 custom_order;
		qint16 board_stream;
		qint16 chip_channel;
		QString port_name; //in documentation, called "Signal group name"
		QString port_prefix; //in documentation, called "Signal group prefix"
		qint16 port_number; //in documentation, "number of channels in signal group"
		float electrode_impedance_magnitude;
		float electrode_impedance_phase;
	} channel_struct;

	channel_struct new_channel;

	//Create structure vectors for each type of data channel
	QVector <channel_struct> amplifier_channels(0);
	QVector <channel_struct> aux_input_channels(0);
	QVector <channel_struct> supply_voltage_channels(0);
	QVector <channel_struct> board_adc_channels(0);
	QVector <channel_struct> board_dig_in_channels(0);
	QVector <channel_struct> board_dig_out_channels(0);

	//Initialize structure arrays' indices
	int amplifier_index = 0;
	int aux_input_index = 0;
	int supply_voltage_index = 0;
	int board_adc_index = 0;
	int board_dig_in_index = 0;
	int board_dig_out_index = 0;

	//Read signal summary from data file header
	inStream >> tempQint16;
	int number_of_signal_groups = tempQint16;

	//Read all "signal groups" present in the .rhd file
	int signal_group;
	for (signal_group = 0; signal_group < number_of_signal_groups; signal_group++) {
		inStream >> tempQString;
		QString signal_group_name = tempQString;
		inStream >> tempQString;
		QString signal_group_prefix = tempQString;

		inStream >> tempQint16;
		int signal_group_enabled = tempQint16;
		inStream >> tempQint16;
		int signal_group_num_channels = tempQint16;
		inStream >> tempQint16;
		//int signal_group_num_amp_channels = tempQint16;

		if (signal_group_num_channels > 0 && signal_group_enabled > 0) {
			new_channel.port_name = signal_group_name;
			new_channel.port_prefix = signal_group_prefix;
			new_channel.port_number = signal_group;

			int signal_channel;
			for (signal_channel = 0; signal_channel < signal_group_num_channels; signal_channel++) {
				inStream >> tempQString;
				new_channel.native_channel_name = tempQString;
				inStream >> tempQString;
				new_channel.custom_channel_name = tempQString;
				inStream >> tempQint16;
				new_channel.native_order = tempQint16;
				inStream >> tempQint16;
				new_channel.custom_order = tempQint16;

				inStream >> tempQint16;
				int signal_type = tempQint16;
				inStream >> tempQint16;
				int channel_enabled = tempQint16;

				inStream >> tempQint16;
				new_channel.chip_channel = tempQint16;
				inStream >> tempQint16;
				new_channel.board_stream = tempQint16;

				inStream >> tempQint16;
				new_trigger_channel.voltage_trigger_mode = tempQint16;
				inStream >> tempQint16;
				new_trigger_channel.voltage_threshold = tempQint16;
				inStream >> tempQint16;
				new_trigger_channel.digital_trigger_channel = tempQint16;
				inStream >> tempQint16;
				new_trigger_channel.digital_edge_polarity = tempQint16;

				inStream >> tempsingle;
				new_channel.electrode_impedance_magnitude = tempsingle;
				inStream >> tempsingle;
				new_channel.electrode_impedance_phase = tempsingle;

				if (channel_enabled) {
					switch (signal_type) {
					case 0:
						amplifier_channels.append(new_channel);
						spike_triggers.append(new_trigger_channel);
						amplifier_index++;
						break;
					case 1:
						aux_input_channels.append(new_channel);
						aux_input_index++;
						break;
					case 2:
						supply_voltage_channels.append(new_channel);
						supply_voltage_index++;
						break;
					case 3:
						board_adc_channels.append(new_channel);
						board_adc_index++;
						break;
					case 4:
						board_dig_in_channels.append(new_channel);
						board_dig_in_index++;
						break;
					case 5:
						board_dig_out_channels.append(new_channel);
						board_dig_out_index++;
						break;

					default:
						qDebug() << "Unknown channel type";
					}
				}
			}
		}
	}

	//Summarize contents of data file
	num_amplifier_channels = amplifier_index;
	int num_aux_input_channels = aux_input_index;
	int num_supply_voltage_channels = supply_voltage_index;
	int num_board_adc_channels = board_adc_index;
	int num_board_dig_in_channels = board_dig_in_index;
	int num_board_dig_out_channels = board_dig_out_index;

	
	//Determine how many samples the data file contains

	//Each data block contains num_samples_per_data_block amplifier samples.
	int bytes_per_block = num_samples_per_data_block * 4; //timestamp data
	bytes_per_block = bytes_per_block + num_samples_per_data_block * 2 * num_amplifier_channels;
	//Auxiliary inputs are sampled 4x slower than amplifiers
	bytes_per_block = bytes_per_block + (num_samples_per_data_block / 4) * 2 * num_aux_input_channels;
	//Supply voltage is sampled once per data block
	bytes_per_block = bytes_per_block + 1 * 2 * num_supply_voltage_channels;
	//Board analog inputs are sampled at same rate as amplifiers
	bytes_per_block = bytes_per_block + num_samples_per_data_block * 2 * num_board_adc_channels;
	//Board digital inputs are sampled at same rate as amplifiers
	if (num_board_dig_in_channels > 0)
		bytes_per_block = bytes_per_block + num_samples_per_data_block * 2;
	//Board digital outputs are sampled at same rate as amplifiers
	if (num_board_dig_out_channels > 0)
		bytes_per_block = bytes_per_block + num_samples_per_data_block * 2;
	//Temp sensor is sampled once per data block
	if (num_temp_sensor_channels > 0)
		bytes_per_block = bytes_per_block + 1 * 2 * num_temp_sensor_channels;

	//How many data blocks remain in this file?
	int data_present = 0;
	int bytes_remaining = filesize - inFile.pos();
	if (bytes_remaining > 0)
		data_present = 1;

	num_data_blocks = bytes_remaining / bytes_per_block;

	int num_amplifier_samples = num_samples_per_data_block * num_data_blocks;
	//int num_board_dig_in_samples = num_samples_per_data_block * num_data_blocks;

	double record_time = num_amplifier_samples / sample_rate;

	//Print header summary
	if (data_present)
		qDebug("File contains %0.3f seconds of data. Amplifiers were sampled at %0.2f kS/s.\n", record_time, sample_rate / 1000);
	else
		qDebug("Header file contains no data. Amplifiers were sampled at %0.2f kS/s.\n", sample_rate / 1000);

	QVector<double> t_amplifier;
	QVector<double> amplifier_data;
	QVector<qint16> board_dig_in_raw;
	QVector<double> board_dig_in_data;
	QVector<QVector<int> > boardDigIn;

	//int num_amplifier_samples = num_samples_per_data_block * num_data_blocks;
	//int num_board_dig_in_samples = num_samples_per_data_block * num_data_blocks;

	//Initialize board_dig_in_raw vector
	//board_dig_in_raw.resize(num_board_dig_in_samples);
	//Initialize board_dig_in_data vector
	//board_dig_in_data.resize(num_board_dig_in_samples);
	//Initialize t_amplifier vector
	//t_amplifier.resize(num_amplifier_samples);
	//Initialize amplifier_data vector
	//amplifier_data.resize(num_amplifier_samples);
	qDebug() << "Reading data from file...";


	int i; //int variables used for various indices

	//Set dig_in_present to 1 if there's at least one digital input channel
	int dig_in_present = 0;
	int dig_out_present = 0;
	if (num_board_dig_in_channels > 0) {
		dig_in_present = 1;
	}
	if (num_board_dig_out_channels > 0) {
		dig_out_present = 1;
	}

	int Barval = 5 + bar->value();
	bar->setValue(Barval);

	//Create union to read input file from an unsigned character array
	union en_union
	{
		uchar u[4];
		qint32 i;
		quint32 ui;
	} en;

	if (data_present)
	{		
		qint64 dDataStart = inFile.pos();

		//Reset inFile's position back to the beginning of the digital data
		inFile.seek(dDataStart);


		//Create Char Array for bulk reading
		char *inCharArray = new char[bytes_per_block];
		uchar *inUCharArray;

		int blockNb = 0;

		allocateIntArray2D(boardDigIn, 16, num_samples_per_data_block * num_data_blocks);

		//for (i = 1; i < num_data_blocks+1; i++)
		for (blockNb = 0; blockNb < num_data_blocks; blockNb++)
		{

			//Read the data in bulk into an array, and then read from that array
			inStream.readRawData(inCharArray, bytes_per_block);
			//Convert char array to unsigned char array
			inUCharArray = (uchar*)inCharArray;

			int m;

			//Read t_amplifier
			for (m = 0; m < num_samples_per_data_block; m++) {

				//Read each byte into a union as a character
				en.u[0] = inUCharArray[4 * m];
				en.u[1] = inUCharArray[4 * m + 1];
				en.u[2] = inUCharArray[4 * m + 2];
				en.u[3] = inUCharArray[4 * m + 3];

				//If the .rhd file is updated to where it allows negative entries to t_amplifier, read t_amplifier data as integers
				if ((data_file_main_version_number == 1 && data_file_secondary_version_number >= 2) || (data_file_main_version_number > 1))
					t_amplifier.push_back(en.i);

				//If the .rhd is not updated and doesn't allow negative entries to t_amplifier, read t_amplifier data as unsigned integers
				else
					t_amplifier.push_back(en.ui);

				//Scale t_amplifier data to a measure in seconds
				//t_amplifier.push_back((double)(t_amplifier[m] / sample_rate));
			}

			if (num_amplifier_channels > 0)
			{
				//Read amplifier data
				//Skip t_amplifier vector (4 * num_samples_per_data_block)
				i = 4 * num_samples_per_data_block;

				//If the current channel being read is a headstage amplifier channel, read all amplifier samples in this data block
				for (m = 0; m < (num_samples_per_data_block*num_amplifier_channels); m++) {
					tempQuint16 = (quint16(inUCharArray[i + 2 * m])) + ((quint16(inUCharArray[i + 1 + 2 * m])) << 8);
					amplifier_data.push_back(tempQuint16);
				}
			}


			if (num_board_dig_in_channels > 0)
			{
				//Skip t_amplifier, amplifier_data, aux_input_data, supply_voltage_data, temp_sensor_data, board_adc_data
				i = (4 * num_samples_per_data_block) + (2 * num_amplifier_channels*num_samples_per_data_block) + (2 * num_aux_input_channels*num_samples_per_data_block / 4);
				//Skip supply_voltage_data, temp_sensor_data, board_adc_data
				i = i + (2 * num_supply_voltage_channels) + (2 * num_temp_sensor_channels) + (2 * num_board_adc_channels*num_samples_per_data_block);

				//Read all the digital input samples of this data block intao board_dig_in_raw
				for (m = 0; m < num_samples_per_data_block; m++) {
					tempQuint16 = (quint16(inUCharArray[i + 2 * m])) + ((quint16(inUCharArray[i + 1 + 2 * m])) << 8);
					board_dig_in_raw.push_back(tempQuint16); //each uint16 contains 16 bits word corresponding to 16 channels high/low values
				}

				int mask;
				//Extract the current digital input channel data from the raw data
				//mask = pow(2, board_dig_in_channels[current_event / 2].native_order);
				mask = 1;
				for (int j = 0; j < num_samples_per_data_block; j++) {
					board_dig_in_data.push_back((board_dig_in_raw[j] & mask) > 0);
				}
			}		
		}
		// Make sure we have read exactly the right amount of data.
		bytes_remaining = filesize - inFile.pos();
		if (bytes_remaining != 0)
		printf("Error: End of file not reached. \n");
	}
	qDebug() << "Closing data file " << bytes_remaining;
	
	// Close data file.
	inFile.close();

	Barval = 10 + bar->value();
	bar->setValue(Barval);

	if (data_present)
	{
		printf("Parsing data \n");
		qDebug() << "Parsing data " << data_present;
	

		//TESTING: if no amplifier data was recorded, get synthetic amplifier_data from synth.rhd
		if (convertToMda && (amplifier_data.size() == 0))
		{
			//read amplifier_data from synthetic file
			QFileInfo inFileInfo(path);
			QString inFileDirPath = inFileInfo.dir().absolutePath();
			QString synthPath = inFileDirPath + "\\synth.rhd";			

			num_amplifier_channels = 0;
			num_data_blocks = 0;
			num_samples_per_data_block = 0;
			amplifier_data = readRhdSynthData(synthPath, saveFolder);
		}
		//END TESTING

		if (convertToMda && (amplifier_data.size() > 0)) //data format uint16
		{
			printf("Converting data \n");
			qDebug() << "Converting data ";

			//Set up the filename for the output file (identical to input file, but with a .mda suffix instead of .rhd)
			QFileInfo inFileInfo(path);
			QString inFileDirPath = inFileInfo.dir().absolutePath();
			QString outFilename = formatSuffix(inFileInfo.fileName());
			QString strOutName = saveFolder + "/" + outFilename;
			QByteArray ba = strOutName.toLatin1();
			const char *outFileName = ba.data();
			FILE* outfile = fopen(outFileName, "wb");

			int nbSamples = amplifier_data.size() / num_amplifier_channels;

			uint16_t* data_out;
			data_out = (uint16_t*)malloc(sizeof(uint16_t) * num_amplifier_channels * nbSamples);
						
			int sp = 0;
			
			for (int blockNb = 0; blockNb < num_data_blocks; blockNb++)
			{				
				if (blockNb == int(num_data_blocks / 2))
				{
					int Barval = 5 + bar->value();
					bar->setValue(Barval);
				}

				for (int j = 0; j < num_samples_per_data_block; j++)
				{
					for (i = 0; i < num_amplifier_channels; i++)
					{
						int ind = (i * num_samples_per_data_block) + j + (blockNb * num_amplifier_channels *num_samples_per_data_block);
						uint16_t val = (uint16_t)(amplifier_data[ind]);
						data_out[sp] = val;
						sp++;
					}
				}
			}

			struct MDAIO_HEADER H;			
			H.dims[0] = num_amplifier_channels;
			H.dims[1] = nbSamples;
			H.num_dims = 2;
			//H.num_bytes_per_entry = sizeof(float);
			H.num_bytes_per_entry = sizeof(int16_t);
			unsigned long int type = -6;
			H.data_type = type;
						
			mda_write_header(&H, outfile);
			mda_write_uint16(data_out, &H, num_amplifier_channels*nbSamples, outfile);

			//clean up and we're done
			free(data_out);
			fclose(outfile);
		}
		else
		{
			int eventCode = 0;
			for (int m = 0; m < t_amplifier.size(); ++m) //int32
			{
				QVariantList event;
				double time = 0.000000000;
				double val;
				val = (double)(t_amplifier[m]);
				time = (double)(val / sample_rate);
				QVariant var(time);
				event.append(var); //timestamp in sec
								
				if ((eventCode != board_dig_in_raw[m]) && (board_dig_in_raw[m] > 0))
				{
					eventCode = board_dig_in_raw[m];
					QVariant var2(eventCode);
					event.append(var2); //aligncode
					event.append(QVariant(0)); //matched
					returnVal.append(event); //list of timestamp(sec),aligncode,matched
				}
				/*for (int k = 0; k < num_board_dig_in_channels; ++k)
				{
					if (board_dig_in_data[k] == 1)
					{
						int val = pow(2, board_dig_in_data[k]);
						eventCode = eventCode + val;
					}
				}*/
				
			}
		}
		
	}

	Barval = 10 + bar->value();
	bar->setValue(Barval);

	if (convertToMda)
		return returnVal;
	
	// update sql file
	QString tableName = "neuralalignevents";
	QString colNames = "dataid,timestamp,aligncode,matched"; //timestamps in sec 
	QStringList placeHolderList = colNames.split(",", QString::SkipEmptyParts);
	for (int i = 0; i < placeHolderList.size(); i++)
	{
		placeHolderList[i] = "?";
	}
	QString placeHolderString = placeHolderList.join(","); 
	QString queryString = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tableName).arg(colNames).arg(placeHolderString);

	int did = 0;	
	query_->finish();
	foreach(QVariantList event, returnVal)
	{
		query_->prepare(queryString);

		did++;
		QVariant dataid(did);
		query_->addBindValue(dataid);
		foreach(QVariant val, event)
			query_->addBindValue(val);

		bool success = query_->exec();
		if (!success)
		{
			return returnVal;
		}
	}	
	
	return returnVal;
}

QVector<double> IntanPlugin::readRhdSynthData(QString path, QString saveFolder)
{
	QVector<double> amp_data;

	//Set up the filename for the output file (identical to input file, but with a .nex suffix instead of .rhd)
	QFileInfo inFileInfo(path);
	QString inFileDirPath = inFileInfo.dir().absolutePath();
	QString outFilename = formatSuffix(inFileInfo.fileName());

	//Open input file, and abort if input file cannot be opened
	QFile inFile(path);
	if (!inFile.open(QIODevice::ReadOnly)) {
		qDebug() << "Unable to open input file.";
		return amp_data;
	}

	//Store the size of the input file
	int filesize = inFile.size();

	//Set up input stream
	QDataStream inStream(&inFile);
	inStream.setVersion(QDataStream::Qt_4_8);
	inStream.setByteOrder(QDataStream::LittleEndian);
	inStream.setFloatingPointPrecision(QDataStream::SinglePrecision);

	//Set up temporary variables
	qint16 tempQint16;
	quint16 tempQuint16;
	quint32 tempQuint32;
	float tempsingle;
	QString tempQString;


	//Read magic number, and abort if this file does not have an .rhd magic number
	inStream >> tempQuint32;
	long magic_number = (long)tempQuint32;
	if (magic_number != 0xc6912702) {
		qDebug() << "Unrecognized file type";
		return amp_data;
	}

	//Read version number
	inStream >> tempQint16;
	int data_file_main_version_number = tempQint16;
	inStream >> tempQint16;
	int data_file_secondary_version_number = tempQint16;

	//Print satus message
	QString version_number = QString::number(data_file_main_version_number) + "." + QString::number(data_file_secondary_version_number);
	qDebug() << "\nReading Intan Technologies RHD2000 Data File, Version" << version_number << "\n";

	//Get correct number of samples per data block
	num_samples_per_data_block = 0;
	if (data_file_main_version_number == 1)
		num_samples_per_data_block = 60;
	else
		num_samples_per_data_block = 128;

	//Read information of sampling rate and amplifier frequency settings
	inStream >> tempsingle;

	//Skip 36 bytes (DSP, actual and desired bandwidths, notch filter mode, and impedance test frequency) because they are not used in this conversion
	inStream.skipRawData(36);

	//Read 3 note QStrings (which are currently unused in this conversion)
	inStream >> tempQString;
	inStream >> tempQString;
	inStream >> tempQString;

	//If data file is from GUI v1.1 or later, see if temperature sensor data was saved
	int num_temp_sensor_channels = 0;
	if ((data_file_main_version_number == 1 && data_file_secondary_version_number >= 1) || (data_file_main_version_number > 1)) {
		inStream >> tempQint16;
	}

	//If data file is from GUI v1.3 or later, load eval board mode.
	if ((data_file_main_version_number == 1 && data_file_secondary_version_number >= 3) || (data_file_main_version_number > 1)) {
		inStream >> tempQint16;
	}

	//If data file is from v2.0 or later (Intan Recording Controller), load name of digital reference channel
	QString reference_channel = "";
	if (data_file_main_version_number > 1) {
		inStream >> tempQString;
	}

	//Define data structure for data channels
	typedef struct channel_structure {
		QString native_channel_name;
		QString custom_channel_name;
		qint16 native_order;
		qint16 custom_order;
		qint16 board_stream;
		qint16 chip_channel;
		QString port_name; //in documentation, called "Signal group name"
		QString port_prefix; //in documentation, called "Signal group prefix"
		qint16 port_number; //in documentation, "number of channels in signal group"
		float electrode_impedance_magnitude;
		float electrode_impedance_phase;
	} channel_struct;

	channel_struct new_channel;

	//Create structure vectors for each type of data channel
	QVector <channel_struct> amplifier_channels(0);
	QVector <channel_struct> aux_input_channels(0);
	QVector <channel_struct> supply_voltage_channels(0);
	QVector <channel_struct> board_adc_channels(0);
	QVector <channel_struct> board_dig_in_channels(0);
	QVector <channel_struct> board_dig_out_channels(0);

	//Initialize structure arrays' indices
	int amplifier_index = 0;
	int aux_input_index = 0;
	int supply_voltage_index = 0;
	int board_adc_index = 0;
	int board_dig_in_index = 0;
	int board_dig_out_index = 0;

	//Read signal summary from data file header
	inStream >> tempQint16;
	int number_of_signal_groups = tempQint16;

	//Read all "signal groups" present in the .rhd file
	int signal_group;
	for (signal_group = 0; signal_group < number_of_signal_groups; signal_group++) {
		inStream >> tempQString;
		QString signal_group_name = tempQString;
		inStream >> tempQString;
		QString signal_group_prefix = tempQString;

		inStream >> tempQint16;
		int signal_group_enabled = tempQint16;
		inStream >> tempQint16;
		int signal_group_num_channels = tempQint16;
		inStream >> tempQint16;
		//int signal_group_num_amp_channels = tempQint16;

		if (signal_group_num_channels > 0 && signal_group_enabled > 0) {
			new_channel.port_name = signal_group_name;
			new_channel.port_prefix = signal_group_prefix;
			new_channel.port_number = signal_group;

			int signal_channel;
			for (signal_channel = 0; signal_channel < signal_group_num_channels; signal_channel++) {
				inStream >> tempQString;
				new_channel.native_channel_name = tempQString;
				inStream >> tempQString;
				new_channel.custom_channel_name = tempQString;
				inStream >> tempQint16;
				new_channel.native_order = tempQint16;
				inStream >> tempQint16;
				new_channel.custom_order = tempQint16;

				inStream >> tempQint16;
				int signal_type = tempQint16;
				inStream >> tempQint16;
				int channel_enabled = tempQint16;

				inStream >> tempQint16;
				new_channel.chip_channel = tempQint16;
				inStream >> tempQint16;
				new_channel.board_stream = tempQint16;

				inStream >> tempQint16;
				inStream >> tempQint16;
				inStream >> tempQint16;
				inStream >> tempQint16;

				inStream >> tempsingle;
				new_channel.electrode_impedance_magnitude = tempsingle;
				inStream >> tempsingle;
				new_channel.electrode_impedance_phase = tempsingle;

				if (channel_enabled) {
					switch (signal_type) {
					case 0:
						amplifier_channels.append(new_channel);
						amplifier_index++;
						break;
					case 1:
						aux_input_channels.append(new_channel);
						aux_input_index++;
						break;
					case 2:
						supply_voltage_channels.append(new_channel);
						supply_voltage_index++;
						break;
					case 3:
						board_adc_channels.append(new_channel);
						board_adc_index++;
						break;
					case 4:
						board_dig_in_channels.append(new_channel);
						board_dig_in_index++;
						break;
					case 5:
						board_dig_out_channels.append(new_channel);
						board_dig_out_index++;
						break;

					default:
						qDebug() << "Unknown channel type";
					}
				}
			}
		}
	}

	//Summarize contents of data file
	num_amplifier_channels = amplifier_index;
	int num_aux_input_channels = aux_input_index;
	int num_supply_voltage_channels = supply_voltage_index;
	int num_board_adc_channels = board_adc_index;
	int num_board_dig_in_channels = board_dig_in_index;
	int num_board_dig_out_channels = board_dig_out_index;


	//Determine how many samples the data file contains

	//Each data block contains num_samples_per_data_block amplifier samples.
	int bytes_per_block = num_samples_per_data_block * 4; //timestamp data
	bytes_per_block = bytes_per_block + num_samples_per_data_block * 2 * num_amplifier_channels;
	//Auxiliary inputs are sampled 4x slower than amplifiers
	bytes_per_block = bytes_per_block + (num_samples_per_data_block / 4) * 2 * num_aux_input_channels;
	//Supply voltage is sampled once per data block
	bytes_per_block = bytes_per_block + 1 * 2 * num_supply_voltage_channels;
	//Board analog inputs are sampled at same rate as amplifiers
	bytes_per_block = bytes_per_block + num_samples_per_data_block * 2 * num_board_adc_channels;
	//Board digital inputs are sampled at same rate as amplifiers
	if (num_board_dig_in_channels > 0)
		bytes_per_block = bytes_per_block + num_samples_per_data_block * 2;
	//Board digital outputs are sampled at same rate as amplifiers
	if (num_board_dig_out_channels > 0)
		bytes_per_block = bytes_per_block + num_samples_per_data_block * 2;
	//Temp sensor is sampled once per data block
	if (num_temp_sensor_channels > 0)
		bytes_per_block = bytes_per_block + 1 * 2 * num_temp_sensor_channels;

	//How many data blocks remain in this file?
	int data_present = 0;
	int bytes_remaining = filesize - inFile.pos();
	if (bytes_remaining > 0)
		data_present = 1;

	num_data_blocks = bytes_remaining / bytes_per_block;

	int num_amplifier_samples = num_samples_per_data_block * num_data_blocks;
	//int num_board_dig_in_samples = num_samples_per_data_block * num_data_blocks;

	double record_time = num_amplifier_samples / sample_rate;

	//Print header summary
	if (data_present)
		qDebug("File contains %0.3f seconds of data. Amplifiers were sampled at %0.2f kS/s.\n", record_time, sample_rate / 1000);
	else
		qDebug("Header file contains no data. Amplifiers were sampled at %0.2f kS/s.\n", sample_rate / 1000);
	
	qDebug() << "Reading data from file...";

	int i; //int variables used for various indices
	if (data_present)
	{
		qint64 dDataStart = inFile.pos();

		//Reset inFile's position back to the beginning of the digital data
		inFile.seek(dDataStart);

		//Create Char Array for bulk reading
		char *inCharArray = new char[bytes_per_block];
		uchar *inUCharArray;

		int blockNb = 0;	
		//for (i = 1; i < num_data_blocks+1; i++)
		for (blockNb = 0; blockNb < num_data_blocks; blockNb++)
		{
			//Read the data in bulk into an array, and then read from that array
			inStream.readRawData(inCharArray, bytes_per_block);
			//Convert char array to unsigned char array
			inUCharArray = (uchar*)inCharArray;

			int m;
			if (num_amplifier_channels > 0)
			{
				//Read amplifier data
				//Skip t_amplifier vector (4 * num_samples_per_data_block)
				i = 4 * num_samples_per_data_block;

				//If the current channel being read is a headstage amplifier channel, read all amplifier samples in this data block
				for (m = 0; m < (num_samples_per_data_block*num_amplifier_channels); m++) {
					tempQuint16 = (quint16(inUCharArray[i + 2 * m])) + ((quint16(inUCharArray[i + 1 + 2 * m])) << 8);
					amp_data.push_back(tempQuint16);
				}
			}
		}
		// Make sure we have read exactly the right amount of data.
		bytes_remaining = filesize - inFile.pos();
		if (bytes_remaining != 0)
			printf("Error: End of file not reached. \n");
	}
	qDebug() << "Closing data file " << bytes_remaining;

	// Close data file.
	inFile.close();

	return amp_data;
}

void IntanPlugin::readRhdHeader()
{
	//Check 'magic number' at beginning of file to make sure this is an Intan Technologies RHD2000 data file.
	uint32_t magic_number = 0;
	bigint num_read = 0;
	num_read = fread(&magic_number, sizeof(uint32_t), 1, infile);
	if (num_read < 1) {
		printf("intan file read header: Problem reading input file **.\n");		
	}

	data_file_main_version_number = 0;
	data_file_secondary_version_number = 0;
	// Read version number.	
	fread(&data_file_main_version_number, 1, sizeof(int16_t), infile);
	fread(&data_file_secondary_version_number, 1, sizeof(int16_t), infile);

	num_samples_per_data_block = 0;
	if (data_file_main_version_number == 1)
		num_samples_per_data_block = 60;
	else
		num_samples_per_data_block = 128;

	//Read information of sampling rate and amplifier frequency settings.
	sample_rate = 0.0000;
	fread(&sample_rate, 1, sizeof(float_t), infile);

	int16_t dsp_enabled = 0;
	fread(&dsp_enabled, 1, sizeof(int16_t), infile);
	float_t actual_dsp_cutoff_frequency = 0;
	fread(&actual_dsp_cutoff_frequency, 1, sizeof(float_t), infile);
	float_t actual_lower_bandwidth = 0;
	fread(&actual_lower_bandwidth, 1, sizeof(float_t), infile);
	float_t actual_upper_bandwidth = 0;
	fread(&actual_upper_bandwidth, 1, sizeof(float_t), infile);

	float_t desired_dsp_cutoff_frequency = 0;
	fread(&desired_dsp_cutoff_frequency, 1, sizeof(float_t), infile);
	float_t desired_lower_bandwidth = 0;
	fread(&desired_lower_bandwidth, 1, sizeof(float_t), infile);
	float_t desired_upper_bandwidth = 0;
	fread(&desired_upper_bandwidth, 1, sizeof(float_t), infile);

	int16_t	notch_filter_mode = 0;
	fread(&notch_filter_mode, 1, sizeof(int16_t), infile);
	int16_t notch_filter_frequency = 0;
	if (notch_filter_mode == 1)
		notch_filter_frequency = 50;
	else if (notch_filter_mode == 2)
		notch_filter_frequency = 60;
	
	float_t desired_impedance_test_frequency = 0;
	fread(&desired_impedance_test_frequency, 1, sizeof(float_t), infile);
	float_t actual_impedance_test_frequency = 0;
	fread(&actual_impedance_test_frequency, 1, sizeof(float_t), infile);

	fread_QString(infile);
	fread_QString(infile);
	fread_QString(infile);

	// If data file is from GUI v1.1 or later, see if temperature sensor data was saved.
	num_temp_sensor_channels = 0;
	if ((data_file_main_version_number == 1 && data_file_secondary_version_number >= 1)
		|| (data_file_main_version_number > 1))
		fread(&num_temp_sensor_channels, 1, sizeof(int16_t), infile);

	// If data file is from GUI v1.3 or later, load eval board mode.
	int16_t	eval_board_mode = 0;
	if ((data_file_main_version_number == 1 && data_file_secondary_version_number >= 3)
		|| (data_file_main_version_number > 1))
		fread(&eval_board_mode, 1, sizeof(int16_t), infile);

	// If data file is from v2.0 or later(Intan Recording Controller), load name of digital reference channel.
	QString reference_channel = "";
	if (data_file_main_version_number > 1)
		reference_channel = fread_QString(infile);
}
/*! \brief Should be used with SessionData values of NEURAL_ALIGN_EVENTS_TYPE.  Returns true if the event represented by
*	vList1 happened before the event represented by vList2.
*/
bool neuralAlignLessThan(const QVariantList& vList1, const QVariantList& vList2)
{
	return vList1[NEURAL_TIMESTAMP_COLUMN].toDouble() < vList2[NEURAL_TIMESTAMP_COLUMN].toDouble();
}
/*! \brief Should be used with SessionData values of BEHAVIORAL_ALIGN_EVENTS_TYPE.  Returns true if the event represented
*	by vList1 happened before the event represented by vList2.
*/
bool behavAlignLessThan(const QVariantList& vList1, const QVariantList& vList2)
{
	return vList1[BEHAV_TIMESTAMP_COLUMN].toDouble() < vList2[BEHAV_TIMESTAMP_COLUMN].toDouble();
}
void IntanPlugin::updateAlignInfo(QList<QVariantList> behavAlignEvents, QList<QVariantList> neuralAlignEvents, QProgressBar* bar)
{
	alignmentTool_ = QSharedPointer<IntanAlignmentTool>(new IntanAlignmentTool());
	alignmentTool_->resetValues();

	double latestNeuralTimestamp_ = 0.;
	double latestBehavioralTimestamp_ = 0.;

	if (behavAlignEvents.size() && neuralAlignEvents.size())
	{
		qSort(behavAlignEvents.begin(), behavAlignEvents.end(), behavAlignLessThan);
		qSort(neuralAlignEvents.begin(), neuralAlignEvents.end(), neuralAlignLessThan);

		QList<QVariantList>::Iterator bIt = behavAlignEvents.begin();
		QList<QVariantList> newAlignEvents;
		if (bIt != behavAlignEvents.end())
		{
			do
			{
				//If this behavioral value is not matchable, move onto the next value
				if ((*bIt)[BEHAV_MATCHED_COLUMN].toInt() != 0)
				{
					continue;
				}
				//Find the next matchable neural value who's event code matches the behavioral event code
				QList<QVariantList>::Iterator nIt = neuralAlignEvents.begin();
				if (nIt != neuralAlignEvents.end())
				{
					do
					{
						//If this neural value is not matchable, move onto the next value
						if ((*nIt)[NEURAL_MATCHED_COLUMN].toInt() != 0)
						{
							continue;
						}
						//Check if the neural and behavioral align codes match
						//Note: We compare the behavioral align event to only the bottom 7 digits of the neural align event.
						//	This way systems can bind the top hardware bit to high if need be (ie. For -onesided mode) and it
						//	won't affect alignment
						//int neuralEventCode = (*nIt)[NEURAL_EVENT_CODE_COLUMN].toInt() & 0x7F;
						//int behavEventCode = (*bIt)[BEHAV_EVENT_CODE_COLUMN].toInt() & 0x7F;
						if (((*nIt)[NEURAL_EVENT_CODE_COLUMN].toInt() & 0x7F) == ((*bIt)[BEHAV_EVENT_CODE_COLUMN].toInt() & 0x7F))
						{	//They Match!
							//Set both alignment events' matched values
							(*nIt)[NEURAL_MATCHED_COLUMN] = 1;
							(*bIt)[BEHAV_MATCHED_COLUMN] = 1;
							//Set the "latest matched align code" timestamps
							latestBehavioralTimestamp_ = (*bIt)[BEHAV_TIMESTAMP_COLUMN].toDouble();
							latestNeuralTimestamp_ = (*nIt)[NEURAL_TIMESTAMP_COLUMN].toDouble();
							//Update coefficients in the alignment tool
							alignmentTool_->updateCoefficients(latestBehavioralTimestamp_, latestNeuralTimestamp_);
							//Add a new align event to our newAlignEvents list
							QVariantList alignEvent;
							int aligneventnumber = ((*bIt)[2]).toInt() + 1401;
							QVariant tmp = (QVariant)aligneventnumber;
							alignEvent << tmp//aligneventnumber
								<< (*bIt)[BEHAV_EVENT_CODE_COLUMN]//aligncode
								<< latestNeuralTimestamp_//neuraltime
								<< latestBehavioralTimestamp_//behavioraltime
								<< alignmentTool_->getJitter(latestBehavioralTimestamp_, latestNeuralTimestamp_)//jitter
								<< alignmentTool_->getCorrelationCoefficient();//correlation
							newAlignEvents.append(alignEvent);
							//Move to the next neural align event
							nIt++;
							//break out of loop
							break;
						}
					} while (++nIt != neuralAlignEvents.end());
				}
			} while (++bIt != behavAlignEvents.end());
		}

		int Barval = 10 + bar->value();
		bar->setValue(Barval);

		//Update all jitter and correlation values in our new align events list with the latest calculated alignment
		//	coefficients then put the events into the cachedDirectorSessionData
		for (QList<QVariantList>::Iterator it = newAlignEvents.begin(); it != newAlignEvents.end(); it++)
		{
			(*it)[4] = alignmentTool_->getJitter((*it)[3].toDouble(), (*it)[2].toDouble());//Jitter
			(*it)[5] = alignmentTool_->getCorrelationCoefficient();//CorrelationCoefficient

			//update sql db		
			{
				QString tableName = "alignevents";
				QString colNames = "aligneventnumber,aligncode,neuraltime,behavioraltime,jitter,correlation";
				QStringList placeHolderList = colNames.split(",", QString::SkipEmptyParts);
				for (int i = 0; i < placeHolderList.size(); i++)
				{
					placeHolderList[i] = "?";
				}
				QString placeHolderString = placeHolderList.join(","); //empty array to bind values to
				QString queryString = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tableName).arg(colNames).arg(placeHolderString);
				query_->finish();
				query_->prepare(queryString);
				foreach(QVariant value, *it)
				{
					query_->addBindValue(value);
				}

				bool success = query_->exec();
				if (!success)
				{
					return;
				}
			}
		}

		Barval = 10 + bar->value();
		bar->setValue(Barval);

		//Update stored alignment coefficients
		double nOffset = alignmentTool_->getNeuralOffsetTime();
		double nFactor = alignmentTool_->getNeuralTemporalFactor();
		
		//Update SessionInfo alignment info with updated data.	
		QString val;
		QSharedPointer<QXmlStreamWriter> xmlStreamWriter(new QXmlStreamWriter(&val));
		if (xmlStreamWriter)
		{
			xmlStreamWriter->writeStartElement("AlignmentInfo");
			xmlStreamWriter->writeAttribute("OffsetTime", QString::number(nOffset, 'e', 14));
			xmlStreamWriter->writeAttribute("TemporalFactor", QString::number(nFactor, 'e', 14));
			xmlStreamWriter->writeEndElement();
		}
		
		QString tableName = "sessioninfo";
		QString colNames = "dataid,key,value";
		QStringList placeHolderList = colNames.split(",", QString::SkipEmptyParts);
		for (int i = 0; i < placeHolderList.size(); i++)
		{
			placeHolderList[i] = "?";
		}
		QString placeHolderString = placeHolderList.join(","); //empty array to bind values to
			
		QString queryString = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tableName).arg(colNames).arg(placeHolderString);
		query_->prepare(queryString);
			
		QString keyVal = "AlignmentInfo";
		int dataid = 101;
		query_->addBindValue(QVariant(dataid));
		query_->addBindValue(QVariant(keyVal));
		query_->addBindValue(QVariant(val));

		bool success = query_->exec();
		if (!success)
		{
			return;
		}
			
	}
	int Barval = 10 + bar->value();
	bar->setValue(Barval);
}

// Allocates memory for a 2-D array of integers.
void IntanPlugin::allocateIntArray2D(QVector<QVector<int> > &array2D, int xSize, int ySize)
{
	int i;

	array2D.resize(xSize);
	for (i = 0; i < xSize; ++i) {
		array2D[i].resize(ySize);
	}
}

QString IntanPlugin::fread_QString(FILE* infile)
{
	QString returnString;
	uint32_t length = 0;
	fread(&length, 1, sizeof(uint32_t), infile);
	if (length < 1)
		return "";

	// convert length from bytes to 16 - bit Unicode words
	int lg = length / 2;
	for (int i = 0; i < lg; ++i)
	{
		uint16_t c = 0;
		fread(&c, 1, sizeof(uint16_t), infile);
		std::string ret; ret = (char)c;

		QString str = QString::fromStdString(ret);
		returnString.append(str);
	}

	return returnString;
}
/* Private function that looks at the read filename, and determines the write filename suffix depending on the input file */
QString IntanPlugin::formatSuffix(QString filename)
{
	QString output = "";

	//If the name is of the form "filename.rhs" or "filename.rhd", just save as "filename.nex"
	if (filename.right(4) == ".rhd") {
		output = filename.left(filename.length() - 4).append(".mda");
	}

	//If the name doesn't have the suffix ".rhs" or ".rhd", just append ".nex" to the end of the filename
	else {
		output = filename.append(".mda");
	}
	return output;
}

