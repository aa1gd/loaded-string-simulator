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

void animate_string(Result result, double *connections);

#endif
