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
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>

#ifndef INCLUDED_MAINAPP
	#include "netOnZeroDXC_merge_main.hpp"
	#define INCLUDED_MAINAPP
#endif

#ifndef INCLUDED_IOFUNCTIONS
	#include "netOnZeroDXC_io.hpp"
	#define INCLUDED_IOFUNCTIONS
#endif

#ifndef INCLUDED_ALGORITHM
#include "netOnZeroDXC_algorithm.hpp"
	#define INCLUDED_ALGORITHM
#endif

void GuiFrame::validatePreprocessData (wxCommandEvent& WXUNUSED(event))
{
	int	i, j;

	if (status_ready) {
		for (i = 0; i < data_container->number_of_systems; i++) {
			for (j = 0; j < data_container->systems_stored[i].recordings_stored.size(); j++) {
				data_container->systems_stored[i].recordings_stored[j].matrices_multieta.clear();
			}
		}
		unsetReadyStatus();
		return;
	}

	if (data_container->number_of_systems == 0) {
		wxMessageBox("No data to validate...", "Warning", wxICON_EXCLAMATION | wxOK);
		return;
	}
	if (data_container->number_of_systems != data_container->systems_stored.size()) {
		wxMessageBox("Validation failed.\nLoaded data are inconsistent (maybe a loading error).", "Error", wxICON_ERROR | wxOK);
		return;
	}
	int	nr_recordings = data_container->systems_stored[0].recordings_stored.size();

	if (nr_recordings == 0) {
		wxMessageBox("Validation failed.\nSome systems have no associated recordings.", "Error", wxICON_ERROR | wxOK);
		return;
	}
	for (i = 0; i < data_container->number_of_systems; i++) {
		if (nr_recordings != data_container->systems_stored[i].recordings_stored.size()) {
			wxMessageBox("Validation failed.\nThe number of recordings is not equal across systems.", "Error", wxICON_ERROR | wxOK);
			return;
		}
	}
	data_container->number_of_recordings = nr_recordings;

	int	r, s;
	std::stringstream		sstm;
	std::vector <std::string>	offending_systems;
	std::vector <std::string>	offending_recordings;
	for (s = 0; s < data_container->number_of_systems; s++) {
		for (r = 0; r < data_container->number_of_recordings; r++) {
			if ((data_container->systems_stored[s].recordings_stored[r].efficiencies.size() == 0) && (data_container->systems_stored[s].recordings_stored[r].matrix_timescales.size() == 0)) {
				offending_systems.push_back(data_container->systems_stored[s].system_name);
				offending_recordings.push_back(data_container->systems_stored[s].recordings_stored[r].recording_name);
			}
		}
	}
	if (offending_systems.size() > 0) {
		sstm << "Validation failed.\nSome recordings have no loaded data.\nList of empty recordings (system -> recording):\n";
		for (r = 0; r < offending_systems.size(); r++)
			sstm << "- " << offending_systems[r] << " -> " << offending_recordings[r] << ".\n";
		wxMessageBox(sstm.str(), "Error", wxICON_ERROR | wxOK);
		return;
	}

	int	number_of_pairs = 0, number_of_widths = 0;
	bool	variable_eta = false;
	if (data_container->systems_stored[0].recordings_stored[0].efficiencies.size() > 0) {
		variable_eta = true;
		number_of_pairs = data_container->systems_stored[0].recordings_stored[0].efficiencies.size();
	}
	data_container->multiple_eta = variable_eta;

	if (variable_eta) {
		number_of_widths = data_container->systems_stored[0].recordings_stored[0].efficiencies[0].size();
		if (variable_eta && (data_container->window_widths.size() != number_of_widths)) {
			wxMessageBox("Validation failed.\nInconsistent lengths of window widths and efficiencies found.", "Error", wxICON_ERROR | wxOK);
			return;
		}
	}

	for (s = 0; s < data_container->number_of_systems; s++) {
		for (r = 0; r < data_container->number_of_recordings; r++) {
			if (variable_eta) {
				if (data_container->systems_stored[s].recordings_stored[r].efficiencies.size() != number_of_pairs) {
					wxMessageBox("Validation failed.\nInconsistent number of loaded efficiencies found.", "Error", wxICON_ERROR | wxOK);
					return;
				}
				for (i = 0; i < data_container->systems_stored[s].recordings_stored[r].efficiencies.size(); i++) {
					if (data_container->systems_stored[s].recordings_stored[r].efficiencies[i].size() != number_of_widths) {
						wxMessageBox("Validation failed.\nInconsistent length of loaded efficiencies found.", "Error", wxICON_ERROR | wxOK);
						return;
					}
				}
			} else {
				if (data_container->systems_stored[s].recordings_stored[r].matrix_timescales.size() != data_container->number_of_nodes) {
					wxMessageBox("Validation failed.\nInconsistent size of loaded matrices found.", "Error", wxICON_ERROR | wxOK);
					return;
				}
			}
		}
	}

	int	k;
	if (variable_eta) {
		netOnZeroDXC_postfill_list_labels(data_container->node_labels, data_container->node_pairs);
		data_container->number_of_nodes = data_container->node_labels.size();
		int	s, r, eta_index;
		std::vector <double>	temp_row(data_container->number_of_nodes, -1.0);
		std::vector < std::vector <double> >	temp_matrix;
		for (s = 0; s < data_container->number_of_systems; s++) {
			for (r = 0; r < data_container->number_of_recordings; r++) {
				for (eta_index = 0; eta_index <= 100; eta_index++) {
					temp_matrix.clear();
					temp_matrix.resize(data_container->number_of_nodes, temp_row);
					for (i = 0; i < data_container->number_of_nodes - 1; i++) {
						temp_matrix[i][i] = 0.0;
						for (j = i + 1; j < data_container->number_of_nodes; j++) {
							k = netOnZeroDXC_associate_index_of_pair(data_container->node_pairs, data_container->node_labels, i, j);
							temp_matrix[i][j] = netOnZeroDXC_compute_wmatrix_element(data_container->systems_stored[s].recordings_stored[r].efficiencies[k],
														data_container->window_widths, ((double) eta_index) / 100.0);
							temp_matrix[j][i] = temp_matrix[i][j];
						}
					}
					temp_matrix[i][i] = 0.0;
					data_container->systems_stored[s].recordings_stored[r].matrices_multieta.push_back(temp_matrix);
				}
			}
		}
	} else {
		double	w_max = -1.0;
		int	r, s;
		for (s = 0; s < data_container->number_of_systems; s++) {
			for (r = 0; r < data_container->number_of_recordings; r++) {
				for (i = 0; i < data_container->number_of_nodes - 1; i++) {
					for (j = i + 1; j < data_container->number_of_nodes; j++) {
						if (data_container->systems_stored[s].recordings_stored[r].matrix_timescales[i][j] > w_max)
							w_max = data_container->systems_stored[s].recordings_stored[r].matrix_timescales[i][j];
					}
				}
			}
		}
		if (w_max < 0) {
			wxMessageBox("Validation failed.\nAll loaded matrices have only negative elements!", "Error", wxICON_ERROR | wxOK);
			return;
		}
		for (i = 1; i <= 100; i++) {
			data_container->window_widths.push_back(w_max * ((double) i) / 100.0);
		}
	}
	sstm << "Validation succeeded.\n";
	sstm << "There are " << data_container->number_of_systems << " systems, each containing " << data_container->number_of_recordings << " recordings.\n";
	sstm << "Loaded data are " << ((variable_eta)? "efficiencies" : "matrices") << "\n";
	if (!variable_eta)
		sstm << "Maximum detected time scale is " << data_container->window_widths.back() << "\n";
	if (data_container->available_node_labels) {
		sstm << "User-defined labels were loaded or detected.\n";
	} else {
		char	*str_buffer = new char[64];
		data_container->node_labels.clear();
		for (i = 0; i < data_container->number_of_nodes; i++) {
			sprintf(str_buffer, "%d", i + 1);
			data_container->node_labels.push_back(std::string(str_buffer));
		}
		delete str_buffer;
		sstm << "No labels were loaded. Integer numbers will be assigned as labels.\n";
	}
	sstm << "Everything is fine.";

	wxMessageBox(sstm.str(), "Info", wxICON_INFORMATION | wxOK);

	spinner_ranking_recordings->SetRange(1, data_container->number_of_recordings);
	spinner_ranking_systems->SetRange(1, data_container->number_of_systems);

	setReadyStatus();

	return;
}

void GuiFrame::addObservedSystem (wxCommandEvent& WXUNUSED(event))
{
	wxString name = wxGetTextFromUser(wxT("Insert system label:"), wxT("Adding new system"), wxT("mysystem"), this);

	if (name == "")
		return;

	int	i;
	for (i = 0; i < data_container->number_of_systems; i++) {
		if (data_container->systems_stored[i].system_name == name) {
			wxMessageBox("Error. System names must be unique.", "Error", wxICON_ERROR | wxOK);
			return;
		}
	}

	ObservedSystem temp_system = ObservedSystem(name.ToStdString());
	data_container->systems_stored.push_back(temp_system);
	data_container->number_of_systems += 1;
	listbox_systems->Append(name);

	return;
}

void GuiFrame::addObservedRecording (wxCommandEvent& WXUNUSED(event))
{
	int system_index = listbox_systems->GetSelection();
	if (system_index == wxNOT_FOUND) {
		wxMessageBox("No systems selected.\nCannot add recording without a parent system.", "Error", wxICON_ERROR | wxOK);
		return;
	}

	int	loading_mode = radiobox_input->GetSelection();
	if (loading_mode == 0) {
		ObservedRecording temp_rec = ObservedRecording("loading...");
		data_container->systems_stored[system_index].addObservedRecording(temp_rec);
		loadEfficiencyFiles(system_index, data_container->systems_stored[system_index].recordings_stored.size() - 1);
	} else {
		loadMatrixFiles(system_index);
	}

	wxCommandEvent ev = wxCommandEvent(wxEVT_NULL, 0);
	refreshListboxRecordings(ev);

	return;
}

void GuiFrame::deleteObservedSystem (wxCommandEvent& WXUNUSED(event))
{
	int	system_index = listbox_systems->GetSelection();
	if (system_index == wxNOT_FOUND) {
		wxMessageBox("No systems selected to be deleted.", "Error", wxICON_ERROR | wxOK);
		return;
	}

	data_container->systems_stored.erase(data_container->systems_stored.begin() + system_index);
	data_container->number_of_systems -= 1;

	wxCommandEvent ev = wxCommandEvent(wxEVT_NULL, 0);
	refreshListboxSystems(ev);

	return;
}

void GuiFrame::deleteObservedRecording (wxCommandEvent& WXUNUSED(event))
{
	int	system_index = listbox_systems->GetSelection();
	int	recording_index = listbox_recordings->GetSelection();
	if ((system_index == wxNOT_FOUND) || (recording_index == wxNOT_FOUND)) {
		wxMessageBox("Invalid selection, cannot delete anything.", "Error", wxICON_ERROR | wxOK);
		return;
	}

	data_container->systems_stored[system_index].recordings_stored.erase(data_container->systems_stored[system_index].recordings_stored.begin() + recording_index);

	wxCommandEvent ev = wxCommandEvent(wxEVT_NULL, 0);
	refreshListboxRecordings(ev);

	return;
}

void GuiFrame::deleteAllData (wxCommandEvent& WXUNUSED(event))
{
	if(data_container->systems_stored.size()) {

		wxMessageDialog dialog_ask_before_clearing(this, "All loaded/computed data will be lost! Continue?", "Warning!", wxYES_NO | wxICON_EXCLAMATION);

		if (dialog_ask_before_clearing.ShowModal() == wxID_NO)
			return;

		data_container->clearWorkspace();
		wxCommandEvent ev = wxCommandEvent(wxEVT_NULL, 0);
		refreshListboxSystems(ev);
		radiobox_input->Enable();
	}
	button_dictionary_load->Hide();
	unsetReadyStatus();

	return;
}

int GuiFrame::getRecordingIndex ()
{
	return	listbox_recordings->GetSelection();
}

int GuiFrame::getSystemIndex ()
{
	return	listbox_systems->GetSelection();
}

ContainerWorkspace::ContainerWorkspace ()
{
	available_node_labels = false;
	clearWorkspace();
}

void ContainerWorkspace::evaluateMergedMatrix (int rank_systems, int rank_recordings, int eta_index)
{
	double	huge = std::numeric_limits<double>::max();
	std::vector <double>			temp_row(number_of_nodes, -1.0);
	ranked_matrix.clear();
	ranked_matrix.resize(number_of_nodes, temp_row);

	int	i, j, s;
	std::vector <double>	temp_list;
	for (i = 0; i < number_of_nodes - 1; i++) {
		ranked_matrix[i][i] = 0.0;
		for (j = i + 1; j < number_of_nodes; j++) {
			temp_list.clear();
			for (s = 0; s < number_of_systems; s++) { // traverse the systems
				temp_list.push_back(systems_stored[s].mergeRecordingMatrices(i, j, rank_recordings, eta_index));
			}
			partial_sort(temp_list.begin(), temp_list.begin() + rank_systems, temp_list.end());
			ranked_matrix[i][j] = (temp_list[rank_systems - 1] == huge)? -1.0 : temp_list[rank_systems - 1];
			ranked_matrix[j][i] = ranked_matrix[i][j];
		}
	}
	ranked_matrix[i][i] = 0.0;

	return;
}

void ContainerWorkspace::evaluateSystemMatrix (int system_index, int rank_recordings, int eta_index)
{
	double	huge = std::numeric_limits<double>::max();
	std::vector <double>			temp_row(number_of_nodes, -1.0);
	ranked_matrix.clear();
	ranked_matrix.resize(number_of_nodes, temp_row);

	int	i, j;
	double	temp_element;
	for (i = 0; i < number_of_nodes - 1; i++) {
		ranked_matrix[i][i] = 0.0;
		for (j = i + 1; j < number_of_nodes; j++) {
			temp_element = systems_stored[system_index].mergeRecordingMatrices(i, j, rank_recordings, eta_index);
			ranked_matrix[i][j] = (temp_element == huge)? -1.0 : temp_element;
			ranked_matrix[j][i] = ranked_matrix[i][j];
		}
	}
	ranked_matrix[i][i] = 0.0;

	return;
}

void ContainerWorkspace::clearWorkspace ()
{
	number_of_systems = 0;
	number_of_recordings = 0;
	number_of_nodes = 0;

	multiple_eta = false;

	systems_stored.clear();
	ranked_matrix.clear();
	window_widths.clear();
	node_pairs.clear();
	node_labels.clear();

	return;
}

ObservedSystem::ObservedSystem (const std::string name)
{
	system_name = name;
}

double ObservedSystem::mergeRecordingMatrices (int i, int j, int rank_recordings, int eta_index)
{
	double	huge = std::numeric_limits<double>::max();
	int	number_of_recordings = recordings_stored.size();
	std::vector <double>	temp_list;
	int	r;
	for (r = 0; r < number_of_recordings; r++) { // traverse the recordings
		if (eta_index < 0) {
			temp_list.push_back(recordings_stored[r].matrix_timescales[i][j]);
		} else {
			temp_list.push_back(recordings_stored[r].matrices_multieta[eta_index][i][j]);
		}
	}
	for (r = 0; r < temp_list.size(); r++) {
		if (temp_list[r] == -1.0)
			temp_list[r] = huge;
	}
	partial_sort(temp_list.begin(), temp_list.begin() + rank_recordings, temp_list.end());
	return temp_list[rank_recordings - 1];
}

ObservedRecording::ObservedRecording (const std::string name)
{
	recording_name = name;
}

void ObservedSystem::addObservedRecording (ObservedRecording added_recording)
{
	recordings_stored.push_back(added_recording);
	return;
}
