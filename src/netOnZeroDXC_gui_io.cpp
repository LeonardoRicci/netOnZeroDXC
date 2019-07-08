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
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>

#include "wx/arrstr.h"

#ifndef INCLUDED_IOFUNCTIONS
	#include "netOnZeroDXC_io.hpp"
	#define INCLUDED_IOFUNCTIONS
#endif
#ifndef INCLUDED_IOFUNCTIONS_WX
	#include "netOnZeroDXC_gui_io.hpp"
	#define INCLUDED_IOFUNCTIONS_WX
#endif

int netOnZeroDXC_load_multi_sequences(std::vector < std::vector <double> > & sequences_table, std::vector <std::string> & node_labels, wxArrayString & list_of_files,
				char separator_char, char filename_delimiter_char, int column_number)
{
	list_of_files.Sort();

	int	i, j;
	int	node_index;
	int	error = 0;
	int	nr_files = list_of_files.GetCount();
	std::string				file_name;
	std::string				temp_label;
	std::vector <double>			temp_sequence;
	std::vector < std::vector <double> >	temp_input;
	for (i = 0; i < nr_files; i++) {
		temp_sequence.clear();
		file_name = list_of_files[i].ToStdString();
		if (netOnZeroDXC_parse_filename_1label(temp_label, file_name, filename_delimiter_char))
			return 1;
		node_labels.push_back(temp_label);
		error = netOnZeroDXC_read_data_table(temp_input, file_name, separator_char);
		if (error)
			return 2;
		if ((temp_input.size() == 0) || (column_number < 0))
			return 3;
		if (column_number > temp_input[0].size())
			return 3;
		for (j = 0; j < temp_input.size(); j++) {
			temp_sequence.push_back(temp_input[j][column_number - 1]);
		}
		sequences_table.push_back(temp_sequence);
	}

	error = netOnZeroDXC_check_linear_sizes(sequences_table);
	if (error)
		return 3;

	error = netOnZeroDXC_check_list_labels(node_labels);
	if (error)
		return 4;

	if (sequences_table.size() < 2)
		return 5;

	return 0;
}

int netOnZeroDXC_load_multi_diagrams (std::vector < std::vector < std::vector <double> > > & diagrams_pvalue, std::vector <PairOfLabels> & list_pairs,
				wxArrayString & list_of_files, char separator_char, char filename_delimiter_char)
{
	list_of_files.Sort();

	int	i;
	int	error = 0;
	int	nr_files = list_of_files.GetCount();
	std::string	file_name;
	PairOfLabels	parsed_label_pair;
	std::vector < std::vector <double> >	temp_diagram;
	for (i = 0; i < nr_files; i++) {
		file_name = list_of_files[i].ToStdString();
		error = netOnZeroDXC_parse_filename_2labels(parsed_label_pair, file_name, filename_delimiter_char);
		if (error)
			return 1;
		list_pairs.push_back(parsed_label_pair);
		error = netOnZeroDXC_read_data_table(temp_diagram, file_name, separator_char);
		if (error)
			return 2;
		diagrams_pvalue.push_back(temp_diagram);
	}

	error = netOnZeroDXC_check_table_sizes(diagrams_pvalue);
	if (error)
		return 3;

	error = netOnZeroDXC_check_list_pairs(list_pairs);
	if (error)
		return 4;

	return 0;
}

int netOnZeroDXC_load_multi_efficiencies (std::vector < std::vector <double> > & efficiencies, std::vector <PairOfLabels> & list_pairs,
					std::vector <double> & window_widths, wxArrayString & list_of_files, char separator_char,
					char filename_delimiter_char)
{
	list_pairs.clear();
	list_of_files.Sort();
	efficiencies.clear();

	bool	inconsistent_w_found = 0;
	int	i, j;
	int	error = 0;
	int	nr_files = list_of_files.GetCount();
	std::string	file_name;
	std::vector <double>	temp_eta;
	std::vector < std::vector <double> >	temp_efficiency;
	PairOfLabels	parsed_label_pair;
	for (i = 0; i < nr_files; i++) {
		file_name = list_of_files[i].ToStdString();
		temp_eta.clear();
		error = netOnZeroDXC_parse_filename_2labels(parsed_label_pair, file_name, filename_delimiter_char);
		if (error)
			return 1;
		list_pairs.push_back(parsed_label_pair);
		error = netOnZeroDXC_read_data_table(temp_efficiency, file_name, separator_char);
		if (error)
			return 2;
		for (j = 0; j < temp_efficiency.size(); j++) {
			if (i) {
				if (window_widths[j] != temp_efficiency[j][0]) {
					inconsistent_w_found = 1;
					break;
				}
			} else {
				window_widths.push_back(temp_efficiency[j][0]);
			}
			temp_eta.push_back(temp_efficiency[j][1]);
		}
		if (inconsistent_w_found)
			return 3;
		efficiencies.push_back(temp_eta);
	}

	error = netOnZeroDXC_check_linear_sizes(efficiencies);
	if (error)
		return 3;

	error = netOnZeroDXC_check_list_pairs(list_pairs);
	if (error)
		return 4;

	return 0;
}
