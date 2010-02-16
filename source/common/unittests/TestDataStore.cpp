#include <QFile>
#include <QSharedPointer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "TestDataStore.h"
#include "../stimuli/BoxGraphic.h"
#include "../stimuli/ArrowGraphic.h"
#include "../stimuli/CircleGraphic.h"
#include "../stimuli/EllipseGraphic.h"
#include "../stimuli/LineGraphic.h"
#include "../stimuli/PictureGraphic.h"
#include "../stimuli/RandomlyFilledGridGraphic.h"

Q_DECLARE_METATYPE(QSharedPointer<Picto::VisualElement>)

TestDataStore::TestDataStore()
{
	randGen_.seed();

	//Set up the VisualElementFactory
	visualElementFactory_.addVisualElementType(Picto::ArrowGraphic::name, &Picto::ArrowGraphic::NewVisualElement);
	visualElementFactory_.addVisualElementType(Picto::BoxGraphic::name, &Picto::BoxGraphic::NewVisualElement);
	visualElementFactory_.addVisualElementType(Picto::CircleGraphic::name, &Picto::CircleGraphic::NewVisualElement);
	visualElementFactory_.addVisualElementType(Picto::EllipseGraphic::name, &Picto::EllipseGraphic::NewVisualElement);
	visualElementFactory_.addVisualElementType(Picto::LineGraphic::name, &Picto::LineGraphic::NewVisualElement);
	visualElementFactory_.addVisualElementType(Picto::PictureGraphic::name, &Picto::PictureGraphic::NewVisualElement);
	visualElementFactory_.addVisualElementType(Picto::RandomlyFilledGridGraphic::name, &Picto::RandomlyFilledGridGraphic::NewVisualElement);
}


/*!	\brief Tests all of the visual elements.
 *	The testing consists of:
 *		1. Creating a new visual element with random properties
 *		2. Generating an XML fragment from the visual element
 *		3. Creating a new visual element by deserializing the XML
 *		4. Generating an XML fragment from the new element
 *		5. Confirming that the XML fragments are identical
 */
void TestDataStore::TestVisualElementDataStore_data()
{

	QTest::addColumn< QSharedPointer<Picto::VisualElement> >("original");

	//////////////////////
	// Test Arrow graphic
	//
	{
	QPoint position(randGen_.randInt(), randGen_.randInt());
	QPoint start(randGen_.randInt(), randGen_.randInt());
	QPoint end(randGen_.randInt(), randGen_.randInt());
	int size = randGen_.randInt()%20;
	QColor color(randGen_.randInt()%256, randGen_.randInt()%256,randGen_.randInt()%256,randGen_.randInt()%256);
	QSharedPointer<Picto::VisualElement> arrow(new Picto::ArrowGraphic(position,start,end,size,color));

	QTest::newRow("Arrow Graphic") << arrow;
	}

	//////////////////////
	// Test Box graphic
	//
	{
	QPoint position(randGen_.randInt(), randGen_.randInt());
	QRect dimensions(randGen_.randInt(), randGen_.randInt(),randGen_.randInt(), randGen_.randInt());
	QColor color(randGen_.randInt()%256, randGen_.randInt()%256,randGen_.randInt()%256,randGen_.randInt()%256);
	QSharedPointer<Picto::VisualElement> box(new Picto::BoxGraphic(position,dimensions,color));

	QTest::newRow("Box Graphic") << box;
	}

	//////////////////////
	// Test Circle graphic
	//
	{
	QPoint position(randGen_.randInt(), randGen_.randInt());
	int radius = randGen_.randInt();
	QColor color(randGen_.randInt()%256, randGen_.randInt()%256,randGen_.randInt()%256,randGen_.randInt()%256);
	QSharedPointer<Picto::VisualElement> circle(new Picto::CircleGraphic(position,radius,color));

	QTest::newRow("Circle Graphic") << circle;
	}
	

	//////////////////////
	// Test Ellipse graphic
	//
	{
	QPoint position(randGen_.randInt(), randGen_.randInt());
	QRect dimensions(randGen_.randInt(), randGen_.randInt(),randGen_.randInt(), randGen_.randInt());
	QColor color(randGen_.randInt()%256, randGen_.randInt()%256,randGen_.randInt()%256,randGen_.randInt()%256);
	QSharedPointer<Picto::VisualElement> ellipse(new Picto::EllipseGraphic(position,dimensions,color));

	QTest::newRow("Ellipse Graphic") << ellipse;
	}

	//////////////////////
	// Test Line graphic
	//
	{
	QPoint position(randGen_.randInt(), randGen_.randInt());
	QColor color(randGen_.randInt()%256, randGen_.randInt()%256,randGen_.randInt()%256,randGen_.randInt()%256);
	
	QVector<QPoint> points;
	int numPoints = randGen_.randInt()%8+3;
	for(int i=0; i<numPoints; i++)
	{
		QPoint point(randGen_.randInt(), randGen_.randInt());
		points.push_back(point);
	}

	QSharedPointer<Picto::VisualElement> line(new Picto::LineGraphic(position,points,color));

	QTest::newRow("Line Graphic") << line;
	}

	//////////////////////
	// Test Picture graphic
	//
	{
	QPoint position(randGen_.randInt(), randGen_.randInt());
	QString filename("NotARealFile.bmp");
	QSharedPointer<Picto::VisualElement> pic(new Picto::PictureGraphic(position,filename));

	QTest::newRow("Picture Graphic") << pic;
	}

}

/*! \brief A simple function for testing a single visual element's data store
 *
 *	Note that to call this function we need to pass in two Visualelement objects, bith
 *	of which have already been created.  The copy object will be turned into a copy of
 *	the original, using DataStores serialization functionality.  Assuming the copy is 
 *	identical to the original, the test will pass.
 *
 *	By turning this into a function, we can easily test different types of VisualElements
 */
void TestDataStore::TestVisualElementDataStore()
 {
	QFETCH(QSharedPointer<Picto::VisualElement>, original);

	//serialize original
	QByteArray originalByteArr;
	QBuffer originalBuffer(&originalByteArr);
	originalBuffer.open(QBuffer::WriteOnly);
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&originalBuffer));

	xmlWriter->setAutoFormatting(true);
	xmlWriter->writeStartDocument();
	original->serializeAsXml(xmlWriter);
	xmlWriter->writeEndDocument();

	originalBuffer.close();

	//create new graphic and deserialize the XML into it
	originalBuffer.open(QBuffer::ReadOnly);
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(&originalBuffer));
	while(xmlReader->name() != "VisualElement")
	{
		xmlReader->readNext();
	}
	QString type = xmlReader->attributes().value("type").toString();
	Picto::VisualElement* copy = visualElementFactory_.generateVisualElement(type);
	copy->deserializeFromXml(xmlReader);

	//confirm that the xml reader is in the </VisualElement> tag
	QCOMPARE(xmlReader->isEndElement(), true);
	QCOMPARE(xmlReader->name().toString(), QString("VisualElement"));

	originalBuffer.close();

	//reserialize the copied element and confirm that the XML is the same
	QByteArray copyByteArr;
	QBuffer copyBuffer(&copyByteArr);
	copyBuffer.open(QBuffer::WriteOnly);
	xmlWriter->setDevice(&copyBuffer);

	xmlWriter->writeStartDocument();
	copy->serializeAsXml(xmlWriter);
	xmlWriter->writeEndDocument();

	QCOMPARE(originalByteArr, copyByteArr);

	

 }

