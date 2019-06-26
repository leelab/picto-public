#include <QtGui>
#include "smartdatastream.h"

QQueue<char> bufferQueue;

/* Constructor */
SmartDataStream::SmartDataStream(QFile *outputFile)
{
    //Initialize bufferIndex to be 0
    bufferIndex = 0;

    //Set up output stream
    outStream.setDevice(outputFile);
    outStream.setVersion(QDataStream::Qt_4_8);
    outStream.setByteOrder(QDataStream::LittleEndian);
    outStream.setFloatingPointPrecision(QDataStream::DoublePrecision);

    //Store outFile as a private variable so pos() and seek() functions can access it
    outFile = outputFile;
}

/* Function that writes a double-precision number into the buffer */
void SmartDataStream::writeDouble(double data)
{
    //If the buffer doesn't have enough room to store a double, write and empty it first
    if (size - bufferIndex < sizeof(double))
        flush();

    //Set up a union to store the double
    union {
        double doubleValue;
        char asChars[sizeof(double)];
    };

    //Store the double in the union
    doubleValue = data;

    //Write the double to the buffer in little-endian format
    buffer[bufferIndex++] = asChars[0];
    buffer[bufferIndex++] = asChars[1];
    buffer[bufferIndex++] = asChars[2];
    buffer[bufferIndex++] = asChars[3];
    buffer[bufferIndex++] = asChars[4];
    buffer[bufferIndex++] = asChars[5];
    buffer[bufferIndex++] = asChars[6];
    buffer[bufferIndex++] = asChars[7];
}

/* Function that writes a QByteArray into the buffer */
void SmartDataStream::writeArray(QByteArray array)
{
    //If the buffer doesn't have enough room to store an array of the current size, write and empty it first
    if (size - bufferIndex < array.size())
        flush();

    //Write each character to the buffer in little-endian format
    int i;
    for (i = 0; i < array.size(); i++) {
        buffer[bufferIndex++] = array.at(i);
    }
}

/* Function that writes a 32-bit integer into the buffer */
void SmartDataStream::writeInt32(qint32 data)
{
    //If the buffer doesn't have enough room to store a 32-bit integer, write and empty it first
    if (size - bufferIndex < sizeof(qint32))
        flush();

    //Write each character to the buffer in little-endian format
    buffer[bufferIndex++] = data & 0x000000ff;
    buffer[bufferIndex++] = (data & 0x0000ff00) >> 8;
    buffer[bufferIndex++] = (data & 0x00ff0000) >> 16;
    buffer[bufferIndex++] = (data & 0xff000000) >> 24;
}

/* Function that writes a 16-bit integer into the buffer */
void SmartDataStream::writeInt16(qint16 data)
{
    //If the buffer doesn't have enough room to store a 16-bit integer, write and empty it first
    if (size - bufferIndex < sizeof(qint16)) {
        flush();
    }

    //Write each character to the buffer in little-endian format
    buffer[bufferIndex++] = data & 0x00ff;
    buffer[bufferIndex++] = (data & 0xff00) >> 8;
}

/* Function that writes a 16-bit unsigned integer into the buffer */
void SmartDataStream::writeUint16(quint16 data)
{
    //If the buffer doesn't have enough room to store a 16-bit unsigned integer, write and empty it first
    if (size - bufferIndex < sizeof(quint16)) {
        flush();
    }

    //Write each character to the buffer in little-endian format
    buffer[bufferIndex++] = data & 0x00ff;
    buffer[bufferIndex++] = (data & 0xff00) >> 8;
}

/* Write all contents of buffer to file, and clear contents */
void SmartDataStream::flush()
{
    //Write the contents of buffer to output file
    outStream.writeRawData(buffer, bufferIndex);

    //Reset bufferIndex
    bufferIndex = 0;
}

/* Return the current position of the output file */
qint64 SmartDataStream::pos()
{
    return outFile->pos();
}

/* Set the current position of the output file to parameter "location" */
void SmartDataStream::seek(qint64 location)
{
    outFile->seek(location);
}
