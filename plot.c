/*----------------------------------------------------------------------------*/
/* plot.c                                                                     */
/* Author: Godwin Duan                                                        */
/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>

#include "plot.h"

#define TIMESTEP 0.01 /* calculate a frame every TIMESTEP seconds */
#define RUNTIME 100 /* number of seconds to be simulated */

#define MAX_INPUT_LENGTH 10

#define LINEWIDTH 3.0
#define DEFAULT_POINTSIZE 3.0
#define MAX_POINTSIZE 5.0
#define MIN_POINTSIZE 2.0

/* Calculates a pointsize relative to the mass of the bead */
static double calc_pointsize(Simulation sim, int mass_index)
{
    double pointsize;
    double max_mass, min_mass;
    double per; /* Percentile mass of the bead */
    int i;

    max_mass = sim.beads[0].mass;
    min_mass = sim.beads[0].mass;
    for (i = 0; i < sim.num_beads; i++)
    {
        if (sim.beads[i].mass > max_mass)
            max_mass = sim.beads[i].mass;
        if (sim.beads[i].mass < min_mass)
            min_mass = sim.beads[i].mass;
    }

    if (max_mass != min_mass)
        per = (sim.beads[mass_index].mass - min_mass) / (max_mass - min_mass);
    else
        per = 1.0;

    pointsize = MIN_POINTSIZE + (per * (MAX_POINTSIZE - MIN_POINTSIZE));

    /* If there's a bunch of beads, we scale down the pointsize */
    if (sim.num_beads > 100)
        pointsize /= 2;

    /* No size for massless beads */
    if (sim.beads[mass_index].mass == 0.0)
        pointsize = 0.0;

    return pointsize;
}

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
    fprintf(gnuplot, "plot '-' u 1:2 t 'Eigenfrequencies' w points pt 7 ps %f\n", DEFAULT_POINTSIZE);
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
    
void plot_normal_modes(Result result, Simulation sim)
{
    double *x, *y, *sizes;
    int modenum; /* one indexed */
    int i;
    char str[MAX_INPUT_LENGTH];
    FILE *gnuplot;

    assert(result.eigenvectors != NULL);
    assert(sim.connections != NULL);

    /* We add two more beads as endpoints */
    x = malloc((result.num_modes + 2) * sizeof(double));
    y = malloc((result.num_modes + 2) * sizeof(double));
    sizes = malloc((result.num_modes + 2) * sizeof(double));
    /* Skipping error checking */

    /* Draw in the two fixed endpoints */
    x[0] = 0;
    y[0] = 0;
    y[result.num_modes + 1] = 0;
    sizes[0] = 0.0;
    sizes[result.num_modes + 1] = 0.0;

    /* Strings have variable spacing */
    if (sim.sim_type == STRING)
        for (i = 1; i <= result.num_modes + 1; i++)
            x[i] = x[i - 1] + sim.connections[i - 1];

    /* Springs have equal spacing */
    if (sim.sim_type == SPRING)
        for (i = 1; i <= result.num_modes + 1; i++)
            x[i] = x[i - 1] + 1;

    /* Determine bead sizes */
    for (i = 1; i <= result.num_modes; i++)
        sizes[i] = calc_pointsize(sim, i - 1);

    gnuplot = popen("gnuplot", "w");
    if (!gnuplot) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fprintf(gnuplot, "set title 'Normal Modes'\n");
    fprintf(gnuplot, "set xlabel 'x (m)'\n");
    fprintf(gnuplot, "set ylabel 'y (m)'\n");
    fprintf(gnuplot, "set yrange [-1:1]\n");

    strcpy(str, "1");
    do
    {
        printf("\n");
        if (strcmp(str, "\n") == 0)
            modenum++;
        else
            modenum = atoi(str);

        if (modenum > result.num_modes || modenum <= 0)
            break;

        for (i = 0; i < result.num_modes; i++)
            y[i + 1] = result.eigenvectors[i][modenum - 1];

        fprintf(gnuplot, "plot '-' u 1:2:3 t 'Mode #%d' ", modenum);
        fprintf(gnuplot, "w linespoints lw %f pt 7 ps variable\n", LINEWIDTH);
        for (i = 0; i < result.num_modes + 2; i++)
            fprintf(gnuplot, "%lf %lf %lf\n", x[i], y[i], sizes[i]);

        fprintf(gnuplot, "e\n");
        fflush(gnuplot);

        printf("Press ENTER to go to next normal mode. Enter number 1-%d to display that mode. Enter 'q' to quit: ", result.num_modes);
    }
    while (strcmp(fgets(str, MAX_INPUT_LENGTH, stdin), "q\n"));

    pclose(gnuplot);
    free(x);
    free(y);
    free(sizes);
}

static void animate_string(Result result, Simulation sim, double time_scale)
{
    double *x, *y, *sizes;
    double t = 0; /* time */
    int i, j;
    double yrange = 0;
    FILE *gnuplot;

    /* We add two more beads as endpoints */
    x = malloc((result.num_modes + 2) * sizeof(double));
    y = malloc((result.num_modes + 2) * sizeof(double));
    sizes = malloc((result.num_modes + 2) * sizeof(double));
    /* Skipping error checking */

    /* Draw in the two fixed endpoints */
    x[0] = 0;
    y[0] = 0;
    y[result.num_modes + 1] = 0;

    /* Strings have variable spacing */
    for (i = 1; i <= result.num_modes + 1; i++)
        x[i] = x[i - 1] + sim.connections[i - 1];

    /* Determine bead sizes */
    sizes[0] = 0.0;
    sizes[result.num_modes + 1] = 0.0;
    for (i = 1; i <= result.num_modes; i++)
        sizes[i] = calc_pointsize(sim, i - 1);

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

    gnuplot = popen("gnuplot", "w");
    if (!gnuplot) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fprintf(gnuplot, "set title 'String Animation'\n");
    fprintf(gnuplot, "set xlabel 'x (m)'\n");
    fprintf(gnuplot, "set ylabel 'y (m)'\n");
    fprintf(gnuplot, "set yrange [%lf:%lf]\n", -1 * yrange, yrange);

    printf("Press CTRL-c to stop simulation.\n");
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

        fprintf(gnuplot, "plot '-' u 1:2:3 w linespoints lw %f pt 7 ps variable\n", LINEWIDTH);
        for (i = 0; i < result.num_modes + 2; i++)
            fprintf(gnuplot, "%lf %lf %lf\n", x[i], y[i], sizes[i]);

        fprintf(gnuplot, "e\n");
        fflush(gnuplot);

        usleep(1000000 * TIMESTEP / time_scale);
        t += TIMESTEP;
    }

    pclose(gnuplot);
    free(x);
    free(y);
    free(sizes);

    return;
}

static void animate_spring(Result result, Simulation sim, double time_scale)
{
    double *x, *sizes;
    double t = 0; /* time */
    int i, j;
    double spacing = 0;
    FILE *gnuplot;

    /* We add two more beads as endpoints */
    x = malloc((result.num_modes + 2) * sizeof(double));
    sizes = malloc((result.num_modes + 2) * sizeof(double));
    /* Skipping error checking */

    /* Calculate spacing needed */
    for (i = 0; i < result.num_modes; i++)
        spacing += sqrt(pow(result.coefficients[i].a, 2)
                + pow(result.coefficients[i].b, 2));
    spacing /= sqrt(result.num_modes);
    spacing *= 2;
    spacing *= 1.5; /* leave extra spacing between beads */

    /* Springs have equal spacing between beads */
    for (i = 0; i < result.num_modes + 2; i++)
        x[i] = i * spacing;

    /* Determine bead sizes */
    sizes[0] = 0.0;
    sizes[result.num_modes + 1] = 0.0;
    for (i = 1; i <= result.num_modes; i++)
        sizes[i] = calc_pointsize(sim, i - 1);

    gnuplot = popen("gnuplot", "w");
    if (!gnuplot) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fprintf(gnuplot, "set title 'Spring Animation'\n");
    fprintf(gnuplot, "set xlabel 'x (m)'\n");
    fprintf(gnuplot, "set yrange [-1:1]\n");

    printf("Press CTRL-c to stop simulation.\n");
    while (t < RUNTIME)
    {
        /* i is bead number, j is mode number */
        for (i = 0; i < result.num_modes; i++)
        {
            /* Equilibrium position */
            x[i + 1] = (i + 1) * spacing;

            /* Add contributions from each normal mode */
            for (j = 0; j < result.num_modes; j++)
            {
                x[i + 1] += result.coefficients[j].a * result.eigenvectors[i][j]
                    * cos(result.eigenfrequencies[j] * t);
                x[i + 1] += result.coefficients[j].b * result.eigenvectors[i][j]
                    * sin(result.eigenfrequencies[j] * t);
            }
        }

        fprintf(gnuplot, "plot '-' u 1:2:3 w linespoints lw %f pt 7 ps variable\n", LINEWIDTH);
        for (i = 0; i < result.num_modes + 2; i++)
            fprintf(gnuplot, "%lf 0.0 %lf\n", x[i], sizes[i]);

        fprintf(gnuplot, "e\n");
        fflush(gnuplot);

        usleep(1000000 * TIMESTEP / time_scale);
        t += TIMESTEP;
    }

    pclose(gnuplot);
    free(x);
    free(sizes);

    return;
}

void animate(Result result, Simulation sim, double time_scale)
{
    assert(result.eigenfrequencies != NULL);
    assert(result.eigenvectors != NULL);
    assert(result.coefficients != NULL);
    assert(sim.connections != NULL);

    if (sim.sim_type == STRING)
        animate_string(result, sim, time_scale);

    if (sim.sim_type == SPRING)
        animate_spring(result, sim, time_scale);

    return;
}
    
