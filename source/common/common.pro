TEMPLATE = lib
unix:TARGET = Picto
win32:TARGET = libPicto
QT = core gui xml network script scripttools sql
CONFIG += qt warn_on debug_and_release
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


HEADERS += $$(PICTO_TREE)/source/common/analysis/AnalysisModule.h
SOURCES += $$(PICTO_TREE)/source/common/analysis/AnalysisModule.cpp

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

HEADERS += $$(PICTO_TREE)/source/common/engine/SignalChannel.h
SOURCES += $$(PICTO_TREE)/source/common/engine/SignalChannel.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/PropertyTable.h
SOURCES += $$(PICTO_TREE)/source/common/engine/PropertyTable.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/MouseSignalChannel.h
SOURCES += $$(PICTO_TREE)/source/common/engine/MouseSignalChannel.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/xYSignalChannel.h
SOURCES += $$(PICTO_TREE)/source/common/engine/xYSignalChannel.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/NetworkSignalChannel.h
SOURCES += $$(PICTO_TREE)/source/common/engine/NetworkSignalChannel.cpp
HEADERS += $$(PICTO_TREE)/source/common/engine/PictoEngine.h
SOURCES += $$(PICTO_TREE)/source/common/engine/PictoEngine.cpp

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

HEADERS += $$(PICTO_TREE)/source/common/iodevices/RewardController.h
SOURCES += $$(PICTO_TREE)/source/common/iodevices/RewardController.cpp
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
HEADERS += $$(PICTO_TREE)/source/common/parameter/TimerParameter.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/TimerParameter.cpp
HEADERS += $$(PICTO_TREE)/source/common/parameter/ParameterFactory.h
SOURCES += $$(PICTO_TREE)/source/common/parameter/ParameterFactory.cpp

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
HEADERS += $$(PICTO_TREE)/source/common/statemachine/UIInfo.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/UIInfo.cpp
HEADERS += $$(PICTO_TREE)/source/common/statemachine/UIEnabled.h
SOURCES += $$(PICTO_TREE)/source/common/statemachine/UIEnabled.cpp


HEADERS += $$(PICTO_TREE)/source/common/stimuli/StimulusElement.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/StimulusElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/VisualElement.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/VisualElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/AudioElement.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/AudioElement.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/ArrowGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/ArrowGraphic.cpp
HEADERS += $$(PICTO_TREE)/source/common/stimuli/BoxGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/BoxGraphic.cpp
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
HEADERS += $$(PICTO_TREE)/source/common/stimuli/OperatorInfoGraphic.h
SOURCES += $$(PICTO_TREE)/source/common/stimuli/OperatorInfoGraphic.cpp
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
HEADERS += $$(PICTO_TREE)/source/common/storage/SessionDataStore.h
SOURCES += $$(PICTO_TREE)/source/common/storage/SessionDataStore.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/BehavioralDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/BehavioralDataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/BehavioralDataUnitPackage.h
SOURCES += $$(PICTO_TREE)/source/common/storage/BehavioralDataUnitPackage.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/PropertyDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/PropertyDataUnit.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/PropertyDataUnitPackage.h
SOURCES += $$(PICTO_TREE)/source/common/storage/PropertyDataUnitPackage.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/PropertyLookup.h
SOURCES += $$(PICTO_TREE)/source/common/storage/PropertyLookup.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/TransitionLookup.h
SOURCES += $$(PICTO_TREE)/source/common/storage/TransitionLookup.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/FrameDataUnitPackage.h
SOURCES += $$(PICTO_TREE)/source/common/storage/FrameDataUnitPackage.cpp
HEADERS += $$(PICTO_TREE)/source/common/storage/FrameDataUnit.h
SOURCES += $$(PICTO_TREE)/source/common/storage/FrameDataUnit.cpp
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
  QMAKE_POST_LINK = copy "$(TargetPath)"  $$(PICTO_TREE)\output\tests\bin\debug
}

build_pass:CONFIG(release, debug|release) {
  QMAKE_POST_LINK = copy "$(TargetPath)" $$(PICTO_TREE)\output\tests\bin\release
}

# Copy sounds directory to output directory
build_pass:CONFIG(debug, debug|release) {
  QMAKE_PRE_LINK = xcopy /Y $$(PICTO_TREE)\source\common\sounds\*.*  $$(PICTO_TREE)\output\bin\debug\sounds\\
}

build_pass:CONFIG(release, debug|release) {
  QMAKE_PRE_LINK = xcopy /Y $$(PICTO_TREE)\source\common\sounds\*.*  $$(PICTO_TREE)\output\bin\release\sounds\\
}