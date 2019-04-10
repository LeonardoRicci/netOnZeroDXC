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
	#include "netOnZeroDXC_merge_main.hpp"
	#define INCLUDED_MAINAPP
#endif
#ifndef INCLUDED_ICON
	#include "netOnZeroDXC_gui_icon.hpp"
	#define INCLUDED_ICON
#endif

// Event table for the binding of some events
wxBEGIN_EVENT_TABLE(GuiFrame, wxFrame)
	EVT_MENU(APP_ABOUT, GuiFrame::showAboutDialog)
	EVT_MENU(APP_QUIT, GuiFrame::onFrameQuit)
wxEND_EVENT_TABLE()

// Frame constructor: this contains the initialization of all buttons, controls and boxes within the main window.
GuiFrame::GuiFrame(const wxString& title)
: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE)
{
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

	data_container = new ContainerWorkspace();
	data_container->clearWorkspace();

	initializeConstants();

	statictext_header_loading = new wxStaticText(this, wxID_ANY, wxT("Data loading"), wxDefaultPosition, wxDefaultSize);
	button_load_confg = new wxButton(this, EVENT_BUTTON_LOADC, wxT("Configured load"), wxDefaultPosition, wxDefaultSize);
	Connect(EVENT_BUTTON_LOADC, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::configuredLoadFiles));

	button_clear_all = new wxButton(this, EVENT_BUTTON_CLEAR, wxT("Clear all"), wxDefaultPosition, wxDefaultSize);
	Connect(EVENT_BUTTON_CLEAR, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::deleteAllData));

	listbox_systems = new wxListBox(this, EVENT_LISTBOX_SELECTION, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE | wxLB_NEEDED_SB);
	Connect(EVENT_LISTBOX_SELECTION, wxEVT_LISTBOX, wxCommandEventHandler(GuiFrame::refreshListboxRecordings));
	listbox_recordings = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE | wxLB_NEEDED_SB);
	button_system_add = new wxButton(this, EVENT_BUTTON_SYSADD, wxT("Add system"), wxDefaultPosition, wxDefaultSize);
	Connect(EVENT_BUTTON_SYSADD, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::addObservedSystem));
	button_system_del = new wxButton(this, EVENT_BUTTON_SYSDEL, wxT("Delete system"), wxDefaultPosition, wxDefaultSize);
	Connect(EVENT_BUTTON_SYSDEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::deleteObservedSystem));

	button_recording_add = new wxButton(this, EVENT_BUTTON_RECADD, wxT("Add recording"), wxDefaultPosition, wxDefaultSize);
	Connect(EVENT_BUTTON_RECADD, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::addObservedRecording));
	button_recording_del = new wxButton(this, EVENT_BUTTON_RECDEL, wxT("Delete recording"), wxDefaultPosition, wxDefaultSize);
	Connect(EVENT_BUTTON_RECDEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::deleteObservedRecording));

	wxArrayString	m_list_of_modalities;
	m_list_of_modalities.Add(wxT("Efficiency functions, N(N-1)/2 files"));
	m_list_of_modalities.Add(wxT("Matrices of time scales"));
	radiobox_input = new wxRadioBox(this, wxID_ANY, wxT("Select input data:"), wxDefaultPosition, wxDefaultSize, m_list_of_modalities, 0, wxRA_SPECIFY_ROWS);
	button_dictionary_load = new wxButton(this, EVENT_BUTTON_LOADL, wxT("Load node labels"), wxDefaultPosition, wxDefaultSize);
	Connect(EVENT_BUTTON_LOADL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::loadLabelsDictionary));
	button_dictionary_load->Hide();
	button_info = new wxButton(this, EVENT_BUTTON_INFO, wxT("Info on recording"), wxDefaultPosition, wxDefaultSize);
	Connect(EVENT_BUTTON_INFO, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::displayRecordingInfo));

	wxArrayString	m_list_of_separators;
	m_list_of_separators.Add(combobox_separators_element_0);
	m_list_of_separators.Add(combobox_separators_element_1);
	m_list_of_separators.Add(combobox_separators_element_2);
	combobox_separators = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,  m_list_of_separators, 0);
	combobox_separators->SetSelection(0);
	statictext_separators = new wxStaticText(this, wxID_ANY, wxT("Column sep.:"), wxDefaultPosition, wxDefaultSize, 0);

	wxArrayString	m_list_of_delimiters;
	m_list_of_delimiters.Add(combobox_delimiters_element_0);
	m_list_of_delimiters.Add(combobox_delimiters_element_1);
	combobox_delimiters = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,  m_list_of_delimiters, 0);
	combobox_delimiters->SetSelection(0);
	statictext_delimiters = new wxStaticText(this, wxID_ANY, wxT("File label delim.:"), wxDefaultPosition, wxDefaultSize, 0);

	button_validate_preproc = new wxButton(this, EVENT_BUTTON_PREPROC, wxT("Validate and prepare"), wxDefaultPosition, wxDefaultSize);
	Connect(EVENT_BUTTON_PREPROC, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::validatePreprocessData));

	statictext_thr_efficiency = new wxStaticText(this, wxID_ANY, wxT("Efficiency threshold:"), wxDefaultPosition, wxDefaultSize);
	statictext_ranking_recordings = new wxStaticText(this, wxID_ANY, wxT("Rank for merging recordings\n(1 = smallest)"), wxDefaultPosition, wxDefaultSize);
	statictext_ranking_systems = new wxStaticText(this, wxID_ANY, wxT("Rank for merging systems\n(1 = smallest)"), wxDefaultPosition, wxDefaultSize);
	spinner_thr_efficiency = new wxSpinCtrlDouble(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.01, 1, 0.5, 0.01);
	spinner_ranking_recordings = new wxSpinCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 9999, 1);
	spinner_ranking_systems = new wxSpinCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 9999, 1);
	button_merge_recordings = new wxButton(this, EVENT_BUTTON_MERGEREC, wxT("Preview system"), wxDefaultPosition, wxDefaultSize);
	Connect(EVENT_BUTTON_MERGEREC, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::popupPreviewBySystem));
	button_merge_global = new wxButton(this, EVENT_BUTTON_MERGEALL, wxT("Preview global"), wxDefaultPosition, wxDefaultSize);
	Connect(EVENT_BUTTON_MERGEALL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::popupPreviewGlobal));

	button_save_sytems = new wxButton(this, EVENT_BUTTON_SAVESYS, wxT("Save merged for each system"), wxDefaultPosition, wxDefaultSize);
	Connect(EVENT_BUTTON_SAVESYS, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::saveMergedSystems));
	button_save_global = new wxButton(this, EVENT_BUTTON_SAVEALL, wxT("Save global"), wxDefaultPosition, wxDefaultSize);
	Connect(EVENT_BUTTON_SAVEALL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuiFrame::saveMergedGlobal));

	// Sizers
	wxBoxSizer *hbox_btns_sys = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *hbox_btns_rec = new wxBoxSizer(wxHORIZONTAL);
	hbox_btns_sys->Add(button_system_add, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	hbox_btns_sys->Add(button_system_del, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	hbox_btns_rec->Add(button_recording_add, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	hbox_btns_rec->Add(button_recording_del, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);

	wxBoxSizer *hbox_separators = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *hbox_delimiters = new wxBoxSizer(wxHORIZONTAL);
	hbox_separators->Add(statictext_separators, 0, wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	hbox_separators->Add(combobox_separators, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	hbox_delimiters->Add(statictext_delimiters, 0, wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	hbox_delimiters->Add(combobox_delimiters, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);

	wxBoxSizer *vbox_manage_sys = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *vbox_manage_rec = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *vbox_manage_load = new wxBoxSizer(wxVERTICAL);
	vbox_manage_sys->Add(hbox_btns_sys, 0, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	vbox_manage_sys->Add(listbox_systems, 1, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	vbox_manage_rec->Add(hbox_btns_rec, 0, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	vbox_manage_rec->Add(listbox_recordings, 1, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);

	vbox_manage_load->Add(statictext_header_loading, 0, wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_manage_load->Add(hbox_separators, 0, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_manage_load->Add(hbox_delimiters, 0, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_manage_load->Add(radiobox_input, 0, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_manage_load->Add(button_load_confg, 0, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_manage_load->Add(button_dictionary_load, 0, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	vbox_manage_load->Add(button_info, 0, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);

	wxBoxSizer *hbox_validate_clear = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *hbox_manage = new wxBoxSizer(wxHORIZONTAL);
	hbox_validate_clear->Add(button_validate_preproc, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	hbox_validate_clear->Add(button_clear_all, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	hbox_manage->Add(vbox_manage_sys, 5, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	hbox_manage->Add(vbox_manage_rec, 5, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	hbox_manage->Add(vbox_manage_load, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);

	wxBoxSizer *vbox_setup = new wxBoxSizer(wxVERTICAL);
	vbox_setup->Add(hbox_manage, 5, wxTOP | wxRIGHT | wxLEFT | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5);
	vbox_setup->Add(hbox_validate_clear, 1, wxTOP | wxRIGHT | wxLEFT | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5);

	wxBoxSizer *hbox_spinner_eta = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *hbox_spinner_rec = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *hbox_spinner_sys = new wxBoxSizer(wxHORIZONTAL);
	hbox_spinner_eta->Add(statictext_thr_efficiency, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	hbox_spinner_eta->Add(spinner_thr_efficiency, 0, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	hbox_spinner_rec->Add(statictext_ranking_recordings, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	hbox_spinner_rec->Add(spinner_ranking_recordings, 0, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	hbox_spinner_sys->Add(statictext_ranking_systems, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	hbox_spinner_sys->Add(spinner_ranking_systems, 0, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);

	wxBoxSizer *vbox_spinner_etarec = new wxBoxSizer(wxVERTICAL);
	vbox_spinner_etarec->Add(hbox_spinner_eta, 0, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5);
	vbox_spinner_etarec->Add(hbox_spinner_rec, 0, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5);
	vbox_spinner_etarec->Add(hbox_spinner_sys, 0, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5);
	wxBoxSizer *vbox_buttons_preview = new wxBoxSizer(wxVERTICAL);
	vbox_buttons_preview->Add(button_merge_recordings, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5);
	vbox_buttons_preview->Add(button_merge_global, 2, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5);
	wxBoxSizer *vbox_buttons_save = new wxBoxSizer(wxVERTICAL);
	vbox_buttons_save->Add(button_save_sytems, 1, wxALL |  wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5);
	vbox_buttons_save->Add(button_save_global, 2, wxALL |  wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5);

	wxBoxSizer *hbox_mergers = new wxBoxSizer(wxHORIZONTAL);
	hbox_mergers->Add(vbox_spinner_etarec, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	hbox_mergers->Add(vbox_buttons_preview, 0, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	hbox_mergers->AddStretchSpacer(1);
	hbox_mergers->Add(vbox_buttons_save, 0, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);

	wxBoxSizer *vbox_all = new wxBoxSizer(wxVERTICAL);
	vbox_all->Add(vbox_setup, 4, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
	vbox_all->Add(hbox_mergers, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);

	#if wxUSE_STATUSBAR
		CreateStatusBar(2);
	#endif // wxUSE_STATUSBAR

	unsetReadyStatus();

	this->SetSizer(vbox_all);

	wxCommandEvent ev = wxCommandEvent(wxEVT_NULL, 0);
	showAboutDialog(ev);

	// Resize and show the main window
	SetMinSize(wxSize(800,700));
	SetSize(800,700);
	Show();
}

void GuiFrame::onFrameQuit (wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void GuiFrame::displayRecordingInfo (wxCommandEvent& WXUNUSED(event))
{
	int	system_index = listbox_systems->GetSelection();
	if (system_index == wxNOT_FOUND)
		return;

	int	recording_index = listbox_recordings->GetSelection();
	if (recording_index == wxNOT_FOUND)
		return;

	int	loaded_efficiencies = data_container->systems_stored[system_index].recordings_stored[recording_index].efficiencies.size();
	int	loaded_matrix = data_container->systems_stored[system_index].recordings_stored[recording_index].matrix_timescales.size();

	std::stringstream	sstm;
	if ((loaded_efficiencies == 0) && (loaded_matrix == 0)) {
		sstm << "No data have been loaded for this recording yet.\n";
	} else if (loaded_efficiencies > 0) {
		sstm << "For this recording, " << loaded_efficiencies << " efficiency files have been loaded.\n";
		sstm << "Efficiencies are defined for " << data_container->systems_stored[system_index].recordings_stored[recording_index].efficiencies[0].size() << " window widths.\n";
		sstm << "Window widths are multiples of " << data_container->systems_stored[system_index].recordings_stored[recording_index].window_widths[0] << ".\n";
	} else {
		sstm << "For this recording, a matrix file has been loaded.\n";
		sstm << "The matrix contains " << data_container->systems_stored[system_index].recordings_stored[recording_index].matrix_timescales.size() << " rows and columns.\n";
	}
	if (data_container->available_node_labels) {
		sstm << "A list of labels is present: nodes will be labelled with names provided by the user.\n";
	} else {
		sstm << "A list of labels is absent: nodes will be labelled with integer numbers.\n";
	}

	wxMessageBox(sstm.str(), "Info", wxOK | wxICON_INFORMATION, NULL, wxDefaultCoord, wxDefaultCoord);

	return;
}

void GuiFrame::popupPreviewBySystem (wxCommandEvent& WXUNUSED(event))
{
	int	eta_index = (data_container->multiple_eta)? (100 * spinner_thr_efficiency->GetValue()) : -1;
	if (listbox_systems->GetSelection() == wxNOT_FOUND) {
		return;
	}
	data_container->evaluateSystemMatrix(listbox_systems->GetSelection(), spinner_ranking_recordings->GetValue(), eta_index);
	std::stringstream	sstm;
	sstm << "Preview merged matrix - " << data_container->systems_stored[listbox_systems->GetSelection()].system_name;
	new PlotFrame(sstm.str(), this, false);

	return;
}

void GuiFrame::popupPreviewGlobal (wxCommandEvent& WXUNUSED(event))
{
	int	eta_index = (data_container->multiple_eta)? (100 * spinner_thr_efficiency->GetValue()) : -1;
	data_container->evaluateMergedMatrix(spinner_ranking_systems->GetValue(), spinner_ranking_recordings->GetValue(), eta_index);
	new PlotFrame("Preview merged matrix of time scales", this, true);

	return;
}

void GuiFrame::setReadyStatus ()
{
	status_ready = true;
	button_system_add->Disable();
	button_system_del->Disable();
	button_recording_add->Disable();
	button_recording_del->Disable();
	button_dictionary_load->Disable();
	button_load_confg->Disable();
	combobox_delimiters->Disable();
	combobox_separators->Disable();
	statictext_thr_efficiency->Show();
	statictext_ranking_recordings->Show();
	statictext_ranking_systems->Show();
	spinner_thr_efficiency->Show();
	spinner_ranking_recordings->Show();
	spinner_ranking_systems->Show();
	button_merge_recordings->Show();
	button_merge_global->Show();
	button_save_sytems->Show();
	button_save_global->Show();

	button_validate_preproc->SetLabel("Add some more data");

	return;
}

void GuiFrame::unsetReadyStatus ()
{
	status_ready = false;
	button_system_add->Enable();
	button_system_del->Enable();
	button_recording_add->Enable();
	button_recording_del->Enable();
	button_dictionary_load->Enable();
	button_load_confg->Enable();
	combobox_delimiters->Enable();
	combobox_separators->Enable();
	statictext_thr_efficiency->Hide();
	statictext_ranking_recordings->Hide();
	statictext_ranking_systems->Hide();
	spinner_thr_efficiency->Hide();
	spinner_ranking_recordings->Hide();
	spinner_ranking_systems->Hide();
	button_merge_recordings->Hide();
	button_merge_global->Hide();
	button_save_sytems->Hide();
	button_save_global->Hide();

	button_validate_preproc->SetLabel("Validate and prepare");

	return;
}

void GuiFrame::refreshListboxSystems (wxCommandEvent& WXUNUSED(event))
{
	listbox_systems->Clear();

	int	N = data_container->systems_stored.size();
	int	i;
	for (i = 0; i < N; i++) {
		std::string temp_string = data_container->systems_stored[i].system_name;
		listbox_systems->Append(temp_string);
	}

	wxCommandEvent ev = wxCommandEvent(wxEVT_NULL, 0);
	refreshListboxRecordings(ev);

	return;
}

void GuiFrame::refreshListboxRecordings (wxCommandEvent& WXUNUSED(event))
{
	listbox_recordings->Clear();

	int system_index = listbox_systems->GetSelection();
	if (system_index == wxNOT_FOUND)
		return;

	int	N = data_container->systems_stored[system_index].recordings_stored.size();
	int	i;
	for (i = 0; i < N; i++) {
		std::string temp_string = data_container->systems_stored[system_index].recordings_stored[i].recording_name;
		listbox_recordings->Append(temp_string);
	}

	return;
}

void GuiFrame::updateSpinnerFromSliders (double eta, int rank_recordings, int rank_systems)
{
	spinner_thr_efficiency->SetValue(eta);
	spinner_ranking_recordings->SetValue(rank_recordings);
	spinner_ranking_systems->SetValue(rank_systems);
	return;
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
	delete	data_container;
	delete	spinner_thr_efficiency;
	delete	spinner_ranking_recordings;
	delete	spinner_ranking_systems;

	delete	listbox_systems;
	delete	listbox_recordings;
	delete	button_load_confg;
	delete	button_system_add;
	delete	button_recording_add;
	delete	button_system_del;
	delete	button_recording_del;
	delete	button_dictionary_load;
	delete	button_clear_all;
	delete	button_info;
	delete	button_validate_preproc;
	delete	button_merge_recordings;
	delete	button_merge_global;
	delete	button_save_sytems;
	delete	button_save_global;
	delete	combobox_delimiters;
	delete	combobox_separators;
	delete	radiobox_input;

	delete	statictext_header_loading;
	delete	statictext_delimiters;
	delete	statictext_separators;
	delete	statictext_thr_efficiency;
	delete	statictext_ranking_recordings;
	delete	statictext_ranking_systems;
}

void GuiFrame::showAboutDialog (wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;
	info.SetName(_("NetOnZeroDXC - Merge"));
	info.SetVersion(_("1.0"));
	info.SetDescription(_("An interface for assessing networks out of the information provided by the companion program, NetOnZeroDXC_analysis.\nIf you use this program for your analyses, please cite:\nChaos 28(6):063127 (2018)."));
	info.SetCopyright(wxT("2019"));

	wxIcon		temp_icon;
	wxBitmap	temp_png = wxBITMAP_PNG_FROM_DATA(bin_icon_64);
	temp_icon.CopyFromBitmap(temp_png);
	info.SetIcon(temp_icon);

	wxAboutBox(info, this);

	return;
}
