CURRENT_DIR := ${CURDIR}
INSTALL_DIR := /usr/bin
SOURCE_DIR := ../../../src

COMPILER := g++
CFLAGS := -fopenmp `gsl-config --cflags` -I$(SOURCE_DIR)
WXCFLAGS := `wx-config --cxxflags`
LIBFLAGS := `gsl-config --libs`
WXLIBFLAGS := `wx-config --libs`

SOURCE_GLOBAL_FUNCT := $(SOURCE_DIR)/netOnZeroDXC_io.cpp $(SOURCE_DIR)/netOnZeroDXC_algorithm.cpp
SOURCE_GLOBAL_GUI := $(SOURCE_DIR)/netOnZeroDXC_gui_colors.cpp $(SOURCE_DIR)/netOnZeroDXC_gui_io.cpp

SOURCE_APP_ANALYSIS := $(SOURCE_DIR)/netOnZeroDXC_analysis_main.cpp $(SOURCE_DIR)/netOnZeroDXC_analysis_gui_layout.cpp $(SOURCE_DIR)/netOnZeroDXC_analysis_io.cpp $(SOURCE_DIR)/netOnZeroDXC_analysis_gui_worker.cpp $(SOURCE_DIR)/netOnZeroDXC_analysis_gui_algorithm.cpp $(SOURCE_DIR)/netOnZeroDXC_analysis_gui_preview.cpp $(SOURCE_GLOBAL_FUNCT) $(SOURCE_GLOBAL_GUI)
SOURCE_APP_MERGE := $(SOURCE_DIR)/netOnZeroDXC_merge_main.cpp $(SOURCE_DIR)/netOnZeroDXC_merge_gui_layout.cpp $(SOURCE_DIR)/netOnZeroDXC_merge_gui_manage.cpp $(SOURCE_DIR)/netOnZeroDXC_merge_io.cpp $(SOURCE_DIR)/netOnZeroDXC_merge_gui_preview.cpp $(SOURCE_GLOBAL_FUNCT) $(SOURCE_GLOBAL_GUI)
SOURCE_CMD_CORR := $(SOURCE_DIR)/netOnZeroDXC_diagram.cpp $(SOURCE_GLOBAL_FUNCT)
SOURCE_CMD_EFF := $(SOURCE_DIR)/netOnZeroDXC_efficiency.cpp $(SOURCE_GLOBAL_FUNCT)


all: netOnZeroDXC_analysis netOnZeroDXC_merge netOnZeroDXC_diagram netOnZeroDXC_efficiency


netOnZeroDXC_analysis: $(SOURCE_APP_ANALYSIS)
	$(COMPILER) $(SOURCE_APP_ANALYSIS) -o netOnZeroDXC_analysis $(CFLAGS) $(LIBFLAGS) $(WXCFLAGS) $(WXLIBFLAGS)

netOnZeroDXC_merge: $(SOURCE_APP_MERGE)
	$(COMPILER) $(SOURCE_APP_MERGE) -o netOnZeroDXC_merge  $(CFLAGS) $(LIBFLAGS) $(WXCFLAGS) $(WXLIBFLAGS)

netOnZeroDXC_diagram: $(SOURCE_CMD_CORR)
	$(COMPILER) $(SOURCE_CMD_CORR) -o netOnZeroDXC_diagram $(CFLAGS) $(LIBFLAGS)

netOnZeroDXC_efficiency: $(SOURCE_CMD_EFF)
	$(COMPILER) $(SOURCE_CMD_EFF) -o netOnZeroDXC_efficiency $(CFLAGS) $(LIBFLAGS)


.PHONY: clean purge binlink bincopy

clean:
	rm -f netOnZeroDXC_analysis
	rm -f netOnZeroDXC_merge
	rm -f netOnZeroDXC_diagram
	rm -f netOnZeroDXC_efficiency

purge:
	sudo rm -f $(INSTALL_DIR)/netOnZeroDXC_analysis
	sudo rm -f $(INSTALL_DIR)/netOnZeroDXC_merge
	sudo rm -f $(INSTALL_DIR)/netOnZeroDXC_diagram
	sudo rm -f $(INSTALL_DIR)/netOnZeroDXC_efficiency


binlink:
	@sudo rm -f /usr/bin/netOnZeroDXC_analysis
	@sudo rm -f /usr/bin/netOnZeroDXC_merge
	@sudo rm -f /usr/bin/netOnZeroDXC_diagram
	@sudo rm -f /usr/bin/netOnZeroDXC_efficiency
	@sudo ln -sf $(CURRENT_DIR)/netOnZeroDXC_analysis $(INSTALL_DIR)
	@sudo ln -sf $(CURRENT_DIR)/netOnZeroDXC_merge $(INSTALL_DIR)
	@sudo ln -sf $(CURRENT_DIR)/netOnZeroDXC_diagram $(INSTALL_DIR)
	@sudo ln -sf $(CURRENT_DIR)/netOnZeroDXC_efficiency $(INSTALL_DIR)

bincopy:
	@sudo rm -f $(INSTALL_DIR)/netOnZeroDXC_analysis
	@sudo rm -f $(INSTALL_DIR)/netOnZeroDXC_merge
	@sudo rm -f $(INSTALL_DIR)/netOnZeroDXC_diagram
	@sudo rm -f $(INSTALL_DIR)/netOnZeroDXC_efficiency
	@if [ -f netOnZeroDXC_analysis ]; then sudo cp $(CURRENT_DIR)/netOnZeroDXC_analysis $(INSTALL_DIR); fi
	@if [ -f netOnZeroDXC_merge ]; then sudo cp $(CURRENT_DIR)/netOnZeroDXC_merge $(INSTALL_DIR); fi
	@if [ -f netOnZeroDXC_diagram ]; then sudo cp $(CURRENT_DIR)/netOnZeroDXC_diagram $(INSTALL_DIR); fi
	@if [ -f netOnZeroDXC_efficiency ]; then sudo cp $(CURRENT_DIR)/netOnZeroDXC_efficiency $(INSTALL_DIR); fi
