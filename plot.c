/*----------------------------------------------------------------------------*/
/* plot.c                                                                     */
/* Author: Godwin Duan                                                        */
/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>

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

    free(modes);
    gnuplot_close(gplot);

    return;
}

/* TODO something's wrong with plotting */
void plot_mode_amplitudes(Result result)
{
    gnuplot_ctrl *gplot;
    double *modes, *amplitudes;
    int i;

    assert(result.coefficients != NULL);

    modes = malloc(result.num_modes * sizeof(double));
    amplitudes = malloc(result.num_modes * sizeof(double));
    /* Skipping error checking */

    for (i = 0; i < result.num_modes; i++)
    {
        modes[i] = i + 1;
        amplitudes[i] = sqrt(pow(result.coefficients[i].a, 2)
                + pow(result.coefficients[i].b, 2));
        printf("Mode #%.2lf Amp %.2lf\n", modes[i], amplitudes[i]);
    }

    printf("Plotting mode amplitudes.\n");

    gplot = gnuplot_init();
    gnuplot_setterm(gplot, "qt", 900, 400);
    gnuplot_set_axislabel(gplot, "x", "Mode Number");
    gnuplot_set_axislabel(gplot, "y", "Amplitude");
    gnuplot_setstyle(gplot, "points");

    gnuplot_cmd(gplot, "set style fill solid 1.0");
    gnuplot_cmd(gplot, "set boxwidth 0.75 relative");
    /*
    gnuplot_plot_coordinates(gplot, modes, amplitudes,
            result.num_modes, "Mode Amplitudes");
            */

    double a[16] = {0};
    gnuplot_plot_coordinates(gplot, modes, a,
            result.num_modes, "Mode Amplitudes");

    printf("Press Enter to continue.\n");
    while (getchar() != '\n') {}

    free(modes);
    free(amplitudes);
    gnuplot_close(gplot);

    return;
}
    
void animate_string(Result result, double *connections)
{
    gnuplot_ctrl *gplot;
    double *x, *y;
    double t; /* time */
    const double ts = 0.02; /* timestep */
    const double runtime = 100;
    int i, j;

    assert(result.eigenfrequencies != NULL);
    assert(result.eigenvectors != NULL);
    assert(result.coefficients != NULL);
    assert(connections != NULL);

    /* We add two more beads: a new first one and new last one that remain
     * stationary */
    /* Plus two more for the temporary hack described below */
    x = malloc((result.num_modes + 2 + 2) * sizeof(double));
    y = malloc((result.num_modes + 2 + 2) * sizeof(double));
    /* Skipping error checking */

    /* TODO: draw in the walls */
    /* Draw in the two fixed endpoints */
    x[0] = 0;
    for (i = 1; i <= result.num_modes + 1; i++)
        x[i] = x[i - 1] + connections[i - 1];
    y[0] = 0;
    y[result.num_modes + 1] = 0;

    /* A temporary hack to have fixed ranges: add two points at the end at +/-
     * the max possible displacement */
    x[result.num_modes + 2] = x[result.num_modes + 1] + 0.01;
    x[result.num_modes + 3] = x[result.num_modes + 1] + 0.01;
    y[result.num_modes + 2] = 1.5;
    y[result.num_modes + 3] = -1.5;

    /* check displacements */
    for (i = 0; i < result.num_modes + 2; i++)
        printf("Bead #%d: %.2lf\t", i, x[i]);
    printf("\n");

    gplot = gnuplot_init();
    gnuplot_setterm(gplot, "qt", 2000, 1000);
    gnuplot_set_axislabel(gplot, "x", "x (m)");
    gnuplot_set_axislabel(gplot, "y", "y (m)");
    /* gnuplot_setstyle(gplot, "linespoints"); */
    gnuplot_fancy_setstyle(gplot, "linespoints", -1, "blue", 4.0, 7, 3.0);

    printf("Animating string.\n");
    printf("Press Enter to stop animation.\n");

    t = 0;
    while (t < runtime)
    {
        gnuplot_resetplot(gplot);

        /* i is bead number, j is mode number */
        for (i = 0; i < result.num_modes; i++)
        {
            y[i + 1] = 0;
            /* Add contributions from each normal mode */
            for (j = 0; j < result.num_modes; j++)
            {
                y[i + 1] += result.coefficients[j].a * result.eigenvectors[i][j]
                    * cos(result.eigenfrequencies[j] * t);
                y[i + 1] += result.coefficients[j].b * result.eigenvectors[i][j]
                    * sin(result.eigenfrequencies[j] * t);
            }
        }

        /* extra +2 is from the temporary hack */
        gnuplot_plot_coordinates(gplot, x, y,
                result.num_modes + 2 + 2, "Animation");

        usleep(33 * 1000); /* 33ms -> 30FPS */
        t += ts;
    }

    free(x);
    free(y);
    gnuplot_close(gplot);

    return;
}
    
