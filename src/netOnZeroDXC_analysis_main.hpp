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

#ifndef INCLUDED_PAIR
	#include "netOnZeroDXC_pair.hpp"
	#define INCLUDED_PAIR
#endif
#ifndef INCLUDED_ICON
	#include "netOnZeroDXC_gui_icon.hpp"
	#define INCLUDED_ICON
#endif

class MainApp;
class GuiFrame;
class ContainerWorkspace;
class WorkerThread;
class PlotFrame;
class PanelPlot;
class PanelColorbox;
class PlotFrame_WholeSeq;
class PanelPlot_WholeSeq;
class PanelColorbox_WholeSeq;

enum
{
	APP_QUIT = wxID_EXIT,
	APP_ABOUT = wxID_ABOUT,
	EVENT_WORKER_UPDATE = wxID_HIGHEST + 1,
	EVENT_BUTTON_FOLDER = wxID_HIGHEST + 2,
	EVENT_BUTTON_LOAD = wxID_HIGHEST + 3,
	EVENT_BUTTON_RUN = wxID_HIGHEST + 4,
	EVENT_CHOSEN_PATHWAY = wxID_HIGHEST + 5,
	EVENT_CHOSEN_MODE = wxID_HIGHEST + 6,
	EVENT_CHECKBOX_SHIFT = wxID_HIGHEST + 7,
	EVENT_BUTTON_PREVIEW = wxID_HIGHEST + 8,
	EVENT_SLIDER_THR_SGN = wxID_HIGHEST + 9,
	EVENT_SLIDER_THR_EFF = wxID_HIGHEST + 10,
	EVENT_CHOSEN_QUANTITY = wxID_HIGHEST + 11,
	EVENT_CHOSEN_PVALUEMODE = wxID_HIGHEST + 12,
	EVENT_PREPARE_PREVIEW = wxID_HIGHEST + 13
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
	bool workCancelled();
	void enablePreview();
	bool			m_cancelled;
	ContainerWorkspace	*m_workspace;

private:
	void onFrameQuit(wxCommandEvent&);
	void initializeConstants();
	void showAboutDialog(wxCommandEvent&);
	void onWorkerStart(wxCommandEvent&);
	void onWorkerEvent(wxThreadEvent&);
	void loadInputFiles(wxCommandEvent&);
	void loadOutputFolder(wxCommandEvent&);
	void onSelectPathway(wxCommandEvent&);
	void onSelectMode(wxCommandEvent&);
	void onSelectShiftCheckbox(wxCommandEvent&);
	void onSelectPvalueMode(wxCommandEvent&);
	void showInputControls(int);
	void showPathsAll();
	void showPathsEfficiency();
	void showPathsMatrix();
	void setReadyStatus(int);
	void setNotReadyStatus();
	int loadParameterTable();
	void onDatasetClear(wxCommandEvent&);
	void onPreparePreview(wxThreadEvent&);
	void popupPreview(wxCommandEvent&);

	bool		m_status_ready;

	wxSize		size_radiobox_pathway;
	wxSize		size_radiobox_mode;

	wxString	combobox_separators_element_0;
	wxString	combobox_separators_element_1;
	wxString	combobox_separators_element_2;
	wxString	combobox_delimiters_element_0;
	wxString	combobox_delimiters_element_1;
	wxString	combobox_delimiters_element_2;

	char		combobox_delimiters_char_0;
	char		combobox_delimiters_char_1;
	char		combobox_delimiters_char_2;
	char		combobox_separators_char_0;
	char		combobox_separators_char_1;
	char		combobox_separators_char_2;

	std::string	description_mode_00;
	std::string	description_mode_10;
	std::string	description_mode_20;
	std::string	description_mode_30;
	std::string	description_mode_22;
	std::string	description_mode_32;
	std::string	description_mode_33;

	wxProgressDialog	*dialog_progress;

	wxRadioBox		*radiobox_selectpathway;
	wxRadioBox		*radiobox_mode;
	wxRadioBox		*radiobox_pvalue_mode;
	wxButton		*button_runworker;
	wxButton		*button_openmanyfiles;
	wxButton		*button_outputfolder;
	wxButton		*button_clear_stored;
	wxButton		*button_preview;
	wxComboBox 		*combobox_separators;
	wxComboBox 		*combobox_delimiters;

	wxSpinCtrl		*spinner_columnr;
	wxSpinCtrl		*spinner_basewidth;
	wxSpinCtrl		*spinner_nr_windowwidths;
	wxSpinCtrl		*spinner_nr_surrogates;
	wxSpinCtrl		*spinner_source_leakage;
	wxSpinCtrl		*spinner_threadnum;
	wxSpinCtrlDouble	*spinner_sampling_period;
	wxSpinCtrlDouble	*spinner_thr_significance;
	wxSpinCtrlDouble	*spinner_thr_efficiency;

	wxCheckBox		*checkbox_source_leakage;
	wxCheckBox		*checkbox_avoid_overlapping;
	wxCheckBox		*checkbox_save_cdiagrams;
	wxCheckBox		*checkbox_save_pdiagrams;
	wxCheckBox		*checkbox_save_efficiencies;
	wxCheckBox		*checkbox_save_wholeseq_xcorr;
	wxCheckBox		*checkbox_parallel_omp;

	wxTextCtrl		*textctrl_save_prefix;

	wxStaticLine		*staticline_title_input;
	wxStaticLine		*staticline_title_output;
	wxStaticText		*statictext_title_input;
	wxStaticText		*statictext_title_output;
	wxStaticText		*statictext_header_separators;
	wxStaticText		*statictext_header_delimiters;
	wxStaticText		*statictext_header_columnr;
	wxStaticText		*statictext_inputfile;
	wxStaticText		*statictext_outputfolder;
	wxStaticText		*statictext_sampling_period;
	wxStaticText		*statictext_basewidth;
	wxStaticText		*statictext_nr_windowwidths;
	wxStaticText		*statictext_nr_surrogates;
	wxStaticText		*statictext_thr_significance;
	wxStaticText		*statictext_thr_efficiency;
	wxStaticText		*statictext_source_leakage;
	wxStaticText		*statictext_save_header;
	wxStaticText		*statictext_save_prefix;
	wxStaticText		*statictext_threadnum;
	wxStaticLine		*staticline_run;
	wxStaticLine		*staticline_parameters;

	wxCriticalSection	m_cs_cancelled;

	wxDECLARE_EVENT_TABLE();
};

class ContainerWorkspace
{
public:
	ContainerWorkspace();
	void clearWorkspace();
	int validateParameterTable();

	int	parameter_computation_pathway;
	int	parameter_computation_target;
	int	parameter_basewidth;
	int	parameter_nr_windowwidths;
	int	parameter_nr_surrogates;
	int	parameter_shift_value;
	double	parameter_samplingperiod;
	double	parameter_thr_significance;
	double	parameter_thr_efficiency;
	bool	parameter_pvalue_by_surrogate;
	bool	parameter_use_shift;
	bool	parameter_overlapping_windows;
	bool	parameter_print_cdiagrams;
	bool	parameter_print_pdiagrams;
	bool	parameter_print_efficiencies;
	bool	parameter_print_wholeseq_xcorr;

	bool	parameter_use_parallel;
	int	parameter_numthreads;

	std::vector < std::vector <double> >			sequences;
	std::vector < std::vector < std::vector <double> > >	diagrams_correlation;
	std::vector < std::vector < std::vector <double> > >	diagrams_pvalue;
	std::vector < std::vector < std::vector <double> > >	diagrams_pvalue_fisher;
	std::vector < std::vector <double> >			efficiencies;
	std::vector <double>					window_widths;
	std::vector <std::string>				node_labels;
	std::vector <bool>					node_valid;
	std::vector <PairOfLabels>				node_pairs;
	std::vector <bool>					node_pairs_valid;
	std::vector < std::vector <double> >			wholeseq_xcorr;
	std::vector < std::vector <double> >			wholeseq_pvalue;

	std::vector < std::vector < std::vector <double> > >			matrices_multieta;
	std::vector < std::vector < std::vector <double> > >			efficiencies_multialpha;
	std::vector < std::vector < std::vector < std::vector <double> > > >	matrices_multieta_multialpha;

	char		path_filename_delimiter;
	std::string	path_output_folder;
	std::string	path_output_prefix;
};

class WorkerThread : public wxThread
{
public:
	WorkerThread(GuiFrame *frame);

	virtual void *Entry();
	virtual void OnExit();

	ContainerWorkspace	*data_container;
	GuiFrame		*parent_frame;

};

class PlotFrame : public wxFrame
{
public:
	PlotFrame(const wxString&, GuiFrame *);
	void OnSlide(wxCommandEvent&);

	ContainerWorkspace	*results_workspace;
	wxSlider		*slider_thr_significance;
	wxStaticText		*statictext_slider_thr_significance;
	wxSlider		*slider_thr_efficiency;
	wxStaticText		*statictext_slider_thr_efficiency;

private:
	PanelPlot		*plot_area;
	PanelColorbox		*colorbox_area;

	wxDECLARE_EVENT_TABLE();
};

class PanelPlot : public wxPanel
{
public:
	PanelPlot(PlotFrame *, wxSize);
	void OnPaint(wxPaintEvent&);
	void OnResize(wxSizeEvent&);

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


class PlotFrame_WholeSeq : public wxFrame
{
public:
	PlotFrame_WholeSeq(const wxString&, GuiFrame *);
	void OnSelectQuantity(wxCommandEvent&);

	ContainerWorkspace	*results_workspace;
	wxRadioBox		*radiobox_xcorr_pvalue;

private:
	PanelPlot_WholeSeq		*plot_area;
	PanelColorbox_WholeSeq		*colorbox_area;

	wxDECLARE_EVENT_TABLE();
};

class PanelPlot_WholeSeq : public wxPanel
{
public:
	PanelPlot_WholeSeq(PlotFrame_WholeSeq *, wxSize);
	void OnPaint(wxPaintEvent&);
	void OnResize(wxSizeEvent&);

private:
	PlotFrame_WholeSeq	*parent_frame;
	ContainerWorkspace	*results_workspace;

	wxDECLARE_EVENT_TABLE();
};

class PanelColorbox_WholeSeq : public wxPanel
{
public:
	PanelColorbox_WholeSeq(PlotFrame_WholeSeq *, wxSize);
	void OnPaint(wxPaintEvent&);
	void OnResize(wxSizeEvent&);

private:
	PlotFrame_WholeSeq	*parent_frame;

	wxDECLARE_EVENT_TABLE();
};
