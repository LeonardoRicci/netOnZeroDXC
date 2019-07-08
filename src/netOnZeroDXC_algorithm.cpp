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
#include <ctime>
#include <cstring>
#include <algorithm>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

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

int netOnZeroDXC_compute_efficiency (std::vector <double> & efficiency, const std::vector < std::vector <double> > & diagram, double threshold_alpha, bool avoid_overlapping)
{
	efficiency.clear();

	int	i, j, n;
	double	eta;

	if (!avoid_overlapping) {
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
	} else if (avoid_overlapping) {
		for (i = 0; i < diagram.size(); i++) {
			eta = 0.0;
			n = 0;
			for (j = 0; j < diagram[i].size(); j += (i + 1)) {
				if (diagram[i][j] < threshold_alpha)
					eta += 1.0;
				n++;
			}
			efficiency.push_back(eta / (double) n);
		}
	}

	return 0;
}

int netOnZeroDXC_compute_cdiagram (std::vector < std::vector <double> > & correlation_diagram, std::vector < std::vector <double> > & pvalue_diagram_fisher,
					const std::vector < std::vector <double> > & sequences, int node_a, int node_b, int w_base, int W, bool apply_shift, int shift)
{
	int	l, j, k, ws;
	double	cross_correlation_coefficient, f_statistics;
	if (apply_shift) {
		for (l = 0; l < W; l++) {
			j = 0;
			ws = (l + 1) * w_base;
			for (k = W * w_base / 2 - 1; k < sequences[node_a].size() - W * w_base / 2 - shift; k = k + w_base) {
				cross_correlation_coefficient = 0.5 * netOnZeroDXC_compute_crosscorr(sequences, node_a, node_b, k + shift - ws/2 + 1, k + shift + ws/2, k - ws/2 + 1, k + ws/2);
				cross_correlation_coefficient += 0.5 * netOnZeroDXC_compute_crosscorr(sequences, node_a, node_b, k - ws/2 + 1, k + ws/2, k + shift - ws/2 + 1, k + shift + ws/2);
				correlation_diagram[l][j] = cross_correlation_coefficient;
				f_statistics = ((double) ws) / (1.0/(cross_correlation_coefficient*cross_correlation_coefficient) - 1.0);
				pvalue_diagram_fisher[l][j] = netOnZeroDXC_cdf_f_distribution_Q(f_statistics, 1.0, ws - 2.0);

				j++;
			}
		}
	} else {
		for (l = 0; l < W; l++) {
			j = 0;
			ws = (l + 1) * w_base;
			for (k = W * w_base / 2 - 1; k < sequences[node_a].size() - W * w_base / 2; k = k + w_base) {
				cross_correlation_coefficient = netOnZeroDXC_compute_crosscorr(sequences, node_a, node_b, k - ws/2 + 1, k + ws/2, k - ws/2 + 1, k + ws/2);
				correlation_diagram[l][j] = cross_correlation_coefficient;
				f_statistics = ((double) ws) / (1.0/(cross_correlation_coefficient*cross_correlation_coefficient) - 1.0);
				pvalue_diagram_fisher[l][j] = netOnZeroDXC_cdf_f_distribution_Q(f_statistics, 1.0, ws - 2.0);

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

double netOnZeroDXC_compute_wholeseq_crosscorr (const std::vector < std::vector <double> > & sequences, int index_a, int index_b, bool apply_shift, int shift)
{
	int	i;
	double	cross_correlation = 0.0;
	if (apply_shift) {
		cross_correlation = 0.5 * netOnZeroDXC_compute_crosscorr(sequences, index_a, index_b, 0, sequences[index_a].size()-shift-1, shift, sequences[index_b].size()-1);
		cross_correlation += 0.5 * netOnZeroDXC_compute_crosscorr(sequences, index_a, index_b, shift, sequences[index_a].size()-1, 0, sequences[index_b].size()-shift-1);
	} else {
		cross_correlation = netOnZeroDXC_compute_crosscorr(sequences, index_a, index_b, 0, sequences[index_a].size()-1, 0, sequences[index_b].size()-1);
	}
	return	cross_correlation;
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

	double	var_a = 0;
	for (j = start_a; j <= end_a; j++)
		var_a += (sequences[index_a][j] - mean_a) * (sequences[index_a][j] - mean_a);

	double	var_b = 0;
	for (j = start_b; j <= end_b; j++)
		var_b += (sequences[index_b][j] - mean_b) * (sequences[index_b][j] - mean_b);

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

	double	cross_correlation_coefficient = 0;
	for (j = ss; j <= ee; j++)
		cross_correlation_coefficient += (sequences[index_a][j + r1] - mean_a) * (sequences[index_b][j + r2] - mean_b);

	cross_correlation_coefficient /= sqrt(var_a);
	cross_correlation_coefficient /= sqrt(var_b);

	return cross_correlation_coefficient;
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

double netOnZeroDXC_cdf_f_distribution_Q(double x, int nu1, int nu2)	// Numerical recipes, 6.14.10
{
	double d1 = (double) nu1 / 2.0;
	double d2 = (double) nu2 / 2.0;
	double y = d1*x;

	if ( x <= 0.0 ) {
		return 1.0;
	} else {
		return (1.0 - netOnZeroDXC_incbeta(d1, d2, y/(d2+y)));
	}
}

double netOnZeroDXC_incbeta(double a, double b, double x)	// Numerical recipes 6.4
{
	if ((a <= 0.0) || (b <= 0.0)) {
		std::cerr << "ERROR: negative degrees of freedom when evaluating F distribution! Exiting...\n";
		exit(1);
	}
	if (x <= 0.0) return 0.0;
	else if (x >= 1.0) return 1.0;

	if ((a > 3000) || (b > 3000)) {
		return netOnZeroDXC_incbeta_approx(a, b, x);
	}

	double bt = exp(netOnZeroDXC_gamma_logarithm(a+b)-netOnZeroDXC_gamma_logarithm(a)-netOnZeroDXC_gamma_logarithm(b)+a*log(x)+b*log(1.0-x));

	if (x < (a+1.0)/(a+b+2.0))
		return bt*netOnZeroDXC_incbeta_continued_fraction(a,b,x)/a;
	else
		return 1.0 - bt*netOnZeroDXC_incbeta_continued_fraction(b,a,1.0-x)/b;
}

double netOnZeroDXC_incbeta_continued_fraction(double a, double b, double x)	// Numerical recipes 6.4
{
	const double EPS = std::numeric_limits<double>::epsilon();
	const double FPMIN = std::numeric_limits<double>::min()/EPS;
	int	m, m2;
	double	aa, c, d, del, h, qab, qam, qap;
	qab=a+b;
	qap=a+1.0;
	qam=a-1.0;
	c=1.0;
	d=1.0-qab*x/qap;
	if (fabs(d) < FPMIN) d=FPMIN;
	d=1.0/d;
	h=d;
	for (m=1;m<10000;m++) {
		m2=2*m;
		aa=m*(b-m)*x/((qam+m2)*(a+m2));
		d=1.0+aa*d;
		if (fabs(d) < FPMIN)
			d=FPMIN;
		c=1.0+aa/c;
		if (fabs(c) < FPMIN)
			c=FPMIN;
		d=1.0/d;
		h *= d*c;
		aa = -(a+m)*(qab+m)*x/((a+m2)*(qap+m2));
		d=1.0+aa*d;
		if (fabs(d) < FPMIN)
			d=FPMIN;
		c=1.0+aa/c;
		if (fabs(c) < FPMIN)
			c=FPMIN;
		d=1.0/d;
		del=d*c;
		h *= del;
		if (fabs(del-1.0) <= EPS)
			break;
	}
		return h;
}

double netOnZeroDXC_incbeta_approx(double a, double b, double x)	// Numerical recipes 6.4
{
	double	y[18] = {0.0021695375159141994,				// Gauss-Legendre quadrature, NR 6.2
			0.011413521097787704,0.027972308950302116,0.051727015600492421,
			0.082502225484340941, 0.12007019910960293,0.16415283300752470,
			0.21442376986779355, 0.27051082840644336, 0.33199876341447887,
			0.39843234186401943, 0.46931971407375483, 0.54413605556657973,
			0.62232745288031077, 0.70331500465597174, 0.78649910768313447,
			0.87126389619061517, 0.95698180152629142};
	double	w[18] = {0.0055657196642445571,
			0.012915947284065419,0.020181515297735382,0.027298621498568734,
			0.034213810770299537,0.040875750923643261,0.047235083490265582,
			0.053244713977759692,0.058860144245324798,0.064039797355015485,
			0.068745323835736408,0.072941885005653087,0.076598410645870640,
			0.079687828912071670,0.082187266704339706,0.084078218979661945,
			0.085346685739338721,0.085983275670394821};
	int	j;
	double	xu, t, sum, ans;
	double	a1 = a-1.0, b1 = b-1.0, mu = a/(a+b);
	double	lnmu = log(mu), lnmuc = log(1.0-mu);
	t = sqrt(a*b/((a+b)*(a+b)*(a+b+1.0)));
	if (x > a/(a+b)) {
		if (x >= 1.0)
			return 1.0;
		xu = std::min(1.0, std::max(mu + 10.0*t, x + 5.0*t));
	} else {
		if (x <= 0.0)
			return 0.0;
		xu = std::max(0.0, std::min(mu - 10.0*t, x - 5.0*t));
	}
	sum = 0.0;
	for (j = 0; j < 18; j++) {
		t = x + (xu-x)*y[j];
		sum += w[j] * exp(a1*(log(t)-lnmu) + b1*(log(1-t)-lnmuc));
	}
	ans = sum * (xu-x) * exp(a1*lnmu-netOnZeroDXC_gamma_logarithm(a)+b1*lnmuc-netOnZeroDXC_gamma_logarithm(b)+netOnZeroDXC_gamma_logarithm(a+b));
	return ((ans > 0.0)? 1.0-ans : -ans);
}

double netOnZeroDXC_gamma_logarithm(double xx)		// Numerical recipes 6.1
{
	int	j;
	double	x, y, temp, ser;
	static const double coeff[14] = {57.1562356658629235,-59.5979603554754912,
					14.1360979747417471,-0.491913816097620199,0.339946499848118887e-4,
					0.465236289270485756e-4,-0.983744753048795646e-4,0.158088703224912494e-3,
					-0.210264441724104883e-3,0.217439618115212643e-3,-0.164318106536763890e-3,
					0.844182239838527433e-4,-0.261908384015814087e-4,0.368991826595316234e-5};
	if (xx <= 0) {
		std::cerr << "ERROR: negative argument when evaluating log(gamma(x))! Exiting...\n";
		exit(1);
	}
	y = x = xx;
	temp = x + 5.24218750000000000;
	temp = (x+0.5)*log(temp) - temp;
	ser = 0.999999999999997092;
	for (j = 0; j < 14; j++) {
		ser += coeff[j]/++y;
	}
	return (temp + log(2.5066282746310005*ser/x));
}
