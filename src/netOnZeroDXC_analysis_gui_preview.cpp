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
#include <ctime>

#ifndef INCLUDED_MAINAPP
	#include "netOnZeroDXC_analysis_main.hpp"
	#define INCLUDED_MAINAPP
#endif

#ifndef INCLUDED_COLORS
	#include "netOnZeroDXC_gui_colors.hpp"
	#define INCLUDED_COLORS
#endif

// Event tables to manage automatically-generated events (resize, refresh)
wxBEGIN_EVENT_TABLE(PlotFrame, wxFrame)
wxEND_EVENT_TABLE()
wxBEGIN_EVENT_TABLE(PanelPlot, wxPanel)
wxEND_EVENT_TABLE()
wxBEGIN_EVENT_TABLE(PanelColorbox, wxPanel)
wxEND_EVENT_TABLE()
wxBEGIN_EVENT_TABLE(PlotFrame_WholeSeq, wxFrame)
wxEND_EVENT_TABLE()
wxBEGIN_EVENT_TABLE(PanelPlot_WholeSeq, wxPanel)
wxEND_EVENT_TABLE()
wxBEGIN_EVENT_TABLE(PanelColorbox_WholeSeq, wxPanel)
wxEND_EVENT_TABLE()

// PlotFrame constructor: defines the layout of the preview window
PlotFrame::PlotFrame (const wxString& title, GuiFrame *parent)
: wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxFRAME_FLOAT_ON_PARENT | wxCLOSE_BOX | wxCAPTION | wxRESIZE_BORDER)
{
	results_workspace = parent->m_workspace;

	int	nr_pixels;

	if (results_workspace->parameter_computation_pathway == 3) {
		nr_pixels = results_workspace->matrices_multieta[0].size();
	} else if (results_workspace->parameter_computation_pathway < 3) {
		nr_pixels = results_workspace->matrices_multieta_multialpha[0][0].size();
	}

	slider_thr_significance = new wxSlider(this, EVENT_SLIDER_THR_SGN, 10, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL | wxSL_MIN_MAX_LABELS);
	statictext_slider_thr_significance = new wxStaticText(this, wxID_ANY, wxT("Significance threshold (1/1000):"), wxDefaultPosition, wxDefaultSize, 0);
	Connect(EVENT_SLIDER_THR_SGN, wxEVT_SLIDER, wxCommandEventHandler(PlotFrame::OnSlide));
	slider_thr_significance->SetValue((int) (1000 * results_workspace->parameter_thr_significance));
	if (results_workspace->parameter_computation_pathway == 3)
		slider_thr_significance->Disable();

	slider_thr_efficiency = new wxSlider(this, EVENT_SLIDER_THR_EFF, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL | wxSL_MIN_MAX_LABELS);
	statictext_slider_thr_efficiency = new wxStaticText(this, wxID_ANY, wxT("Efficiency threshold (\%):"), wxDefaultPosition, wxDefaultSize, 0);
	Connect(EVENT_SLIDER_THR_EFF, wxEVT_SLIDER, wxCommandEventHandler(PlotFrame::OnSlide));
	slider_thr_efficiency->SetValue((int) (100 * results_workspace->parameter_thr_efficiency));

	plot_area = new PanelPlot(this, wxDefaultSize);
	colorbox_area = new PanelColorbox(this, wxDefaultSize);

	wxBoxSizer	*vbox_slider_sgn = new wxBoxSizer(wxVERTICAL);
	vbox_slider_sgn->Add(statictext_slider_thr_significance, 0, wxALIGN_CENTER_HORIZONTAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	vbox_slider_sgn->Add(slider_thr_significance, 1, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);

	wxBoxSizer	*vbox_slider_eff = new wxBoxSizer(wxVERTICAL);
	vbox_slider_eff->Add(statictext_slider_thr_efficiency, 0, wxALIGN_CENTER_HORIZONTAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	vbox_slider_eff->Add(slider_thr_efficiency, 1, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);

	wxBoxSizer	*hbox_sliders = new wxBoxSizer(wxHORIZONTAL);
	hbox_sliders->Add(vbox_slider_sgn, 1, wxEXPAND | wxLEFT | wxRIGHT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 20);
	hbox_sliders->Add(vbox_slider_eff, 1, wxEXPAND | wxLEFT | wxRIGHT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 20);

	wxBoxSizer	*hbox_plot = new wxBoxSizer(wxHORIZONTAL);
	hbox_plot->Add(plot_area, 4, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	hbox_plot->Add(colorbox_area, 1, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);

	wxBoxSizer	*vbox_plot_all = new wxBoxSizer(wxVERTICAL);
	vbox_plot_all->Add(hbox_sliders, 0, wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	vbox_plot_all->Add(hbox_plot, 1, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);

	this->SetSizer(vbox_plot_all);
	this->SetAutoLayout(true);

	int	frame_size;
	if (nr_pixels < 10) {
		frame_size = 40 * nr_pixels + 180;
	} else if (nr_pixels < 30) {
		frame_size = 20 * nr_pixels + 180;
	} else {
		frame_size = 12 * nr_pixels + 180;
	}

	SetSize(frame_size + ((frame_size < 500)? 210 : 0), frame_size);
	SetMinSize(wxSize(frame_size + ((frame_size < 500)? 210 : 0), frame_size));
	Show();
}

void PlotFrame::OnSlide (wxCommandEvent & WXUNUSED(event))
{
	plot_area->Refresh();
}

PanelPlot::PanelPlot (PlotFrame *parent, wxSize panel_size)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, panel_size, wxBORDER_SIMPLE)
{
	parent_frame = parent;
	results_workspace = parent->results_workspace;

	Connect(wxEVT_PAINT, wxPaintEventHandler(PanelPlot::OnPaint));
	Connect(wxEVT_SIZE, wxSizeEventHandler(PanelPlot::OnResize));
}

void PanelPlot::OnResize (wxSizeEvent & WXUNUSED(event))
{
	this->Refresh();
}

void PanelPlot::OnPaint (wxPaintEvent & WXUNUSED(event))
{
	wxPaintDC dc(this);

	bool	variable_alpha = false;
	int	nr_pixels;
	if (results_workspace->parameter_computation_pathway == 3) {
		nr_pixels = results_workspace->matrices_multieta[0].size();
	} else if (results_workspace->parameter_computation_pathway < 3) {
		nr_pixels = results_workspace->matrices_multieta_multialpha[0][0].size();
		variable_alpha = true;
	}

	double	w_max = results_workspace->window_widths.back();

	int	size_x, size_y;
	this->GetSize(&size_x, &size_y);
	int	size_displace = 25;
	int	size_m = ( (size_x > size_y)? size_y : size_x ) - 2*size_displace - 5;
	if (size_m < 1) {
		return;
	}
	int	pxsize = size_m / nr_pixels;
	int	i;
	for (i = 0; i < nr_pixels; i++) {
		dc.DrawLabel(results_workspace->node_labels[i], wxRect(wxPoint(0, size_displace + i*pxsize),wxSize(size_displace, pxsize)), wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	}

	dc.SetPen(*wxTRANSPARENT_PEN);
	int	selected_threshold_alpha = parent_frame->slider_thr_significance->GetValue();
	int	selected_threshold_eta = parent_frame->slider_thr_efficiency->GetValue();
	int	j;
	double	w_to_draw;
	for (i = 0; i < nr_pixels; i++) {
		for (j = 0; j < nr_pixels; j++) {
			wxBrush brush1;
			if (variable_alpha) {
				w_to_draw = results_workspace->matrices_multieta_multialpha[selected_threshold_alpha][selected_threshold_eta][i][j] / w_max;
			} else {
				w_to_draw = results_workspace->matrices_multieta[selected_threshold_eta][i][j] / w_max;
			}
			brush1 = wxBrush(netOnZeroDXC_color_palette(w_to_draw));
			dc.SetBrush(brush1);
			dc.DrawRectangle(wxRect(size_displace + j*pxsize, size_displace + i*pxsize, pxsize, pxsize));
		}
	}

	return;
}

PanelColorbox::PanelColorbox (PlotFrame *parent, wxSize panel_size)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, panel_size, wxBORDER_SIMPLE)
{
	parent_frame = parent;

	Connect(wxEVT_PAINT, wxPaintEventHandler(PanelColorbox::OnPaint));
	Connect(wxEVT_SIZE, wxSizeEventHandler(PanelColorbox::OnResize));
}

void PanelColorbox::OnPaint (wxPaintEvent & WXUNUSED(event))
{
	wxPaintDC dc(this);

	int	nr_widths = parent_frame->results_workspace->window_widths.size();

	int	size_x, size_y;
	this->GetSize(&size_x, &size_y);
	if (size_y < 1) {
		return;
	}
	int	size_displace = 25;
	int	pxsize = (size_y - 2*size_displace) / nr_widths;

	dc.DrawLabel("Time scale:", wxRect(wxPoint(10, 0), wxSize(pxsize, size_displace)), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

	dc.SetPen(*wxTRANSPARENT_PEN);
	int	i;
	int	segment_spacer;
	for (i = 1; i <= nr_widths; i++) {
		wxBrush brush1(netOnZeroDXC_color_palette(i, nr_widths));
		dc.SetBrush(brush1);
		dc.DrawRectangle(wxRect(wxPoint(10, (i-1)*pxsize + size_displace), wxSize(40, pxsize)));
		if (nr_widths < 10) {
			segment_spacer = 1;
		} else {
			segment_spacer = nr_widths / 10;
		}
		if ((nr_widths - i) % segment_spacer == 0) {
		 	char	temp_label[64];
		 	sprintf(temp_label, "%.2e", parent_frame->results_workspace->window_widths[(nr_widths - i)]);
		 	std::string	label_to_draw(temp_label);
		 	dc.DrawLabel(label_to_draw, wxRect(wxPoint(52, (i-1)*pxsize + size_displace), wxSize(40, pxsize)), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		}
	}

	return;
}

void PanelColorbox::OnResize (wxSizeEvent & WXUNUSED(event))
{
	this->Refresh();
}


// PlotFrame constructor: defines the layout of the preview window
PlotFrame_WholeSeq::PlotFrame_WholeSeq (const wxString& title, GuiFrame *parent)
: wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxCAPTION | wxDEFAULT_DIALOG_STYLE | wxCLOSE_BOX | wxRESIZE_BORDER)
{
	results_workspace = parent->m_workspace;

	int	nr_pixels;

	nr_pixels = results_workspace->wholeseq_xcorr.size();

	wxArrayString	m_list_of_alternatives;
	m_list_of_alternatives.Add(wxT("Cross-correlation coefficient"));
	m_list_of_alternatives.Add(wxT("p-values"));
	radiobox_xcorr_pvalue = new wxRadioBox(this, EVENT_CHOSEN_QUANTITY, wxT("Select quantity to show:"), wxDefaultPosition, wxDefaultSize, m_list_of_alternatives, 0, wxRA_SPECIFY_ROWS);
	Connect(EVENT_CHOSEN_QUANTITY, wxEVT_RADIOBOX, wxCommandEventHandler(PlotFrame_WholeSeq::OnSelectQuantity));
	if (results_workspace->parameter_computation_target == 4) {
		radiobox_xcorr_pvalue->Enable(0, 0);
		radiobox_xcorr_pvalue->Enable(1, 0);
		radiobox_xcorr_pvalue->SetSelection(0);
	} else if (results_workspace->parameter_computation_target == 5) {
		radiobox_xcorr_pvalue->SetSelection(1);
	}

	plot_area = new PanelPlot_WholeSeq(this, wxDefaultSize);
	colorbox_area = new PanelColorbox_WholeSeq(this, wxDefaultSize);

	wxBoxSizer	*hbox_plot = new wxBoxSizer(wxHORIZONTAL);
	hbox_plot->Add(plot_area, 4, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	hbox_plot->Add(colorbox_area, 1, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);

	wxBoxSizer	*hbox_radio = new wxBoxSizer(wxHORIZONTAL);
	hbox_radio->Add(radiobox_xcorr_pvalue, 1, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);

	wxBoxSizer	*vbox_plot_all = new wxBoxSizer(wxVERTICAL);
	vbox_plot_all->Add(hbox_radio, 0, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	vbox_plot_all->Add(hbox_plot, 1, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);

	this->SetSizer(vbox_plot_all);
	this->SetAutoLayout(true);

	int	frame_size;
	if (nr_pixels < 10) {
		frame_size = 40 * nr_pixels + 180;
	} else if (nr_pixels < 30) {
		frame_size = 20 * nr_pixels + 180;
	} else {
		frame_size = 12 * nr_pixels + 180;
	}

	SetSize(frame_size + ((frame_size < 500)? 210 : 0), frame_size);
	SetMinSize(wxSize(frame_size + ((frame_size < 500)? 210 : 0), frame_size));
	Show();
}

void PlotFrame_WholeSeq::OnSelectQuantity (wxCommandEvent & WXUNUSED(event))
{
	plot_area->Refresh();
	colorbox_area->Refresh();
}

PanelPlot_WholeSeq::PanelPlot_WholeSeq (PlotFrame_WholeSeq *parent, wxSize panel_size)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, panel_size, wxBORDER_SIMPLE)
{
	parent_frame = parent;
	results_workspace = parent->results_workspace;

	Connect(wxEVT_PAINT, wxPaintEventHandler(PanelPlot_WholeSeq::OnPaint));
	Connect(wxEVT_SIZE, wxSizeEventHandler(PanelPlot_WholeSeq::OnResize));
}

void PanelPlot_WholeSeq::OnResize (wxSizeEvent & WXUNUSED(event))
{
	this->Refresh();
}

void PanelPlot_WholeSeq::OnPaint (wxPaintEvent & WXUNUSED(event))
{
	wxPaintDC dc(this);

	bool	variable_alpha = false;
	int	nr_pixels;
	nr_pixels = results_workspace->wholeseq_xcorr[0].size();

	int	size_x, size_y;
	this->GetSize(&size_x, &size_y);
	int	size_displace = 25;
	int	size_m = ( (size_x > size_y)? size_y : size_x ) - 2*size_displace - 5;
	if (size_m < 1) {
		return;
	}
	int	pxsize = size_m / nr_pixels;
	int	i;
	for (i = 0; i < nr_pixels; i++) {
		dc.DrawLabel(results_workspace->node_labels[i], wxRect(wxPoint(0, size_displace + i*pxsize),wxSize(size_displace, pxsize)), wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	}

	dc.SetPen(*wxTRANSPARENT_PEN);
	int	selected_quantity = parent_frame->radiobox_xcorr_pvalue->GetSelection();
	int	j;
	double	w_to_draw;
	for (i = 0; i < nr_pixels; i++) {
		for (j = 0; j < nr_pixels; j++) {
			wxBrush brush1;
			if (selected_quantity == 0) {
				w_to_draw = 0.5*(results_workspace->wholeseq_xcorr[i][j] + 1);
			} else {
				w_to_draw = results_workspace->wholeseq_pvalue[i][j];
				w_to_draw = (w_to_draw > 0.05)? 1.0 : (w_to_draw/0.05 + 1e-12);	// 1e-12 correction to avoid graphing in black
			}
			brush1 = wxBrush(netOnZeroDXC_color_palette(w_to_draw));
			dc.SetBrush(brush1);
			dc.DrawRectangle(wxRect(size_displace + j*pxsize, size_displace + i*pxsize, pxsize, pxsize));
		}
	}

	return;
}

PanelColorbox_WholeSeq::PanelColorbox_WholeSeq (PlotFrame_WholeSeq *parent, wxSize panel_size)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, panel_size, wxBORDER_SIMPLE)
{
	parent_frame = parent;

	Connect(wxEVT_PAINT, wxPaintEventHandler(PanelColorbox_WholeSeq::OnPaint));
	Connect(wxEVT_SIZE, wxSizeEventHandler(PanelColorbox_WholeSeq::OnResize));
}

void PanelColorbox_WholeSeq::OnPaint (wxPaintEvent & WXUNUSED(event))
{
	wxPaintDC dc(this);

	int	selected_quantity = parent_frame->radiobox_xcorr_pvalue->GetSelection();

	int	size_x, size_y;
	this->GetSize(&size_x, &size_y);
	if (size_y < 1) {
		return;
	}
	int	size_displace = 25;
	int	pxsize = (size_y - 2*size_displace) / 100;

	double	number_to_draw;
	if (selected_quantity == 0) {
		dc.DrawLabel("Corr coeff:", wxRect(wxPoint(10, 0), wxSize(pxsize, size_displace)), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		number_to_draw = 1.0;
	} else if (selected_quantity == 1) {
		dc.DrawLabel("p-value:", wxRect(wxPoint(10, 0), wxSize(pxsize, size_displace)), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		number_to_draw = 0.05;
	}

	dc.SetPen(*wxTRANSPARENT_PEN);
	int	i;
	int	segment_spacer;
	for (i = 1; i <= 100; i++) {
		wxBrush brush1(netOnZeroDXC_color_palette(i, 100));
		dc.SetBrush(brush1);
		dc.DrawRectangle(wxRect(wxPoint(10, (i-1)*pxsize + size_displace), wxSize(40, pxsize)));
		segment_spacer = 10;
		if ((((101-i) % segment_spacer) == 0) || (i == 100)) {
		 	char	temp_label[64];
			if (selected_quantity == 0) {
		 		sprintf(temp_label, "%.2f", number_to_draw);
				number_to_draw -= 0.2;
			} else if (selected_quantity == 1) {
				if (number_to_draw == 0.05) {
					sprintf(temp_label, "> %.3f", number_to_draw);
				} else {
					sprintf(temp_label, "%.3f", number_to_draw);
				}
				number_to_draw -= 0.005;
			}
		 	std::string	label_to_draw(temp_label);
		 	dc.DrawLabel(label_to_draw, wxRect(wxPoint(52, (i-1)*pxsize + size_displace), wxSize(40, pxsize)), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		}
	}

	return;
}

void PanelColorbox_WholeSeq::OnResize (wxSizeEvent & WXUNUSED(event))
{
	this->Refresh();
}
