#ifndef Copier_H
#define Copier_H

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include "EditorState.h"
using namespace Picto;

//! [0]
class Copier : public QObject
{
	Q_OBJECT
public:
	Copier(QSharedPointer<EditorState> editorState);
	virtual ~Copier();

	void copy(QList<QSharedPointer<Asset>> assets,bool copyAnalysis = false);
	void paste(QSharedPointer<Asset> pasteParent, QPoint pastePosition = QPoint(0,0));
private:
	static bool getContentsUntilEndTag(QSharedPointer<QXmlStreamReader> xmlReader, QString endTag, QString& output);
	QSharedPointer<EditorState> editorState_;
};
//! [0]

#endif
