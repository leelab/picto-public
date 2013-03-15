TEMPLATE = lib
unix:TARGET = Picto
win32:TARGET = libPicto
QT = core gui xml network script scripttools sql multimedia svg concurrent widgets
CONFIG += qt warn_on debug_and_release 
CONFIG -= flat
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += $$[QT_INSTALL_PREFIX]/src/3rdparty/zlib

# Definitions
DEFINES += PICTOLIB_EXPORTS

# Input
HEADERS += $$(PICTO_TREE)/source/common/common.h
HEADERS += $$(PICTO_TREE)/source/common/namedefs.h
SOURCES += $$(PICTO_TREE)/source/common/namedefs.cpp
HEADERS += $$(PICTO_TREE)/source/common/globals.h
SOURCES += $$(PICTO_TREE)/source/common/globals.cpp
HEADERS += $$(PICTO_TREE)/source/common/mainmemleakdetect.h
HEADERS += $$(PICTO_TREE)/source/common/memleakdetect.h


HEADERS += $$(PICTO_TREE)/source/common/analysis/EventOrderIndex.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/EventOrderIndex.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/AnalysisContainer.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/AnalysisContainer.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/AnalysisDefinition.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/AnalysisDefinition.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/AnalysisTool.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/AnalysisTool.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/AnalysisOutput.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/AnalysisOutput.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/NumericVariable.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/NumericVariable.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/FileOutput.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/FileOutput.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/AnalysisOutputWidget.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/AnalysisOutputWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/FileOutputWidget.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/FileOutputWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/AnalysisDataSource.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/AnalysisDataSource.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/AnalysisDataIterator.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/AnalysisDataIterator.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/AnalysisValue.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/AnalysisValue.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/PropertyDataSource.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/PropertyDataSource.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/FrameDataSource.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/FrameDataSource.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/AlignDataSource.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/AlignDataSource.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/SpikeDataSource.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/SpikeDataSource.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/LFPDataSource.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/LFPDataSource.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/SignalDataSource.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/SignalDataSource.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/TimeDataSource.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/TimeDataSource.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/DefaultDataSource.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/DefaultDataSource.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/AnalysisTrigger.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/AnalysisTrigger.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/ElementTrigger.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/ElementTrigger.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/ElementDataSource.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/ElementDataSource.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/ElementDataIterator.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/ElementDataIterator.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/PropertyTrigger.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/PropertyTrigger.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/FrameTrigger.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/FrameTrigger.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/AlignTrigger.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/AlignTrigger.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/TransitionTrigger.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/TransitionTrigger.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/TransitionDataIterator.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/TransitionDataIterator.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/SpikeTrigger.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/SpikeTrigger.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/TimeTrigger.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/TimeTrigger.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/LFPTrigger.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/LFPTrigger.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/SignalTrigger.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/SignalTrigger.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/AnalysisPeriod.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/AnalysisPeriod.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/StarterContainer.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/StarterContainer.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/EnderContainer.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/EnderContainer.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/PropertyDataIterator.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/PropertyDataIterator.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/FrameDataIterator.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/FrameDataIterator.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/AlignDataIterator.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/AlignDataIterator.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/TimeDataIterator.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/TimeDataIterator.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/LFPDataIterator.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/LFPDataIterator.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/SignalDataIterator.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/SignalDataIterator.cpp
HEADERS += $$(PICTO_TREE)/source/common/analysis/SpikeDataIterator.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/SpikeDataIterator.cpp

HEADERS += $$(PICTO_TREE)/source/common/archives/archives.h
SOURCES += $$(PICTO_TREE)/source/common/archives/archives.cpp
HEADERS += $$(PICTO_TREE)/source/common/archives/zip.h
SOURCES += $$(PICTO_TREE)/source/common/archives/zip.c
HEADERS += $$(PICTO_TREE)/source/common/archives/unzip.h
SOURCES += $$(PICTO_TREE)/source/common/archives/unzip.c
HEADERS += $$(PICTO_TREE)/source/common/archives/ioapi.h
SOURCES += $$(PICTO_TREE)/source/common/archives/ioapi.c

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
HEADERS += $$(PICTO_TREE)/source/common/compositor/SoftwareCompositingSurface.h
SOURCES += $$(PICTO_TREE)/source/common/compositor/SoftwareCompositingSurface.cpp
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

HEADERS += $$(PICTO_TREE)/source/common/controlelements/TestController.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/TestController.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/StopwatchController.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/StopwatchController.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/ScriptController.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/ScriptController.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/Timer.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/Timer.cpp
# HEADERS += $$(PICTO_TREE)/source/common/controlelements/IntervalController.h
# SOURCES += $$(PICTO_TREE)/source/common/controlelements/IntervalController.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/TargetController.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/TargetController.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/ChoiceController.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/ChoiceController.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/ControlTarget.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/ControlTarget.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/RectTarget.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/RectTarget.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/CircleTarget.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/CircleTarget.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/ControlResult.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/ControlResult.cpp
# HEADERS += $$(PICTO_TREE)/source/common/controlelements/DynamicController.h
# SOURCES += $$(PICTO_TREE)/source/common/controlelements/DynamicController.cpp
# HEADERS += $$(PICTO_TREE)/source/common/controlelements/CustomController.h
# SOURCES += $$(PICTO_TREE)/source/common/controlelements/CustomController.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/ControlElement.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/ControlElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/ControlLink.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/ControlLink.cpp
HEADERS += $$(PICTO_TREE)/source/common/controlelements/ControlElementFactory.h
SOURCES += $$(PICTO_TREE)/source/common/controlelements/ControlElementFactory.cpp

HEADERS += $$(PICTO_TREE)/source/common/design/Design.h
SOURCES += $$(PICTO_TREE)/source/common/design/Design.cpp
HEADERS += $$(PICTO_TREE)/source/common/design/DesignRoot.h
SOURCES += $$(PICTO_TREE)/source/common/design/DesignRoot.cpp
HEADERS += $$(PICTO_TREE)/source/common/design/PictoData.h
SOURCES += $$(PICTO_TREE)/source/common/design/PictoData.cpp

HEADERS += $$(PICTO_TREE)/source/common/designer/EditorState.h
SOURCES += $$(PICTO_TREE)/source/common/designer/EditorState.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/arrow.h
SOURCES += $$(PICTO_TREE)/source/common/designer/arrow.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/diagramitem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/diagramitem.cpp
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
HEADERS += $$(PICTO_TREE)/source/common/designer/ResultItem.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ResultItem.cpp
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
HEADERS += $$(PICTO_TREE)/source/common/designer/AssetToolGroup.h
SOURCES += $$(PICTO_TREE)/source/common/designer/AssetToolGroup.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/PropertyToolGroup.h
SOURCES += $$(PICTO_TREE)/source/common/designer/PropertyToolGroup.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/BackgroundToolGroup.h
SOURCES += $$(PICTO_TREE)/source/common/designer/BackgroundToolGroup.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/PropertyEditorFactory.h
SOURCES += $$(PICTO_TREE)/source/common/designer/PropertyEditorFactory.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/PropertyFrame.h
SOURCES += $$(PICTO_TREE)/source/common/designer/PropertyFrame.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/PropertyGroupWidget.h
SOURCES += $$(PICTO_TREE)/source/common/designer/PropertyGroupWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/PropertyBrowser.h
SOURCES += $$(PICTO_TREE)/source/common/designer/PropertyBrowser.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/AssetInfoBox.h
SOURCES += $$(PICTO_TREE)/source/common/designer/AssetInfoBox.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ScriptTextEdit.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ScriptTextEdit.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ScriptWidget.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ScriptWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ScriptSyntaxHighlighter.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ScriptSyntaxHighlighter.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/SearchableTextEdit.h
SOURCES += $$(PICTO_TREE)/source/common/designer/SearchableTextEdit.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/DeletableWidget.h
SOURCES += $$(PICTO_TREE)/source/common/designer/DeletableWidget.cpp
HEADERS += $$(PICTO_TREE)/source/common/designer/ViewerWindow.h
SOURCES += $$(PICTO_TREE)/source/common/designer/ViewerWindow.cpp


HEADERS += $$(PICTO_TREE)/source/common/engine/InputPort.h
SOURCES += $$(PICTO_TREE)/source/common/engine/InputPort.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/SignalChannel.h
SOURCES += $$(PICTO_TREE)/source/common/engine/SignalChannel.cpp
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
HEADERS += $$(PICTO_TREE)/source/common/engine/PictoEngine.h
SOURCES += $$(PICTO_TREE)/source/common/engine/PictoEngine.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/ControlPanelInterface.h
SOURCES += $$(PICTO_TREE)/source/common/engine/ControlPanelInterface.cpp

HEADERS += $$(PICTO_TREE)/source/common/experiment/Experiment.h
SOURCES += $$(PICTO_TREE)/source/common/experiment/Experiment.cpp
HEADERS += $$(PICTO_TREE)/source/common/experiment/SessionData.h
SOURCES += $$(PICTO_TREE)/source/common/experiment/SessionData.cpp
HEADERS += $$(PICTO_TREE)/source/common/experiment/Calibration.h
SOURCES += $$(PICTO_TREE)/source/common/experiment/Calibration.cpp
HEADERS += $$(PICTO_TREE)/source/common/experiment/MediaItem.h
SOURCES += $$(PICTO_TREE)/source/common/experiment/MediaItem.cpp
HEADERS += $$(PICTO_TREE)/source/common/experiment/RewardCalibration.h
SOURCES += $$(PICTO_TREE)/source/common/experiment/RewardCalibration.cpp
HEADERS += $$(PICTO_TREE)/source/common/experiment/EyeTrackerCalibration.h
SOURCES += $$(PICTO_TREE)/source/common/experiment/EyeTrackerCalibration.cpp

HEADERS += $$(PICTO_TREE)/source/common/experimentalsystem/ExperimentalSystem.h
SOURCES += $$(PICTO_TREE)/source/common/experimentalsystem/ExperimentalSystem.cpp
HEADERS += $$(PICTO_TREE)/source/common/experimentalsystem/CameraEquipment.h
SOURCES += $$(PICTO_TREE)/source/common/experimentalsystem/CameraEquipment.cpp
HEADERS += $$(PICTO_TREE)/source/common/experimentalsystem/DataSourceEquipment.h
SOURCES += $$(PICTO_TREE)/source/common/experimentalsystem/DataSourceEquipment.cpp
HEADERS += $$(PICTO_TREE)/source/common/experimentalsystem/BehavioralInputEquipment.h
SOURCES += $$(PICTO_TREE)/source/common/experimentalsystem/BehavioralInputEquipment.cpp
HEADERS += $$(PICTO_TREE)/source/common/experimentalsystem/RewardEquipment.h
SOURCES += $$(PICTO_TREE)/source/common/experimentalsystem/RewardEquipment.cpp
HEADERS += $$(PICTO_TREE)/source/common/experimentalsystem/AudioEquipment.h
SOURCES += $$(PICTO_TREE)/source/common/experimentalsystem/AudioEquipment.cpp
HEADERS += $$(PICTO_TREE)/source/common/experimentalsystem/DisplayEquipment.h
SOURCES += $$(PICTO_TREE)/source/common/experimentalsystem/DisplayEquipment.cpp
HEADERS += $$(PICTO_TREE)/source/common/experimentalsystem/SubjectPosition.h
SOURCES += $$(PICTO_TREE)/source/common/experimentalsystem/SubjectPosition.cpp
HEADERS += $$(PICTO_TREE)/source/common/experimentalsystem/Environmental.h
SOURCES += $$(PICTO_TREE)/source/common/experimentalsystem/Environmental.cpp

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

HEADERS += $$(PICTO_TREE)/source/common/network/ServerDiscoverer.h
SOURCES += $$(PICTO_TREE)/source/common/network/ServerDiscoverer.cpp
HEADERS += $$(PICTO_TREE)/source/common/network/CommandChannel.h
SOURCES += $$(PICTO_TREE)/source/common/network/CommandChannel.cpp
HEADERS += $$(PICTO_TREE)/source/common/network/ComponentInterface.h
SOURCES += $$(PICTO_TREE)/source/common/network/ComponentInterface.cpp
HEADERS += $$(PICTO_TREE)/source/common/network/ComponentStatusManager.h
SOURCES += $$(PICTO_TREE)/source/common/network/ComponentStatusManager.cpp

HEADERS += $$(PICTO_TREE)/source/common/parameter/Parameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/Parameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/BooleanParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/BooleanParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/OperatorClickParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/OperatorClickParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/SignalValueParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/SignalValueParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/ChoiceParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/ChoiceParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/NumericParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/NumericParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/RangeParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/RangeParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/StringParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/StringParameter.cpp
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
HEADERS += $$(PICTO_TREE)/source/common/parameter/ParameterFactory.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/ParameterFactory.cpp

HEADERS += $$(PICTO_TREE)/source/common/playback/DataState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/DataState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/DataStateList.h
SOURCES += $$(PICTO_TREE)/source/common/playback/DataStateList.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/VectorDataStateList.h
SOURCES += $$(PICTO_TREE)/source/common/playback/VectorDataStateList.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/ListDataStateList.h
SOURCES += $$(PICTO_TREE)/source/common/playback/ListDataStateList.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/LinkedDataStateList.h
SOURCES += $$(PICTO_TREE)/source/common/playback/LinkedDataStateList.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/EventState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/EventState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/FileSessionLoader.h
SOURCES += $$(PICTO_TREE)/source/common/playback/FileSessionLoader.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/FrameState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/FrameState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/LfpState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/LfpState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/IndexedData.h
SOURCES += $$(PICTO_TREE)/source/common/playback/IndexedData.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/PlaybackData.h
SOURCES += $$(PICTO_TREE)/source/common/playback/PlaybackData.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/PlaybackInterfaces.h
SOURCES += $$(PICTO_TREE)/source/common/playback/PlaybackInterfaces.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/PropertyCollectionState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/PropertyCollectionState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/PropertyState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/PropertyState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/RewardState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/RewardState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/SessionPlayer.h
SOURCES += $$(PICTO_TREE)/source/common/playback/SessionPlayer.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/SessionState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/SessionState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/SessionLoader.h
SOURCES += $$(PICTO_TREE)/source/common/playback/SessionLoader.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/SessionLoaderThread.h
SOURCES += $$(PICTO_TREE)/source/common/playback/SessionLoaderThread.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/SessionVersionInterfacer.h
SOURCES += $$(PICTO_TREE)/source/common/playback/SessionVersionInterfacer.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/SignalState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/SignalState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/SpikeState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/SpikeState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/TransitionState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/TransitionState.cpp
HEADERS += $$(PICTO_TREE)/source/common/playback/ValueState.h
SOURCES += $$(PICTO_TREE)/source/common/playback/ValueState.cpp

HEADERS += $$(PICTO_TREE)/source/common/property/Property.h
SOURCES += $$(PICTO_TREE)/source/common/property/Property.cpp
HEADERS += $$(PICTO_TREE)/source/common/property/PropertyContainer.h
SOURCES += $$(PICTO_TREE)/source/common/property/PropertyContainer.cpp
HEADERS += $$(PICTO_TREE)/source/common/property/ColorProperty.h
SOURCES += $$(PICTO_TREE)/source/common/property/ColorProperty.cpp
HEADERS += $$(PICTO_TREE)/source/common/property/PointProperty.h
SOURCES += $$(PICTO_TREE)/source/common/property/PointProperty.cpp
HEADERS += $$(PICTO_TREE)/source/common/property/RectProperty.h
SOURCES += $$(PICTO_TREE)/source/common/property/RectProperty.cpp
HEADERS += $$(PICTO_TREE)/source/common/property/EnumProperty.h
SOURCES += $$(PICTO_TREE)/source/common/property/EnumProperty.cpp

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

HEADERS += $$(PICTO_TREE)/source/common/random/mtrand.h
SOURCES += $$(PICTO_TREE)/source/common/random/mtrand.cpp
HEADERS += $$(PICTO_TREE)/source/common/random/MersenneTwister.h
SOURCES += $$(PICTO_TREE)/source/common/random/PictoIdentifiableObject.cpp
HEADERS += $$(PICTO_TREE)/source/common/random/PictoIdentifiableObject.h

HEADERS += $$(PICTO_TREE)/source/common/statemachine/Scene.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/Scene.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/Reward.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/Reward.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/RequiredResult.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/RequiredResult.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/Result.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/Result.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/StateMachine.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/StateMachine.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/MachineContainer.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/MachineContainer.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/PausePoint.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/PausePoint.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/ResultContainer.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/ResultContainer.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/Transition.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/Transition.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/TrialData.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/TrialData.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/StateMachineElement.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/StateMachineElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/ScriptElement.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/ScriptElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/State.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/State.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/Scriptable.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/Scriptable.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/ScriptableContainer.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/ScriptableContainer.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/ScriptFunction.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/ScriptFunction.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/UIInfo.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/UIInfo.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/UIEnabled.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/UIEnabled.cpp


HEADERS += $$(PICTO_TREE)/source/common/stimuli/StimulusElement.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/StimulusElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/VisualElement.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/VisualElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/OutputSignal.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/OutputSignal.cpp
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
HEADERS += $$(PICTO_TREE)/source/common/stimuli/CircleGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/CircleGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/LineGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/LineGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/GridGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/GridGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/PictureGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/PictureGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/WaveSound.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/WaveSound.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/RandomlyFilledGridGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/RandomlyFilledGridGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/TextGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/TextGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/TokenTrayGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/TokenTrayGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/OperatorInfoGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/OperatorInfoGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/BinaryDataOutput.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/BinaryDataOutput.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/DigitalOutput.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/DigitalOutput.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/VisualElementFactory.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/VisualElementFactory.cpp


HEADERS += $$(PICTO_TREE)/source/common/storage/DataStore.h
SOURCES += $$(PICTO_TREE)/source/common/storage/DataStore.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/Serializable.h
SOURCES += $$(PICTO_TREE)/source/common/storage/Serializable.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/ExperimentConfig.h
SOURCES += $$(PICTO_TREE)/source/common/storage/ExperimentConfig.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/Asset.h
SOURCES += $$(PICTO_TREE)/source/common/storage/Asset.cpp
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
SOURCES += $$(PICTO_TREE)/source/common/storage/SignalChannelInfo.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/SignalChannelInfo.h
SOURCES += $$(PICTO_TREE)/source/common/storage/DataSourceInfo.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/DataSourceInfo.h
SOURCES += $$(PICTO_TREE)/source/common/storage/AlignmentInfo.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/AlignmentInfo.h
SOURCES += $$(PICTO_TREE)/source/common/storage/BehavioralDataUnitPackage.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/PropertyDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/PropertyDataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/PropertyDataUnitPackage.h
SOURCES += $$(PICTO_TREE)/source/common/storage/PropertyDataUnitPackage.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/PropertyLookup.h
SOURCES += $$(PICTO_TREE)/source/common/storage/PropertyLookup.cpp
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
HEADERS += $$(PICTO_TREE)/source/common/storage/PositionalDataStore.h
SOURCES += $$(PICTO_TREE)/source/common/storage/PositionalDataStore.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/PhysiologicalDataStore.h
SOURCES += $$(PICTO_TREE)/source/common/storage/PhysiologicalDataStore.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/NeuralDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/NeuralDataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/AlignmentDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/AlignmentDataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/SingleUnitDataStore.h
SOURCES += $$(PICTO_TREE)/source/common/storage/SingleUnitDataStore.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/LFPDataUnitPackage.h
SOURCES += $$(PICTO_TREE)/source/common/storage/LFPDataUnitPackage.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/PupilDiameterDataStore.h
SOURCES += $$(PICTO_TREE)/source/common/storage/PupilDiameterDataStore.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/MagneticResonanceImagingDataStore.h
SOURCES += $$(PICTO_TREE)/source/common/storage/MagneticResonanceImagingDataStore.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/DeviceInterface.h
SOURCES += $$(PICTO_TREE)/source/common/storage/DeviceInterface.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/PlexonInterface.h
SOURCES += $$(PICTO_TREE)/source/common/storage/PlexonInterface.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/TDTInterface.h
SOURCES += $$(PICTO_TREE)/source/common/storage/TDTInterface.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/ObsoleteAsset.h
SOURCES += $$(PICTO_TREE)/source/common/storage/ObsoleteAsset.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/RewardUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/RewardUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/DirectorData.h
SOURCES += $$(PICTO_TREE)/source/common/storage/DirectorData.cpp

HEADERS += $$(PICTO_TREE)/source/common/task/Signal.h
SOURCES += $$(PICTO_TREE)/source/common/task/Signal.cpp
HEADERS += $$(PICTO_TREE)/source/common/task/Task.h
SOURCES += $$(PICTO_TREE)/source/common/task/Task.cpp
HEADERS += $$(PICTO_TREE)/source/common/task/TrialResult.h
SOURCES += $$(PICTO_TREE)/source/common/task/TrialResult.cpp
HEADERS += $$(PICTO_TREE)/source/common/task/Constraint.h
SOURCES += $$(PICTO_TREE)/source/common/task/Constraint.cpp
HEADERS += $$(PICTO_TREE)/source/common/task/BlockGenerator.h
SOURCES += $$(PICTO_TREE)/source/common/task/BlockGenerator.cpp

HEADERS += $$(PICTO_TREE)/source/common/timing/Timestamper.h
SOURCES += $$(PICTO_TREE)/source/common/timing/Timestamper.cpp
HEADERS += $$(PICTO_TREE)/source/common/timing/Stopwatch.h
SOURCES += $$(PICTO_TREE)/source/common/timing/Stopwatch.cpp
HEADERS += $$(PICTO_TREE)/source/common/timing/TimedMessageBox.h
SOURCES += $$(PICTO_TREE)/source/common/timing/TimedMessageBox.cpp

HEADERS += $$(PICTO_TREE)/source/common/update/UpdateDownloader.h
SOURCES += $$(PICTO_TREE)/source/common/update/UpdateDownloader.cpp

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
}
build_pass:CONFIG(release, debug|release) {
  win32:DESTDIR = $$(PICTO_TREE)/intermediates/lib/release
  unix:!macx:DESTDIR = $$(PICTO_TREE)/output/bin/release/shared
  macx:DESTDIR += $$(PICTO_TREE)/intermediates/lib/release
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/common/release
  win32:DLLDESTDIR = $$(PICTO_TREE)/output/bin/release
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
#Output PDB file to appropriate directory
QMAKE_CXXFLAGS+=/Fd$(IntDir)
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

# Copy Output dll to Test Directories for Testing
build_pass:CONFIG(debug, debug|release) {
  QMAKE_POST_LINK = copy "$(TargetPath)"  $$(PICTO_TREE)\\output\\tests\\bin\\debug
}

build_pass:CONFIG(release, debug|release) {
  QMAKE_POST_LINK = copy "$(TargetPath)" $$(PICTO_TREE)\\output\\tests\\bin\\release
}