/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QLabel>
#include <QMenuBar>

#include "toolbox.h"
#include "stateeditviewer.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "diagramtextitem.h"
#include "AssetItem.h"
#include "PropertyBrowser.h"
#include "AssetInfoBox.h"
#include "ViewerWindow.h"
#include "../../common/memleakdetect.h"

//! [0]
StateEditViewer::StateEditViewer(QWidget *parent) :
	Viewer(parent),
	expDesigner_(new Designer())
{
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(expDesigner_);
	setLayout(mainLayout);
}

StateEditViewer::~StateEditViewer()
{
}
//! [0]
void StateEditViewer::init()
{
	//if(!pictoData_ || !pictoData_->validateTree())
	//{
	//	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(pictoDataText_->toPlainText()));

	//	//read until we either see an experiment tag, or the end of the file
	//	while(xmlReader->name() != "PictoData" && !xmlReader->atEnd()) 
	//		xmlReader->readNext();

	//	if(xmlReader->atEnd())
	//	{
	//		QMessageBox msg;
	//		msg.setText("PictoData XML did not contain <PictoData> tag");
	//		msg.exec();
	//		return;
	//	}

	//	pictoData_ = QSharedPointer<Picto::PictoData>(Picto::PictoData::Create().staticCast<PictoData>());
	//	Picto::Asset::clearErrors();
	//	
	//	if(!pictoData_->fromXml(xmlReader,false))
	//	{
	//		pictoData_ = QSharedPointer<Picto::PictoData>();
	//		QMessageBox msg;
	//		msg.setText("Failed to load current definition.  Please attempt to correct PictoData XML in Text Editor");
	//		msg.exec();
	//		return;
	//	}

	//}
	expDesigner_->loadDesign("Experiment",0,designRoot_);
}

void StateEditViewer::deinit()
{
}

void StateEditViewer::aboutToSave()
{
}