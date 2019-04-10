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
#include <ctime>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "omp.h"

#ifndef INCLUDED_MAINAPP
	#include "netOnZeroDXC_analysis_main.hpp"
	#define INCLUDED_MAINAPP
#endif
#ifndef INCLUDED_ALGORITHM
	#include "netOnZeroDXC_algorithm.hpp"
	#define INCLUDED_ALGORITHM
#endif


int netOnZeroDXC_compute_pdiagram (std::vector < std::vector <double> > & pvalue_diagram, WorkerThread* owner_thread, ContainerWorkspace* workspace,
				double & progress, int index_a, int index_b, int index_diagram, int M, int w_base, int W, bool apply_shift,
				int shift, int number_threads)
{
	std::vector <double>	distribution_values_a, distribution_values_b;
	std::vector <double>	fft_amplitudes_a, fft_amplitudes_b;

	netOnZeroDXC_initialize_surrogate_generation(distribution_values_a, fft_amplitudes_a, workspace->sequences, index_a);
	netOnZeroDXC_initialize_surrogate_generation(distribution_values_b, fft_amplitudes_b, workspace->sequences, index_b);
	std::vector <double>	temp_vector(workspace->diagrams_correlation[index_diagram][0].size(), 0.0);

	bool	go_flag = 1;
	int	old_progress = -1;
	int	i;
	unsigned int	seed;
	double	progress_step;

	if (number_threads > 1) {
		omp_set_num_threads(number_threads);

		progress_step = 100.0 * number_threads / ((double) M);

		int	threads_to_run;
		i = 0;
		while (i < M) {
			if (owner_thread->TestDestroy() || owner_thread->parent_frame->workCancelled()) {
				go_flag = 0;
				break;
			}

			threads_to_run = ((M - i) < number_threads)? (M - i) : number_threads;
			seed = (unsigned int) clock() + 2*i;

			#pragma omp parallel for firstprivate(seed)
			for (int j = 0; j < threads_to_run; j++) {
				std::vector < std::vector <double> >	surrogate_cdiagram(W, temp_vector);
				std::vector <double>			sequence_surrogate_a;
				std::vector <double>			sequence_surrogate_b;
				std::vector < std::vector <double> >	sequences_surrogate;

				seed = seed + 2*j;
				netOnZeroDXC_generate_surrogate_sequence(sequence_surrogate_a, workspace->sequences, index_a, distribution_values_a, fft_amplitudes_a, TOLERANCE_SURROGATES, seed);
				seed = seed + 1;
				netOnZeroDXC_generate_surrogate_sequence(sequence_surrogate_b, workspace->sequences, index_b, distribution_values_b, fft_amplitudes_b, TOLERANCE_SURROGATES, seed);
				sequences_surrogate.push_back(sequence_surrogate_a);
				sequences_surrogate.push_back(sequence_surrogate_b);
				netOnZeroDXC_compute_cdiagram(surrogate_cdiagram, sequences_surrogate, 0, 1, w_base, W, apply_shift, shift);
				#pragma omp critical
				{
					netOnZeroDXC_update_pdiagram(pvalue_diagram, workspace->diagrams_correlation[index_diagram], surrogate_cdiagram, W, M);
					i++;
				}
			}

			if (((int) progress) != old_progress) {
				old_progress = (progress >= 100)? 99 : (int) progress;	// Progress dialog is nasty, values > 100 will make it crash in a bad way.
				wxThreadEvent eventProgress(wxEVT_THREAD, EVENT_WORKER_UPDATE);
				eventProgress.SetInt((int) old_progress);
				wxQueueEvent(owner_thread->parent_frame, eventProgress.Clone());
			}
			progress += progress_step;
		}
	} else {
		std::vector < std::vector <double> >	surrogate_cdiagram(W, temp_vector);
		std::vector <double>			sequence_surrogate_a;
		std::vector <double>			sequence_surrogate_b;
		std::vector < std::vector <double> >	sequences_surrogate;
		progress_step = 100.0 / ((double) M);

		for (i = 0; i < M; i++) {
			if (owner_thread->TestDestroy() || owner_thread->parent_frame->workCancelled()) {
				go_flag = 0;
				break;
			}

			seed = (unsigned int) clock();

			sequences_surrogate.clear();
			netOnZeroDXC_generate_surrogate_sequence(sequence_surrogate_a, workspace->sequences, index_a, distribution_values_a, fft_amplitudes_a, TOLERANCE_SURROGATES, seed);
			seed = seed + 1;
			netOnZeroDXC_generate_surrogate_sequence(sequence_surrogate_b, workspace->sequences, index_b, distribution_values_b, fft_amplitudes_b, TOLERANCE_SURROGATES, seed);
			sequences_surrogate.push_back(sequence_surrogate_a);
			sequences_surrogate.push_back(sequence_surrogate_b);

			netOnZeroDXC_compute_cdiagram(surrogate_cdiagram, sequences_surrogate, 0, 1, w_base, W, apply_shift, shift);
			netOnZeroDXC_update_pdiagram(pvalue_diagram, workspace->diagrams_correlation[index_diagram], surrogate_cdiagram, W, M);

			if (((int) progress) != old_progress) {
				old_progress = (progress >= 100)? 99 : (int) progress;
				wxThreadEvent eventProgress(wxEVT_THREAD, EVENT_WORKER_UPDATE);
				eventProgress.SetInt((int) old_progress);
				wxQueueEvent(owner_thread->parent_frame, eventProgress.Clone());
			}
			progress += progress_step;
		}
	}


	if (!go_flag) {
		return 1;
	}

	return 0;
}
