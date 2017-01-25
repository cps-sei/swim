#
# OMNeT++/OMNEST Makefile for swim
#
# This file was generated with the command:
#  opp_makemake -f --deep -o swim -Xmanagers/monitor/notUsed -I../queueinglib -Isrc/model/pladaptMock -I../../queueinglib -L/home/gmoreno/research/plasa/code/plasasim/libs -L/home/gmoreno/research/plasa/code/queueinglib/out/gcc-debug -lqueueinglib -lboost_serialization -lboost_system -lboost_filesystem -lpthread -KQUEUEINGLIB_PROJ=../../queueinglib -- -lboost_system
#

# Name of target to be created (-o option)
TARGET = swim$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(QTENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)
#USERIF_LIBS = $(QTENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I../queueinglib \
    -Isrc/model/pladaptMock \
    -I../../queueinglib \
    -I. \
    -IexternalControl \
    -Imanagers \
    -Imanagers/adaptation \
    -Imanagers/execution \
    -Imanagers/monitor \
    -Imodel \
    -Imodel/pladaptMock \
    -Imodel/pladaptMock/pladapt \
    -Imodules \
    -Iutil

# Additional object and library files to link with
EXTRA_OBJS = -lboost_system

# Additional libraries (-L, -l options)
LIBS = -L../libs -L$(QUEUEINGLIB_PROJ)/out/gcc-debug  -lqueueinglib -lboost_serialization -lboost_system -lboost_filesystem -lpthread
LIBS += -Wl,-rpath,`abspath ../libs` -Wl,-rpath,`abspath $(QUEUEINGLIB_PROJ)/out/gcc-debug`

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/externalControl/AdaptInterface.o \
    $O/managers/adaptation/UtilityScorer.o \
    $O/managers/execution/ExecutionManagerMod.o \
    $O/managers/execution/ExecutionManagerHAProxy.o \
    $O/managers/execution/ExecutionManagerModBase.o \
    $O/managers/monitor/SimpleMonitor.o \
    $O/managers/monitor/HAProxyProbe.o \
    $O/managers/monitor/IProbe.o \
    $O/managers/monitor/SimProbe.o \
    $O/model/Configuration.o \
    $O/model/Observations.o \
    $O/model/Environment.o \
    $O/model/Model.o \
    $O/modules/MTBrownoutServer.o \
    $O/modules/MTServer.o \
    $O/modules/PredictableRateSource.o \
    $O/modules/PassiveQueueDyn.o \
    $O/modules/ArrivalMonitor.o \
    $O/modules/PredictableRandomSource.o \
    $O/modules/PredictableSource.o \
    $O/util/TimeWindowStats.o \
    $O/util/MMcQueue.o \
    $O/util/ServerUtilization.o \
    $O/util/GMcQueue.o \
    $O/util/Utils.o \
    $O/util/HAProxySocketCommand.o \
    $O/managers/execution/BootComplete_m.o

# Message files
MSGFILES = \
    managers/execution/BootComplete.msg

# SM files
SMFILES =

# Other makefile variables (-K)
QUEUEINGLIB_PROJ=../../queueinglib

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)
SMCOPTS =

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# inserted from file 'makefrag':
LDFLAGS+=-Wl,-rpath=\$$ORIGIN/../libs -rdynamic

# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(Q)$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders smheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc $(MSGCOPTS) $?

%_sm.cc %_sm.h: %.sm
	$(qecho) SMC: $<
	$(Q)$(SMC) -c++ -suffix cc $(SMCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

smheaders: $(SMFILES:.sm=_sm.h)

clean:
	$(qecho) Cleaning...
	$(Q)-rm -rf $O
	$(Q)-rm -f swim swim.exe libswim.so libswim.a libswim.dll libswim.dylib
	$(Q)-rm -f ./*_m.cc ./*_m.h ./*_sm.cc ./*_sm.h
	$(Q)-rm -f externalControl/*_m.cc externalControl/*_m.h externalControl/*_sm.cc externalControl/*_sm.h
	$(Q)-rm -f managers/*_m.cc managers/*_m.h managers/*_sm.cc managers/*_sm.h
	$(Q)-rm -f managers/adaptation/*_m.cc managers/adaptation/*_m.h managers/adaptation/*_sm.cc managers/adaptation/*_sm.h
	$(Q)-rm -f managers/execution/*_m.cc managers/execution/*_m.h managers/execution/*_sm.cc managers/execution/*_sm.h
	$(Q)-rm -f managers/monitor/*_m.cc managers/monitor/*_m.h managers/monitor/*_sm.cc managers/monitor/*_sm.h
	$(Q)-rm -f model/*_m.cc model/*_m.h model/*_sm.cc model/*_sm.h
	$(Q)-rm -f model/pladaptMock/*_m.cc model/pladaptMock/*_m.h model/pladaptMock/*_sm.cc model/pladaptMock/*_sm.h
	$(Q)-rm -f model/pladaptMock/pladapt/*_m.cc model/pladaptMock/pladapt/*_m.h model/pladaptMock/pladapt/*_sm.cc model/pladaptMock/pladapt/*_sm.h
	$(Q)-rm -f modules/*_m.cc modules/*_m.h modules/*_sm.cc modules/*_sm.h
	$(Q)-rm -f util/*_m.cc util/*_m.h util/*_sm.cc util/*_sm.h

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES) $(SM_CC_FILES)  ./*.cc externalControl/*.cc managers/*.cc managers/adaptation/*.cc managers/execution/*.cc managers/monitor/*.cc model/*.cc model/pladaptMock/*.cc model/pladaptMock/pladapt/*.cc modules/*.cc util/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/externalControl/AdaptInterface.o: externalControl/AdaptInterface.cc \
	externalControl/AdaptInterface.h \
	managers/execution/BootComplete_m.h \
	managers/execution/ExecutionManager.h \
	managers/execution/ExecutionManagerMod.h \
	managers/execution/ExecutionManagerModBase.h \
	managers/monitor/IProbe.h \
	model/Configuration.h \
	model/Environment.h \
	model/Model.h \
	model/Observations.h \
	model/pladaptMock/pladapt/Configuration.h \
	model/pladaptMock/pladapt/Environment.h \
	$(QUEUEINGLIB_PROJ)/SocketRTScheduler.h
$O/managers/adaptation/UtilityScorer.o: managers/adaptation/UtilityScorer.cc \
	managers/adaptation/UtilityScorer.h \
	model/Configuration.h \
	model/Environment.h \
	model/Model.h \
	model/Observations.h \
	model/pladaptMock/pladapt/Configuration.h \
	model/pladaptMock/pladapt/Environment.h
$O/managers/execution/BootComplete_m.o: managers/execution/BootComplete_m.cc \
	managers/execution/BootComplete_m.h
$O/managers/execution/ExecutionManagerHAProxy.o: managers/execution/ExecutionManagerHAProxy.cc \
	managers/execution/BootComplete_m.h \
	managers/execution/ExecutionManager.h \
	managers/execution/ExecutionManagerHAProxy.h \
	managers/execution/ExecutionManagerModBase.h \
	model/Configuration.h \
	model/Environment.h \
	model/Model.h \
	model/Observations.h \
	model/pladaptMock/pladapt/Configuration.h \
	model/pladaptMock/pladapt/Environment.h \
	util/HAProxySocketCommand.h \
	util/Utils.h
$O/managers/execution/ExecutionManagerMod.o: managers/execution/ExecutionManagerMod.cc \
	managers/execution/BootComplete_m.h \
	managers/execution/ExecutionManager.h \
	managers/execution/ExecutionManagerMod.h \
	managers/execution/ExecutionManagerModBase.h \
	model/Configuration.h \
	model/Environment.h \
	model/Model.h \
	model/Observations.h \
	model/pladaptMock/pladapt/Configuration.h \
	model/pladaptMock/pladapt/Environment.h \
	modules/MTBrownoutServer.h \
	modules/MTServer.h \
	util/Utils.h \
	$(QUEUEINGLIB_PROJ)/IPassiveQueue.h \
	$(QUEUEINGLIB_PROJ)/IServer.h \
	$(QUEUEINGLIB_PROJ)/PassiveQueue.h \
	$(QUEUEINGLIB_PROJ)/QueueingDefs.h \
	$(QUEUEINGLIB_PROJ)/SelectionStrategies.h
$O/managers/execution/ExecutionManagerModBase.o: managers/execution/ExecutionManagerModBase.cc \
	managers/execution/BootComplete_m.h \
	managers/execution/ExecutionManager.h \
	managers/execution/ExecutionManagerModBase.h \
	model/Configuration.h \
	model/Environment.h \
	model/Model.h \
	model/Observations.h \
	model/pladaptMock/pladapt/Configuration.h \
	model/pladaptMock/pladapt/Environment.h
$O/managers/monitor/HAProxyProbe.o: managers/monitor/HAProxyProbe.cc \
	managers/ModulePriorities.h \
	managers/execution/BootComplete_m.h \
	managers/execution/ExecutionManager.h \
	managers/execution/ExecutionManagerModBase.h \
	managers/monitor/HAProxyProbe.h \
	managers/monitor/IProbe.h \
	model/Configuration.h \
	model/Environment.h \
	model/Model.h \
	model/Observations.h \
	model/pladaptMock/pladapt/Configuration.h \
	model/pladaptMock/pladapt/Environment.h \
	util/HAProxySocketCommand.h
$O/managers/monitor/IProbe.o: managers/monitor/IProbe.cc \
	managers/monitor/IProbe.h \
	model/Environment.h \
	model/Observations.h \
	model/pladaptMock/pladapt/Environment.h
$O/managers/monitor/SimProbe.o: managers/monitor/SimProbe.cc \
	managers/execution/BootComplete_m.h \
	managers/execution/ExecutionManager.h \
	managers/execution/ExecutionManagerModBase.h \
	managers/monitor/IProbe.h \
	managers/monitor/SimProbe.h \
	model/Configuration.h \
	model/Environment.h \
	model/Model.h \
	model/Observations.h \
	model/pladaptMock/pladapt/Configuration.h \
	model/pladaptMock/pladapt/Environment.h \
	util/TimeWindowStats.h
$O/managers/monitor/SimpleMonitor.o: managers/monitor/SimpleMonitor.cc \
	managers/ModulePriorities.h \
	managers/adaptation/UtilityScorer.h \
	managers/execution/BootComplete_m.h \
	managers/execution/ExecutionManager.h \
	managers/execution/ExecutionManagerModBase.h \
	managers/monitor/IProbe.h \
	managers/monitor/SimpleMonitor.h \
	model/Configuration.h \
	model/Environment.h \
	model/Model.h \
	model/Observations.h \
	model/pladaptMock/pladapt/Configuration.h \
	model/pladaptMock/pladapt/Environment.h
$O/model/Configuration.o: model/Configuration.cc \
	model/Configuration.h \
	model/pladaptMock/pladapt/Configuration.h
$O/model/Environment.o: model/Environment.cc \
	model/Environment.h \
	model/pladaptMock/pladapt/Environment.h
$O/model/Model.o: model/Model.cc \
	managers/execution/BootComplete_m.h \
	managers/execution/ExecutionManager.h \
	managers/execution/ExecutionManagerModBase.h \
	model/Configuration.h \
	model/Environment.h \
	model/Model.h \
	model/Observations.h \
	model/pladaptMock/pladapt/Configuration.h \
	model/pladaptMock/pladapt/Environment.h \
	modules/PredictableRandomSource.h \
	modules/PredictableSource.h \
	util/Utils.h \
	$(QUEUEINGLIB_PROJ)/QueueingDefs.h \
	$(QUEUEINGLIB_PROJ)/Source.h
$O/model/Observations.o: model/Observations.cc \
	model/Observations.h
$O/modules/ArrivalMonitor.o: modules/ArrivalMonitor.cc \
	modules/ArrivalMonitor.h
$O/modules/MTBrownoutServer.o: modules/MTBrownoutServer.cc \
	modules/MTBrownoutServer.h \
	modules/MTServer.h \
	$(QUEUEINGLIB_PROJ)/IServer.h \
	$(QUEUEINGLIB_PROJ)/Job.h \
	$(QUEUEINGLIB_PROJ)/Job_m.h \
	$(QUEUEINGLIB_PROJ)/QueueingDefs.h
$O/modules/MTServer.o: modules/MTServer.cc \
	modules/MTServer.h \
	$(QUEUEINGLIB_PROJ)/IPassiveQueue.h \
	$(QUEUEINGLIB_PROJ)/IServer.h \
	$(QUEUEINGLIB_PROJ)/Job.h \
	$(QUEUEINGLIB_PROJ)/Job_m.h \
	$(QUEUEINGLIB_PROJ)/QueueingDefs.h \
	$(QUEUEINGLIB_PROJ)/SelectionStrategies.h
$O/modules/PassiveQueueDyn.o: modules/PassiveQueueDyn.cc \
	modules/PassiveQueueDyn.h \
	$(QUEUEINGLIB_PROJ)/IPassiveQueue.h \
	$(QUEUEINGLIB_PROJ)/PassiveQueue.h \
	$(QUEUEINGLIB_PROJ)/QueueingDefs.h \
	$(QUEUEINGLIB_PROJ)/SelectionStrategies.h
$O/modules/PredictableRandomSource.o: modules/PredictableRandomSource.cc \
	modules/PredictableRandomSource.h \
	modules/PredictableSource.h \
	$(QUEUEINGLIB_PROJ)/QueueingDefs.h \
	$(QUEUEINGLIB_PROJ)/Source.h
$O/modules/PredictableRateSource.o: modules/PredictableRateSource.cc \
	modules/PredictableRateSource.h \
	modules/PredictableSource.h \
	$(QUEUEINGLIB_PROJ)/QueueingDefs.h \
	$(QUEUEINGLIB_PROJ)/Source.h
$O/modules/PredictableSource.o: modules/PredictableSource.cc \
	modules/PredictableSource.h \
	$(QUEUEINGLIB_PROJ)/Job.h \
	$(QUEUEINGLIB_PROJ)/Job_m.h \
	$(QUEUEINGLIB_PROJ)/QueueingDefs.h \
	$(QUEUEINGLIB_PROJ)/Source.h
$O/util/GMcQueue.o: util/GMcQueue.cc \
	util/GMcQueue.h
$O/util/HAProxySocketCommand.o: util/HAProxySocketCommand.cc \
	util/HAProxySocketCommand.h
$O/util/MMcQueue.o: util/MMcQueue.cc \
	util/MMcQueue.h
$O/util/ServerUtilization.o: util/ServerUtilization.cc \
	util/ServerUtilization.h
$O/util/TimeWindowStats.o: util/TimeWindowStats.cc \
	util/TimeWindowStats.h
$O/util/Utils.o: util/Utils.cc \
	util/Utils.h

