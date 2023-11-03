/*--------------------------------------------------------------------*/
/* asolve.c                                                           */
/* Author: Godwin Duan                                                */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_eigen.h>

#include "asolve.h"

static void print_matrix(const gsl_matrix *m)
{
    int i, j;
    int rows, cols;

    assert(m != NULL);
    
    /* Generate test matrix to orient */
    /*
    for (i = 0; i < rows; i++)
        for (j = 0; j < cols; j++)
            gsl_matrix_set(m, i, j, 10 * (i + 1) + j + 1);
    */

    rows = m->size1;
    cols = m->size2;

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            printf("%.2lf\t", gsl_matrix_get(m, i, j));
        }
        printf("\n");
    }
}


static gsl_matrix *create_mass_matrix(const Bead *beads, int num_beads)
{
    int i;
    gsl_matrix *m;

    assert(beads != NULL);
    assert(num_beads > 0);
    
    m = gsl_matrix_calloc(num_beads, num_beads);
    /* Skipping error checking */

    for (i = 0; i < num_beads; i++)
        gsl_matrix_set(m, i, i, beads[i].mass);

    return m;
}

static gsl_matrix *create_spring_k_matrix(const double *connections,
        int num_beads)
{
    int i, j;
    gsl_matrix *m;

    assert(connections != NULL);
    assert(num_beads > 0);

    m = gsl_matrix_calloc(num_beads, num_beads);
    /* Skipping error checking */

    /* i is row number, j is col number */
    for (i = 0; i < num_beads; i++)
    {
        for (j = 0; j < num_beads; j++)
        {
            if (i == j)
                gsl_matrix_set(m, i, j, connections[i] + connections[i + 1]);
            if (i == j - 1) 
                gsl_matrix_set(m, i, j, -1 * connections[i + 1]);
            if (i == j + 1)
                gsl_matrix_set(m, i, j, -1 * connections[i]);
        }
    }

    return m;
}

static gsl_matrix *create_string_k_matrix(const double *connections,
        double tension, int num_beads)
{
    int i, j;
    gsl_matrix *m;

    assert(connections != NULL);
    assert(num_beads > 0);

    m = gsl_matrix_calloc(num_beads, num_beads);
    /* Skipping error checking */

    for (i = 0; i < num_beads; i++)
    {
        for (j = 0; j < num_beads; j++)
        {
            if (i == j)
                gsl_matrix_set(m, i, j, tension * (1 / connections[i] +
                            1 / connections[i + 1]));
            if (i == j - 1) 
                gsl_matrix_set(m, i, j, -1 * tension * (1 / connections[i + 1]));
            if (i == j + 1)
                gsl_matrix_set(m, i, j, -1 * tension * (1 / connections[i]));
        }
    }

    return m;
}

static gsl_matrix *create_invsqrt_mass_matrix(const gsl_matrix *mass_matrix)
{
    gsl_matrix *invsqrtm;
    int i, j;

    assert(mass_matrix != NULL);

    invsqrtm = gsl_matrix_alloc(mass_matrix->size1, mass_matrix->size2);
    gsl_matrix_memcpy(invsqrtm, mass_matrix);
    for (i = 0; i < invsqrtm->size1; i++)
        for (j = 0; j < invsqrtm->size2; j++)
            if (i == j)
                gsl_matrix_set(invsqrtm, i, j,
                        pow(gsl_matrix_get(invsqrtm, i, j), -0.5));

    /*
    printf("invsqrtm:\n");
    print_matrix(invsqrtm);
    */

    return invsqrtm;
}



static gsl_matrix *create_d_matrix(const gsl_matrix *mass_matrix,
        const gsl_matrix *k_matrix)
{
    gsl_matrix *d_matrix, *invsqrtm, *tempm;

    assert(mass_matrix != NULL);
    assert(k_matrix != NULL);

    /* Both matrices must be square and the same size */
    assert(mass_matrix->size1 == mass_matrix->size2);
    assert(k_matrix->size1 == k_matrix->size2);
    assert(mass_matrix->size1 == k_matrix->size1);

    tempm = gsl_matrix_alloc(mass_matrix->size1, mass_matrix->size2);
    d_matrix = gsl_matrix_alloc(mass_matrix->size1, mass_matrix->size2);
    /* Skipping error checking */

    invsqrtm = create_invsqrt_mass_matrix(mass_matrix);

    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, k_matrix, invsqrtm, 0.0,
            tempm);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, invsqrtm, tempm, 0.0,
            d_matrix);
    
    printf("d_matrix:\n");
    print_matrix(d_matrix);

    gsl_matrix_free(invsqrtm);
    gsl_matrix_free(tempm);

    return d_matrix;
}

/* TODO: let find_normal_modes call find_d_matrix */
static Result find_normal_modes(gsl_matrix *d_matrix, gsl_matrix *mass_matrix)
{
    Result result;
    gsl_eigen_symmv_workspace *w;
    gsl_vector *eval;
    gsl_matrix *evec;
    gsl_matrix *invsqrtm;
    int i, j;

    assert(d_matrix != NULL);

    w = gsl_eigen_symmv_alloc(d_matrix->size1);
    eval = gsl_vector_alloc(d_matrix->size1);
    evec = gsl_matrix_alloc(d_matrix->size1, d_matrix->size2);

    /* Destroys d_matrix */
    gsl_eigen_symmv(d_matrix, eval, evec, w);
    gsl_eigen_symmv_free(w);
    gsl_eigen_symmv_sort(eval, evec, GSL_EIGEN_SORT_ABS_ASC);

    result.num_modes = d_matrix->size1;
    /* Load in eigenfrequencies */
    /* Eigenfrequency = sqrt(eigenvalue) */
    result.eigenfrequencies = malloc(result.num_modes * sizeof(double));
    for (i = 0; i < result.num_modes; i++)
        result.eigenfrequencies[i] = sqrt(gsl_vector_get(eval, i));

    /* Translate eigenvectors back to regular coordinates */
    /* Could do invsqrtm x each eigenvector, but that's more code */
    /* Then load in the translated eigenvectors */
    result.eigenvectors = malloc(result.num_modes * sizeof(double*));
    invsqrtm = create_invsqrt_mass_matrix(mass_matrix);
    for (i = 0; i < result.num_modes; i++)
    {
        result.eigenvectors[i] = malloc(result.num_modes * sizeof(double));
        double mscalar = gsl_matrix_get(invsqrtm, i, i);
        for (j = 0; j < result.num_modes; j++)
            result.eigenvectors[i][j] = mscalar * gsl_matrix_get(evec, i, j);
    }

    /* Normalize the translated eigenvectors */
    for (i = 0; i < result.num_modes; i++)
    {
        double mag = 0;
        for (j = 0; j < result.num_modes; j++)
            mag += pow(result.eigenvectors[j][i], 2);

        mag = sqrt(mag);

        for (j = 0; j < result.num_modes; j++)
            result.eigenvectors[j][i] /= mag;
    }

    /* Check eigenvalues and eigenvectors */
    /*
    for (i = 0; i < d_matrix->size1; i++)
    {
        double eval_i = gsl_vector_get (eval, i);
        gsl_vector_view evec_i = gsl_matrix_column (evec, i);

        printf ("eigenvalue = %g\n", eval_i);
        printf ("eigenvector = \n");
        gsl_vector_fprintf (stdout,
                &evec_i.vector, "%g");
    }
    */

    gsl_vector_free(eval);
    gsl_matrix_free(evec);
    gsl_matrix_free(invsqrtm);

    return result;
}

Result asolve(Simulation sim)
{
    Result result;
    gsl_matrix *mass_matrix, *k_matrix;
    gsl_matrix *d_matrix;

    assert(sim.beads != NULL);
    assert(sim.connections != NULL);

    printf("Performing an analytical solution for a ");
    if (sim.sim_type == STRING)
        printf("string with ");
    else if (sim.sim_type == SPRING)
        printf("spring with ");
    printf("%d beads.\n", sim.num_beads);

    /* Check that data was loaded correctly
    if (sim.sim_type == STRING)
        printf("String tension: %f\n", sim.tension);

    for (i = 0; i < sim.num_beads; i++)
    {
        printf("Length: %lf\n", sim.connections[i]);
        printf("Mass: %lf x0: %lf v0: %lf\n", sim.beads[i].mass,
                sim.beads[i].x0, sim.beads[i].v0);
    }
    printf("Length: %lf\n", sim.connections[sim.num_beads]);
    */

    mass_matrix = create_mass_matrix(sim.beads, sim.num_beads);
    if (sim.sim_type == SPRING)
        k_matrix = create_spring_k_matrix(sim.connections, sim.num_beads);
    if (sim.sim_type == STRING)
        k_matrix = create_string_k_matrix(sim.connections, sim.tension,
                sim.num_beads);

    /* Check that matrices were loaded correctly */
    printf("Mass matrix:\n");
    print_matrix(mass_matrix);
    printf("k matrix:\n");
    print_matrix(k_matrix);

    d_matrix = create_d_matrix(mass_matrix, k_matrix);

    result = find_normal_modes(d_matrix, mass_matrix);

    gsl_matrix_free(mass_matrix);
    gsl_matrix_free(k_matrix);
    gsl_matrix_free(d_matrix);

    /* result.num_modes = sim.num_beads; */
    return result;
}


