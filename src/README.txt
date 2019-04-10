# List of dependencies of the four programs in the package

all four programs depend on the following source files
	netOnZeroDXC_algorithm.cpp, *.hpp		(Algorithm functions implementation)
	netOnZeroDXC_io.cpp, *.hpp			(Low-level I/O functions)
	netOnZeroDXC_pair.hpp				(Auxiliary data type)
	gsl/*.h						(GNU Scientific libraries headers)

all two GUI apps depend on the following source files
	netOnZeroDXC_gui_io.cpp, *.hpp			(Auxiliary I/O functions)
	netOnZeroDXC_gui_colors.cpp, *.hpp		(Graphical utilities)
	netOnZeroDXC_gui_icon.hpp			(Icons implemented as unsigned char array)
	wx/*.h						(wxWidgets library headers)

netOnZeroDXC_analysis
	netOnZeroDXC_analysis_main.cpp, *.hpp		(Class definitions, app implementation and initialization function)
	netOnZeroDXC_analysis_gui_layout.cpp		(Main window layout and functions)
	netOnZeroDXC_analysis_io.cpp			(Auxiliary I/O functions)
	netOnZeroDXC_analysis_gui_worker.cpp		(Analysis thread functions)
	netOnZeroDXC_analysis_gui_algorithm.cpp, *.hpp	(Auxiliary analysis functions)
	netOnZeroDXC_analysis_gui_preview.cpp		(Preview window layout and functions)

netOnZeroDXC_merge
	netOnZeroDXC_merge_main.cpp, *.hpp		(Class definitions, app implementation and initialization function)
	netOnZeroDXC_merge_gui_layout.cpp		(Main window layout and functions)
	netOnZeroDXC_merge_gui_manage.cpp		(Data management functions)
	netOnZeroDXC_merge_io.cpp			(Auxiliary I/O functions)
	netOnZeroDXC_merge_gui_preview.cpp		(Preview window layout and functions)

netOnZeroDXC_diagram
	netOnZeroDXC_diagram.cpp			(Main)

netOnZeroDXC_efficiency
	netOnZeroDXC_efficiency.cpp			(Main)
