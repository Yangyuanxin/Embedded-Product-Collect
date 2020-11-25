#******************************************************************************
#  Filename     :  Makefile
#  Project      :  General Project
#  Language     :  GUN makefile
#  Description  :  Created to define make platform
#  Edit History :  Created by goome at 2018-07-25
#******************************************************************************

#******************************************************************************
#PLATFORM
#******************************************************************************
ARMTOOL_PATH=C:\Progra~1\ARM\RVCT
#*****************************************************************************
# Make file MACRO definitions
#*****************************************************************************

DIR_ARM=$(strip $(ARMTOOL_PATH))

DIR_TOOL       =  $(DIR_ARM)\Programs\3.1\569\win_32-pentium
DIR_ARMLIB     =  $(DIR_ARM)\Data\3.1\569\lib
DIR_ARMINC     =  $(DIR_ARM)\Data\3.1\569\include\windows
DIR_TOOL      :=  $(strip $(DIR_TOOL))
CC32 		   =  $(DIR_TOOL)\armcc.exe --arm
CC	           =  $(DIR_TOOL)\armcc.exe --thumb
LINK           =  $(DIR_TOOL)\armlink.exe          # Linker
ASM            =  $(DIR_TOOL)\armasm.exe           # ARM assembler
AR             =  $(DIR_TOOL)\armar.exe            # Library tool
BIN_TOOL       =  $(DIR_TOOL)\fromelf.exe          # Binary tool
MAKE		   =  make.exe
