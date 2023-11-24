/*----------------------------------------------------------------------------*/
/* plot.h                                                                     */
/* Author: Godwin Duan                                                        */
/*----------------------------------------------------------------------------*/

#ifndef PLOT_INCLUDED
#define PLOT_INCLUDED

#include "types.h"

/* Prints eigenfrequencies, eigenvectors, and coefficients of the simulation */
void print_result(Result result);

/* Plots a scatterplot of eigenfrequencies vs. mode number */
void plot_eigenfrequencies(Result result);

/* Plots a bar graph of amplitudes of all normal modes */
void plot_mode_amplitudes(Result result);

/* Plots normal modes of system. User can select which mode to display. Spring
 * simulations are plotted as if they were a string simulation. */
void plot_normal_modes(Result result, Simulation sim);

/* Animates the simulation, sped up/down by factor time_scale. If time_scale =
 * 1.0, the simulation plays at real speed. */
void animate(Result result, Simulation sim, double time_scale);

#endif
