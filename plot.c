/*----------------------------------------------------------------------------*/
/* plot.c                                                                     */
/* Author: Godwin Duan                                                        */
/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "plot.h"
#include "./gnuplot_i/gnuplot_i.h"

void print_result(Result result)
{
    int i, j;

    /* Print eigenfrequencies */
    printf("Eigenfrequencies: ");
    for (i = 0; i < result.num_modes; i++)
        printf("%.2lf\t", result.eigenfrequencies[i]);
    printf("\n");

    /* Print eigenvectors */
    printf("Eigenvectors:\n");
    for (i = 0; i < result.num_modes; i++)
    {
        printf("Mode #%d:\n", i + 1);
        for (j = 0; j < result.num_modes; j++)
            printf("%.2lf\t", result.eigenvectors[j][i]);
        printf("\n");
    }
}

void plot_eigenfrequencies(Result result)
{
    gnuplot_ctrl *gplot;
    double *modes;
    int i;

    assert(result.eigenfrequencies != NULL);

    modes = malloc(result.num_modes * sizeof(double));
    /* Skipping error checking */

    for (i = 0; i < result.num_modes; i++)
        modes[i] = i + 1;

    gplot = gnuplot_init();
    gnuplot_setterm(gplot, "qt", 900, 400);

    gnuplot_cmd(gplot, "set pointsize 100");
    /* Leave plot up until enter is pressed */
    gnuplot_plot_once("points", "Mode Number", "Eigenfrequency (rad/s)",
            modes, result.eigenfrequencies, result.num_modes,
            "Eigenfrequencies");

    gnuplot_close(gplot);
}
    
