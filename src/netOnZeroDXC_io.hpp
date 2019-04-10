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

#ifndef INCLUDED_PAIR
	#include "netOnZeroDXC_pair.hpp"
	#define INCLUDED_PAIR
#endif

int netOnZeroDXC_load_single_file (std::vector < std::vector <double> > &, std::vector <std::string> &, std::string, char);
int netOnZeroDXC_load_single_matrix (std::vector < std::vector <double> > &, std::string, char);
int netOnZeroDXC_load_labels_dictionary (std::vector <std::string> &, std::string, char);
int netOnZeroDXC_load_stdin (std::vector < std::vector <double> > &, char);
int netOnZeroDXC_load_single_table (std::vector < std::vector <double> > &, std::string, char);

int netOnZeroDXC_read_dictionary (std::vector <std::string> &, std::string, char);
int netOnZeroDXC_read_data_table (std::vector < std::vector <double> > &, std::string, char);
int netOnZeroDXC_parse_line (std::vector <double> &, std::string &, char);
int netOnZeroDXC_parse_dictionary_line (std::string &, int &, std::string &, char);
int netOnZeroDXC_parse_configuration_line (std::string &, std::string &, bool &, std::string &, std::string &, char);

int netOnZeroDXC_parse_filename_1label(std::string &, std::string &, char);
int netOnZeroDXC_parse_filename_2labels(PairOfLabels &, std::string &, char);

int netOnZeroDXC_check_table_sizes(const std::vector < std::vector < std::vector <double> > > &);
int netOnZeroDXC_check_linear_sizes(const std::vector < std::vector <double> > &);
int netOnZeroDXC_check_list_pairs(const std::vector <PairOfLabels> &);
int netOnZeroDXC_check_list_labels(const std::vector <std::string> &);
int netOnZeroDXC_check_list_numbers (bool &, const std::vector <int> &);
int netOnZeroDXC_check_label_format (const std::string &);

int netOnZeroDXC_fill_list_pairs(std::vector <PairOfLabels> &, const std::vector <std::string> &);
int netOnZeroDXC_postfill_list_labels(std::vector <std::string> &, const std::vector <PairOfLabels> &);
int netOnZeroDXC_associate_index_of_pair(const std::vector <PairOfLabels> &, const std::vector <std::string> &, int, int);
int netOnZeroDXC_check_new_label_pairs(std::vector <PairOfLabels> &, const std::vector <PairOfLabels> &);

std::string netOnZeroDXC_generate_filepath(std::string, std::string, std::string, char, std::string, std::string);
int netOnZeroDXC_save_diagram(const std::vector < std::vector <double> > &, std::string, std::string, std::string, char, std::string, std::string, char);
int netOnZeroDXC_save_linear_data(const std::vector <double> &, const std::vector <double> &, std::string, std::string, std::string, char, std::string, std::string, char);
int netOnZeroDXC_save_single_file(const std::vector < std::vector <double> > &, std::string, char);
int netOnZeroDXC_save_log_file(const std::stringstream &, std::string);
