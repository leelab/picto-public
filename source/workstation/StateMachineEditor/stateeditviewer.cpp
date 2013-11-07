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

#include <QtWidgets>
#include <QLabel>
#include <QMenuBar>
#include <QDialog>

#include "stateeditviewer.h"
#include "../../common/memleakdetect.h"
#define AUTOSAVEINTERVALMS 30000	//auto save every 30 seconds
//! [0]
StateEditViewer::StateEditViewer(QWidget *parent) :
	Viewer(parent),
	expDesigner_(new Designer()),
	autoSaver_(AutoSaver::getSingleton()),
	checkedAutoSave_(false)
{
	connect(autoSaver_.data(),SIGNAL(openDesign(QSharedPointer<DesignRoot>)),this,SIGNAL(loadDesignRoot(QSharedPointer<DesignRoot>)));
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
	designRoot_->enableRunMode(false);
	expDesigner_->activate(true);
	expDesigner_->loadDesign(designRoot_);
	autoSaver_->start(AUTOSAVEINTERVALMS);

	if(!checkedAutoSave_)
	{
		checkedAutoSave_ = true;		
		QDialog* autoSaveDialog = autoSaver_->getDesignRestoreDialog(this);
		if(autoSaveDialog)
		{
			autoSaveDialog->exec();
		}
	}
}

void StateEditViewer::deinit()
{
	//Whenever we're leaving the StateEditViewer, set an undo point.  This way we will
	//be sure that the text stored in the design root matches the current state of the
	//software design
	designRoot_->setUndoPoint();
	expDesigner_->activate(false);
	autoSaver_->stop();
	emit deinitComplete();
}

void StateEditViewer::aboutToSave()
{
}