/*----------------------------------------------------------------------------*/
/* asolve.c                                                                   */
/* Author: Godwin Duan                                                        */
/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_linalg.h>

#include "asolve.h"

/* Prints gsl_matrix m. Used for debugging. */
#ifndef NDEBUG
static void print_matrix(const gsl_matrix *m)
{
    int i, j;
    int rows, cols;

    assert(m != NULL);
    
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
#endif

/* Creates and returns a diagonal square matrix of size num_beads x num_beads.
 * Diagonal entries correspond to masses of beads. Caller responsible for
 * freeing matrix. */
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

/* Creates and returns a tridiagonal square matrix of size num_beads x
 * num_beads. Entries correspond to the spring constants that connect beads.
 * Caller responsible for freeing matrix. */
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

/* Creates and returns a tridiagonal square matrix of size num_beads x
 * num_beads. Entries correspond to the string tensions and lengths that
 * connect beads. Caller responsible for freeing matrix. */
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

/* Creates and returns the inverse square root of mass_matrix. Caller
 * responsible for freeing matrix. */
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

    return invsqrtm;
}

/* Creates and returns the dynamical D matrix given by
 * (mass_matrix)^-1/2(k_matrix)(mass_matrix)^-1/2. Caller responsible for
 * freeing matrix. */
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
    
    gsl_matrix_free(invsqrtm);
    gsl_matrix_free(tempm);

    return d_matrix;
}

/* Calculates the normal modes of the system defined by mass_matrix and k_matrix
 * and saves resultant eigenfrequencies and eigenvectors in the Result it
 * returns. Caller responsible for freeing eigenfrequencies and eigenvectors. */
static Result find_normal_modes(const gsl_matrix *mass_matrix,
        const gsl_matrix *k_matrix)
{
    Result result;
    gsl_eigen_symmv_workspace *w;
    gsl_vector *eval;
    gsl_matrix *evec;
    gsl_matrix *d_matrix;
    gsl_matrix *invsqrtm;
    int i, j;

    assert(mass_matrix != NULL);
    assert(k_matrix != NULL);

    d_matrix = create_d_matrix(mass_matrix, k_matrix);

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

    gsl_vector_free(eval);
    gsl_matrix_free(evec);
    gsl_matrix_free(invsqrtm);
    gsl_matrix_free(d_matrix);

    return result;
}

/* Given result containing eigenfrequencies and eigenvectors, finds and returns
 * coefficients that satisfy the initial conditions given in beads. a
 * corresponds to the cosine term and b corresponds to the sine term. Caller
 * responsible for freeing coefficient array. */
static Coefficient *apply_ics(Bead *beads, Result result)
{
    Coefficient *coeffs;
    gsl_vector *ix, *iv, *a, *b;
    gsl_matrix *eigv;
    gsl_permutation *p;
    int i, j;

    assert(beads != NULL);
    assert(result.eigenfrequencies != NULL);
    assert(result.eigenvectors != NULL);

    coeffs = malloc(result.num_modes * sizeof(Coefficient));
    /* Skipping error checking */

    a = gsl_vector_alloc(result.num_modes);
    b = gsl_vector_alloc(result.num_modes);

    ix = gsl_vector_alloc(result.num_modes);
    iv = gsl_vector_alloc(result.num_modes);
    for (i = 0; i < result.num_modes; i++)
    {
        gsl_vector_set(ix, i, beads[i].x0);
        gsl_vector_set(iv, i, beads[i].v0);
    }

    eigv = gsl_matrix_alloc(result.num_modes, result.num_modes);
    for (i = 0; i < result.num_modes; i++)
        for (j = 0; j < result.num_modes; j++)
            gsl_matrix_set(eigv, i, j, result.eigenvectors[i][j]);

    /* Initial positions calculation */
    /* Solved using LU decomposition */
    p = gsl_permutation_alloc(result.num_modes);
    gsl_linalg_LU_decomp (eigv, p, &i);
    gsl_linalg_LU_solve (eigv, p, ix, a);
    
    /* Initial velocities calculation */
    gsl_linalg_LU_solve (eigv, p, iv, b);

    /* Load results into coefficients array */
    for (i = 0; i < result.num_modes; i++)
    {
        coeffs[i].a = gsl_vector_get(a, i);
        /* For velocity terms, we divide by the eigenfrequency since we took a 
         * derivative */
        coeffs[i].b = gsl_vector_get(b, i) / result.eigenfrequencies[i];
    }

    gsl_vector_free(a);
    gsl_vector_free(b);
    gsl_vector_free(ix);
    gsl_vector_free(iv);
    gsl_matrix_free(eigv);
    gsl_permutation_free(p);

    return coeffs;
}

Result asolve(Simulation sim)
{
    Result result;
    gsl_matrix *mass_matrix, *k_matrix;

    assert(sim.beads != NULL);
    assert(sim.connections != NULL);

    printf("Performing an analytical solution for a ");
    if (sim.sim_type == STRING)
        printf("string with ");
    else if (sim.sim_type == SPRING)
        printf("spring with ");
    printf("%d beads.\n", sim.num_beads);
    printf("\n");

    mass_matrix = create_mass_matrix(sim.beads, sim.num_beads);
    if (sim.sim_type == SPRING)
        k_matrix = create_spring_k_matrix(sim.connections, sim.num_beads);
    if (sim.sim_type == STRING)
        k_matrix = create_string_k_matrix(sim.connections, sim.tension,
                sim.num_beads);

    result = find_normal_modes(mass_matrix, k_matrix);
    result.coefficients = apply_ics(sim.beads, result);

    gsl_matrix_free(mass_matrix);
    gsl_matrix_free(k_matrix);

    return result;
}


