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

#define TOLERANCE_SURROGATES 1e-6

double netOnZeroDXC_compute_wmatrix_element (const std::vector <double> &, const std::vector <double> &, double);
int netOnZeroDXC_compute_efficiency (std::vector <double> &, const std::vector < std::vector <double> > &, double, bool);
int netOnZeroDXC_compute_cdiagram (std::vector < std::vector <double> > &, std::vector < std::vector <double> > &, const std::vector < std::vector <double> > &, int, int, int, int, bool, int);

double netOnZeroDXC_compute_wholeseq_crosscorr (const std::vector < std::vector <double> > &, int, int, bool, int);
double netOnZeroDXC_compute_crosscorr (const std::vector < std::vector <double> > &, int, int, int, int, int, int);
int netOnZeroDXC_update_pdiagram (std::vector < std::vector <double> > &, const std::vector < std::vector <double> > &, const std::vector < std::vector <double> > &, int, int);
void netOnZeroDXC_initialize_temp_diagram (std::vector < std::vector <double> > &, int, int);
void netOnZeroDXC_initialize_nan_diagram(std::vector < std::vector <double> > &, int, int);
void netOnZeroDXC_initialize_nan_efficiency(std::vector <double> &, int);

int netOnZeroDXC_generate_surrogate_sequence (std::vector <double> &, const std::vector < std::vector <double> > &, int, const std::vector <double> &, const std::vector <double> &, double, unsigned int);
int netOnZeroDXC_initialize_surrogate_generation (std::vector <double> &, std::vector <double> &, const std::vector < std::vector <double> > &, int);
int netOnZeroDXC_restore_fft_amplitude (double *, const std::vector <double> &, int);
int netOnZeroDXC_rescale_sequence (double *, const std::vector <double> &, int);
bool netOnZeroDXC_check_iteration_convergence (double *, double *, int, double);

double netOnZeroDXC_cdf_f_distribution_Q(double, int, int);
double netOnZeroDXC_incbeta(double, double, double);
double netOnZeroDXC_incbeta_continued_fraction(double, double, double);
double netOnZeroDXC_incbeta_approx(double, double, double);
double netOnZeroDXC_gamma_logarithm(double);
