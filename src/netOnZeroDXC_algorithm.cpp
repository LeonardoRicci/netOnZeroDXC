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

#include <gsl/gsl_rng.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>

#ifndef INCLUDED_ALGORITHM
	#include "netOnZeroDXC_algorithm.hpp"
	#define INCLUDED_ALGORITHM
#endif

struct PairValueId {
	int index;
	double value;
};

bool netOnZeroDXC_sort_values (PairValueId a, PairValueId b) {return a.value < b.value;}

double netOnZeroDXC_compute_wmatrix_element (const std::vector <double> & efficiency, const std::vector <double> & window_widths, double threshold_eta)
{
	int	i;
	for (i = 0; i < efficiency.size(); i++) {
		if (efficiency[i] > threshold_eta)
			return window_widths[i];
	}

	return -1.0;
}

int netOnZeroDXC_compute_efficiency (std::vector <double> & efficiency, const std::vector < std::vector <double> > & diagram, double threshold_alpha)
{
	efficiency.clear();

	int	i, j, n;
	double	eta;
	for (i = 0; i < diagram.size(); i++) {
		eta = 0.0;
		n = 0;
		for (j = 0; j < diagram[i].size(); j++) {
			if (diagram[i][j] < threshold_alpha)
				eta += 1.0;
			n++;
		}
		efficiency.push_back(eta / (double) n);
	}

	return 0;
}

int netOnZeroDXC_compute_cdiagram (std::vector < std::vector <double> > & correlation_diagram, const std::vector < std::vector <double> > & sequences,
				int node_a, int node_b, int w_base, int W, bool apply_shift, int shift)
{
	int	l, j, k, ws;
	double	temp;
	if (apply_shift) {
		for (l = 0; l < W; l++) {
			j = 0;
			ws = (l + 1) * w_base;
			for (k = W * w_base / 2 - 1; k < sequences[node_a].size() - W * w_base / 2 - shift; k = k + w_base) {
				temp = 0.5 * netOnZeroDXC_compute_crosscorr(sequences, node_a, node_b, k + shift - ws/2 + 1, k + shift + ws/2, k - ws/2 + 1, k + ws/2);
				temp += 0.5 * netOnZeroDXC_compute_crosscorr(sequences, node_a, node_b, k - ws/2 + 1, k + ws/2, k + shift - ws/2 + 1, k + shift + ws/2);
				correlation_diagram[l][j] = temp;
				j++;
			}
		}
	} else {
		for (l = 0; l < W; l++) {
			j = 0;
			ws = (l + 1) * w_base;
			for (k = W * w_base / 2 - 1; k < sequences[node_a].size() - W * w_base / 2; k = k + w_base) {
				correlation_diagram[l][j] = netOnZeroDXC_compute_crosscorr(sequences, node_a, node_b, k - ws/2 + 1, k + ws/2, k - ws/2 + 1, k + ws/2);
				j++;
			}
		}
	}

	return 0;
}

int netOnZeroDXC_update_pdiagram (std::vector < std::vector <double> > & pvalue_diagram, const std::vector < std::vector <double> > & cdiagram_data,
				const std::vector < std::vector <double> > & cdiagram_surr, int W, int M)
{
	int	K = pvalue_diagram[0].size();
	int	l, k;
	for (l = 0; l < W; l++) {
		k = 0;
		for (k = 0; k < K; k++) {
			if (cdiagram_data[l][k] < cdiagram_surr[l][k]) {
				pvalue_diagram[l][k] += 1.0 / (double) M;
			}
		}
	}

	return 0;
}

int netOnZeroDXC_initialize_surrogate_generation (std::vector <double> & values_distribution, std::vector <double> & fft_amplitudes,
						const std::vector < std::vector <double> > & sequences, int index)
{
	int	N	= sequences[index].size();
	double	*data	= new double[N];

	gsl_fft_real_wavetable		*wavetable_real		= gsl_fft_real_wavetable_alloc(N);
	gsl_fft_real_workspace		*workspace		= gsl_fft_real_workspace_alloc(N);

	values_distribution.clear();
	int	i;
	for (i = 0; i < N; i++) {
		data[i] = sequences[index][i];
		values_distribution.push_back(sequences[index][i]);
	}
	std::sort(values_distribution.begin(), values_distribution.end());

	gsl_fft_real_transform(data, 1, N, wavetable_real, workspace);
	fft_amplitudes.clear();
	for (i = 0; i < N; i++) {
		if (i == 0) {
			fft_amplitudes.push_back(fabs(data[i]));
		} else if ((i < N-1) || (N%2 != 0)) {
			fft_amplitudes.push_back(sqrt(data[i]*data[i] + data[i+1]*data[i+1]));
			i++;
		} else {
			fft_amplitudes.push_back(fabs(data[i]));
		}
	}

	delete[] data;
	gsl_fft_real_workspace_free(workspace);
	gsl_fft_real_wavetable_free(wavetable_real);

	return 0;
}

int netOnZeroDXC_generate_surrogate_sequence (std::vector <double> & surrogate_sequence, const std::vector < std::vector <double> > & sequences, int index,
					const std::vector <double> & values_distribution, const std::vector <double> & fft_amplitudes, double tolerance,
					unsigned int random_engine_seed)
{

	int	N		= sequences[index].size();
	double	*data		= new double[N];
	double	*data_prev_iter = new double[N];

	std::vector <double>	original_sequence(N,0);
	int	i;
	for (i = 0; i < N; i++) {
		data[i] = sequences[index][i];
		original_sequence[i] = sequences[index][i];
	}

	// Scramble randomly the original sequence
	int	r = 0;
	gsl_rng *random_generator = gsl_rng_alloc(gsl_rng_mt19937);
	gsl_rng_set(random_generator, random_engine_seed);
	for (i = 0; i < N; i++) {
		r = gsl_rng_uniform_int(random_generator, original_sequence.size());
		data[i] = original_sequence[r];
		original_sequence.erase(original_sequence.begin() + r);
	}

	// Iteratively refine
	gsl_fft_real_wavetable		*wavetable_real		= gsl_fft_real_wavetable_alloc(N);
	gsl_fft_halfcomplex_wavetable	*wavetable_halfcomplex	= gsl_fft_halfcomplex_wavetable_alloc(N);
	gsl_fft_real_workspace		*workspace		= gsl_fft_real_workspace_alloc(N);
	while(r < 1000) {
		r++;
		gsl_fft_real_transform(data, 1, N, wavetable_real, workspace);
		netOnZeroDXC_restore_fft_amplitude(data, fft_amplitudes, N);
		gsl_fft_halfcomplex_inverse(data, 1, N, wavetable_halfcomplex, workspace);
		netOnZeroDXC_rescale_sequence(data, values_distribution, N);
		if (netOnZeroDXC_check_iteration_convergence(data, data_prev_iter, N, tolerance))
			break;
		memcpy(data_prev_iter, data, N * sizeof(double));
	}

	surrogate_sequence.clear();
	for (i = 0; i < N; i++)
		surrogate_sequence.push_back(data[i]);


	delete[] data;
	delete[] data_prev_iter;

	gsl_fft_halfcomplex_wavetable_free(wavetable_halfcomplex);
	gsl_fft_real_wavetable_free (wavetable_real);
	gsl_fft_real_workspace_free (workspace);
	gsl_rng_free(random_generator);

	return 0;
}



int netOnZeroDXC_restore_fft_amplitude (double * data, const std::vector <double> & fft_amplitudes, int N)
{
	int	i;
	double	phase;
	for (i = 0; i < N; i++) {
		if (i == 0) {
			data[i] = fft_amplitudes[i/2];			// Zero freq. is real (FT symmetry)
		} else if ((i < N-1) || (N%2 != 0)) {
			phase = atan2(data[i+1], data[i]);
			data[i] = fft_amplitudes[i/2 + 1] * cos(phase);
			data[i+1] = fft_amplitudes[i/2 + 1] * sin(phase);
			i++;
		} else {
			data[i] = fft_amplitudes[i/2 + 1];		// Also last freq. is real, if N is even (FT symmetry)
		}
	}

	return 0;
}

int netOnZeroDXC_rescale_sequence (double * data, const std::vector <double> & values_distribution, int N)
{
	std::vector <PairValueId>	temp_vector(N);
	int	i;
	for (i = 0; i < N; i++) {
		temp_vector[i].value = data[i];
		temp_vector[i].index = i;
	}

	sort(temp_vector.begin(), temp_vector.end(), netOnZeroDXC_sort_values);

	for (i = 0; i < N; i++)
		data[temp_vector[i].index] = values_distribution[i];

	return 0;
}

bool netOnZeroDXC_check_iteration_convergence (double * data, double * data_prev_iter, int N, double tolerance)
{
	int	i;
	double	z = 0, I = 0;		// Compare the sum of squared differences (i.e. approximately N*std.dev.) with the signal energy
	for (i = 0; i < N; i++) {
		z += (data[i] - data_prev_iter[i])*(data[i] - data_prev_iter[i]);
		I += data[i] * data[i];
	}

	if ((z / I) > tolerance)
		return false;
	else
		return true;
}

double netOnZeroDXC_compute_crosscorr (const std::vector < std::vector <double> > & sequences, int index_a, int index_b,
				int start_a, int end_a, int start_b, int end_b)
{
	int	j = 0;
	int	n = 0;
	double	mean_a = 0;
	for (j = start_a; j <= end_a; j++) {
		mean_a += sequences[index_a][j];
		n++;
	}
	mean_a /= (double) n;
	n = 0;

	double	mean_b = 0;
	for (j = start_b; j <= end_b; j++) {
		mean_b += sequences[index_b][j];
		n++;
	}
	mean_b /= (double) n;

	double	norm_a = 0;
	for (j = start_a; j <= end_a; j++)
		norm_a += (sequences[index_a][j] - mean_a) * (sequences[index_a][j] - mean_a);

	double	norm_b = 0;
	for (j = start_b; j <= end_b; j++)
		norm_b += (sequences[index_b][j] - mean_b) * (sequences[index_b][j] - mean_b);

	int	ss = start_a;
	int	ee = end_a;
	int	r1 = 0;
	int	r2 = 0;
	if (start_a > start_b) {
		ss = start_b;
		ee = end_b;
		r1 = (start_a - start_b);
	} else if (start_a < start_b) {
		r2 = (start_b - start_a);
	}

	double	scalar_product = 0;
	for (j = ss; j <= ee; j++)
		scalar_product += (sequences[index_a][j + r1] - mean_a) * (sequences[index_b][j + r2] - mean_b);

	scalar_product /= sqrt(norm_a);
	scalar_product /= sqrt(norm_b);

	return scalar_product;
}

void netOnZeroDXC_initialize_temp_diagram(std::vector < std::vector <double> > & diagram, int size_x, int size_y)
{
	std::vector <double>	temp_row(size_x, 0);

	diagram.clear();
	int	i;
	for (i = 0; i < size_y; i++)
		diagram.push_back(temp_row);

	return;
}
