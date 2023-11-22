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

/* Simulation doesn't run in real time. */
#define TIMESTEP 0.02 /* calculate a frame every TIMESTEP seconds */
#define RUNTIME 100 /* number of seconds to be simulated */
#define FPS 30

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
    int *modes;
    int i;
    FILE *gnuplot;

    assert(result.eigenfrequencies != NULL);

    modes = malloc(result.num_modes * sizeof(int));
    /* Skipping error checking */

    for (i = 0; i < result.num_modes; i++)
        modes[i] = i + 1;

    printf("Plotting eigenfrequencies.\n");

    gnuplot = popen("gnuplot", "w");
    if (!gnuplot) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fprintf(gnuplot, "set title 'Eigenfrequencies vs. Mode Number'\n");
    fprintf(gnuplot, "set xlabel 'Mode Number'\n");
    fprintf(gnuplot, "set ylabel 'Eigenfrequency (rad/s)'\n");
    fprintf(gnuplot, "plot '-' u 1:2 t 'Eigenfrequencies' w points pt 7 ps 2\n");
    for (i = 0; i < result.num_modes; i++)
        fprintf(gnuplot, "%d %lf\n", modes[i], result.eigenfrequencies[i]);
    fprintf(gnuplot, "e\n");
    fflush(gnuplot);

    printf("Press Enter to continue.\n");
    while (getchar() != '\n') {}

    pclose(gnuplot);
    free(modes);

    return;
}

void plot_mode_amplitudes(Result result)
{
    int *modes;
    double *amplitudes;
    int i;
    FILE *gnuplot;

    assert(result.coefficients != NULL);

    modes = malloc(result.num_modes * sizeof(int));
    amplitudes = malloc(result.num_modes * sizeof(double));
    /* Skipping error checking */

    for (i = 0; i < result.num_modes; i++)
    {
        modes[i] = i + 1;
        amplitudes[i] = sqrt(pow(result.coefficients[i].a, 2)
                + pow(result.coefficients[i].b, 2));
        /*printf("Mode #%d Amp %.2lf\n", modes[i], amplitudes[i]);*/
    }

    printf("Plotting mode amplitudes.\n");

    gnuplot = popen("gnuplot", "w");
    if (!gnuplot) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fprintf(gnuplot, "set title 'Mode Amplitudes'\n");
    fprintf(gnuplot, "set xlabel 'Mode Number'\n");
    fprintf(gnuplot, "set ylabel 'Amplitude (m)'\n");
    fprintf(gnuplot, "set style fill solid 1.0\n");
    fprintf(gnuplot, "set boxwidth 0.75 relative\n");
    fprintf(gnuplot, "plot '-' u 1:2 t 'Amplitude' w boxes\n");
    for (i = 0; i < result.num_modes; i++)
        fprintf(gnuplot, "%d %lf\n", modes[i], amplitudes[i]);
    fprintf(gnuplot, "e\n");
    fflush(gnuplot);

    printf("Press Enter to continue.\n");
    while (getchar() != '\n') {}

    pclose(gnuplot);
    free(modes);
    free(amplitudes);

    return;
}
    
void animate_string(Result result, double *connections)
{
    double *x, *y;
    double t = 0; /* time */
    int i, j;
    double yrange = 0;
    FILE *gnuplot;

    assert(result.eigenfrequencies != NULL);
    assert(result.eigenvectors != NULL);
    assert(result.coefficients != NULL);
    assert(connections != NULL);

    /* We add two more beads as endpoints */
    x = malloc((result.num_modes + 2) * sizeof(double));
    y = malloc((result.num_modes + 2) * sizeof(double));
    /* Skipping error checking */

    /* Draw in the two fixed endpoints */
    x[0] = 0;
    for (i = 1; i <= result.num_modes + 1; i++)
        x[i] = x[i - 1] + connections[i - 1];
    y[0] = 0;
    y[result.num_modes + 1] = 0;

    /* The max displacement cannot be larger than the sum of the coefficients;
     * so use the sum of coefficients as lower and upper y range */
    for (i = 0; i < result.num_modes; i++)
        yrange += sqrt(pow(result.coefficients[i].a, 2)
                + pow(result.coefficients[i].b, 2));

    /* This is an interesting problem: given the amplitudes and frequencies,
     * what's the average amplitude of the superimposed wave going to be? The
     * above overestimates yrange by a lot in cases with many modes. */

    /* Scaling down yrange - the sqrt is a guess */
    yrange /= sqrt(result.num_modes);

    /* TODO: potential solution is dynamically scaling y range */

    /* check displacements - for debugging */
    /*
    for (i = 0; i < result.num_modes + 2; i++)
        printf("Bead #%d: %.2lf\t", i, x[i]);
    printf("\n");
    */

    gnuplot = popen("gnuplot", "w");
    if (!gnuplot) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fprintf(gnuplot, "set title 'String Animation'\n");
    fprintf(gnuplot, "set xlabel 'x (m)'\n");
    fprintf(gnuplot, "set ylabel 'y (m)'\n");
    fprintf(gnuplot, "set yrange [%lf:%lf]\n", -1 * yrange, yrange);

    /* TODO: press key to stop simulation */
    while (t < RUNTIME)
    {
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

        fprintf(gnuplot, "plot '-' u 1:2 w linespoints lw 4.0 pt 7 ps 3\n");
        for (i = 0; i < result.num_modes + 2; i++)
            fprintf(gnuplot, "%lf %lf\n", x[i], y[i]);
        fprintf(gnuplot, "e\n");
        fflush(gnuplot);

        /* If running in real time, use usleep(1000000 * TIMESTEP) */
        usleep(1000000 / FPS);
        t += TIMESTEP;
    }

    pclose(gnuplot);
    free(x);
    free(y);

    return;
}
    
