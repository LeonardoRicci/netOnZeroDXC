// --------------------------------------------------------------------------
//
// This file is part of the NetOnZeroDXC software package.
//
// Version 1.1 - July 2019
//
//
// The NetOnZeroDXC package is free software; you can use it, redistribute it,
// and/or modify it under the terms of the GNU General Public License
// version 3 as published by the Free Software Foundation. The full text
// of the license can be found in the file LICENSE.txt at the top level of
// the package distribution.
//
// Authors:
//		Alessio Perinelli and Leonardo Ricci
//		Department of Physics, University of Trento
//		I-38123 Trento, Italy
//		alessio.perinelli@unitn.it
//		leonardo.ricci@unitn.it
//		https://github.com/LeonardoRicci/netOnZeroDXC
//
//
// If you use the NetOnZeroDXC package for your analyses, please cite:
//
//	A. Perinelli, D. E. Chiari and L. Ricci,
//	"Correlation in brain networks at different time scale resolution".
//	Chaos 28 (6):063127, 2018
//
// --------------------------------------------------------------------------

#include <vector>
#include <string>

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif // WX_PRECOMP
#include "wx/thread.h"
#include "wx/button.h"
#include "wx/numdlg.h"
#include "wx/msgdlg.h"
#include "wx/progdlg.h"
#include "wx/dirdlg.h"
#include "wx/wfstream.h"
#include "wx/gdicmn.h"
#include "wx/combobox.h"
#include "wx/arrstr.h"
#include "wx/radiobox.h"
#include "wx/radiobut.h"
#include "wx/bmpbuttn.h"
#include "wx/stattext.h"
#include "wx/statline.h"
#include "wx/spinctrl.h"
#include "wx/aboutdlg.h"

#ifndef INCLUDED_COLORS
	#include "netOnZeroDXC_gui_colors.hpp"
	#define INCLUDED_COLORS
#endif
#ifndef INCLUDED_PAIR
	#include "netOnZeroDXC_pair.hpp"
	#define INCLUDED_PAIR
#endif


class MainApp;
class GuiFrame;
class ObservedSystem;
class ObservedRecording;
class ContainerWorkspace;
class PlotFrame;
class PanelPlot;
class PanelColorbox;

enum
{
	APP_QUIT = wxID_EXIT,
	APP_ABOUT = wxID_ABOUT,
	EVENT_WORKER_UPDATE = wxID_HIGHEST + 1,
	EVENT_BUTTON_SYSADD = wxID_HIGHEST + 2,
	EVENT_BUTTON_RECADD = wxID_HIGHEST + 3,
	EVENT_LISTBOX_SELECTION = wxID_HIGHEST + 4,
	EVENT_BUTTON_LOADQ = wxID_HIGHEST + 5,
	EVENT_BUTTON_LOADC = wxID_HIGHEST + 6,
	EVENT_BUTTON_LOADM = wxID_HIGHEST + 7,
	EVENT_BUTTON_SYSDEL = wxID_HIGHEST + 8,
	EVENT_BUTTON_RECDEL = wxID_HIGHEST + 9,
	EVENT_BUTTON_CLEAR = wxID_HIGHEST + 10,
	EVENT_BUTTON_LOADL = wxID_HIGHEST + 11,
	EVENT_BUTTON_INFO = wxID_HIGHEST + 12,
	EVENT_BUTTON_PREPROC = wxID_HIGHEST + 13,
	EVENT_BUTTON_MERGEREC = wxID_HIGHEST + 14,
	EVENT_BUTTON_MERGEALL = wxID_HIGHEST + 15,
	EVENT_SLIDER_THR_EFF = wxID_HIGHEST + 16,
	EVENT_SLIDER_RNK_REC = wxID_HIGHEST + 17,
	EVENT_SLIDER_RNK_SYS = wxID_HIGHEST + 18,
	EVENT_BUTTON_SAVESYS = wxID_HIGHEST + 19,
	EVENT_BUTTON_SAVEALL = wxID_HIGHEST + 20
};

class MainApp : public wxApp
{
public:
	MainApp();
	virtual ~MainApp(){}
	virtual bool OnInit() ;

};

class GuiFrame : public wxFrame
{
public:
	GuiFrame(const wxString& title);
	~GuiFrame();
	int getRecordingIndex();
	int getSystemIndex();
	void updateSpinnerFromSliders(double, int, int);

	ContainerWorkspace	*data_container;
	wxSpinCtrlDouble	*spinner_thr_efficiency;
	wxSpinCtrl		*spinner_ranking_recordings;
	wxSpinCtrl		*spinner_ranking_systems;

private:
	void onFrameQuit(wxCommandEvent&);
	void showAboutDialog(wxCommandEvent&);
	void initializeConstants();
	void addObservedSystem(wxCommandEvent&);
	void addObservedRecording(wxCommandEvent&);
	void deleteObservedSystem(wxCommandEvent&);
	void deleteObservedRecording(wxCommandEvent&);
	void refreshListboxRecordings(wxCommandEvent&);
	void refreshListboxSystems(wxCommandEvent&);
	void loadEfficiencyFiles(int, int);
	void loadMatrixFiles(int);
	void configuredLoadFiles(wxCommandEvent&);
	void loadLabelsDictionary(wxCommandEvent&);
	void deleteAllData(wxCommandEvent&);
	void displayRecordingInfo(wxCommandEvent&);
	void validatePreprocessData(wxCommandEvent&);
	void setReadyStatus();
	void unsetReadyStatus();
	void popupPreviewBySystem(wxCommandEvent&);
	void popupPreviewGlobal(wxCommandEvent&);
	void saveMergedSystems(wxCommandEvent&);
	void saveMergedGlobal(wxCommandEvent&);

	int loadConfiguredRecording(ObservedRecording &, std::vector<PairOfLabels> &, bool, const std::string &, const std::string &, char, char);

	bool			status_ready;

	wxListBox		*listbox_systems;
	wxListBox		*listbox_recordings;
	// wxButton		*button_load_quick;
	wxButton		*button_load_confg;
	// wxButton		*button_load_manual;
	wxButton		*button_system_add;
	wxButton		*button_recording_add;
	wxButton		*button_system_del;
	wxButton		*button_recording_del;
	wxButton		*button_dictionary_load;
	wxButton		*button_clear_all;
	wxButton		*button_info;
	wxButton		*button_validate_preproc;
	wxButton		*button_merge_recordings;
	wxButton		*button_merge_global;
	wxButton		*button_save_sytems;
	wxButton		*button_save_global;
	wxComboBox		*combobox_delimiters;
	wxComboBox		*combobox_separators;
	wxRadioBox		*radiobox_input;

	wxStaticText		*statictext_header_loading;
	wxStaticText		*statictext_delimiters;
	wxStaticText		*statictext_separators;
	wxStaticText		*statictext_thr_efficiency;
	wxStaticText		*statictext_ranking_recordings;
	wxStaticText		*statictext_ranking_systems;

	wxString	combobox_separators_element_0;
	wxString	combobox_separators_element_1;
	wxString	combobox_separators_element_2;
	wxString	combobox_delimiters_element_0;
	wxString	combobox_delimiters_element_1;

	char		combobox_separators_char_0;
	char		combobox_separators_char_1;
	char		combobox_separators_char_2;
	char		combobox_delimiters_char_0;
	char		combobox_delimiters_char_1;

	wxDECLARE_EVENT_TABLE();
};

class ObservedSystem
{
public:
	ObservedSystem(const std::string);
	void addObservedRecording(ObservedRecording);
	double mergeRecordingMatrices(int, int, int, int);

	std::string	system_name;
	std::vector <ObservedRecording>		recordings_stored;
};

class ObservedRecording
{
public:
	ObservedRecording(const std::string);

	std::string	recording_name;
	std::vector < std::vector <double> >			matrix_timescales;
	std::vector < std::vector < std::vector <double> > >	matrices_multieta;
	std::vector < std::vector <double> >	efficiencies;
	std::vector <double>			window_widths;
};


class ContainerWorkspace
{
public:
	ContainerWorkspace();
	void clearWorkspace();
	void evaluateMergedMatrix(int, int, int);
	void evaluateSystemMatrix(int, int, int);

	bool	multiple_eta;
	bool	available_node_labels;

	int	number_of_systems;
	int	number_of_nodes;
	int	number_of_recordings;

	std::vector <ObservedSystem>	systems_stored;
	std::vector <std::string>	node_labels;
	std::vector <PairOfLabels>	node_pairs;
	std::vector <double>		window_widths;
	std::vector < std::vector <double> >	ranked_matrix;
};

class PlotFrame : public wxFrame
{
public:
	PlotFrame(const wxString&, GuiFrame *, bool);
	void OnSlide(wxCommandEvent&);

	bool			preview_all;
	bool			variable_eta;
	int			system_index;
	ContainerWorkspace	*results_workspace;

	wxSlider		*slider_thr_efficiency;
	wxStaticText		*statictext_slider_thr_efficiency;
	wxSlider		*slider_rnk_recordings;
	wxStaticText		*statictext_slider_rnk_recordings;
	wxSlider		*slider_rnk_systems;
	wxStaticText		*statictext_slider_rnk_systems;

private:
	PanelPlot		*plot_area;
	PanelColorbox		*colorbox_area;
	GuiFrame		*parent_window;

	wxDECLARE_EVENT_TABLE();
};

class PanelPlot : public wxPanel
{
public:
	PanelPlot(PlotFrame *, wxSize);
	void OnPaint(wxPaintEvent&);
	void OnResize(wxSizeEvent &);

private:
	PlotFrame		*parent_frame;
	ContainerWorkspace	*results_workspace;

	wxDECLARE_EVENT_TABLE();
};

class PanelColorbox : public wxPanel
{
public:
	PanelColorbox(PlotFrame *, wxSize);
	void OnPaint(wxPaintEvent&);
	void OnResize(wxSizeEvent&);

private:
	PlotFrame		*parent_frame;

	wxDECLARE_EVENT_TABLE();
};
