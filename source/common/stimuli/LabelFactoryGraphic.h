#ifndef _LabelFactoryGraphic_H_
#define _LabelFactoryGraphic_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

	/*!	\brief Displays any number of small Label texts with custom colors, content and positions.
	*	\details A Label Factory is a very general class that can be used for clustering lots of text labels to work together.  
	*	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
	*	\date 2009-2017
	*/
	class PICTOLIB_CLASS LabelFactoryGraphic : public VisualElement
	{
		Q_OBJECT
		/*! \brief Sets/gets the default Label text content.*/
		Q_PROPERTY(QString labelContent READ getLabelContent WRITE setLabelContent)
		/*! \brief Sets/gets the default Label width.*/
		Q_PROPERTY(int labelWidth READ getLabelWidth WRITE setLabelWidth)
		/*! \brief Sets/gets the default Label height.*/
		Q_PROPERTY(int labelHeight READ getLabelHeight WRITE setLabelHeight)
		/*! \brief Sets/gets the default Label text content.*/
		Q_PROPERTY(QString labelFontSize READ getLabelFontSize WRITE setLabelFontSize)
		/*! \brief Sets/gets the number of Labels to be displayed.*/
		Q_PROPERTY(int numLabels READ getNumLabels WRITE setNumLabels)

	public:
		LabelFactoryGraphic(QPoint position = QPoint(), QRect dimensions = QRect(), QColor color = QColor());

		void draw();
		static QSharedPointer<Asset> Create();
		static const QString type;

		virtual void enteredScope();

		QRect getLabelDimensions();
		void setLabelDimensions(QRect dimensions);
		/*! \brief Gets the default Label width.*/
		int getLabelWidth(){ return getLabelDimensions().width(); };
		/*! \brief Gets the default Label height.*/
		int getLabelHeight(){ return getLabelDimensions().height(); };
		/*! \brief Sets the default Label width.*/
		void setLabelWidth(int w){ QRect dims = getLabelDimensions(); dims.setWidth(w); setLabelDimensions(dims); };
		/*! \brief Sets the default Label height.*/
		void setLabelHeight(int h){ QRect dims = getLabelDimensions(); dims.setHeight(h); setLabelDimensions(dims); };
		QString getLabelContent();
		void setLabelContent(QString content);
		QString getLabelFontSize();
		void setLabelFontSize(QString fontsize);
		void setNumLabels(int num);
		int getNumLabels();
		QPoint getPositionOffset();

		virtual QString getUITemplate(){ return "LabelFactoryElement"; };
		virtual QString friendlyTypeName(){ return "Text Factory"; };
		
	public slots:
		/*! \brief Sets the default size of Labels.*/
		void setLabelDimensions(int w, int h){ setLabelWidth(w); setLabelHeight(h); };
		void setLabelPos(int index, int x, int y);
		void setLabelLocalPos(int index, int x, int y);
		void setLabelX(int index, int x);
		void setLabelY(int index, int y);
		void setLabelLocalX(int index, int x);
		void setLabelLocalY(int index, int y);
		void setLabelSize(int index, double size);
		void setLabelColor(int index, int r, int g, int b, int a = 255);
		void setLabelColor(int index, QVariant color);
		void setLabelContent(int index, QString Content);
		void setLabelFontSize(int index, QString fontsize);
		int getLabelX(int index);
		int getLabelY(int index);
		int getLabelLocalX(int index);
		int getLabelLocalY(int index);
		double getLabelSize(int index);
		int getLabelRed(int index);
		int getLabelGreen(int index);
		int getLabelBlue(int index);
		int getLabelAlpha(int index);
		QColor getLabelColor(int index);
		QString getLabelContent(int index);
		QString getLabelFontSize(int index);


	protected:
		virtual void postDeserialize();
		virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	private:
		void updateListSizes();
		QRect getImageDims();
		QPoint posOffset_;
		QString Content_;
		//QHash<int, QString> sizeDelta_;
		//QHash<int, QString> colorDelta_;
		//QHash<int, QString> ContentDelta_;
		//QHash<int, QString> fontSizeDelta_;
		//QHash<int, QString> xDelta_;
		//QHash<int, QString> yDelta_;
		//QVector<QHash<int, QString>*> deltaHashs_;
		QStringList defaults_;
		QStringList listNames_;

	};


}; //namespace Picto

#endif
