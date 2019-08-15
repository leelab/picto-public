#ifndef SMARTDATASTREAM_H
#define SMARTDATASTREAM_H

#include <QtGui>

const int size = 1000000; //size of buffer

class SmartDataStream
{
public:
    SmartDataStream(QFile*); //Constructor
    void writeInt32(qint32 data); //Function that writes a 32-bit integer into the buffer
    void writeInt16(qint16 data); //Function that writes a 16-bit integer into the buffer
    void writeUint16(quint16 data); //Function that writes a 16-bit unsigned integer into the buffer
    void writeArray(QByteArray array); //Function that writes a QByteArray into the buffer
    void writeDouble(double data); //Function that writes a double-precision number into the buffer
    void flush(); //Write all contents of buffer to file, and clear contents
    qint64 pos(); //Return the current position of the output file
    void seek(qint64 location); //Set the current position of the output file to parameter "location"
    QDataStream outStream; //output stream

private:
    char buffer[size]; //character array that stores buffer data
    QFile *outFile; //output file
    int bufferIndex; //variable that stores the current index of the buffer
};

#endif // SMARTDATASTREAM_H
