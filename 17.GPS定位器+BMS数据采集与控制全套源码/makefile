#******************************************************************************
#  Filename     :  Makefile
#  Project      :  
#  Language     :  
#  Description  :  
#  Edit History :  
#******************************************************************************

#*****************************************************************************
# Target name
#*****************************************************************************
TARGET = build\GmAppMain

#*****************************************************************************
# Link File Definitions
#*****************************************************************************
LINK_FILES   = 

DEBUG_SYMBOLS = 1
export DEBUG_SYMBOLS
#*****************************************************************************
# Source File Definitions
#*****************************************************************************
LIB_SRCS_ASM = app_start.s
include makefile_src.inc
include makefile.inc

ifeq ($(DEBUG_SYMBOLS), 1)
   GM_DEF += -D__GOOME_TRACE_DEBUG_ENABLE__=1 
else
   GM_DEF += -D__GOOME_TRACE_DEBUG_ENABLE__=0
endif

export APP_BUILD_TIME

GM_DEF += -DSW_APP_BUILD_DATE_TIME="\"$(APP_BUILD_TIME)"\"

#*****************************************************************************
# Inclue platfrom information
#*****************************************************************************
include makefile_plat.inc

PLATFORM_SYMBOL_FILE  = app_start.sym
PLATFORM_HEADER_FILE  = app_start.o


#*****************************************************************************
# Directory Definition
#*****************************************************************************
ALL_DIRS = $(SUB_DIRS) LIB

#*****************************************************************************
# Depedency Library Definition
#*****************************************************************************
ALL_LIBS =  

#*****************************************************************************
# Depedency Library full path
#*****************************************************************************
ALL_LIB_FILES =  $(addsuffix .lib, $(addprefix $(LIB_DIR)/,$(ALL_LIBS)))              
ALL_LINK_LIB_FILES  = $(ALL_LIB_FILES)




#*****************************************************************************
# Object creation
#*****************************************************************************
	
default: prepare $(ALL_DIRS) $(ALL_LIBS) $(TARGET).axf

$(TARGET).axf: $(ALL_OBJS) $(ALL_LIB_FILES)
	@copy $(LIB_DIR)\app_start.sym  $(OBJ_DIR)\app_start.sym
	
	
	$(LINK) -o $@ $(LINK_FLAGS) $(TARGET).map -entry Service_Entry -first $(PLATFORM_HEADER_FILE)\(ServiceHeader\)\
	        $(OBJ_DIR)/$(PLATFORM_SYMBOL_FILE) $(OBJ_DIR)/$(PLATFORM_HEADER_FILE) $(ALL_OBJS)\
	        $(ALL_LINK_LIB_FILES)
	        
	$(BIN_TOOL) $(TARGET).axf -bin -o $(TARGET).bin		
	
prepare:
	@if exist $(strip $(OBJ_DIR))\*.log \
		@del /f  $(strip $(OBJ_DIR))\*.log
	@echo ;Building Log > .\$(BUILDLOG)
       
$(ALL_DIRS):
	@$(MKDIR) $@
	

$(ALL_LIBS):
	 @$(MAKE)  -C prj/$@  

all_tools:
	 @$(MAKE)  -C $(TOOLS_DIR)


#*****************************************************************************
# Clean Rule  @del  build\*
#*****************************************************************************
.PHONY: clean
clean:
	@del  build\*.o
	@del  build\*.sym
	@del  build\*.ex
	@del  build\*.axf
	@del  build\*.bin
	@del  build\*.htm
	@del  build\*.map
	@del  build\*.log
	@del  output\*.bin
	@del  output\*.ex

.PHONY: clean_sub
clean_sub:
	 for D in $(ALL_LIBS); do cd prj/$$D ; $(MAKE) clean;cd ../..;done

.PHONY: clean_all
clean_all:
	$(MAKE) clean
	$(MAKE) clean_sub
#	$(MAKE) -C $(COMMON_DIR) clean

	
#
# make / clean dependency list
#

MAKEFILENAME = makefile
CFLAGSDEPEND = -MM  $(C_FLAGS)                     # for make depend
DEPENDENT_START = \# DO NOT EDIT AFTER THIS LINE -- make depend will generate it.

depend:
	$(MAKE) -f $(MAKEFILENAME)  clean_depend
	$(ECHO) "$(DEPENDENT_START)" >> $(MAKEFILENAME)
	$(CC) $(CFLAGSDEPEND) $(ALL_SRCS_C) | sed -e 's/^\(.*\)\.o/OBJ\/\1.o/g' >> $(MAKEFILENAME)

clean_depend:
	chmod u+w $(MAKEFILENAME)
	(awk 'BEGIN{f=1}\
	/^$(DEPENDENT_START)/{f=0} \
	{if (f) print $0}'\
	< $(MAKEFILENAME) > .depend && 	mv .depend $(MAKEFILENAME)) || exit 1;	
