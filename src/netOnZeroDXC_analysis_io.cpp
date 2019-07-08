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

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

#ifndef INCLUDED_MAINAPP
	#include "netOnZeroDXC_analysis_main.hpp"
	#define INCLUDED_MAINAPP
#endif
#ifndef INCLUDED_IOFUNCTIONS
	#include "netOnZeroDXC_io.hpp"
	#define INCLUDED_IOFUNCTIONS
#endif
#ifndef INCLUDED_IOFUNCTIONS_WX
	#include "netOnZeroDXC_gui_io.hpp"
	#define INCLUDED_IOFUNCTIONS_WX
#endif

void GuiFrame::loadInputFiles(wxCommandEvent& WXUNUSED(event))
{
	if(m_status_ready) {
		wxMessageDialog dialog_ask_before_loading(this, "By proceeding in a new load operation, all data loaded/computed so far will be lost. Continue?", "Warning!", wxYES_NO | wxICON_EXCLAMATION);
		if (dialog_ask_before_loading.ShowModal() == wxID_NO)
			return;
	}
	m_workspace->clearWorkspace();

	int	loading_mode = radiobox_selectpathway->GetSelection();
	int		found_last_dir_separator;
	wxArrayString	list_of_files;
	wxString	selected_file_name;
	std::string	file_name;
	std::string	folder_name;
	if (loading_mode) {
		wxFileDialog openFileDialog(this, _("Load data files"), "", "", "all files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
		if (openFileDialog.ShowModal() == wxID_CANCEL)
			return;
		openFileDialog.GetPaths(list_of_files);
		file_name = list_of_files[0].ToStdString();
		found_last_dir_separator = file_name.find_last_of("/\\");
		folder_name = file_name.substr(0, found_last_dir_separator);
	} else {
		wxFileDialog openFileDialog(this, _("Load data file"), "", "", "all files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
		if (openFileDialog.ShowModal() == wxID_CANCEL)
	    		return;
		selected_file_name = openFileDialog.GetPath();
		file_name = selected_file_name.ToStdString();
		found_last_dir_separator = file_name.find_last_of("/\\");
		folder_name = file_name.substr(0, found_last_dir_separator);
	}

	char	separator_char;
	wxString	separator_label = combobox_separators->GetStringSelection();
	if (separator_label == combobox_separators_element_0) {
		separator_char = combobox_separators_char_0;
	} else if (separator_label == combobox_separators_element_1) {
		separator_char = combobox_separators_char_1;
	} else if (separator_label == combobox_separators_element_2) {
		separator_char = combobox_separators_char_2;
	} else {
		wxMessageBox("Invalid column separator.\nContinuing with default (TAB)", "Warning!", wxOK | wxICON_EXCLAMATION, NULL, wxDefaultCoord, wxDefaultCoord);
		separator_char = combobox_separators_char_0;
		separator_label = combobox_separators_element_0;
		combobox_separators->SetSelection(0);
	}

	char	filename_delimiter_char;
	wxString	delimiter_label = combobox_delimiters->GetStringSelection();
	if (delimiter_label == combobox_delimiters_element_0) {
		filename_delimiter_char = combobox_delimiters_char_0;
	} else if (delimiter_label == combobox_delimiters_element_1) {
		filename_delimiter_char = combobox_delimiters_char_1;
	} else {
		wxMessageBox("Invalid filename delimiter.\nContinuing with default.", "Warning!", wxOK | wxICON_EXCLAMATION, NULL, wxDefaultCoord, wxDefaultCoord);
		filename_delimiter_char = combobox_delimiters_char_0;
		delimiter_label = combobox_delimiters_element_0;
		combobox_delimiters->SetSelection(0);
	}

	int	column_number;
	if (loading_mode == 1)
		column_number = spinner_columnr->GetValue();

	std::string		message_filecheck;
	std::stringstream	sstm;
	std::stringstream	displayed_file_info_stream;
	std::stringstream	displayed_folder_info_stream;
	int			loading_error;
	if (loading_mode == 0)
		loading_error = netOnZeroDXC_load_single_file(m_workspace->sequences, m_workspace->node_labels, file_name, separator_char);
	else if (loading_mode == 1)
		loading_error = netOnZeroDXC_load_multi_sequences(m_workspace->sequences, m_workspace->node_labels, list_of_files, separator_char, filename_delimiter_char, column_number);
	else if (loading_mode == 2)
		loading_error = netOnZeroDXC_load_multi_diagrams(m_workspace->diagrams_pvalue, m_workspace->node_pairs, list_of_files, separator_char, filename_delimiter_char);
	else if (loading_mode == 3)
		loading_error = netOnZeroDXC_load_multi_efficiencies(m_workspace->efficiencies, m_workspace->node_pairs, m_workspace->window_widths, list_of_files, separator_char, filename_delimiter_char);

	switch (loading_error) {
		case 1:
			sstm << "Error in reading files!\nFilenames cannot be correctly parsed. Maybe a wrong delimiter?\nNoting was loaded.";
			m_workspace->clearWorkspace();
			displayed_file_info_stream << "Nothing loaded!";
			displayed_folder_info_stream << "No output folder selected.";
			break;
		case 2:
			sstm << "Error in loading files!\n.Unknown I/O error when opening some of the files.\nNoting was loaded.";
			m_workspace->clearWorkspace();
			displayed_file_info_stream << "Nothing loaded!";
			displayed_folder_info_stream << "No output folder selected.";
			break;
		case 3:
			if ((loading_mode == 0) || (loading_mode == 1))
				sstm << "Error in sequences format!\nInconsistent sequences lengths were found.\nNothing was loaded";
			else if (loading_mode == 2)
				sstm << "Error in diagrams format!\nInconsistent diagram sizes were found.\nNoting was loaded.";
			else if (loading_mode == 3)
				sstm << "Error in efficiencies format!\nInconsistent efficiency lengths.\nNoting was loaded.";
			m_workspace->clearWorkspace();
			displayed_file_info_stream << "Nothing loaded!";
			displayed_folder_info_stream << "No output folder selected.";
			break;
		case 4:
			if (loading_mode == 1)
				sstm << "Error in loading sequences!\nDuplicate indexes found (two or more files are labelled by the same index).\nNothing was loaded";
			else if ((loading_mode == 2) || (loading_mode == 3))
				sstm << "Error in loading files!\nDuplicate pairs of indexes found (two or more file are labelled by the same indexes pair).\nNothing was loaded";
			displayed_file_info_stream << "Nothing loaded!";
			displayed_folder_info_stream << "No output folder selected.";
			break;
		case 5:
			if (loading_mode == 0)
				sstm << "Error in loading sequences!\nOnly one sequence detected: maybe a wrong separator?\nIt is meaningless to operate on a single sequence.\nNothing was loaded";
			else if (loading_mode == 1)
				sstm << "Error in loading sequences!\nOnly one sequence was read: maybe a wrong column number?\nIt is meaningless to operate on a single sequence.\nNothing was loaded";
			displayed_file_info_stream << "Nothing loaded!";
			displayed_folder_info_stream << "No output folder selected.";
			break;
		case 0:
			if (loading_mode == 0) {
				sstm << "Successfully read '" << file_name << "'.\n";
				sstm << "Loaded " << m_workspace->sequences.size() << " sequences with separator " << separator_label << ", content read:\n";
				sstm << "- Detected sequences length: " << m_workspace->sequences[0].size() << "\n";
				sstm << "Consistency between sequences was checked. Everything is fine.";
				displayed_file_info_stream << "Sequences loaded: " << m_workspace->sequences.size() << ". Length: " << m_workspace->sequences[0].size();
			} else if (loading_mode == 1) {
				sstm << "Successfully read " << list_of_files.GetCount() << " files with index delimiter " << delimiter_label << ".\n";
				sstm << "Loaded " << m_workspace->sequences.size() << " sequences with separator " << separator_label << " at column " << column_number << ", content read:\n";
				sstm << "- Detected sequences length: " << m_workspace->sequences[0].size() << "\n";
				sstm << "Consistency between sequences was checked. Everything is fine.";
				displayed_file_info_stream << "Sequences loaded: " << m_workspace->sequences.size() << ". Length: " << m_workspace->sequences[0].size();
			} else if (loading_mode == 2) {
				sstm << "Successfully read " << list_of_files.GetCount() << " files with index delimiter " << delimiter_label << ".\n";
				sstm << "Loaded " << m_workspace->diagrams_pvalue.size() << " diagrams with separator " << separator_label << ", content read:\n";
				sstm << "- Detected data rows (i.e. window widths): " << m_workspace->diagrams_pvalue[0].size() << "\n";
				sstm << "- Detected data columns (i.e. window positions): " << m_workspace->diagrams_pvalue[0][0].size() << "\n";
				sstm << "Consistency between diagrams was checked. Everything is fine.";
				displayed_file_info_stream << "Diagrams loaded: " << m_workspace->diagrams_pvalue.size() << ". Size: " << m_workspace->diagrams_pvalue[0].size() << " x " << m_workspace->diagrams_pvalue[0][0].size();
			} else if (loading_mode == 3) {
				sstm << "Successfully read " << list_of_files.GetCount() << " files with index delimiter " << delimiter_label << ".\n";
				sstm << "Loaded " << m_workspace->efficiencies.size() << " efficiencies with separator " << separator_label << ", content read:\n";
				sstm << "- Detected data rows (i.e. window widths): " << m_workspace->efficiencies[0].size() << "\n";
				sstm << "Consistency between efficiencies was checked. Everything is fine.";
				displayed_file_info_stream << "Efficiencies loaded: " << m_workspace->efficiencies.size() << ". Length: " << m_workspace->efficiencies[0].size();
			}
			displayed_folder_info_stream << "Output folder:\n" << folder_name;
			m_workspace->path_output_folder = folder_name;
			m_workspace->path_filename_delimiter = filename_delimiter_char;
			break;
		default:
			break;
	}
	message_filecheck = sstm.str();
	wxMessageBox(message_filecheck.c_str(), ((loading_error > 0)? "Error" : "Info"), ((loading_error > 0)? wxICON_ERROR : wxICON_INFORMATION ) | wxOK, NULL, wxDefaultCoord, wxDefaultCoord);

	if (loading_error) {
		GuiFrame::setNotReadyStatus();
	} else {
		GuiFrame::setReadyStatus(loading_mode);
		statictext_inputfile->SetLabel(displayed_file_info_stream.str());
		statictext_outputfolder->SetLabel(displayed_folder_info_stream.str());
	}

	return;
}

void GuiFrame::loadOutputFolder(wxCommandEvent& WXUNUSED(event))
{
	wxString		selected_folder;
	std::string		folder_name;

	wxDirDialog chooseDirDialog(this, _("Choose output folder"), "", wxDD_DIR_MUST_EXIST);
	if (chooseDirDialog.ShowModal() == wxID_CANCEL)
		return;
	selected_folder = chooseDirDialog.GetPath();
	folder_name = selected_folder.ToStdString();

	m_workspace->path_output_folder = folder_name;

	std::stringstream	displayed_folder_info_stream;
	displayed_folder_info_stream << "Output folder:\n" << folder_name << "\n";
	statictext_outputfolder->SetLabel(displayed_folder_info_stream.str());

	return;
}
