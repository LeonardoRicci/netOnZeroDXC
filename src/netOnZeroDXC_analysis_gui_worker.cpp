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

#include "omp.h"

#ifndef INCLUDED_MAINAPP
	#include "netOnZeroDXC_analysis_main.hpp"
	#define INCLUDED_MAINAPP
#endif
#ifndef INCLUDED_ALGORITHM
	#include "netOnZeroDXC_algorithm.hpp"
	#define INCLUDED_ALGORITHM
#endif
#ifndef INCLUDED_ALGORITHM_GUI
	#include "netOnZeroDXC_analysis_gui_algorithm.hpp"
	#define INCLUDED_ALGORITHM_GUI
#endif
#ifndef INCLUDED_IOFUNCTIONS
	#include "netOnZeroDXC_io.hpp"
	#define INCLUDED_IOFUNCTIONS
#endif

void GuiFrame::setNotReadyStatus ()
{
	m_status_ready = 0;
	radiobox_mode->Hide();
	button_runworker->Hide();
	button_clear_stored->Hide();
	statictext_outputfolder->Hide();
	button_outputfolder->Hide();

	spinner_sampling_period->Hide();
	spinner_basewidth->Hide();
	spinner_nr_windowwidths->Hide();
	spinner_nr_surrogates->Hide();
	spinner_thr_significance->Hide();
	spinner_thr_efficiency->Hide();
	statictext_sampling_period->Hide();
	statictext_basewidth->Hide();
	statictext_nr_windowwidths->Hide();
	statictext_nr_surrogates->Hide();
	statictext_thr_significance->Hide();
	statictext_thr_efficiency->Hide();

	checkbox_source_leakage->Hide();
	statictext_source_leakage->Hide();
	spinner_source_leakage->Hide();

	statictext_save_header->Hide();
	checkbox_save_cdiagrams->Hide();
	checkbox_save_pdiagrams->Hide();
	checkbox_save_efficiencies->Hide();

	statictext_threadnum->Hide();
	checkbox_parallel_omp->Hide();
	spinner_threadnum->Hide();

	statictext_save_prefix->Hide();
	textctrl_save_prefix->Hide();

	staticline_parameters->Hide();
	staticline_run->Hide();

	int i;
	for (i = 0; i < 4; i++)
		radiobox_selectpathway->Enable(i, 1);

	combobox_delimiters->Enable();
	combobox_separators->Enable();

	m_workspace->parameter_computation_pathway = -1;

	statictext_inputfile->SetLabel("Nothing loaded.");
	statictext_outputfolder->SetLabel("No output folder chosen.");
	button_preview->Hide();

	return;
}

void GuiFrame::setReadyStatus (int chosen_pathway)
{
	m_status_ready = 1;
	radiobox_mode->Show();
	button_runworker->Show();
	button_clear_stored->Show();
	statictext_outputfolder->Show();
	button_outputfolder->Show();

	spinner_sampling_period->Show();
	spinner_basewidth->Show();
	spinner_nr_windowwidths->Show();
	spinner_nr_surrogates->Show();
	spinner_thr_significance->Show();
	spinner_thr_efficiency->Show();
	statictext_sampling_period->Show();
	statictext_basewidth->Show();
	statictext_nr_windowwidths->Show();
	statictext_nr_surrogates->Show();
	statictext_thr_significance->Show();
	statictext_thr_efficiency->Show();
	wxCommandEvent ev(wxEVT_RADIOBOX, EVENT_CHOSEN_MODE);
	radiobox_mode->GetEventHandler()->ProcessEvent(ev);

	checkbox_source_leakage->Show();

	statictext_save_header->Show();
	checkbox_save_cdiagrams->Show();
	checkbox_save_pdiagrams->Show();
	checkbox_save_efficiencies->Show();

	statictext_save_prefix->Show();
	textctrl_save_prefix->Show();

	statictext_threadnum->Show();
	checkbox_parallel_omp->Show();
	spinner_threadnum->Show();

	staticline_parameters->Show();
	staticline_run->Show();

	int i;
	for (i = 0; i < 4; i++) {
		if (i == chosen_pathway)
			radiobox_selectpathway->Enable(i, 1);
		else
			radiobox_selectpathway->Enable(i, 0);
	}

	combobox_delimiters->Disable();
	combobox_separators->Disable();
	spinner_columnr->Disable();

	m_workspace->parameter_computation_pathway = chosen_pathway;
	if (chosen_pathway == 2) {
		spinner_nr_windowwidths->SetValue(m_workspace->diagrams_pvalue[0].size());
	} else if (chosen_pathway == 3) {
		spinner_nr_windowwidths->SetValue(m_workspace->efficiencies[0].size());
	}

	return;
}

void GuiFrame::enablePreview ()
{
	button_preview->Show();
}

void GuiFrame::onDatasetClear (wxCommandEvent& WXUNUSED(event))
{
	if(m_status_ready) {
		wxMessageDialog dialog_ask_before_loading(this, "All loaded/computed data will be lost! Continue?", "Warning!", wxYES_NO | wxICON_EXCLAMATION);
		if (dialog_ask_before_loading.ShowModal() == wxID_NO)
			return;
		m_workspace->clearWorkspace();
		setNotReadyStatus();
	}
	return;
}

int GuiFrame::loadParameterTable ()
{
	m_workspace->parameter_computation_target = radiobox_mode->GetSelection();
	m_workspace->parameter_samplingperiod = spinner_sampling_period->GetValue();
	m_workspace->parameter_basewidth = spinner_basewidth->GetValue();
	m_workspace->parameter_nr_windowwidths = spinner_nr_windowwidths->GetValue();
	m_workspace->parameter_nr_surrogates = spinner_nr_surrogates->GetValue();
	m_workspace->parameter_thr_significance = spinner_thr_significance->GetValue();
	m_workspace->parameter_thr_efficiency = spinner_thr_efficiency->GetValue();

	m_workspace->parameter_print_cdiagrams = checkbox_save_cdiagrams->GetValue();
	m_workspace->parameter_print_pdiagrams = checkbox_save_pdiagrams->GetValue();
	m_workspace->parameter_print_efficiencies = checkbox_save_efficiencies->GetValue();

	m_workspace->parameter_use_shift = checkbox_source_leakage->GetValue();
	m_workspace->parameter_shift_value = spinner_source_leakage->GetValue();

	m_workspace->parameter_use_parallel = checkbox_parallel_omp->GetValue();
	m_workspace->parameter_numthreads = spinner_threadnum->GetValue();

	wxString	prefix = textctrl_save_prefix->GetLineText(0);
	m_workspace->path_output_prefix = prefix.ToStdString();

	int error = m_workspace->validateParameterTable();
	if (error) {
		return 1;
	}

	if (m_workspace->parameter_computation_pathway < 2)
		netOnZeroDXC_fill_list_pairs(m_workspace->node_pairs, m_workspace->node_labels);

	if (m_workspace->parameter_computation_pathway > 1)
		netOnZeroDXC_postfill_list_labels(m_workspace->node_labels, m_workspace->node_pairs);

	return 0;
}

bool GuiFrame::workCancelled ()
{
	wxCriticalSectionLocker lock(m_cs_cancelled);
	return m_cancelled;
}

void GuiFrame::onWorkerStart (wxCommandEvent& WXUNUSED(event))
{
	WorkerThread *thread = new WorkerThread(this);

	if (!m_status_ready || (m_workspace->parameter_computation_pathway == -1)) {
		wxMessageBox("Error:\nsomething is wrong with your data.\nMaybe a failed loading?.", "Error", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
		thread->Delete();
		return;
	}
	if (thread->Create() != wxTHREAD_NO_ERROR) {
		wxMessageBox("Runtime error:\ncannot start computation.", "Error", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
		thread->Delete();
		return;
	}

	int error = loadParameterTable();
	if (error) {
		thread->Delete();
		return;
	}

	dialog_progress = new wxProgressDialog("Running...", "Processing request:\ninitializing data for the selected pathway.\n", 100, this, wxPD_CAN_ABORT | wxPD_APP_MODAL | wxPD_ELAPSED_TIME | wxPD_REMAINING_TIME);
	m_cancelled = false;
	thread->Run();		// Run the thread
}

void GuiFrame::onWorkerEvent (wxThreadEvent& event)
{
	// static bool	dialog_is_dead = false;
	int n = event.GetInt();
	// if ((!dialog_is_dead) && ((n == -1) || (n == -2) || (n == -3) || (n == -4))) {
	if ((n == -1) || (n == -2) || (n == -3) || (n == -4)) {
		dialog_progress->Destroy();
		dialog_progress = (wxProgressDialog *) NULL;
		// dialog_is_dead = true;
		wxWakeUpIdle();
		if (n == -1)
			wxMessageBox("Computation ended.", "Info", wxOK, NULL, wxDefaultCoord, wxDefaultCoord);
		if (n == -3)
			wxMessageBox("Error while writing output files!\nPlease check paths and permissions.", "Error", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
		if (n == -4)
			wxMessageBox("Unknown error in node labels.\nPlease check file naming and labels.", "Error", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
	} else if (n == -255) {
		dialog_progress->Update(0, "Computing correlation diagrams.\nPress [Cancel] to abort.");
	} else if ( (n == -254)) {
		dialog_progress->Update(0, "Computing p-value diagrams by surrogate generation.\nThis can take a very long time.\nPress [Cancel] to abort.");
	} else if ( (n == -253)) {
		dialog_progress->Update(0, "Computing efficiencies.\nPress [Cancel] to abort.");
	} else if ( (n == -252)) {
		dialog_progress->Update(0, "Preparing data for preview.\nPress [Cancel] to abort.");
	} else if ( (n == -127)) {
		dialog_progress->Update(0, "Writing data to file.\nClosing this window will result in loss of data.");
	} else if ( (n == -126)) {
		dialog_progress->Update(0, "Writing data to file.\nClosing this window will result in loss of data.");
	} else if ( (n == -63)) {
		dialog_progress->Update(0, event.GetString());
	} else if ( (!m_cancelled)) {
		if (!dialog_progress->Update(n)) {
			wxCriticalSectionLocker lock(m_cs_cancelled);
			m_cancelled = true;
			wxMessageBox("Computation was cancelled by the user.", "Info", wxOK, NULL, wxDefaultCoord, wxDefaultCoord);
			wxThreadEvent eventKill(wxEVT_THREAD, EVENT_WORKER_UPDATE);
			eventKill.SetInt(-2);
			wxQueueEvent(this, eventKill.Clone());
		}
	}
}

WorkerThread::WorkerThread (GuiFrame *frame)
: wxThread()
{
	parent_frame = frame;
	data_container = parent_frame->m_workspace;
}

void WorkerThread::workerExit () {}

wxThread::ExitCode WorkerThread::Entry ()
{
	bool	asked_to_exit = false;
	bool	apply_shift = data_container->parameter_use_shift;
	bool	print_cdiagrams = data_container->parameter_print_cdiagrams;
	bool	print_pdiagrams = data_container->parameter_print_pdiagrams;
	bool	print_efficiencies = data_container->parameter_print_efficiencies;

	int	W = data_container->parameter_nr_windowwidths;
	int	M = data_container->parameter_nr_surrogates;
	int	shift_value = data_container->parameter_shift_value;
	int	pathway = data_container->parameter_computation_pathway;
	int	target = data_container->parameter_computation_target;
	int	L = data_container->parameter_basewidth;

	double	T = data_container->parameter_samplingperiod;
	double	alpha = data_container->parameter_thr_significance;
	double	eta_0 = data_container->parameter_thr_efficiency;

	int	number_threads;
	if (data_container->parameter_use_parallel) {
		number_threads = data_container->parameter_numthreads;
	} else {
		number_threads = -1;
	}

	char		filename_delimiter = data_container->path_filename_delimiter;
	std::string	output_path = data_container->path_output_folder;
	std::string	output_prefix = data_container->path_output_prefix;

	if (target == 0)
		print_cdiagrams = true;
	if (target == 1)
		print_pdiagrams = true;
	if (target == 2)
		print_efficiencies = true;

	if (pathway < 2) {								// If input is a set of sequences
		wxThreadEvent eventStartPath0(wxEVT_THREAD, EVENT_WORKER_UPDATE);
		eventStartPath0.SetInt(-255);
		wxQueueEvent(parent_frame, eventStartPath0.Clone());

		int	k_size = 0;
		int	k;
		if (apply_shift) {
			for (k = W*L / 2 - 1; k < data_container->sequences[0].size() - W*L / 2 - shift_value; k = k + L)
				k_size++;
		} else {
			for (k = W*L / 2 - 1; k < data_container->sequences[0].size() - W*L / 2; k = k + L)
				k_size++;
		}

		int	i, j;
		std::vector < std::vector <double> >	temp_diagram;
		data_container->diagrams_correlation.clear();
		for (i = 0; i < data_container->node_labels.size() - 1; i++) {
			for (j = i + 1; j < data_container->node_labels.size(); j++) {	// Compute all correlation diagrams
				if (parent_frame->workCancelled() || TestDestroy()) {
					asked_to_exit = 1;
					break;
				}
				netOnZeroDXC_initialize_temp_diagram(temp_diagram, k_size, W);
				netOnZeroDXC_compute_cdiagram (temp_diagram, data_container->sequences, i, j, L, W, apply_shift, shift_value);
				data_container->diagrams_correlation.push_back(temp_diagram);

				wxThreadEvent eventUpdate0(wxEVT_THREAD, EVENT_WORKER_UPDATE);
				eventUpdate0.SetInt(100 * i / data_container->node_labels.size());
				wxQueueEvent(parent_frame, eventUpdate0.Clone());
			}
			if (asked_to_exit)
				break;
		}

		if (asked_to_exit)
			return NULL;

		if (print_cdiagrams) {							// If necessary, write them in output
			wxThreadEvent eventPrint0(wxEVT_THREAD, EVENT_WORKER_UPDATE);
			eventPrint0.SetInt(-127);
			wxQueueEvent(parent_frame, eventPrint0.Clone());
			int	error;
			for (i = 0; i < data_container->node_pairs.size(); i++) {
				error = netOnZeroDXC_save_diagram(data_container->diagrams_correlation[i], output_path, output_prefix, "cdiag", '_', data_container->node_pairs[i].label_a, data_container->node_pairs[i].label_b, '\t');
				if (error) {
					wxThreadEvent eventError0(wxEVT_THREAD, EVENT_WORKER_UPDATE);
					eventError0.SetInt(-3);
					wxQueueEvent(parent_frame, eventError0.Clone());
					return NULL;
				}
			}
		}
		if (target == 0) {
			wxThreadEvent eventEnd0(wxEVT_THREAD, EVENT_WORKER_UPDATE);
			eventEnd0.SetInt(-1); // that's it
			wxQueueEvent(parent_frame, eventEnd0.Clone());
			return NULL;
		}									// Otherwise, compute all p-value diagrams

		double	progress_shared = 0.0;
		wxThreadEvent eventStartPath1(wxEVT_THREAD, EVENT_WORKER_UPDATE);
		eventStartPath1.SetInt(-254);
		wxQueueEvent(parent_frame, eventStartPath1.Clone());
		data_container->diagrams_pvalue.clear();
		k = 0;
		int	error;
		for (i = 0; i < data_container->node_labels.size() - 1; i++) {
			for (j = i + 1; j < data_container->node_labels.size(); j++) {
				netOnZeroDXC_initialize_temp_diagram(temp_diagram, k_size, W);
				asked_to_exit = netOnZeroDXC_compute_pdiagram(temp_diagram, this, data_container, progress_shared, i, j, k, M, L, W, apply_shift, shift_value, number_threads);
				k++;
				if (asked_to_exit) {
					break;
				} else {
					data_container->diagrams_pvalue.push_back(temp_diagram);

					std::stringstream	message_updated;
					message_updated << "Computing p-value diagrams by surrogate generation.\nThis can take a very long time.\nPair ";
					message_updated << k << " out of " << data_container->node_pairs.size() << "\nPress [Cancel] to abort.";
					wxString	message = message_updated.str();
					wxThreadEvent eventNewPair(wxEVT_THREAD, EVENT_WORKER_UPDATE);
					eventNewPair.SetInt(-63);
					eventNewPair.SetString(message);
					wxQueueEvent(parent_frame, eventNewPair.Clone());
					progress_shared = 0.0;

					if (print_pdiagrams) {
						error = netOnZeroDXC_save_diagram(temp_diagram, output_path, output_prefix, "pdiag", '_', data_container->node_labels[i], data_container->node_labels[j], '\t');
						if (error) {
							wxThreadEvent eventError0(wxEVT_THREAD, EVENT_WORKER_UPDATE);
							eventError0.SetInt(-3);
							wxQueueEvent(parent_frame, eventError0.Clone());
							return NULL;
						}
					}
				}
			}
			if (asked_to_exit)
				break;
		}
		asked_to_exit = parent_frame->workCancelled();

		if (asked_to_exit)
			return NULL;

		if (target == 1) {							// If this is all the user needs, exit
			wxThreadEvent eventEnd1(wxEVT_THREAD, EVENT_WORKER_UPDATE);
			eventEnd1.SetInt(-1); // that's it
			wxQueueEvent(parent_frame, eventEnd1.Clone());
			return NULL;
		}
	} // End-If (pathway < 2), i.e. end of the section to be run if we had only sequences in input.

	// When pathway < 3 (AND we haven't returned yet) efficiencies must be computed.
	if (pathway < 3) {
		wxThreadEvent eventStartPath1(wxEVT_THREAD, EVENT_WORKER_UPDATE);
		eventStartPath1.SetInt(-253);
		wxQueueEvent(parent_frame, eventStartPath1.Clone());

		int	i;
		std::vector <double>	temp_efficiency;
		data_container->efficiencies.clear();
		data_container->window_widths.clear();
		for (i = 0; i < data_container->diagrams_pvalue[0].size(); i++)
			data_container->window_widths.push_back((i + 1) * L * T);

		for (i = 0; i < data_container->diagrams_pvalue.size(); i++) {
			if (parent_frame->workCancelled() || TestDestroy()) {
				asked_to_exit = 1;
				break;
			}
			temp_efficiency.clear();
			netOnZeroDXC_compute_efficiency(temp_efficiency, data_container->diagrams_pvalue[i], alpha);
			data_container->efficiencies.push_back(temp_efficiency);

			wxThreadEvent eventUpdate2(wxEVT_THREAD, EVENT_WORKER_UPDATE);
			eventUpdate2.SetInt(100 * i / data_container->diagrams_pvalue.size());
			wxQueueEvent(parent_frame, eventUpdate2.Clone());
		}
		asked_to_exit = parent_frame->workCancelled();

		if (asked_to_exit)
			return NULL;

		if (print_efficiencies) {							// If necessary, write them in output
			wxThreadEvent eventPrint2(wxEVT_THREAD, EVENT_WORKER_UPDATE);
			eventPrint2.SetInt(-126);
			wxQueueEvent(parent_frame, eventPrint2.Clone());
			int	error;
			for (i = 0; i < data_container->node_pairs.size(); i++) {
				error = netOnZeroDXC_save_linear_data(data_container->window_widths, data_container->efficiencies[i], output_path, output_prefix, "eff", '_', data_container->node_pairs[i].label_a, data_container->node_pairs[i].label_b, '\t');
				if (error) {
					wxThreadEvent eventError2(wxEVT_THREAD, EVENT_WORKER_UPDATE);
					eventError2.SetInt(-3);
					wxQueueEvent(parent_frame, eventError2.Clone());
					return NULL;
				}
			}
		}

		if (target == 2) {
			wxThreadEvent eventEnd2(wxEVT_THREAD, EVENT_WORKER_UPDATE);
			eventEnd2.SetInt(-1); // that's it
			wxQueueEvent(parent_frame, eventEnd2.Clone());
			return NULL;
		}

		if (target == 3) {	// In case of target matrix, we prepare efficiencies at different significance thresholds
			int	k;
			std::vector < std::vector <double> >	temp_efficiency_list;
			for (k = 0; k <= 100; k++) {
				temp_efficiency_list.clear();
				for (i = 0; i < data_container->diagrams_pvalue.size(); i++) {
					if (parent_frame->workCancelled() || TestDestroy()) {
						return NULL;
					}
					temp_efficiency.clear();
					netOnZeroDXC_compute_efficiency(temp_efficiency, data_container->diagrams_pvalue[i], ((double) k) / 1000.0);
					temp_efficiency_list.push_back(temp_efficiency);
				}
				data_container->efficiencies_multialpha.push_back(temp_efficiency_list);
				wxThreadEvent eventUpdate3(wxEVT_THREAD, EVENT_WORKER_UPDATE);
				eventUpdate3.SetInt((k < 100)? k : 99);
				wxQueueEvent(parent_frame, eventUpdate3.Clone());
			}
		}
	} // End-if (pathway < 3)

	// If we haven't returned yet, the matrix of time scales must be computed.
	std::vector <double>			matrix_row(data_container->node_labels.size(), -1.0);
	std::vector < std::vector <double> >	timescale_matrix(data_container->node_labels.size(), matrix_row);

	int	i, j, k = 0;
	for (i = 0; i < data_container->node_labels.size() - 1; i++) {
		if (parent_frame->workCancelled() || TestDestroy()) {
			return NULL;
		}
		timescale_matrix[i][i] = 0.0;
		for (j = i + 1; j < data_container->node_labels.size(); j++) {
			k = netOnZeroDXC_associate_index_of_pair(data_container->node_pairs, data_container->node_labels, i, j);
			timescale_matrix[i][j] = netOnZeroDXC_compute_wmatrix_element(data_container->efficiencies[k], data_container->window_widths, eta_0);
			timescale_matrix[j][i] = timescale_matrix[i][j];
		}
	}
	timescale_matrix[i][i] = 0.0;

	wxThreadEvent eventUpdate4(wxEVT_THREAD, EVENT_WORKER_UPDATE);
	eventUpdate4.SetInt(-252);
	wxQueueEvent(parent_frame, eventUpdate4.Clone());

	if (pathway == 3) {
		std::vector < std::vector <double> >	temp_matrix(data_container->node_labels.size(), matrix_row);
		data_container->matrices_multieta.clear();
		int	eta_index;
		for (eta_index = 0; eta_index <= 100; eta_index++) {
			if (parent_frame->workCancelled() || TestDestroy()) {
				return NULL;
			}
			temp_matrix.clear();
			temp_matrix.resize(data_container->node_labels.size(), matrix_row);
			for (i = 0; i < data_container->node_labels.size() - 1; i++) {
				temp_matrix[i][i] = 0.0;
				for (j = i + 1; j < data_container->node_labels.size(); j++) {
					k = netOnZeroDXC_associate_index_of_pair(data_container->node_pairs, data_container->node_labels, i, j);
					temp_matrix[i][j] = netOnZeroDXC_compute_wmatrix_element(data_container->efficiencies[k], data_container->window_widths, ((double) eta_index) / 100.0);
					temp_matrix[j][i] = temp_matrix[i][j];
				}
			}
			temp_matrix[i][i] = 0.0;
			data_container->matrices_multieta.push_back(temp_matrix);
			wxThreadEvent eventUpdate5(wxEVT_THREAD, EVENT_WORKER_UPDATE);
			eventUpdate5.SetInt((eta_index < 100)? eta_index : 99);
			wxQueueEvent(parent_frame, eventUpdate5.Clone());
		}
	} else {
		std::vector < std::vector <double> >			temp_matrix(data_container->node_labels.size(), matrix_row);
		std::vector < std::vector < std::vector <double> > >	temp_matrix_list;
		data_container->matrices_multieta_multialpha.clear();
		int	alpha_index, eta_index;
		for (alpha_index = 0; alpha_index <= 100; alpha_index++) {
			temp_matrix_list.clear();
			for (eta_index = 0; eta_index <= 100; eta_index++) {
				temp_matrix.clear();
				temp_matrix.resize(data_container->node_labels.size(), matrix_row);
				if (parent_frame->workCancelled() || TestDestroy()) {
					return NULL;
				}
				for (i = 0; i < data_container->node_labels.size() - 1; i++) {
					temp_matrix[i][i] = 0.0;
					for (j = i + 1; j < data_container->node_labels.size(); j++) {
						k = netOnZeroDXC_associate_index_of_pair(data_container->node_pairs, data_container->node_labels, i, j);
						temp_matrix[i][j] = netOnZeroDXC_compute_wmatrix_element(data_container->efficiencies_multialpha[alpha_index][k], data_container->window_widths, ((double) eta_index) / 100.0);
						temp_matrix[j][i] = temp_matrix[i][j];
					}
				}
				temp_matrix[i][i] = 0.0;
				temp_matrix_list.push_back(temp_matrix);
			}
			data_container->matrices_multieta_multialpha.push_back(temp_matrix_list);
			wxThreadEvent eventUpdate5(wxEVT_THREAD, EVENT_WORKER_UPDATE);
			eventUpdate5.SetInt((alpha_index < 100)? alpha_index : 99);
			wxQueueEvent(parent_frame, eventUpdate5.Clone());
		}
	}

	parent_frame->enablePreview();

	int	error;
	error = netOnZeroDXC_save_diagram(timescale_matrix, output_path, output_prefix, "matrix", '_', "", "", '\t');
	if (error) {
		wxThreadEvent eventError4(wxEVT_THREAD, EVENT_WORKER_UPDATE);
		eventError4.SetInt(-3);
		wxQueueEvent(parent_frame, eventError4.Clone());
		return NULL;
	}

	wxThreadEvent eventFinal(wxEVT_THREAD, EVENT_WORKER_UPDATE);
	eventFinal.SetInt(-1); // that's it
	wxQueueEvent(parent_frame, eventFinal.Clone());
	return NULL;

}

ContainerWorkspace::ContainerWorkspace ()
{
	clearWorkspace();
};

void ContainerWorkspace::clearWorkspace ()
{
	parameter_computation_pathway = -1;
	parameter_computation_target = -1;
	parameter_basewidth = -1;
	parameter_nr_windowwidths = -1;
	parameter_nr_surrogates = -1;
	parameter_shift_value = -1;
	parameter_samplingperiod = -1.0;
	parameter_thr_significance = -1.0;
	parameter_thr_efficiency = -1.0;
	parameter_use_shift = false;
	parameter_print_cdiagrams = 0;
	parameter_print_pdiagrams = 0;
	parameter_print_efficiencies = 0;
	parameter_use_parallel = false;
	parameter_numthreads = 1;

	sequences.clear();
	diagrams_correlation.clear();
	diagrams_pvalue.clear();
	efficiencies.clear();
	window_widths.clear();
	node_labels.clear();
	node_pairs.clear();

	matrices_multieta.clear();
	efficiencies_multialpha.clear();
	matrices_multieta_multialpha.clear();

	path_filename_delimiter = '_';
	path_output_folder.clear();
	path_output_prefix.clear();
}

int ContainerWorkspace::validateParameterTable ()
{
	if ((parameter_samplingperiod <= 0) || (parameter_basewidth <= 0)) {
		wxMessageBox("Error: unable to read parameters.\nSomething went wrong: please check parameter controls.", "Error", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
		return 1;
	}
	if ((parameter_basewidth % 2 ) != 0) {
		parameter_basewidth = parameter_basewidth - 1;
		wxMessageBox("Warning! Base width must be an even number of points.\nBasewidth was reduced by 1 with respect to the loaded settings.", "Warning", wxOK | wxICON_EXCLAMATION, NULL, wxDefaultCoord, wxDefaultCoord);
	}
	if (parameter_computation_pathway < 2) {
		if ((parameter_basewidth * parameter_nr_windowwidths / 2 + 1) > sequences[0].size()) {
			wxMessageBox("Error: too large / too many windows.\nWindow settings and sequences length are incompatible.", "Error", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
			return 2;
		}
	}
	if (path_output_prefix.size()) {
		if (path_output_prefix.find_first_of(" ") != std::string::npos) {
			wxMessageBox("Error: bad file names.\nPrefix of file names should not contain spaces.", "Error", wxOK | wxICON_ERROR, NULL, wxDefaultCoord, wxDefaultCoord);
			return 3;
		}
	}

	std::stringstream	sstmpnt;
	if ((parameter_use_parallel) && (parameter_numthreads > omp_get_max_threads())) {
		sstmpnt << "Warning! Too many requested threads!\nCreating more threads than the number of logical cores would result in loss of performances.\n";
		sstmpnt << " > Requested " << parameter_numthreads << "\n";
		sstmpnt << " > Detected " << omp_get_max_threads() << "\n";
		sstmpnt << "Will continue anyway, using the maximum number of threads (" << omp_get_max_threads() << ").";
		parameter_numthreads = omp_get_max_threads();
		wxMessageBox(sstmpnt.str(), "Warning", wxOK | wxICON_EXCLAMATION, NULL, wxDefaultCoord, wxDefaultCoord);
	}

	std::stringstream	sstm;
	if (parameter_computation_pathway == 3) {
		if (window_widths[0] != (parameter_samplingperiod * parameter_basewidth)) {
			sstm << "Warning! Inconsistent window widths!\nThe window width read in the loaded files does not match the parameters settings.\n";
			sstm << " > In the loaded file, w = " << window_widths[0] << "\n";
			sstm << " > In your settings, w = " << parameter_samplingperiod * parameter_basewidth << "\n";
			sstm << "Which setting should be used?\n";
			wxArrayString	choices;
			choices.Add("Those from the loaded file ");
			choices.Add("Those from the settings");
			int	choice = wxGetSingleChoiceIndex(sstm.str(), "Warning", choices);
			if (choice == 1) {
				int	i;
				for (i = 0; i < window_widths.size(); i++)
					window_widths[i] = (i + 1) * (parameter_samplingperiod * parameter_basewidth);
			}
		}
	}

	return 0;
}

void GuiFrame::popupPreview (wxCommandEvent& WXUNUSED(event))
{
	if (m_workspace->matrices_multieta.size() || m_workspace->matrices_multieta_multialpha.size()) {
		new PlotFrame("Preview matrix of time scales", this);
	}

	return;
}
