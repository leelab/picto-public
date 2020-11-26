TEMPLATE = lib
unix:TARGET = Picto
win32:TARGET = libPicto
QT = core gui xml network script scripttools sql multimedia svg concurrent widgets
CONFIG += qt warn_on debug_and_release 
CONFIG -= flat
CONFIG += c++11
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += $$[QT_INSTALL_PREFIX]/src/3rdparty/zlib
INCLUDEPATH += $$(PICTO_TREE)/3rdparty/matlab
MACHINE_TYPE = $$(PICTO_MACHINE_TYPE)

# Definitions
DEFINES += PICTOLIB_EXPORTS

# Input
HEADERS += $$(PICTO_TREE)/source/common/common.h
HEADERS += $$(PICTO_TREE)/source/common/namedefs.h
SOURCES += $$(PICTO_TREE)/source/common/namedefs.cpp
HEADERS += $$(PICTO_TREE)/source/common/portdefs.h
SOURCES += $$(PICTO_TREE)/source/common/portdefs.cpp
HEADERS += $$(PICTO_TREE)/source/common/globals.h
SOURCES += $$(PICTO_TREE)/source/common/globals.cpp
HEADERS += $$(PICTO_TREE)/source/common/mainmemleakdetect.h
HEADERS += $$(PICTO_TREE)/source/common/memleakdetect.h

HEADERS += $$(PICTO_TREE)/source/common/compositor/RenderingTarget.h
SOURCES += $$(PICTO_TREE)/source/common/compositor/RenderingTarget.cpp
HEADERS += $$(PICTO_TREE)/source/common/compositor/VisualTarget.h
SOURCES += $$(PICTO_TREE)/source/common/compositor/VisualTarget.cpp
HEADERS += $$(PICTO_TREE)/source/common/compositor/VisualTargetHost.h
SOURCES += $$(PICTO_TREE)/source/common/compositor/VisualTargetHost.cpp
HEADERS += $$(PICTO_TREE)/source/common/compositor/AuralTarget.h
SOURCES += $$(PICTO_TREE)/source/common/compositor/AuralTarget.cpp
HEADERS += $$(PICTO_TREE)/source/common/compositor/PixmapVisualTarget.h
SOURCES += $$(PICTO_TREE)/source/common/compositor/PixmapVisualTarget.cpp
HEADERS += $$(PICTO_TREE)/source/common/compositor/PCMAuralTarget.h
SOURCES += $$(PICTO_TREE)/source/common/compositor/PCMAuralTarget.cpp
HEADERS += $$(PICTO_TREE)/source/common/compositor/CompositingSurface.h
SOURCES += $$(PICTO_TREE)/source/common/compositor/CompositingSurface.cpp
HEADERS += $$(PICTO_TREE)/source/common/compositor/PixmapCompositingSurface.h
SOURCES += $$(PICTO_TREE)/source/common/compositor/PixmapCompositingSurface.cpp
HEADERS += $$(PICTO_TREE)/source/common/compositor/NullCompositingSurface.h
SOURCES += $$(PICTO_TREE)/source/common/compositor/NullCompositingSurface.cpp
HEADERS += $$(PICTO_TREE)/source/common/compositor/MixingSample.h
SOURCES += $$(PICTO_TREE)/source/common/compositor/MixingSample.cpp
HEADERS += $$(PICTO_TREE)/source/common/compositor/PCMMixingSample.h
SOURCES += $$(PICTO_TREE)/source/common/compositor/PCMMixingSample.cpp
HEADERS += $$(PICTO_TREE)/source/common/compositor/OutputSignalWidget.h
SOURCES += $$(PICTO_TREE)/source/common/compositor/OutputSignalWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/compositor/MediaMuxer.h
SOURCES += $$(PICTO_TREE)/source/common/compositor/MediaMuxer.cpp


HEADERS += $$(PICTO_TREE)/source/common/controlelements/TestController.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/TestController.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/StopwatchController.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/StopwatchController.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/ScriptController.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/ScriptController.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/FrameResolutionTimer.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/FrameResolutionTimer.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/FrameTimerFactory.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/FrameTimerFactory.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/FrameTracker.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/FrameTracker.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/TargetController.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/TargetController.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/FixationController.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/FixationController.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/ChoiceController.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/ChoiceController.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/ControlTarget.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/ControlTarget.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/RectTarget.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/RectTarget.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/CircleTarget.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/CircleTarget.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/ControlTargetResult.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/ControlTargetResult.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/ControlElement.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/ControlElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/TargetFactory.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/TargetFactory.cpp

HEADERS += $$(PICTO_TREE)/source/common/design/DesignRoot.h
SOURCES += $$(PICTO_TREE)/source/common/design/DesignRoot.cpp
HEADERS += $$(PICTO_TREE)/source/common/design/PictoData.h
SOURCES += $$(PICTO_TREE)/source/common/design/PictoData.cpp

HEADERS += $$(PICTO_TREE)/source/common/designer/EditorState.h
SOURCES += $$(PICTO_TREE)/source/common/designer/EditorState.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/AboutElementBox.h
SOURCES += $$(PICTO_TREE)/source/common/designer/AboutElementBox.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/arrow.h
SOURCES += $$(PICTO_TREE)/source/common/designer/arrow.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/AssetDescriber.h
SOURCES += $$(PICTO_TREE)/source/common/designer/AssetDescriber.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/AutoSaver.h
SOURCES += $$(PICTO_TREE)/source/common/designer/AutoSaver.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/AutoSaveDialog.h
SOURCES += $$(PICTO_TREE)/source/common/designer/AutoSaveDialog.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/Copier.h
SOURCES += $$(PICTO_TREE)/source/common/designer/Copier.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/diagramitem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/diagramitem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ElementNotesWidget.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ElementNotesWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ArrowPortItem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ArrowPortItem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ArrowSourceItem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ArrowSourceItem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ArrowDestinationItem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ArrowDestinationItem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/StartBarItem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/StartBarItem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/DiagramItemFactory.h
SOURCES += $$(PICTO_TREE)/source/common/designer/DiagramItemFactory.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/WireableItem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/WireableItem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/AssetItem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/AssetItem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/AnalysisItem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/AnalysisItem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ExperimentItem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ExperimentItem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ResultArrowSourceItem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ResultArrowSourceItem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ScriptItem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ScriptItem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ScriptItemManager.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ScriptItemManager.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/WireableResultItem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/WireableResultItem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/StateMachineElementItem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/StateMachineElementItem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ControlElementItem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ControlElementItem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/diagramscene.h
SOURCES += $$(PICTO_TREE)/source/common/designer/diagramscene.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/diagramtextitem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/diagramtextitem.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/Designer.h
SOURCES += $$(PICTO_TREE)/source/common/designer/Designer.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/Toolbox.h
SOURCES += $$(PICTO_TREE)/source/common/designer/Toolbox.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ToolGroup.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ToolGroup.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/LevelUpButton.h
SOURCES += $$(PICTO_TREE)/source/common/designer/LevelUpButton.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/AssetToolGroup.h
SOURCES += $$(PICTO_TREE)/source/common/designer/AssetToolGroup.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/PropertyEditorFactory.h
SOURCES += $$(PICTO_TREE)/source/common/designer/PropertyEditorFactory.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/PropertyManager.h
SOURCES += $$(PICTO_TREE)/source/common/designer/PropertyManager.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/PropertyFrame.h
SOURCES += $$(PICTO_TREE)/source/common/designer/PropertyFrame.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/PropertyListWidget.h
SOURCES += $$(PICTO_TREE)/source/common/designer/PropertyListWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/PropertyGroupWidget.h
SOURCES += $$(PICTO_TREE)/source/common/designer/PropertyGroupWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/PropertyBrowser.h
SOURCES += $$(PICTO_TREE)/source/common/designer/PropertyBrowser.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/AnalysisOptionWidget.h
SOURCES += $$(PICTO_TREE)/source/common/designer/AnalysisOptionWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/AnalysisSelectorWidget.h
SOURCES += $$(PICTO_TREE)/source/common/designer/AnalysisSelectorWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/AssetInfoBox.h
SOURCES += $$(PICTO_TREE)/source/common/designer/AssetInfoBox.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/FileSelectWidget.h
SOURCES += $$(PICTO_TREE)/source/common/designer/FileSelectWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ScriptTextEdit.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ScriptTextEdit.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ScriptWidget.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ScriptWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ViewPositionWidget.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ViewPositionWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ScriptSyntaxHighlighter.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ScriptSyntaxHighlighter.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/SearchableTextEdit.h
SOURCES += $$(PICTO_TREE)/source/common/designer/SearchableTextEdit.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/SearchablePlainTextEdit.h
SOURCES += $$(PICTO_TREE)/source/common/designer/SearchablePlainTextEdit.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/SyntaxErrorBox.h
SOURCES += $$(PICTO_TREE)/source/common/designer/SyntaxErrorBox.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/RunSelectorWidget.h
SOURCES += $$(PICTO_TREE)/source/common/designer/RunSelectorWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ViewerWindow.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ViewerWindow.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/UIHelper.h
SOURCES += $$(PICTO_TREE)/source/common/designer/UIHelper.cpp


HEADERS += $$(PICTO_TREE)/source/common/engine/InputPort.h
SOURCES += $$(PICTO_TREE)/source/common/engine/InputPort.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/SignalChannel.h
SOURCES += $$(PICTO_TREE)/source/common/engine/SignalChannel.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/DoubletSignalChannel.h
SOURCES += $$(PICTO_TREE)/source/common/engine/DoubletSignalChannel.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/SlaveEvent.h
SOURCES += $$(PICTO_TREE)/source/common/engine/SlaveEvent.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/SlaveEventQueue.h
SOURCES += $$(PICTO_TREE)/source/common/engine/SlaveEventQueue.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/SlaveExperimentDriver.h
SOURCES += $$(PICTO_TREE)/source/common/engine/SlaveExperimentDriver.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/StateUpdater.h
SOURCES += $$(PICTO_TREE)/source/common/engine/StateUpdater.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/PropertyTable.h
SOURCES += $$(PICTO_TREE)/source/common/engine/PropertyTable.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/MouseInputPort.h
SOURCES += $$(PICTO_TREE)/source/common/engine/MouseInputPort.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/xYSignalChannel.h
SOURCES += $$(PICTO_TREE)/source/common/engine/xYSignalChannel.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/GenericInput.h
SOURCES += $$(PICTO_TREE)/source/common/engine/GenericInput.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/PictoEngine.h
SOURCES += $$(PICTO_TREE)/source/common/engine/PictoEngine.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/ControlPanelInterface.h
SOURCES += $$(PICTO_TREE)/source/common/engine/ControlPanelInterface.cpp

HEADERS += $$(PICTO_TREE)/source/common/experiment/Experiment.h
SOURCES += $$(PICTO_TREE)/source/common/experiment/Experiment.cpp

HEADERS += $$(PICTO_TREE)/source/common/iodevices/BufferFileGenerator.h
SOURCES += $$(PICTO_TREE)/source/common/iodevices/BufferFileGenerator.cpp
HEADERS += $$(PICTO_TREE)/source/common/iodevices/RewardController.h
SOURCES += $$(PICTO_TREE)/source/common/iodevices/RewardController.cpp
HEADERS += $$(PICTO_TREE)/source/common/iodevices/OutputSignalController.h
SOURCES += $$(PICTO_TREE)/source/common/iodevices/OutputSignalController.cpp
HEADERS += $$(PICTO_TREE)/source/common/iodevices/VirtualOutputSignalController.h
SOURCES += $$(PICTO_TREE)/source/common/iodevices/VirtualOutputSignalController.cpp
HEADERS += $$(PICTO_TREE)/source/common/iodevices/EventCodeGenerator.h
SOURCES += $$(PICTO_TREE)/source/common/iodevices/EventCodeGenerator.cpp
HEADERS += $$(PICTO_TREE)/source/common/iodevices/NullRewardController.h
SOURCES += $$(PICTO_TREE)/source/common/iodevices/NullRewardController.cpp
HEADERS += $$(PICTO_TREE)/source/common/iodevices/NullEventCodeGenerator.h
SOURCES += $$(PICTO_TREE)/source/common/iodevices/NullEventCodeGenerator.cpp
HEADERS += $$(PICTO_TREE)/source/common/iodevices/AudioRewardController.h
SOURCES += $$(PICTO_TREE)/source/common/iodevices/AudioRewardController.cpp

HEADERS += $$(PICTO_TREE)/source/common/operator/OperatorPlot.h
SOURCES += $$(PICTO_TREE)/source/common/operator/OperatorPlot.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/BarBase.h
SOURCES += $$(PICTO_TREE)/source/common/operator/BarBase.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/BarPlot.h
SOURCES += $$(PICTO_TREE)/source/common/operator/BarPlot.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/BarAxisHandler.h
SOURCES += $$(PICTO_TREE)/source/common/operator/BarAxisHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/HistogramPlot.h
SOURCES += $$(PICTO_TREE)/source/common/operator/HistogramPlot.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/SamplingBarBase.h
SOURCES += $$(PICTO_TREE)/source/common/operator/SamplingBarBase.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/SamplingBarPlot.h
SOURCES += $$(PICTO_TREE)/source/common/operator/SamplingBarPlot.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/SamplingHistogramPlot.h
SOURCES += $$(PICTO_TREE)/source/common/operator/SamplingHistogramPlot.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/DataViewElement.h
SOURCES += $$(PICTO_TREE)/source/common/operator/DataViewElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/DataViewSpecs.h
SOURCES += $$(PICTO_TREE)/source/common/operator/DataViewSpecs.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/OperatorPlotHandler.h
SOURCES += $$(PICTO_TREE)/source/common/operator/OperatorPlotHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/BarBasePlotHandler.h
SOURCES += $$(PICTO_TREE)/source/common/operator/BarBasePlotHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/BarPlotPlotHandler.h
SOURCES += $$(PICTO_TREE)/source/common/operator/BarPlotPlotHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/SamplingBarBasePlotHandler.h
SOURCES += $$(PICTO_TREE)/source/common/operator/SamplingBarBasePlotHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/SamplingBarPlotPlotHandler.h
SOURCES += $$(PICTO_TREE)/source/common/operator/SamplingBarPlotPlotHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/PSTHPlot.h
SOURCES += $$(PICTO_TREE)/source/common/operator/PSTHPlot.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/PSTHPlotHandler.h
SOURCES += $$(PICTO_TREE)/source/common/operator/PSTHPlotHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/RasterPlot.h
SOURCES += $$(PICTO_TREE)/source/common/operator/RasterPlot.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/RasterPlotHandler.h
SOURCES += $$(PICTO_TREE)/source/common/operator/RasterPlotHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/RasterBase.h
SOURCES += $$(PICTO_TREE)/source/common/operator/RasterBase.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/RasterBasePlotHandler.h
SOURCES += $$(PICTO_TREE)/source/common/operator/RasterBasePlotHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/operator/NeuralDataListener.h
SOURCES += $$(PICTO_TREE)/source/common/operator/NeuralDataListener.cpp

HEADERS += $$(PICTO_TREE)/source/common/network/ServerDiscoverer.h
SOURCES += $$(PICTO_TREE)/source/common/network/ServerDiscoverer.cpp
HEADERS += $$(PICTO_TREE)/source/common/network/CommandChannel.h
SOURCES += $$(PICTO_TREE)/source/common/network/CommandChannel.cpp
HEADERS += $$(PICTO_TREE)/source/common/network/ComponentInterface.h
SOURCES += $$(PICTO_TREE)/source/common/network/ComponentInterface.cpp
HEADERS += $$(PICTO_TREE)/source/common/network/ComponentStatusManager.h
SOURCES += $$(PICTO_TREE)/source/common/network/ComponentStatusManager.cpp

HEADERS += $$(PICTO_TREE)/source/common/parameter/Analysis.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/Analysis.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisDataSource.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisDataSource.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisFunction.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisFunction.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisFileOutput.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisFileOutput.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisPlotOutput.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisPlotOutput.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisBinaryOutputWidget.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisBinaryOutputWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisFileOutputWidget.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisFileOutputWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisOutput.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisOutput.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisOutputWidget.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisOutputWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisOutputWidgetContainer.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisOutputWidgetContainer.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisScriptHolder.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisScriptHolder.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisFrameData.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisFrameData.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisLfpData.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisLfpData.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisRewardData.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisRewardData.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisRunNotesData.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisRunNotesData.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisSpikeData.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisSpikeData.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisSignalData.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisSignalData.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisTimer.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisTimer.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisNumberVariable.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisNumberVariable.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisStringVariable.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisStringVariable.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisVariableMap.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisVariableMap.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisVariable.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisVariable.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisMatlabObj.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisMatlabObj.cpp

HEADERS += $$(PICTO_TREE)/source/common/parameter/AnalysisVariableList.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AnalysisVariableList.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AssociateElement.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AssociateElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AssociateHostLink.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AssociateHostLink.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AssociateRoot.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AssociateRoot.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AssociateRootHost.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AssociateRootHost.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/AudioFileParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/AudioFileParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/Parameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/Parameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/BooleanParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/BooleanParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/ColorParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/ColorParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/FileParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/FileParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/ImageFileParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/ImageFileParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/OperatorClickParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/OperatorClickParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/SignalValueParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/SignalValueParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/NumericParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/NumericParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/RangeParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/RangeParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/PseudorandomIntParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/PseudorandomIntParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/RandomIntParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/RandomIntParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/RandomDoubleParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/RandomDoubleParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/DoubleParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/DoubleParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/TimerParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/TimerParameter.cpp

HEADERS += $$(PICTO_TREE)/source/common/playback/DataState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/DataState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/LiveFrameReader.h
SOURCES += $$(PICTO_TREE)/source/common/playback/LiveFrameReader.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/LiveLfpReader.h
SOURCES += $$(PICTO_TREE)/source/common/playback/LiveLfpReader.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/LiveRewardReader.h
SOURCES += $$(PICTO_TREE)/source/common/playback/LiveRewardReader.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/LiveRunNotesReader.h
SOURCES += $$(PICTO_TREE)/source/common/playback/LiveRunNotesReader.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/LiveSignalReader.h
SOURCES += $$(PICTO_TREE)/source/common/playback/LiveSignalReader.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/LiveSpikeReader.h
SOURCES += $$(PICTO_TREE)/source/common/playback/LiveSpikeReader.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/FileSessionLoader.h
SOURCES += $$(PICTO_TREE)/source/common/playback/FileSessionLoader.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/FrameState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/FrameState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/LfpState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/LfpState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/PlaybackIndex.h
SOURCES += $$(PICTO_TREE)/source/common/playback/PlaybackIndex.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/PlaybackInterfaces.h
SOURCES += $$(PICTO_TREE)/source/common/playback/PlaybackInterfaces.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/PropertyState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/PropertyState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/RewardState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/RewardState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/RunNotesState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/RunNotesState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/SessionPlayer.h
SOURCES += $$(PICTO_TREE)/source/common/playback/SessionPlayer.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/SessionState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/SessionState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/SessionVersionInterfacer.h
SOURCES += $$(PICTO_TREE)/source/common/playback/SessionVersionInterfacer.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/SignalState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/SignalState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/SpikeState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/SpikeState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/TransitionState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/TransitionState.cpp

HEADERS += $$(PICTO_TREE)/source/common/property/Property.h
SOURCES += $$(PICTO_TREE)/source/common/property/Property.cpp
HEADERS += $$(PICTO_TREE)/source/common/property/PropertyContainer.h
SOURCES += $$(PICTO_TREE)/source/common/property/PropertyContainer.cpp
HEADERS += $$(PICTO_TREE)/source/common/property/ColorProperty.h
SOURCES += $$(PICTO_TREE)/source/common/property/ColorProperty.cpp
HEADERS += $$(PICTO_TREE)/source/common/property/ListProperty.h
SOURCES += $$(PICTO_TREE)/source/common/property/ListProperty.cpp
HEADERS += $$(PICTO_TREE)/source/common/property/MapProperty.h
SOURCES += $$(PICTO_TREE)/source/common/property/MapProperty.cpp
HEADERS += $$(PICTO_TREE)/source/common/property/PointProperty.h
SOURCES += $$(PICTO_TREE)/source/common/property/PointProperty.cpp
HEADERS += $$(PICTO_TREE)/source/common/property/SizeProperty.h
SOURCES += $$(PICTO_TREE)/source/common/property/SizeProperty.cpp
HEADERS += $$(PICTO_TREE)/source/common/property/EnumProperty.h
SOURCES += $$(PICTO_TREE)/source/common/property/EnumProperty.cpp
HEADERS += $$(PICTO_TREE)/source/common/property/ViewProperty.h
SOURCES += $$(PICTO_TREE)/source/common/property/ViewProperty.cpp

HEADERS += $$(PICTO_TREE)/source/common/protocol/AnnounceCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/AnnounceCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/DiscoverCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/DiscoverCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/GetCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/GetCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/TerminateCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/TerminateCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/Protocol.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/Protocol.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/ProtocolCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/ProtocolCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/ProtocolCommand.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/ProtocolCommand.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/ProtocolResponse.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/ProtocolResponse.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/ProtocolResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/ProtocolResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/OKResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/OKResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/NewSessionResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/NewSessionResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/LoadExpResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/LoadExpResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/StartResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/StartResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/StopResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/StopResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/PauseResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/PauseResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/ResumeResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/ResumeResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/EndSessionResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/EndSessionResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/ParameterResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/ParameterResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/ClickResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/ClickResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/RewardResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/RewardResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/common/protocol/ErrorResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/common/protocol/ErrorResponseHandler.cpp

HEADERS += $$(PICTO_TREE)/source/common/random/MersenneTwister.h

HEADERS += $$(PICTO_TREE)/source/common/statemachine/Scene.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/Scene.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/Reward.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/Reward.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/LogicResult.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/LogicResult.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/Result.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/Result.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/StateMachine.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/StateMachine.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/UIData.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/UIData.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/UIElement.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/UIElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/MachineContainer.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/MachineContainer.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/OutputElementContainer.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/OutputElementContainer.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/PausePoint.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/PausePoint.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/ResultContainer.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/ResultContainer.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/Transition.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/Transition.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/StateMachineElement.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/StateMachineElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/SwitchElement.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/SwitchElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/State.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/State.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/Scriptable.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/Scriptable.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/ScriptableContainer.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/ScriptableContainer.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/ScriptFunction.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/ScriptFunction.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/UIEnabled.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/UIEnabled.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/StateMachineElementContainer.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/StateMachineElementContainer.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/ContainerElement.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/ContainerElement.cpp


HEADERS += $$(PICTO_TREE)/source/common/stimuli/VisualElement.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/VisualElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/OutputElement.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/OutputElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/OutputSignal.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/OutputSignal.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/InputSignal.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/InputSignal.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/AudioElement.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/AudioElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/ArrowGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/ArrowGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/BoxGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/BoxGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/DiamondGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/DiamondGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/ShapeShifterGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/ShapeShifterGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/CursorGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/CursorGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/EllipseGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/EllipseGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/FractalGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/FractalGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/CircleGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/CircleGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/TriangleGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/TriangleGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/PolygonGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/PolygonGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/ImageGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/ImageGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/LineGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/LineGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/GridGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/GridGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/MultiplatformSound.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/MultiplatformSound.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/PictureGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/PictureGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/PreloadedSound.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/PreloadedSound.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/RandomlyFilledGridGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/RandomlyFilledGridGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/TextGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/TextGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/TokenFactoryGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/TokenFactoryGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/LabelFactoryGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/LabelFactoryGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/TokenTrayGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/TokenTrayGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/OperatorInfoGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/OperatorInfoGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/BinaryDataOutput.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/BinaryDataOutput.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/DigitalOutput.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/DigitalOutput.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/AnalogInput.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/AnalogInput.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/TextFactoryGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/TextFactoryGraphic.cpp


HEADERS += $$(PICTO_TREE)/source/common/storage/DataStore.h
SOURCES += $$(PICTO_TREE)/source/common/storage/DataStore.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/Serializable.h
SOURCES += $$(PICTO_TREE)/source/common/storage/Serializable.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/DesignConfig.h
SOURCES += $$(PICTO_TREE)/source/common/storage/DesignConfig.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/TaskConfig.h
SOURCES += $$(PICTO_TREE)/source/common/storage/TaskConfig.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/Asset.h
SOURCES += $$(PICTO_TREE)/source/common/storage/Asset.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/AssetExportImport.h
SOURCES += $$(PICTO_TREE)/source/common/storage/AssetExportImport.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/DataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/DataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/AssetFactory.h
SOURCES += $$(PICTO_TREE)/source/common/storage/AssetFactory.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/PropertyFactory.h
SOURCES += $$(PICTO_TREE)/source/common/storage/PropertyFactory.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/SearchRequest.h
HEADERS += $$(PICTO_TREE)/source/common/storage/SessionDataPackage.h
SOURCES += $$(PICTO_TREE)/source/common/storage/SessionDataPackage.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/BehavioralDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/BehavioralDataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/BehavioralDataUnitPackage.h
SOURCES += $$(PICTO_TREE)/source/common/storage/BehavioralDataUnitPackage.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/InputDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/InputDataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/InputDataUnitPackage.h
SOURCES += $$(PICTO_TREE)/source/common/storage/InputDataUnitPackage.cpp
SOURCES += $$(PICTO_TREE)/source/common/storage/SignalChannelInfo.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/SignalChannelInfo.h
SOURCES += $$(PICTO_TREE)/source/common/storage/DataSourceInfo.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/DataSourceInfo.h
SOURCES += $$(PICTO_TREE)/source/common/storage/AlignmentInfo.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/AlignmentInfo.h
HEADERS += $$(PICTO_TREE)/source/common/storage/PropertyDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/PropertyDataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/PropertyDataUnitPackage.h
SOURCES += $$(PICTO_TREE)/source/common/storage/PropertyDataUnitPackage.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/FrameDataUnitPackage.h
SOURCES += $$(PICTO_TREE)/source/common/storage/FrameDataUnitPackage.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/FrameDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/FrameDataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/TaskRunDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/TaskRunDataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/RewardDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/RewardDataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/StateDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/StateDataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/StateDataUnitPackage.h
SOURCES += $$(PICTO_TREE)/source/common/storage/StateDataUnitPackage.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/NeuralDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/NeuralDataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/AlignmentDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/AlignmentDataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/LFPDataUnitPackage.h
SOURCES += $$(PICTO_TREE)/source/common/storage/LFPDataUnitPackage.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/ObsoleteAsset.h
SOURCES += $$(PICTO_TREE)/source/common/storage/ObsoleteAsset.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/ObsoleteNameAsset.h
SOURCES += $$(PICTO_TREE)/source/common/storage/ObsoleteNameAsset.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/OldVersionAsset.h
SOURCES += $$(PICTO_TREE)/source/common/storage/OldVersionAsset.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/RewardUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/RewardUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/DirectorData.h
SOURCES += $$(PICTO_TREE)/source/common/storage/DirectorData.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/AlignmentTool.h
SOURCES += $$(PICTO_TREE)/source/common/storage/AlignmentTool.cpp

HEADERS += $$(PICTO_TREE)/source/common/task/Task.h
SOURCES += $$(PICTO_TREE)/source/common/task/Task.cpp

HEADERS += $$(PICTO_TREE)/source/common/timing/Timestamper.h
SOURCES += $$(PICTO_TREE)/source/common/timing/Timestamper.cpp
HEADERS += $$(PICTO_TREE)/source/common/timing/Stopwatch.h
SOURCES += $$(PICTO_TREE)/source/common/timing/Stopwatch.cpp
HEADERS += $$(PICTO_TREE)/source/common/timing/TimedMessageBox.h
SOURCES += $$(PICTO_TREE)/source/common/timing/TimedMessageBox.cpp

HEADERS += $$(PICTO_TREE)/source/common/update/UpdateDownloader.h
SOURCES += $$(PICTO_TREE)/source/common/update/UpdateDownloader.cpp

HEADERS += $$(PICTO_TREE)/source/common/variable/Variable.h
SOURCES += $$(PICTO_TREE)/source/common/variable/Variable.cpp
HEADERS += $$(PICTO_TREE)/source/common/variable/NumberVariable.h
SOURCES += $$(PICTO_TREE)/source/common/variable/NumberVariable.cpp
HEADERS += $$(PICTO_TREE)/source/common/variable/StringVariable.h
SOURCES += $$(PICTO_TREE)/source/common/variable/StringVariable.cpp
HEADERS += $$(PICTO_TREE)/source/common/variable/VariableList.h
SOURCES += $$(PICTO_TREE)/source/common/variable/VariableList.cpp
HEADERS += $$(PICTO_TREE)/source/common/variable/VariableMap.h
SOURCES += $$(PICTO_TREE)/source/common/variable/VariableMap.cpp

win32 {

}

wince* {

HEADERS += $$(PICTO_TREE)/source/common/time_ce/time_ce.h
SOURCES += $$(PICTO_TREE)/source/common/time_ce/time_ce.cpp

}

include($$(PICTO_TREE)/source/common/common.pri)

# Resources
RESOURCES += $$(PICTO_TREE)/source/common/common.qrc

# Translations
TRANSLATIONS += $$(PICTO_TREE)/source/common/translations/common_ko.ts

# Output
build_pass:CONFIG(debug, debug|release) {
  win32:DESTDIR = $$(PICTO_TREE)/intermediates/lib/debug
  unix:!macx:DESTDIR = $$(PICTO_TREE)/output/bin/debug/shared
  macx:DESTDIR += $$(PICTO_TREE)/intermediates/lib/debug
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/common/debug
  win32:DLLDESTDIR = $$(PICTO_TREE)/output/bin/debug
  win32:LIBS += qwtd.lib
}
build_pass:CONFIG(release, debug|release) {
  win32:DESTDIR = $$(PICTO_TREE)/intermediates/lib/release
  unix:!macx:DESTDIR = $$(PICTO_TREE)/output/bin/release/shared
  macx:DESTDIR += $$(PICTO_TREE)/intermediates/lib/release
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/common/release
  win32:DLLDESTDIR = $$(PICTO_TREE)/output/bin/release
  win32:LIBS += qwt.lib
}
RCC_DIR = $$(PICTO_TREE)/intermediates/resources/common
UI_DIR = $$(PICTO_TREE)/intermediates/ui/common
MOC_DIR = $$(PICTO_TREE)/intermediates/moc/common

# Platform Specific Configuration

win32 {
#Perform compilation using multiple processes if using
#Microsoft compiler version 15 (Visual Studio 2008) or greater
#Version 14 (Visual Studio 2005) supported the necessary compiler
#switch, but had bugs which could lead to corrupt PDB files
#
#Also, Visual Studio 2008 changed the default behavior for service
#packs so that the RTM version of the CRT, ATL, MFC, and OPENMP
#libraries are specified in the manifest by default.  Since we're
#likely to use the CRT as a private assembly, we need to specify
#that the compiler should bind to the current version.  This should
#be done using the _BIND_TO_CURRENT_VCLIBS_VERSION preprocessor
#definition (which specifies to use the latest version of each of
#the CRT, ATL, MFC, and OPENMP); however, the Visual Studio 2008
#feature pack has a bug with this definition, so instead we'll use
#_BIND_TO_CURRENT_CRT_VERSION since we only use the CRT anyway
for(compilerDefine, QMAKE_COMPILER_DEFINES) {
    compilerVersion = $$find(compilerDefine, "_MSC_VER=*")
    !isEmpty(compilerVersion) {
        compilerVersion = $$replace(compilerVersion, "_MSC_VER=", "")
        !contains(compilerVersion,1200) {
        !contains(compilerVersion,1300) {
        !contains(compilerVersion,1310) {
        !contains(compilerVersion,1400) {
            DEFINES += _BIND_TO_CURRENT_CRT_VERSION
            #Note: This switch has been moved to a custom mkspecs
            #file for win32-msvc2008
            #QMAKE_CXXFLAGS+=/MP
            visualStudioProject = $$find(TEMPLATE, "vc")
            build_pass:CONFIG(release, debug|release):!isEmpty(visualStudioProject) {
                message(Warnings regarding Compiler option: -MP can safely be ignored)
            }
        }}}}
    }
}

build_pass:CONFIG(debug, debug|release) {
	#Output PDB file to appropriate directory
	QMAKE_CXXFLAGS+=/Fd$(IntDir)
}
}

macx {
CONFIG += x86 ppc
QMAKE_LFLAGS_SONAME = -install_name$${LITERAL_WHITESPACE}@executable_path/../Libraries/picto/
}

# Machine Type

win32:QMAKE_LFLAGS += /MACHINE:$$(PICTO_MACHINE_TYPE)

# Debug Naming

build_pass:CONFIG(debug, debug|release) {
  TARGET 		= $$member(TARGET, 0)_debug
}



# Section below was copied directly from qtffmpegwrapper's config.pro with modified paths in "Modify here" section.
# Include the configuration file below in the QT .pro file, and modify the path accordingly.




# ##############################################################################
# ##############################################################################
# FFMPEG: START OF CONFIGURATION BELOW ->
# Copy these lines into your own project
# Make sure to set the path variables for:
# 1) QTFFmpegWrapper sources (i.e. where the QVideoEncoder.cpp and QVideoDecoder.cpp lie),
# 2) FFMPEG include path (i.e. where the directories libavcodec, libavutil, etc. lie),
# 3) the binary FFMPEG libraries (that must be compiled separately).
# Under Linux path 2 and 3 may not need to be set as these are usually in the standard include and lib path.
# Under Windows, path 2 and 3 must be set to the location where you placed the FFMPEG includes and compiled binaries
# Note that the FFMPEG dynamic librairies (i.e. the .dll files) must be in the PATH
# ##############################################################################
# ##############################################################################

# ##############################################################################
# Modify here: set FFMPEG_LIBRARY_PATH and FFMPEG_INCLUDE_PATH
# ##############################################################################

# Set QTFFMPEGWRAPPER_SOURCE_PATH to point to the directory containing the QTFFmpegWrapper sources
QTFFMPEGWRAPPER_SOURCE_PATH = $$(PICTO_TREE)/3rdparty/QTFFmpegWrapper/QTFFmpegWrapper

# Set FFMPEG_LIBRARY_PATH to point to the directory containing the FFmpeg import libraries (if needed - typically for Windows), i.e. the dll.a files

contains(MACHINE_TYPE,X86) {
	FFMPEG_LIBRARY_PATH = $$(PICTO_TREE)/3rdparty/QTFFmpegWrapper/ffmpeg_lib_win32
	}
contains(MACHINE_TYPE,X64) {
	FFMPEG_LIBRARY_PATH = $$(PICTO_TREE)/3rdparty/QTFFmpegWrapper/ffmpeg_lib_win64
	}

# Set FFMPEG_INCLUDE_PATH to point to the directory containing the FFMPEG includes (if needed - typically for Windows)
FFMPEG_INCLUDE_PATH = $$(PICTO_TREE)/3rdparty/QTFFmpegWrapper/QTFFmpegWrapper

# ##############################################################################
# Do not modify: FFMPEG default settings
# ##############################################################################
# Sources for QT wrapper
SOURCES += $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoEncoder.cpp \
    $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoDecoder.cpp
HEADERS += $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoEncoder.h \
    $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoDecoder.h

# Set list of required FFmpeg libraries
LIBS += -lavutil \
    -lavcodec \
    -lavformat \
    -lswscale

# Add the path
LIBS += -L$$FFMPEG_LIBRARY_PATH
INCLUDEPATH += QVideoEncoder
INCLUDEPATH += $$FFMPEG_INCLUDE_PATH

# Requied for some C99 defines
DEFINES += __STDC_CONSTANT_MACROS

# ##############################################################################
# FFMPEG: END OF CONFIGURATION
# ##############################################################################

HEADERS += $$(PICTO_TREE)/3rdparty/matlab/binocdfCoderWrap.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/binocdfCoderWrap.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/binocdfCoderWrap_data.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/binocdfCoderWrap_data.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/binocdfCoderWrap_emxAPI.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/binocdfCoderWrap_emxAPI.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/binocdfCoderWrap_emxutil.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/binocdfCoderWrap_emxutil.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/binocdfCoderWrap_initialize.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/binocdfCoderWrap_initialize.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/binocdfCoderWrap_terminate.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/binocdfCoderWrap_terminate.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/binocdfCoderWrap_types.h
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/binodeviance.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/binodeviance.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/binopdf.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/binopdf.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/binopdfCoderWrap.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/binopdfCoderWrap.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/eml_rand_mt19937ar_stateful.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/eml_rand_mt19937ar_stateful.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/fileManager.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/fileManager.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/fprintf.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/fprintf.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/gammaln.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/gammaln.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/normrndCoderWrap.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/normrndCoderWrap.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/rand.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/rand.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/randCoderWrap.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/randCoderWrap.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/randn.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/randn.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/rt_nonfinite.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/rt_nonfinite.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/rtGetInf.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/rtGetInf.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/rtGetNaN.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/rtGetNaN.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/rtwtypes.h
SOURCES += $$(PICTO_TREE)/3rdparty/matlab/stirlerr.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/matlab/stirlerr.h


