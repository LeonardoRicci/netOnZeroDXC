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
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#ifndef INCLUDED_MAINAPP
	#include "netOnZeroDXC_merge_main.hpp"
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

void GuiFrame::loadEfficiencyFiles (int system_index, int recording_index)
{
	wxArrayString	list_of_files;
	wxString	selected_file_name;
	std::string	file_name;
	int		found_last_dir_separator;

	wxFileDialog openFileDialog(this, _("Load data files"), "", "", "all files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		data_container->systems_stored[system_index].recordings_stored.erase(data_container->systems_stored[system_index].recordings_stored.begin() + recording_index);
    		return;
	}
	openFileDialog.GetPaths(list_of_files);
	file_name = list_of_files[0].ToStdString();

	char	separator_char = '\t';
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

	char	filename_delimiter_char = '_';
	wxString	delimiter_label = combobox_delimiters->GetStringSelection();
	if (delimiter_label == combobox_delimiters_element_0) {
		filename_delimiter_char = combobox_delimiters_char_0;
	} else if (delimiter_label == combobox_delimiters_element_1) {
		filename_delimiter_char = combobox_delimiters_char_1;
	} else {
		wxMessageBox("Invalid filename delimiter.\nContinuing with default (_).", "Warning!", wxOK | wxICON_EXCLAMATION, NULL, wxDefaultCoord, wxDefaultCoord);
		filename_delimiter_char = combobox_delimiters_char_0;
		delimiter_label = combobox_delimiters_element_0;
		combobox_delimiters->SetSelection(0);
	}

	std::string	temp_recording_name = file_name.substr(file_name.find_last_of("/\\") + 1, std::string::npos);
	temp_recording_name = temp_recording_name.substr(0, temp_recording_name.find_last_of(filename_delimiter_char));
	temp_recording_name = temp_recording_name.substr(0, temp_recording_name.find_last_of(filename_delimiter_char));

	int	loading_error;
	std::stringstream		sstm;
	std::vector <PairOfLabels>	temp_label_pairs;
	data_container->systems_stored[system_index].recordings_stored[recording_index].efficiencies.clear();
	loading_error = netOnZeroDXC_load_multi_efficiencies(data_container->systems_stored[system_index].recordings_stored[recording_index].efficiencies,
			 					temp_label_pairs, data_container->systems_stored[system_index].recordings_stored[recording_index].window_widths,
								list_of_files, separator_char, filename_delimiter_char);
	if (loading_error == 0) {
		if (data_container->node_pairs.size()) {
			if (netOnZeroDXC_check_new_label_pairs(temp_label_pairs, data_container->node_pairs) != 0) {
				loading_error = 4;
			}
			if (data_container->window_widths.size() != data_container->systems_stored[system_index].recordings_stored[recording_index].window_widths.size()) {
				loading_error = 5;
			} else if (data_container->window_widths[0] != data_container->systems_stored[system_index].recordings_stored[recording_index].window_widths[0]) {
				loading_error = 5;
			}
		} else {
			data_container->node_pairs = temp_label_pairs;
			data_container->window_widths = data_container->systems_stored[system_index].recordings_stored[recording_index].window_widths;
		}
	}

	switch (loading_error) {
		case 1:
			sstm << "Error in reading files!\nFilenames and labels cannot be correctly parsed. Maybe a wrong separator?\nNoting was loaded.";
			break;
		case 2:
			sstm << "Error in loading files!\nUnknown I/O error when opening the file(s).\nNoting was loaded.";
			break;
		case 3:
			sstm << "Error in efficiencies format!\nInconsistent efficiency lengths, or inconsistent window widths.\nNoting was loaded.";
			break;
		case 4:
			sstm << "Error in loading files!\nLabels are inconsistent with previously loaded files,\nor pair duplicates have been found.\nNothing was loaded";
			break;
		case 5:
			sstm << "Error in loading efficiencies!\nFile length or window widths are inconsistent with previously loaded files.\nNothing was loaded";
			break;
		case 0:
			sstm << "Successfully read " << list_of_files.size() << " efficiency files.\n";
			sstm << "Loaded " << data_container->systems_stored[system_index].recordings_stored[recording_index].efficiencies.size() << " efficiencies with separator " << separator_label << ", content read:\n";
			sstm << "- Detected data rows (i.e. window widths): " << data_container->systems_stored[system_index].recordings_stored[recording_index].efficiencies[0].size() << "\n";
			sstm << "Consistency between efficiencies was checked. Everything is fine.";
			break;
		default:
			break;
	}
	wxMessageBox(sstm.str(), ((loading_error > 0)? "Error" : "Info"), ((loading_error > 0)? wxICON_ERROR : wxICON_INFORMATION ) | wxOK, NULL, wxDefaultCoord, wxDefaultCoord);

	if (!loading_error) {
		radiobox_input->Disable();
		data_container->systems_stored[system_index].recordings_stored[recording_index].recording_name = temp_recording_name;
		data_container->available_node_labels = true;
	} else {
		data_container->systems_stored[system_index].recordings_stored.erase(data_container->systems_stored[system_index].recordings_stored.begin() + recording_index);
	}

	return;
}

void GuiFrame::loadMatrixFiles (int system_index)
{
	wxArrayString	list_of_files;
	wxString	selected_file_name;
	std::string	file_name;
	int		found_last_dir_separator;
	std::string	folder_name;
	wxFileDialog openFileDialog(this, _("Load data files"), "", "", "all files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;

	openFileDialog.GetPaths(list_of_files);
	file_name = list_of_files[0].ToStdString();
	found_last_dir_separator = file_name.find_last_of("/\\");
	folder_name = file_name.substr(0, found_last_dir_separator);

	char	separator_char = '\t';
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

	int	i;
	int	loading_error;
	std::vector < std::vector <double> >	temp_matrix;
	std::stringstream	sstm;
	std::string		recording_name;
	for (i = 0; i < list_of_files.size(); i++) {
		sstm.str(std::string());
		sstm.clear();
		file_name = list_of_files[i].ToStdString();

		loading_error = netOnZeroDXC_load_single_matrix(temp_matrix, file_name, separator_char);

		if (loading_error == 0) {
			if (data_container->number_of_nodes > 0) {
				if (temp_matrix.size() != data_container->number_of_nodes) {
					loading_error = 3;
				}
			} else {
				data_container->number_of_nodes = temp_matrix.size();
			}
		}
		switch (loading_error) {
			case 1:
				sstm << "Error in reading file! (File: " << file_name << ")\nFilenames cannot be correctly parsed. Maybe a wrong delimiter?\nNoting was loaded.";
				break;
			case 2:
				sstm << "Error in loading file! (File: " << file_name << ")\n.Unknown I/O error when opening the file(s).\nNoting was loaded.";
				break;
			case 3:
				sstm << "Error in matrix format! (File: " << file_name << ")\nMatrix size inconsistent with previously loaded files,\nor matrix is not square.\nNoting was loaded.";
				break;
			default:
				break;
		}
		if (loading_error) {
			wxMessageBox(sstm.str(), "Error", wxICON_ERROR | wxOK, NULL, wxDefaultCoord, wxDefaultCoord);
			return;
		} else {
			recording_name = file_name.substr(file_name.find_last_of("/\\") + 1, std::string::npos);
			recording_name = recording_name.substr(0, recording_name.find_first_of("."));
			sstm << recording_name;
			ObservedRecording temp_rec = ObservedRecording(sstm.str());
			temp_rec.matrix_timescales = temp_matrix;
			data_container->systems_stored[system_index].addObservedRecording(temp_rec);
			wxCommandEvent ev = wxCommandEvent(wxEVT_NULL, 0);
			refreshListboxRecordings(ev);
		}
	}
	sstm.str(std::string());
	sstm.clear();
	sstm << "Successfully loaded " << i << " matrices with separator " << separator_label << ", content read:\n";
	sstm << "- Matrices for " << data_container->number_of_nodes << " nodes.\n";
	sstm << "Matrices are square. Everything is fine.";
	wxMessageBox(sstm.str(), "Info", wxICON_INFORMATION | wxOK, NULL, wxDefaultCoord, wxDefaultCoord);

	radiobox_input->SetSelection(1);
	radiobox_input->Disable();
	button_dictionary_load->Show();

	return;
}

void GuiFrame::configuredLoadFiles (wxCommandEvent& WXUNUSED(event))
{
	wxString	selected_file_name;
	std::string	file_name;
	int		found_last_dir_separator;
	std::string	folder_name;
	wxFileDialog openFileDialog(this, _("Load data file"), "", "", "all files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;
	selected_file_name = openFileDialog.GetPath();
	file_name = selected_file_name.ToStdString();
	found_last_dir_separator = file_name.find_last_of("/\\");
	folder_name = file_name.substr(0, found_last_dir_separator);

	char	separator_char = '\t';
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

	char	filename_delimiter_char = '_';
	wxString	delimiter_label = combobox_delimiters->GetStringSelection();
	if (delimiter_label == combobox_delimiters_element_0) {
		filename_delimiter_char = combobox_delimiters_char_0;
	} else if (delimiter_label == combobox_delimiters_element_1) {
		filename_delimiter_char = combobox_delimiters_char_1;
	} else {
		wxMessageBox("Invalid filename delimiter.\nContinuing with default (_).", "Warning!", wxOK | wxICON_EXCLAMATION, NULL, wxDefaultCoord, wxDefaultCoord);
		filename_delimiter_char = combobox_delimiters_char_0;
		delimiter_label = combobox_delimiters_element_0;
		combobox_delimiters->SetSelection(0);
	}

	std::ifstream	selected_file_stream;
	selected_file_stream.open(file_name.c_str(), std::ifstream::in);
	if (selected_file_stream.fail()) {
		wxMessageBox("Error in opening configuration file.\nPlease check file permissions.\nNothing was loaded.", "Error!", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
		return;
	}

	int	error;
	char	*buffered_line = new char[256];
	bool	matrix_mode = true;
	bool	temp_matrix_mode;
	std::string	line;
	std::string	system_name;
	std::string	recording_name;
	std::string	temp_file_name;
	std::vector <ObservedSystem>	temp_loaded_observations;
	std::vector <PairOfLabels>	temp_loaded_pair_labels;
	std::vector <PairOfLabels>	loaded_pair_labels;
	while(selected_file_stream.getline(buffered_line, 256)) {
		line = buffered_line;
		if (line.size()) {
			error = netOnZeroDXC_parse_configuration_line(system_name, recording_name, temp_matrix_mode, temp_file_name, line, separator_char);
			if (error == 1) {
				wxMessageBox("Error in reading configuration file.\nFormat is invalid, maybe a wrong separator?\nNothing was loaded.", "Error!", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
				break;
			} else if (error == 2) {
				wxMessageBox("Error in reading configuration file.\nInvalid loading mode specifier label. Please use 'm' or 'e' only.\nNothing was loaded.", "Error!", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
				break;
			}
			if (data_container->number_of_systems > 0) {
				if ((data_container->multiple_eta && matrix_mode) || ((!data_container->multiple_eta) && (!matrix_mode))) {
					wxMessageBox("Error in reading configuration file.\nLoading mode label is inconsistent with previously loaded data.\nNothing was loaded.", "Error!", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
					error = 1;
					break;
				}
			}

			ObservedRecording	temp_recording = ObservedRecording(recording_name);
			if (temp_loaded_observations.size() == 0) {
				matrix_mode = temp_matrix_mode;
				error = loadConfiguredRecording(temp_recording, temp_loaded_pair_labels, matrix_mode, temp_file_name, folder_name, separator_char, filename_delimiter_char);
				if (error)
					break;
				if (!matrix_mode && (data_container->number_of_systems == 0)) {
					loaded_pair_labels = temp_loaded_pair_labels;
				} else if (!matrix_mode) {
					loaded_pair_labels = temp_loaded_pair_labels;
					error = netOnZeroDXC_check_new_label_pairs(temp_loaded_pair_labels, data_container->node_pairs);
				}
				if (error)
					break;

				ObservedSystem		temp_system = ObservedSystem(system_name);
				temp_system.addObservedRecording(temp_recording);
				temp_loaded_observations.push_back(temp_system);
			} else {
				if (matrix_mode != temp_matrix_mode) {
					wxMessageBox("Error in reading configuration file.\nLoading mode specifier labels are inconsistent across the file.\nNothing was loaded.", "Error!", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
					break;
				}
				error = loadConfiguredRecording(temp_recording, temp_loaded_pair_labels, matrix_mode, temp_file_name, folder_name, separator_char, filename_delimiter_char);
				if (error)
					break;
				if (!matrix_mode) {
					error = netOnZeroDXC_check_new_label_pairs(temp_loaded_pair_labels, loaded_pair_labels);
				}
				if (error)
					break;
				if (system_name == temp_loaded_observations.back().system_name) {
					temp_loaded_observations.back().addObservedRecording(temp_recording);
				} else {
					ObservedSystem		temp_system = ObservedSystem(system_name);
					temp_system.addObservedRecording(temp_recording);
					temp_loaded_observations.push_back(temp_system);
				}
			}
		}
	}
	selected_file_stream.close();
	delete	buffered_line;

	if (error)
		return;

	int	i, j;
	if (data_container->number_of_systems == 0) {
		if (matrix_mode) {
			data_container->number_of_nodes = temp_loaded_observations[0].recordings_stored[0].matrix_timescales.size();
		} else {
			data_container->window_widths = temp_loaded_observations[0].recordings_stored[0].window_widths;
		}
	}

	if (matrix_mode) {
		for (i = 0; i < temp_loaded_observations.size(); i++) {
			for (j = 0; j < temp_loaded_observations[i].recordings_stored.size(); j++) {
				if (temp_loaded_observations[i].recordings_stored[j].matrix_timescales.size() != data_container->number_of_nodes) {
					wxMessageBox("Error in loading data from configuration file.\nMatrices sizes are inconsistent.\nNothing was loaded.", "Error!", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
					return;
				}
			}
		}
	} else {
		for (i = 0; i < temp_loaded_observations.size(); i++) {
			for (j = 0; j < temp_loaded_observations[i].recordings_stored.size(); j++) {
				if (temp_loaded_observations[i].recordings_stored[j].efficiencies.size() != temp_loaded_observations[0].recordings_stored[0].efficiencies.size()) {
					wxMessageBox("Error in loading data from configuration file.\nNumber of efficiency files is inconsistent.\nNothing was loaded.", "Error!", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
					return;
				}
				if (temp_loaded_observations[i].recordings_stored[j].efficiencies[0].size() != data_container->window_widths.size()) {
					wxMessageBox("Error in loading data from configuration file.\nLengths of efficiencies are inconsistent.\nNothing was loaded.", "Error!", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
					return;
				}
				if (temp_loaded_observations[i].recordings_stored[j].window_widths[0] != data_container->window_widths[0]) {
					wxMessageBox("Error in loading data from configuration file.\nWindow widths are inconsistent.\nNothing was loaded.", "Error!", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
					return;
				}
			}
		}
	}

	// Got here, means we're *good*
	if (!matrix_mode) {
		data_container->node_pairs = loaded_pair_labels;
		data_container->available_node_labels = true;
		radiobox_input->SetSelection(0);
		radiobox_input->Disable();
	} else {
		radiobox_input->SetSelection(1);
		radiobox_input->Disable();
		button_dictionary_load->Show();
	}
	for (i = 0; i < temp_loaded_observations.size(); i++) {
		data_container->systems_stored.push_back(temp_loaded_observations[i]);
		data_container->number_of_systems += 1;
		listbox_systems->Append(temp_loaded_observations[i].system_name);
	}
	std::stringstream	sstm;
	sstm << "Successfully parsed and fulfilled configuration file '" << file_name << "'\n";
	sstm << "Created " << temp_loaded_observations.size() << " systems.\n";
	wxMessageBox(sstm.str(), "Info", wxOK | wxICON_INFORMATION, NULL, wxDefaultCoord, wxDefaultCoord);

	return;
}

void GuiFrame::loadLabelsDictionary (wxCommandEvent& WXUNUSED(event))
{
	wxString	selected_file_name;
	if(data_container->available_node_labels) {
		wxMessageDialog dialog_ask_before_loading(this, "A list of labels is already present.\nBy proceeding in a new load operation, labels will be overwritten. Continue?", "Warning!", wxYES_NO | wxICON_EXCLAMATION);
		if (dialog_ask_before_loading.ShowModal() == wxID_NO)
			return;
		data_container->node_labels.clear();
		data_container->available_node_labels = false;
	}
	wxFileDialog openFileDialog(this, _("Load data file"), "", "", "all files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;
	selected_file_name = openFileDialog.GetPath();

	char	separator_char = '\t';
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

	std::stringstream		sstm;
	std::vector <std::string>	temp_labels;
	int				loading_error;
	loading_error = netOnZeroDXC_load_labels_dictionary(data_container->node_labels, selected_file_name.ToStdString(), separator_char);
	if ((loading_error == 0) && (data_container->node_labels.size() != data_container->number_of_nodes))
		loading_error = 4;
	switch (loading_error) {
		case 1:
			sstm << "Error in reading labels!\nFile could not be opened.\nNoting was loaded.";
			break;
		case 2:
			sstm << "Error in reading labels!\nDuplicates or invalid labels found.\nNoting was loaded.";
			break;
		case 3:
			sstm << "Error in reading labels indexes!\nIndexes must be consecutive and must start from 0 or 1.\nNoting was loaded.";
			break;
		case 4:
			sstm << "Error in loading labels!\nThe selected file contains a number of labels inconsistent with the already loaded data.\nNothing was loaded.";
			break;
		case 0:
			sstm << "Successfully read '" << selected_file_name.ToStdString() << "'.\n";
			sstm << "Loaded " << data_container->node_labels.size() << " labels with separator " << separator_label << ".\n";
			sstm << "Consistency of indexes and label uniqueness were checked. Everything is fine.";
			break;
		default:
			break;
	}
	wxMessageBox(sstm.str(), ((loading_error > 0)? "Error" : "Info"), ((loading_error > 0)? wxICON_ERROR : wxICON_INFORMATION ) | wxOK, NULL, wxDefaultCoord, wxDefaultCoord);

	if (!loading_error) {
		radiobox_input->SetSelection(1);
		radiobox_input->Disable();
		data_container->available_node_labels = true;
	}

	return;
}

int GuiFrame::loadConfiguredRecording (ObservedRecording & recording, std::vector<PairOfLabels> & label_pairs, bool matrix_mode,
					const std::string & file_name, const std::string & folder_name, char separator, char filename_delimiter)
{
	char	directory_char = '/';
	if (folder_name.find_first_of('\\') != std::string::npos)
		directory_char = '\\';

	std::stringstream	file_fullpath;
	file_fullpath << folder_name;
	if ((folder_name.find_last_of(directory_char) != folder_name.size()) && (file_name.find_first_of(directory_char) != 0))
		file_fullpath << directory_char;
	file_fullpath << file_name;

	int	loading_error;
	std::stringstream	sstm;
	if (matrix_mode) {
		loading_error = netOnZeroDXC_load_single_matrix(recording.matrix_timescales, file_fullpath.str(), separator);
		switch (loading_error) {
			case 2:
				sstm << "Error in reading file " << file_fullpath.str() << "\nPlease check file permissions.\nNothing was loaded.";
				break;
			case 3:
				sstm << "Error in format of matrix file " << file_fullpath.str() << "\nMatrix is not square?\nNothing was loaded.";
				break;
			default:
				break;
		}
	} else {
		std::string		selected_file_name = file_fullpath.str();
		std::ifstream		selected_file_stream;
		selected_file_stream.open(selected_file_name.c_str(), std::ifstream::in);
		if (selected_file_stream.fail()) {
			sstm << "Error in opening efficiencies list " << selected_file_name << "\nCannot load anything.";
			wxMessageBox(sstm.str(), "Error!", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
			return 1;
		}
		wxArrayString	list_of_files;
		std::string	line;
		char	*buffered_line = new char[256];
		while(selected_file_stream.getline(buffered_line, 256)) {
			std::stringstream	temp_file_path;
			std::string		line = buffered_line;
			if (line.size()) {
				temp_file_path << folder_name;
				if ((folder_name.find_last_of(directory_char) != folder_name.size()) && (line.find_first_of(directory_char) != 0))
					temp_file_path << directory_char;
				temp_file_path << line;
			}
			list_of_files.Add(temp_file_path.str());
		}
		delete buffered_line;
		loading_error = netOnZeroDXC_load_multi_efficiencies(recording.efficiencies, label_pairs, recording.window_widths, list_of_files, separator, filename_delimiter);
		switch (loading_error) {
			case 1:
				sstm << "Error in reading files!\nFilenames and labels cannot be correctly parsed. Maybe a wrong separator?\nNoting was loaded.";
				break;
			case 2:
				sstm << "Error in loading files!\nUnknown I/O error when opening the file(s).\nNoting was loaded.";
				break;
			case 3:
				sstm << "Error in efficiencies format!\nInconsistent efficiency lengths, or inconsistent window widths.\nNoting was loaded.";
				break;
			case 4:
				sstm << "Error in loading files!\nLabels are inconsistent, or pair duplicates have been found.\nNothing was loaded";
				break;
			case 5:
				sstm << "Error in loading efficiencies!\nFile length or window widths are inconsistent.\nNothing was loaded";
				break;
			default:
				break;
		}
	}

	if (loading_error) {
		wxMessageBox(sstm.str(), "Error!", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
		return 1;
	}

	return 0;
}

void GuiFrame::saveMergedSystems (wxCommandEvent& WXUNUSED(event))
{
	wxString		selected_folder;
	std::string		folder_name;

	wxDirDialog choose_folder_dialog(this, _("Choose output folder"), "", wxDD_DIR_MUST_EXIST);
	if (choose_folder_dialog.ShowModal() == wxID_CANCEL)
		return;
	selected_folder = choose_folder_dialog.GetPath();
	folder_name = selected_folder.ToStdString();

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

	std::string		filename_prefix;
	std::stringstream	sstm;
	sstm << "Type a prefix for the output files.\nFiles will be named as\n\t<prefix>" << filename_delimiter_char << "<system name>.dat\n";
	sstm << "The default prefix is 'merged'";
	wxString selected_prefix = wxGetTextFromUser(sstm.str(), wxT("Output file prefix"), wxT(""), this);
	if (selected_prefix == "") {
		filename_prefix = "merged";
	} else {
		filename_prefix = selected_prefix.ToStdString();
	}

	char	separator_char = '\t';
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

	int	eta_index = (data_container->multiple_eta)? ((int) (spinner_thr_efficiency->GetValue() * 100)) : -1;

	int	s, error;
	for (s = 0; s < data_container->number_of_systems; s++) {
		data_container->evaluateSystemMatrix(s, spinner_ranking_recordings->GetValue(), eta_index);
		error = netOnZeroDXC_save_diagram(data_container->ranked_matrix, folder_name, filename_prefix, data_container->systems_stored[s].system_name, filename_delimiter_char, "", "", separator_char);
		if (error) {
			wxMessageBox("Some error occurred while saving files.\nOutput files are probably damaged.", "Error!", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
			return;
		}
	}

	std::stringstream	logfile_content;
	logfile_content << "# This log file was automatically-generated by netOnZeroDXC-merge.\n";
	logfile_content << "# Merged matrices were written for each of the " << data_container->number_of_systems << " system(s).\n";
	logfile_content << "# Each systems had " << data_container->number_of_recordings << " recording(s).\n";
	logfile_content << "# Results were written to files in folder " << folder_name << "\n";
	logfile_content << "# with filename prefix " << filename_prefix << "\n";
	logfile_content << "# Loaded data consisted of " << ((data_container->multiple_eta)? "efficiency files" : "a matrix") << " for each recording.\n";
	logfile_content << "# Parameters and characteristics:\n";
	logfile_content << "# - Number of nodes: " << data_container->ranked_matrix.size() << "\n";
	if (data_container->multiple_eta)
		logfile_content << "# - Efficiency threshold: " << spinner_thr_efficiency->GetValue() << "\n";
	logfile_content << "# - Recording-wise rank: " << spinner_ranking_recordings->GetValue() << "\n";

	std::string		temp_filename;
	std::stringstream	filename_log;
	temp_filename = netOnZeroDXC_generate_filepath(folder_name, filename_prefix, "info", filename_delimiter_char, "", "");
	temp_filename = temp_filename.substr(0, temp_filename.find_last_of("."));
	filename_log << temp_filename << ".log";
	netOnZeroDXC_save_log_file(logfile_content, filename_log.str()); // this is error-uncatched, for now

	return;
}

void GuiFrame::saveMergedGlobal (wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog	save_file_dialog(this, wxT("Save global merged matrix"), "", "", "any file (*.*)|*.*", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (save_file_dialog.ShowModal() == wxID_CANCEL)
		return;

	wxString file_name = save_file_dialog.GetPath();

	char	separator_char = '\t';
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

	int	eta_index = (data_container->multiple_eta)? ((int) (spinner_thr_efficiency->GetValue() * 100)) : -1;
	data_container->evaluateMergedMatrix(spinner_ranking_systems->GetValue(), spinner_ranking_recordings->GetValue(), eta_index);

	int	error;
	error = netOnZeroDXC_save_single_file(data_container->ranked_matrix, file_name.ToStdString(), separator_char);
	if (error) {
		wxMessageBox("Some error occurred while saving file.\nOutput file is probably damaged.", "Error!", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
	}

	std::stringstream	logfile_content;
	logfile_content << "# This log file was automatically-generated by netOnZeroDXC-merge.\n";
	logfile_content << "# A single merged matrix was written in output, merging information from " << data_container->number_of_systems << " system(s).\n";
	logfile_content << "# Each systems had " << data_container->number_of_recordings << " recording(s).\n";
	logfile_content << "# Results were written to file " << file_name.ToStdString() << "\n";
	logfile_content << "# Loaded data consisted of " << ((data_container->multiple_eta)? "efficiency files" : "a matrix") << " for each recording.\n";
	logfile_content << "# Parameters and characteristics:\n";
	logfile_content << "# - Number of nodes: " << data_container->ranked_matrix.size() << "\n";
	if (data_container->multiple_eta)
		logfile_content << "# - Efficiency threshold: " << spinner_thr_efficiency->GetValue() << "\n";
	logfile_content << "# - Recording-wise rank: " << spinner_ranking_recordings->GetValue() << "\n";
	logfile_content << "# - Systems-wise rank: " << spinner_ranking_systems->GetValue() << "\n";

	std::string		temp_filename = file_name.ToStdString();
	std::stringstream	filename_log;
	temp_filename = temp_filename.substr(0, temp_filename.find_last_of("."));
	filename_log << temp_filename << "_info.log";
	netOnZeroDXC_save_log_file(logfile_content, filename_log.str()); // this is error-uncatched, for now

	return;
}
