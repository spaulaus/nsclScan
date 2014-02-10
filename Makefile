#!/bin/make
SHELL=/bin/bash

#If you want RooFit functionality uncomment the following line
#USEROOFIT = 1

#path for ddasdumper
DUMPER=/user/paulausk/programs/ddasdumper

#Define the virtual paths
vpath %.cpp src/
vpath %.hpp inc/
vpath %.o obj/

#Set some of the compile options
CXX = g++
CXXFLAGS = -fPIC -g -std=c++11 -Wall $(CINCLUDEDIRS)
LDLIBS = -ldl -lpthread -L$(DUMPER) -lddaschannel
CINCLUDEDIRS = -Iinc -I$(DUMPER)/
c++SrcSuf = cpp

#Set the name of the program to be compiled
PROGRAM = he6scan

#Define Objects
DETECTORLIBRARYO = DetectorLibrary.o
IDENTIFIERO      = Identifier.o
MAINO            = he6scan.o
MAPFILEO         = MapFile.o

#Make the object list and prefix the object directory
OBJS = $(DETECTORLIBRARYO) $(IDENTIFIERO) $(MAINO) $(MAPFILEO)

OBJDIR = obj
OBJS_W_DIR = $(addprefix $(OBJDIR)/,$(OBJS))

#Add the ROOT config stuff to the compilation
ROOTCONFIG   := root-config
CXXFLAGS     += $(shell $(ROOTCONFIG) --cflags)
LDFLAGS      += $(shell $(ROOTCONFIG) --ldflags)
LDLIBS       += $(shell $(ROOTCONFIG) --libs)
ifeq ($(USEROOFIT),1)
LDLIBS      += -lRooFit -lRooFitCore
endif
.SUFFIXES: .$(c++SrcSuf)

all: $(OBJS_W_DIR) $(PROGRAM)

$(OBJS_W_DIR): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(PROGRAM): $(OBJS_W_DIR)
	$(CXX) $(CXXFLAGS) $(LDLIBS) $^ -o $@

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean: 
	@echo "Cleaning..."
	@rm -f $(OBJDIR)/* $(PROGRAM) *~ src/*~ include/*~