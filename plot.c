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
    printf("\n\n");

    /* Print eigenvectors */
    printf("Eigenvectors:\n");
    for (i = 0; i < result.num_modes; i++)
    {
        printf("Mode #%d:\n", i + 1);
        for (j = 0; j < result.num_modes; j++)
            printf("%.2lf\t", result.eigenvectors[j][i]);
        printf("\n\n");
    }

    /* Print coefficients */
    printf("Coefficients:\n");
    for (i = 0; i < result.num_modes; i++)
        printf("A%d: %.2f\tB%d: %.2f\n", i + 1, result.coefficients[i].a, i + 1,
                result.coefficients[i].b);
    printf("\n");

    return;
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

    printf("Plotting eigenfrequencies.\n");

    gplot = gnuplot_init();
    gnuplot_setterm(gplot, "qt", 900, 400);
    gnuplot_set_axislabel(gplot, "x", "Mode Number");
    gnuplot_set_axislabel(gplot, "y", "Eigenfrequency (rad/s)");
    gnuplot_fancy_setstyle(gplot, "points", -2, "", -2, 7, 3);
    gnuplot_plot_coordinates(gplot, modes, result.eigenfrequencies,
            result.num_modes, "Eigenfrequencies");

    printf("Press Enter to continue.\n");
    while (getchar() != '\n') {}

    gnuplot_close(gplot);

    return;
}
    
