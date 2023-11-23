/*----------------------------------------------------------------------------*/
/* plot.h                                                                     */
/* Author: Godwin Duan                                                        */
/*----------------------------------------------------------------------------*/

#ifndef PLOT_INCLUDED
#define PLOT_INCLUDED

#include "types.h"

void print_result(Result result);

void plot_eigenfrequencies(Result result);

void plot_mode_amplitudes(Result result);

void plot_normal_modes(Result result, Simulation sim);

void animate(Result result, Simulation sim, double time_scale);

#endif
