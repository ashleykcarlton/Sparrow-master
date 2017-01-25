#############################################################################
#
# name: Makefile
# date: 29 Dec 15
# auth: Zach Hartwig
#
# desc: This file is the GNU makefile that controls the Sparrow build
#       system. Users beware that this is no ordinary Geant4 code! The
#       build system handles a number fancy maneuvers including: 
#
#       -- optional parallel build of Sparrow with Open MPI 
#       -- generating  dictionaries for data readout into ROOT framework 
#       -- inclusion Boost C++ library dependencies
#       -- inclusion of ADAQ simulation readout extension library
#
#       To build the simulation for sequential processing:
#         $ make
#
#       To build the simulation with Open MPI for parallel processing
#       (requires the ASIM libraries to be built with Open MPI):
#         $ make par
#
#       To cleanup the parallel transient build directory:
#         $ make parclean
#
# dpnd: 1. The ROOT toolkit (mandatory)
#       2. The Boost C+ libraries (mandatory)
#       3. ADAQ libraries (mandatory)
#       4. Open MPI (optional)
#
#############################################################################

# Define sequential (SEQ) processing as default
PROC := SEQ

# Set the target name based on seq/par processing. This enables a seq
# and par version of the binary to co-exist
ifeq ($(PROC),SEQ)
   name := Sparrow
else
   name := Sparrow_MPI
endif


##################
#  G4 Makefile  #
##################

# Set the G4TARGET variable. Note that the 'name' variable is required
# to set the name of the build directory in $G4INSTALL/tmp/$G4SYSTEM/

G4TARGET := $(name)
G4EXLIB := true

ifndef G4INSTALL
  G4INSTALL = ../../..
endif

 .PHONY: all
all: lib bin

ifeq ($(PROC),PAR)
  include $(G4INSTALL)/config/binmake.gmk#.NO_VISUALIZATION
else
  include $(G4INSTALL)/config/binmake.gmk
endif

# Prevent shadow warnings needed to suppress ROOT/Geant4 shadowing
# mainly of global variable "s". This may come back to bite me...
CXXFLAGS := $(subst -Wshadow,,$(CXXFLAGS))


##########
#  ROOT  #
##########

# Several ROOT classes are presently used in Sparrow for their immense
# utility; this requires compiling and linking against ROOT as a
# dependency. Use 'root-config' to obtain the header and library locations. 
ROOTINCLUDES = -I$(shell root-config --incdir)
ROOTDISTLIBS = $(shell root-config --nonew --libs --glibs)

CPPFLAGS += $(ROOTINCLUDES)
LDLIBS += $(ROOTDISTLIBS)


########
# ASIM #
########

# The ADAQ Simulation Readout (ASIM) libraries are used for readout of
# detector data into persistent ROOT simulation (*.asim.root) files
CPPFLAGS += -I$(ADAQHOME)/include
LDLIBS += -L$(ADAQHOME)/lib/$(HOSTTYPE) -lASIMReadout -lASIMStorage


#########################
#  Parallization / MPI  #
#########################

# Necessary flags to build Sparrow with MPI parallelization. Note that
# the user should have the environment variable MPIHOME set to point
# to the top-level Open MPI installation directory
ifeq ($(PROC),PAR)

  # Open MPI C++ compiler
  CXX := mpic++

  # Necessary flags for parallel compilation
  CPPFLAGS += -I$(MPIHOME)/include/ -DMPI_ENABLED
endif


################
# PHONY target #
################

.PHONY:

# Build Sparrow with Open MPI parallelization

# Get the number of processors
NPROC=$(nproc)

# Build in parallel
par:
	@ echo -e "\nBuilding Sparrow binary as Sparrow_MPI with Open MPI parallelization...\n"
	@cp Sparrow.cc Sparrow_MPI.cc
	@make PROC=PAR -j$(NPROC)
	@rm Sparrow_MPI.cc -f
	@ echo -e "\nThe Sparrow_MPI binary has been built with Open MPI parallelization!\n"

# Clean the parallel build transient files
parclean:
	@ echo -e "\nRemoving the Sparrow_MPI transient file directory...\n"
	@rm $(G4WORKDIR)/tmp/Linux-g++/Sparrow_MPI -rf
