// --------------------------------------------------------------------------
//
// This file is part of the NetOnZeroDXC software package.
//
// Version 1.0 - April 2019
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

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

#ifndef INCLUDED_MAINAPP
	#include "netOnZeroDXC_analysis_main.hpp"
	#define INCLUDED_MAINAPP
#endif

// Event table for the binding of some events
wxBEGIN_EVENT_TABLE(GuiFrame, wxFrame)
	EVT_MENU(APP_ABOUT, GuiFrame::showAboutDialog)
	EVT_MENU(APP_QUIT, GuiFrame::onFrameQuit)
	EVT_THREAD(EVENT_WORKER_UPDATE, GuiFrame::onWorkerEvent)
wxEND_EVENT_TABLE()

// My frame constructor: this contains the initialization of all buttons, controls and boxes within the main window.
GuiFrame::GuiFrame (const wxString& title)
: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE)
{
	initializeConstants();
	wxFont font_bold(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,  wxFONTWEIGHT_BOLD);

	// The menu bar
	wxMenuBar *menuBar = new wxMenuBar;
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(APP_ABOUT, "A&bout\tCtrl-A");
	menuFile->AppendSeparator();
	menuFile->Append(APP_QUIT, "E&xit\tCtrl-Q");
	menuBar->Append(menuFile, "&File");
	SetMenuBar(menuBar);

	wxBitmap	temp_png = wxBITMAP_PNG_FROM_DATA(bin_icon_32);
	wxIcon		temp_icon;
	temp_icon.CopyFromBitmap(temp_png);
	this->SetIcon(temp_icon);

	// Titles
	statictext_title_input = new wxStaticText(this, wxID_ANY, wxT("Input/output settings"), wxDefaultPosition, wxDefaultSize, 0);
	statictext_title_output = new wxStaticText(this, wxID_ANY, wxT("Configuration"), wxDefaultPosition, wxDefaultSize, 0);
	statictext_title_input->SetFont(font_bold);
	statictext_title_output->SetFont(font_bold);
	staticline_title_input = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(-1,1));
	staticline_title_output = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(-1,1));

	// Choice of the computation pathway
	wxArrayString	m_list_of_pathways;
	m_list_of_pathways.Add(wxT("Sequences: single file, N columns"));
	m_list_of_pathways.Add(wxT("Sequences: N files, single column"));
	m_list_of_pathways.Add(wxT("Diagrams of p-value: N(N-1)/2 files"));
	m_list_of_pathways.Add(wxT("Efficiencies: N(N-1)/2 files"));
	radiobox_selectpathway = new wxRadioBox(this, EVENT_CHOSEN_PATHWAY,  wxT("Select starting data:"), wxDefaultPosition, wxDefaultSize,  m_list_of_pathways, 0, wxRA_SPECIFY_ROWS);
	Connect(EVENT_CHOSEN_PATHWAY, wxEVT_RADIOBOX, wxCommandEventHandler(GuiFrame::onSelectPathway));

	statictext_header_separators = new wxStaticText(this, wxID_ANY, wxT("Column separator:"), wxDefaultPosition, wxDefaultSize, 0);
	statictext_header_delimiters = new wxStaticText(this, wxID_ANY, wxT("File label delimiter:"), wxDefaultPosition, wxDefaultSize, 0);
	statictext_header_columnr = new wxStaticText(this, wxID_ANY, wxT("Column number:"), wxDefaultPosition, wxDefaultSize, 0);

	// Button to open files
	button_openmanyfiles = new wxButton(this, EVENT_BUTTON_LOAD, wxT("Load files"), wxDefaultPosition);
	Connect(EVENT_BUTTON_LOAD, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::loadInputFiles));
	statictext_inputfile = new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0);

	// Button to choose output folder
	button_outputfolder = new wxButton(this, EVENT_BUTTON_FOLDER, wxT("Browse"), wxDefaultPosition);
	Connect(EVENT_BUTTON_FOLDER, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::loadOutputFolder));
	statictext_outputfolder = new wxStaticText(this, wxID_ANY, "-\n-", wxDefaultPosition, wxDefaultSize, 0);

	// Button to launch computation
	button_runworker = new wxButton(this, EVENT_BUTTON_RUN, wxT("Run"), wxDefaultPosition);
	Connect(EVENT_BUTTON_RUN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::onWorkerStart));

	// Choice of the computational final stage
	wxArrayString	m_list_of_modalities;
	m_list_of_modalities.Add(wxT("Diagrams of cross-correlation"));
	m_list_of_modalities.Add(wxT("Diagrams of p-values"));
	m_list_of_modalities.Add(wxT("Efficiency functions"));
	m_list_of_modalities.Add(wxT("Matrix of time scales"));
	radiobox_mode = new wxRadioBox(this, EVENT_CHOSEN_MODE, wxT("Select target output:"), wxDefaultPosition, wxDefaultSize, m_list_of_modalities, 0, wxRA_SPECIFY_ROWS);
	Connect(EVENT_CHOSEN_MODE, wxEVT_RADIOBOX, wxCommandEventHandler(GuiFrame::onSelectMode));

	// List of separators for the input files
	wxArrayString	m_list_of_separators;
	m_list_of_separators.Add(combobox_separators_element_0);
	m_list_of_separators.Add(combobox_separators_element_1);
	m_list_of_separators.Add(combobox_separators_element_2);
	combobox_separators = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,  m_list_of_separators, 0);
	combobox_separators->SetSelection(0);

	// List of delimiters for labels in file names
	wxArrayString	m_list_of_delimiters;
	m_list_of_delimiters.Add(combobox_delimiters_element_0);
	m_list_of_delimiters.Add(combobox_delimiters_element_1);
	combobox_delimiters = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,  m_list_of_delimiters, 0);
	combobox_delimiters->SetSelection(0);

	// Column number selector
	spinner_columnr = new wxSpinCtrl(this, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 256, 1);

	// Button to clear all data
	button_clear_stored = new wxButton(this, wxID_DELETE, wxT("Clear"), wxDefaultPosition, wxDefaultSize);
	Connect(wxID_DELETE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::onDatasetClear));

	// Computation parameters
	spinner_sampling_period = new wxSpinCtrlDouble(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.001, 1000, 1, 0.001);
	spinner_basewidth = new wxSpinCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 2, 10000, 20);
	spinner_nr_windowwidths = new wxSpinCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 9999, 100);
	spinner_nr_surrogates = new wxSpinCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 50, 10000, 1000);
	spinner_thr_significance = new wxSpinCtrlDouble(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.001, 1, 0.01, 0.001);
	spinner_thr_efficiency = new wxSpinCtrlDouble(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.01, 1, 0.5, 0.01);
	statictext_sampling_period = new wxStaticText(this, wxID_ANY, wxT("Sampling period:"), wxDefaultPosition, wxDefaultSize, 0);
	statictext_basewidth = new wxStaticText(this, wxID_ANY, wxT("Base width (pts.):"), wxDefaultPosition, wxDefaultSize, 0);
	statictext_nr_windowwidths = new wxStaticText(this, wxID_ANY, wxT("Nr. of widths:"), wxDefaultPosition, wxDefaultSize, 0);
	statictext_nr_surrogates = new wxStaticText(this, wxID_ANY, wxT("Nr. of surrogates:"), wxDefaultPosition, wxDefaultSize, 0);
	statictext_thr_significance = new wxStaticText(this, wxID_ANY, wxT("Significance threshold:"), wxDefaultPosition, wxDefaultSize, 0);
	statictext_thr_efficiency = new wxStaticText(this, wxID_ANY, wxT("Efficiency threshold:"), wxDefaultPosition, wxDefaultSize, 0);
	checkbox_source_leakage = new wxCheckBox(this, EVENT_CHECKBOX_SHIFT, wxT("Assess zero-delay cross-corr\nas the average of two delayed\ncross-corr (±τ)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE | wxALIGN_RIGHT);
	Connect(EVENT_CHECKBOX_SHIFT, wxEVT_CHECKBOX, wxCommandEventHandler(GuiFrame::onSelectShiftCheckbox));
	spinner_source_leakage = new wxSpinCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 10, 1);
	statictext_source_leakage = new wxStaticText(this, wxID_ANY, wxT(" τ"), wxDefaultPosition, wxDefaultSize, 0);

	// Output selection
	checkbox_save_cdiagrams = new wxCheckBox(this, wxID_ANY, wxT("Correlation diagrams"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	checkbox_save_pdiagrams = new wxCheckBox(this, wxID_ANY, wxT("p-value diagrams"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	checkbox_save_efficiencies = new wxCheckBox(this, wxID_ANY, wxT("Efficiencies"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	statictext_save_header = new wxStaticText(this, wxID_ANY, wxT("Save intermediate results:"), wxDefaultPosition, wxDefaultSize, 0);

	// Output filename prefix
	statictext_save_prefix = new wxStaticText(this, wxID_ANY, wxT("Prefix of output file names:"), wxDefaultPosition, wxDefaultSize, 0);
	textctrl_save_prefix = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_LEFT);

	// Parallel computation controls
	checkbox_parallel_omp = new wxCheckBox(this, wxID_ANY, wxT("Enable parallel computing"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE | wxALIGN_RIGHT);
	spinner_threadnum = new wxSpinCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 256, 4);
	statictext_threadnum = new wxStaticText(this, wxID_ANY, wxT("Nr. threads:"), wxDefaultPosition, wxDefaultSize, 0);

	staticline_run = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(-1,1));
	staticline_parameters = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(-1,1));

	// Button for preview
	button_preview = new wxButton(this, EVENT_BUTTON_PREVIEW, wxT("Preview"), wxDefaultPosition, wxDefaultSize);
	Connect(EVENT_BUTTON_PREVIEW, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::popupPreview));

	// Progress dialog init
	dialog_progress = NULL;

	// Create workspace to store data
	m_workspace = new ContainerWorkspace();


	// Sizers to manage layout
	wxBoxSizer *vbox_tf_separator = new wxBoxSizer(wxVERTICAL);
	vbox_tf_separator->Add(statictext_header_separators, 0, wxBOTTOM | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_tf_separator->Add(combobox_separators, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	wxBoxSizer *vbox_tf_delimiter = new wxBoxSizer(wxVERTICAL);
	vbox_tf_delimiter->Add(statictext_header_delimiters, 0, wxBOTTOM | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_tf_delimiter->Add(combobox_delimiters, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	wxBoxSizer *vbox_tf_columnr = new wxBoxSizer(wxVERTICAL);
	vbox_tf_columnr->Add(statictext_header_columnr, 0, wxBOTTOM | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_tf_columnr->Add(spinner_columnr, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	wxBoxSizer *hbox_in1 = new wxBoxSizer(wxHORIZONTAL);
	hbox_in1->Add(vbox_tf_separator, 1, wxEXPAND | wxRIGHT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 20);
	hbox_in1->Add(vbox_tf_delimiter, 1, wxEXPAND | wxRIGHT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 20);
	hbox_in1->Add(vbox_tf_columnr, 1, wxEXPAND | wxRIGHT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 20);

	wxBoxSizer *hbox_tf_load = new wxBoxSizer(wxHORIZONTAL);
	hbox_tf_load->Add(button_openmanyfiles, 1, wxRIGHT | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	hbox_tf_load->Add(statictext_inputfile, 5, wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	wxBoxSizer *hbox_tf_browse = new wxBoxSizer(wxHORIZONTAL);
	hbox_tf_browse->Add(button_outputfolder, 1, wxRIGHT | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	hbox_tf_browse->Add(statictext_outputfolder, 5, wxALIGN_TOP | wxRESERVE_SPACE_EVEN_IF_HIDDEN);

	wxBoxSizer *vbox_input_ctrl = new wxBoxSizer(wxVERTICAL);
	vbox_input_ctrl->Add(hbox_in1, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_input_ctrl->Add(hbox_tf_load, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_input_ctrl->Add(hbox_tf_browse, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);

	wxBoxSizer *hbox_all_in = new wxBoxSizer(wxHORIZONTAL);
	hbox_all_in->Add(radiobox_selectpathway, 1, wxLEFT | wxRIGHT | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 10);
	hbox_all_in->Add(vbox_input_ctrl, 3, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);

	wxBoxSizer *hbox_head_in = new wxBoxSizer(wxHORIZONTAL);
	hbox_head_in->Add(statictext_title_input, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 10);
	hbox_head_in->Add(staticline_title_input, 10, wxALL | wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 10);

	wxBoxSizer *vbox_tf_sampling = new wxBoxSizer(wxVERTICAL);
	vbox_tf_sampling->Add(statictext_sampling_period, 0, wxBOTTOM | wxALIGN_LEFT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_tf_sampling->Add(spinner_sampling_period, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	wxBoxSizer *vbox_tf_basewidth = new wxBoxSizer(wxVERTICAL);
	vbox_tf_basewidth->Add(statictext_basewidth, 0, wxBOTTOM | wxALIGN_LEFT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_tf_basewidth->Add(spinner_basewidth, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	wxBoxSizer *vbox_tf_nrwidths = new wxBoxSizer(wxVERTICAL);
	vbox_tf_nrwidths->Add(statictext_nr_windowwidths, 0, wxBOTTOM | wxALIGN_LEFT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_tf_nrwidths->Add(spinner_nr_windowwidths, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	wxBoxSizer *vbox_tf_nrsurr = new wxBoxSizer(wxVERTICAL);
	vbox_tf_nrsurr->Add(statictext_nr_surrogates, 0, wxBOTTOM | wxALIGN_LEFT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_tf_nrsurr->Add(spinner_nr_surrogates, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	wxBoxSizer *vbox_tf_thrsignif = new wxBoxSizer(wxVERTICAL);
	vbox_tf_thrsignif->Add(statictext_thr_significance, 0, wxBOTTOM | wxALIGN_LEFT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_tf_thrsignif->Add(spinner_thr_significance, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	wxBoxSizer *vbox_tf_threff = new wxBoxSizer(wxVERTICAL);
	vbox_tf_threff->Add(statictext_thr_efficiency, 0, wxBOTTOM | wxALIGN_LEFT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_tf_threff->Add(spinner_thr_efficiency, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);

	wxFlexGridSizer *gbox_parameters = new wxFlexGridSizer(2, 15, 50);
	gbox_parameters->Add(vbox_tf_sampling, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	gbox_parameters->Add(vbox_tf_nrsurr, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	gbox_parameters->Add(vbox_tf_basewidth, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	gbox_parameters->Add(vbox_tf_thrsignif, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	gbox_parameters->Add(vbox_tf_nrwidths, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	gbox_parameters->Add(vbox_tf_threff, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);

	wxBoxSizer *vbox_tf_prefix = new wxBoxSizer(wxVERTICAL);
	vbox_tf_prefix->Add(statictext_save_prefix, 1, wxBOTTOM | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_tf_prefix->Add(textctrl_save_prefix, 0, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	wxBoxSizer *vbox_tf_save = new wxBoxSizer(wxVERTICAL);
	vbox_tf_save->Add(statictext_save_header, 0, wxBOTTOM | wxALIGN_LEFT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_tf_save->Add(checkbox_save_cdiagrams, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	vbox_tf_save->Add(checkbox_save_pdiagrams, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	vbox_tf_save->Add(checkbox_save_efficiencies, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	wxBoxSizer *hbox_output = new wxBoxSizer(wxHORIZONTAL);
	hbox_output->Add(vbox_tf_prefix, 1, wxRIGHT | wxTOP | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 10);
	hbox_output->Add(20, 0, 0, 0, 0);
	hbox_output->Add(vbox_tf_save, 1,  wxTOP | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 10);

	wxBoxSizer *hbox_tf_sourceleakage = new wxBoxSizer(wxHORIZONTAL);
	hbox_tf_sourceleakage->Add(checkbox_source_leakage, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	hbox_tf_sourceleakage->Add(spinner_source_leakage, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	hbox_tf_sourceleakage->Add(statictext_source_leakage, 2, wxALL | wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);

	wxBoxSizer *vbox_parameters = new wxBoxSizer(wxVERTICAL);
	vbox_parameters->Add(gbox_parameters, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	vbox_parameters->Add(hbox_tf_sourceleakage, 0, wxTOP | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 15);
	vbox_parameters->Add(hbox_output, 0, wxTOP | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);

	wxBoxSizer *hbox_config = new wxBoxSizer(wxHORIZONTAL);
	hbox_config->Add(radiobox_mode, 1, wxLEFT | wxRIGHT | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 10);
	hbox_config->Add(vbox_parameters, 3, wxLEFT | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 20);

	wxBoxSizer *hbox_head_comp = new wxBoxSizer(wxHORIZONTAL);
	hbox_head_comp->Add(statictext_title_output, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 10);
	hbox_head_comp->Add(staticline_title_output, 10, wxALL | wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 10);

	wxBoxSizer *hbox_head_run = new wxBoxSizer(wxHORIZONTAL);
	hbox_head_run->Add(staticline_run, 10, wxALL | wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 10);

	wxBoxSizer *hbox_threadnum = new wxBoxSizer(wxHORIZONTAL);
	hbox_threadnum->Add(statictext_threadnum, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	hbox_threadnum->Add(spinner_threadnum, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);

	wxBoxSizer *vbox_parallel = new wxBoxSizer(wxVERTICAL);
	vbox_parallel->Add(checkbox_parallel_omp, 0, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	vbox_parallel->Add(hbox_threadnum, 0,  wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);

	wxBoxSizer *hbox_all_run = new wxBoxSizer(wxHORIZONTAL);
	hbox_all_run->Add(vbox_parallel, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	hbox_all_run->Add(100, 0, 0, 0, 0);
	hbox_all_run->Add(button_runworker, 1, wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	hbox_all_run->Add(button_clear_stored, 1, wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	hbox_all_run->Add(button_preview, 1, wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);

	wxBoxSizer *vbox_all = new wxBoxSizer(wxVERTICAL);
	vbox_all->Add(hbox_head_in, 0, wxTOP | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	vbox_all->Add(hbox_all_in, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	vbox_all->Add(hbox_head_comp, 0, wxTOP | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	vbox_all->Add(hbox_config, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	vbox_all->Add(hbox_head_run, 0, wxTOP | wxBOTTOM | wxRIGHT | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 10);
	vbox_all->Add(hbox_all_run, 1, wxALL | wxALIGN_RIGHT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);

	// Set frame global sizer
	this->SetSizer(vbox_all);

	#if wxUSE_STATUSBAR
		CreateStatusBar(2);
	#endif // wxUSE_STATUSBAR

	// Initialize status
	GuiFrame::showInputControls(0);
	GuiFrame::setNotReadyStatus();

	// Show startup "about" dialog
	wxCommandEvent ev = wxCommandEvent(wxEVT_NULL, 0);
	showAboutDialog(ev);

	// Resize and show the main window
	SetSize(800,800);
	SetMinSize(wxSize(800,800));
	Show();
}

void GuiFrame::onFrameQuit (wxCommandEvent& WXUNUSED(event) )
{
	Close(true);
}

void GuiFrame::onSelectPathway (wxCommandEvent& WXUNUSED(event))
{
	int n = radiobox_selectpathway->GetSelection();
	switch (n) {
		case 0:
			GuiFrame::showPathsAll();
			break;
		case 1:
			GuiFrame::showPathsAll();
			break;
		case 2:
			GuiFrame::showPathsEfficiency();
			break;
		case 3:
			GuiFrame::showPathsMatrix();
			break;
		default:
			break;
	}
	GuiFrame::showInputControls(n);
	return;
}

void GuiFrame::showInputControls (int selected_pathway)
{
	switch (selected_pathway) {
		case 0:
			combobox_delimiters->Enable();
			spinner_columnr->Disable();
			break;
		case 1:
			combobox_delimiters->Enable();
			spinner_columnr->Enable();
			break;
		case 2:
			combobox_delimiters->Enable();
			spinner_columnr->Disable();
			break;
		case 3:
			combobox_delimiters->Enable();
			spinner_columnr->Disable();
			break;
		default:
			break;
	}
	return;
}


void GuiFrame::showPathsAll ()
{
	radiobox_mode->Enable(0, 1);
	radiobox_mode->Enable(1, 1);
	radiobox_mode->Enable(2, 1);
	radiobox_mode->Enable(3, 1);
	radiobox_mode->SetSelection(0);
	wxCommandEvent ev(wxEVT_RADIOBOX, EVENT_CHOSEN_MODE);
	radiobox_mode->GetEventHandler()->ProcessEvent(ev);
}

void GuiFrame::showPathsEfficiency ()
{
	radiobox_mode->SetSelection(2);
	radiobox_mode->Enable(0, 0);
	radiobox_mode->Enable(1, 0);
	radiobox_mode->Enable(2, 1);
	radiobox_mode->Enable(3, 1);
	wxCommandEvent ev(wxEVT_RADIOBOX, EVENT_CHOSEN_MODE);
	radiobox_mode->GetEventHandler()->ProcessEvent(ev);
}

void GuiFrame::showPathsMatrix ()
{
	radiobox_mode->SetSelection(3);
	radiobox_mode->Enable(0, 0);
	radiobox_mode->Enable(1, 0);
	radiobox_mode->Enable(2, 0);
	radiobox_mode->Enable(3, 1);
	wxCommandEvent ev(wxEVT_RADIOBOX, EVENT_CHOSEN_MODE);
	radiobox_mode->GetEventHandler()->ProcessEvent(ev);
}

void GuiFrame::onSelectMode (wxCommandEvent& WXUNUSED(event))
{
	int n = radiobox_mode->GetSelection();
	int m = radiobox_selectpathway->GetSelection();
	if (m < 2) {
		switch (n) {
			case 0:
				spinner_sampling_period->Enable();
				spinner_basewidth->Enable();
				spinner_nr_windowwidths->Enable();
				spinner_nr_surrogates->Disable();
				spinner_thr_significance->Disable();
				spinner_thr_efficiency->Disable();
				checkbox_save_cdiagrams->SetValue(1);
				checkbox_save_cdiagrams->Disable();
				checkbox_save_pdiagrams->SetValue(0);
				checkbox_save_pdiagrams->Disable();
				checkbox_save_efficiencies->SetValue(0);
				checkbox_save_efficiencies->Disable();
				checkbox_source_leakage->SetValue(0);
				checkbox_source_leakage->Enable();
				break;
			case 1:
				spinner_sampling_period->Enable();
				spinner_basewidth->Enable();
				spinner_nr_windowwidths->Enable();
				spinner_nr_surrogates->Enable();
				spinner_thr_significance->Disable();
				spinner_thr_efficiency->Disable();
				checkbox_save_cdiagrams->SetValue(0);
				checkbox_save_cdiagrams->Enable();
				checkbox_save_pdiagrams->SetValue(1);
				checkbox_save_pdiagrams->Disable();
				checkbox_save_efficiencies->SetValue(0);
				checkbox_save_efficiencies->Disable();
				checkbox_source_leakage->SetValue(0);
				checkbox_source_leakage->Enable();
				break;
			case 2:
				spinner_sampling_period->Enable();
				spinner_basewidth->Enable();
				spinner_nr_windowwidths->Enable();
				spinner_nr_surrogates->Enable();
				spinner_thr_significance->Enable();
				spinner_thr_efficiency->Disable();
				checkbox_save_cdiagrams->SetValue(0);
				checkbox_save_cdiagrams->Enable();
				checkbox_save_pdiagrams->SetValue(0);
				checkbox_save_pdiagrams->Enable();
				checkbox_save_efficiencies->SetValue(1);
				checkbox_save_efficiencies->Disable();
				checkbox_source_leakage->SetValue(0);
				checkbox_source_leakage->Enable();
				break;
			case 3:
				spinner_sampling_period->Enable();
				spinner_basewidth->Enable();
				spinner_nr_windowwidths->Enable();
				spinner_nr_surrogates->Enable();
				spinner_thr_significance->Enable();
				spinner_thr_efficiency->Enable();
				checkbox_save_cdiagrams->SetValue(0);
				checkbox_save_cdiagrams->Enable();
				checkbox_save_pdiagrams->SetValue(0);
				checkbox_save_pdiagrams->Enable();
				checkbox_save_efficiencies->SetValue(0);
				checkbox_save_efficiencies->Enable();
				checkbox_source_leakage->SetValue(0);
				checkbox_source_leakage->Enable();
				break;
		}
	} else if (m == 2) {
		switch (n) {
			case 2:
				spinner_sampling_period->Enable();
				spinner_basewidth->Enable();
				spinner_nr_windowwidths->Disable();
				spinner_nr_surrogates->Disable();
				spinner_thr_significance->Enable();
				spinner_thr_efficiency->Disable();
				checkbox_save_cdiagrams->SetValue(0);
				checkbox_save_cdiagrams->Disable();
				checkbox_save_pdiagrams->SetValue(0);
				checkbox_save_pdiagrams->Disable();
				checkbox_save_efficiencies->SetValue(1);
				checkbox_save_efficiencies->Disable();
				checkbox_source_leakage->SetValue(0);
				checkbox_source_leakage->Disable();
				break;
			case 3:
				spinner_sampling_period->Enable();
				spinner_basewidth->Enable();
				spinner_nr_windowwidths->Disable();
				spinner_nr_surrogates->Disable();
				spinner_thr_significance->Enable();
				spinner_thr_efficiency->Enable();
				checkbox_save_cdiagrams->SetValue(0);
				checkbox_save_cdiagrams->Disable();
				checkbox_save_pdiagrams->SetValue(0);
				checkbox_save_pdiagrams->Disable();
				checkbox_save_efficiencies->SetValue(0);
				checkbox_save_efficiencies->Enable();
				checkbox_source_leakage->SetValue(0);
				checkbox_source_leakage->Disable();
				break;
		}
	} else if (m == 3) {
		spinner_sampling_period->Enable();
		spinner_basewidth->Enable();
		spinner_nr_windowwidths->Disable();
		spinner_nr_surrogates->Disable();
		spinner_thr_significance->Disable();
		spinner_thr_efficiency->Enable();
		checkbox_save_cdiagrams->SetValue(0);
		checkbox_save_cdiagrams->Disable();
		checkbox_save_pdiagrams->SetValue(0);
		checkbox_save_pdiagrams->Disable();
		checkbox_save_efficiencies->SetValue(0);
		checkbox_save_efficiencies->Disable();
		checkbox_source_leakage->SetValue(0);
		checkbox_source_leakage->Disable();
	}
	return;
}

void GuiFrame::onSelectShiftCheckbox (wxCommandEvent& WXUNUSED(event)) {
	if (checkbox_source_leakage->GetValue()) {
		spinner_source_leakage->Show();
		statictext_source_leakage->Show();
	} else {
		spinner_source_leakage->Hide();
		statictext_source_leakage->Hide();
	}
}

void GuiFrame::initializeConstants ()
{
	// Constant strings
	combobox_separators_element_0 = wxT("tab [\\t]");
	combobox_separators_element_1 = wxT("space [ ]");
	combobox_separators_element_2 = wxT("comma [,]");
	combobox_delimiters_element_0 = wxT("underscore [_]");
	combobox_delimiters_element_1 = wxT("dash [-]");

	// Constant chars
	combobox_separators_char_0 = '\t';
	combobox_separators_char_1 = ' ';
	combobox_separators_char_2 = ',';
	combobox_delimiters_char_0 = '_';
	combobox_delimiters_char_1 = '-';

	return;
}

GuiFrame::~GuiFrame ()
{
	delete	m_workspace;

	delete	dialog_progress;

	delete	radiobox_selectpathway;
	delete	radiobox_mode;
	delete	button_runworker;
	delete	button_openmanyfiles;
	delete	button_outputfolder;
	delete	button_clear_stored;
	delete	button_preview;
	delete	combobox_separators;
	delete	combobox_delimiters;

	delete	spinner_columnr;
	delete	spinner_basewidth;
	delete	spinner_nr_windowwidths;
	delete	spinner_nr_surrogates;
	delete	spinner_source_leakage;
	delete	spinner_threadnum;
	delete	spinner_sampling_period;
	delete	spinner_thr_significance;
	delete	spinner_thr_efficiency;

	delete	checkbox_source_leakage;
	delete	checkbox_save_cdiagrams;
	delete	checkbox_save_pdiagrams;
	delete	checkbox_save_efficiencies;
	delete	checkbox_parallel_omp;

	delete	textctrl_save_prefix;

	delete	staticline_title_input;
	delete	staticline_title_output;
	delete	statictext_title_input;
	delete	statictext_title_output;
	delete	statictext_header_separators;
	delete	statictext_header_delimiters;
	delete	statictext_header_columnr;
	delete	statictext_inputfile;
	delete	statictext_outputfolder;
	delete	statictext_sampling_period;
	delete	statictext_basewidth;
	delete	statictext_nr_windowwidths;
	delete	statictext_nr_surrogates;
	delete	statictext_thr_significance;
	delete	statictext_thr_efficiency;
	delete	statictext_source_leakage;
	delete	statictext_save_header;
	delete	statictext_save_prefix;
	delete	statictext_threadnum;
	delete	staticline_run;
	delete	staticline_parameters;
}

void GuiFrame::showAboutDialog (wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;
	info.SetName(wxT("NetOnZeroDXC - Analysis"));
	info.SetVersion(wxT("1.0"));
	info.SetDescription(wxT("An interface for assessing zero-delay cross correlation between time series associated to nodes in a network.\nIf you use this program for your analyses, please cite:\nChaos 28(6):063127 (2018)."));
	info.SetCopyright(wxT("2019"));

	wxIcon		temp_icon;
	wxBitmap	temp_png = wxBITMAP_PNG_FROM_DATA(bin_icon_64);
	temp_icon.CopyFromBitmap(temp_png);
	info.SetIcon(temp_icon);

	wxAboutBox(info, this);

	return;
}
