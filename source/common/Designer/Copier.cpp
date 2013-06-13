#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QWeakPointer>
#include "Copier.h"
#include "../parameter/AssociateElement.h"
#include "../storage/DesignConfig.h"
#include "../../common/memleakdetect.h"
Copier::Copier(QSharedPointer<EditorState> editorState)	:
editorState_(editorState)
{

}

Copier::~Copier()
{
}

void Copier::copy(QList<QSharedPointer<Asset>> assets,bool copyAnalysis)
{
	//Get the AssociateRootHost (Experiment or Analysis)
	AssociateRootHost* assocRootHost;
	if(copyAnalysis)
		assocRootHost = dynamic_cast<AssociateRootHost*>(editorState_->getCurrentAnalysis().data());
	else
		assocRootHost = dynamic_cast<AssociateRootHost*>(editorState_->getTopLevelAsset().data());
	if(!assocRootHost)
		return;

	QString copyText = exportImporter_.exportToText(assets,assocRootHost);
	if(copyText.isEmpty())
	{
		QMessageBox::warning(NULL,"Copy failed",exportImporter_.getLatestMessage());
		return;
	}
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(	copyText );
}

void Copier::paste(QSharedPointer<Asset> pasteParent, QPoint pastePosition)
{
	bool analysisDescendantsDeleted = false;
	AssociateRootHost* analysisRootHost = dynamic_cast<AssociateRootHost*>(editorState_->getCurrentAnalysis().data());
	if(analysisRootHost && availablePasteType() == AssetExportImport::ANALYSIS_IMPORT)
	{
		//When performing an analysis import, all analysis descendants of the paste parent must be removed first.
		if(pasteParent.staticCast<DataStore>()->getAssociateDescendants(dynamic_cast<AssociateRoot*>(analysisRootHost)->getAssociateId()).size())
		{
			if(QMessageBox::Yes != QMessageBox::warning(NULL,"Remove Existing Analysis Elements","The analysis import target already contains Analysis elements.  These Analysis elements must be deleted to perform the import.  Continue?",
				QMessageBox::Yes | QMessageBox::Cancel,
                                QMessageBox::Cancel))
				return;
			analysisDescendantsDeleted = true;
			foreach(QSharedPointer<Asset> associateDesc,pasteParent.staticCast<DataStore>()->getAssociateDescendants(dynamic_cast<AssociateRoot*>(analysisRootHost)->getAssociateId()))
			{
				associateDesc->setDeleted();
			}
			pasteParent.staticCast<DataStore>()->ClearAssociateDescendants(dynamic_cast<AssociateRoot*>(analysisRootHost)->getAssociateId());
		}
	}

	QClipboard *clipboard = QApplication::clipboard();
	QString copiedText = clipboard->text();

	//Get experiment and analysis hosts
	AssociateRootHost* expRootHost = dynamic_cast<AssociateRootHost*>(editorState_->getTopLevelAsset().data());

	int result = exportImporter_.importFromText(pasteParent,copiedText,expRootHost,pastePosition,analysisRootHost);

	switch(result)
	{
	case AssetExportImport::IMPORT_SUCCEEDED:
		editorState_->setLastActionUndoable();
		editorState_->triggerExperimentReset();
		break;
	case AssetExportImport::IMPORT_SUCCEEDED_WITH_WARNINGS:
		if(QMessageBox::Yes == QMessageBox::warning(NULL, "Paste Succeeded with Warnings",
								QString("%1\nDo you want to accept the changes?").arg(exportImporter_.getLatestMessage()),
                                QMessageBox::Yes | QMessageBox::Cancel,
                                QMessageBox::Cancel))
		{
			editorState_->setLastActionUndoable();
			editorState_->triggerExperimentReset();
		}
		else
			editorState_->requestUndo();
		break;
	case AssetExportImport::IMPORT_FAILED:
		if(analysisDescendantsDeleted)
		{
			editorState_->requestUndo();
		}
		QMessageBox::warning(NULL,"Paste failed",exportImporter_.getLatestMessage());
		break;
	case AssetExportImport::IMPORT_FAILED_ROLLBACK:
		editorState_->requestUndo();
		QMessageBox::warning(NULL,"Paste failed",exportImporter_.getLatestMessage());
		break;
	default:
		Q_ASSERT(false);
	}
}

enum PASTE_TYPE{NONE,EXPERIMENT_PASTE,ANALYSIS_PASTE,ANALYSIS_IMPORT};
int Copier::availablePasteType()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString copiedText = clipboard->text();
	int importType = AssetExportImport::commandTypeOfText(copiedText);
	switch(importType)
	{
	case AssetExportImport::NONE:
		return NONE;
	case AssetExportImport::EXPERIMENT_PASTE:
		return EXPERIMENT_PASTE;
	case AssetExportImport::ANALYSIS_PASTE:
		return ANALYSIS_PASTE;
	case AssetExportImport::ANALYSIS_IMPORT:
		return ANALYSIS_IMPORT;
	}
	return NONE;
}