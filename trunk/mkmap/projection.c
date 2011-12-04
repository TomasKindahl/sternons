#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "meta.h"
#include "mathx.h"
#include "projection.h"

/* ================ LAMBERT CONFORMAL CONIC PROJECTION ================ |
   (http://en.wikipedia.org/wiki/Lambert_conformal_conic_projection and |
    http://mathworld.wolfram.com/LambertConformalConicProjection.html)  |
                                                                        |
| λ longitude, λ₀ reference longitude, (middle?)                        |
| φ latitude, φ₀ reference latitude, (middle?)                          |
| φ₁, φ₂ standard parallels    (chosen by map designer)                 |
                                                                        |
    n = ln(cos φ₁·sec φ₂)/ln(tan(¼π + ½φ₂)·cot(¼π + ½φ₁))               |
    F = cos φ₁·tanⁿ(¼π + ½φ₁)/n                                         |
    ρ = F·cotⁿ(¼π + ½φ)                                                 |
    ρ₀ = F·cotⁿ(¼π + ½φ₀)                                               |
                                                                        |
    x = ρ·sin(n(λ - λ₀))                                                |
    y = ρ₀ - ρ·cos(n(λ - λ₀))                                           |
                                                                        |
| where sec ν = 1/cos ν                                                 |
                                                                        |
| The inverse computation (Javascript or OpenGL implementation):        |
    ρ = sign(n)√(x²+(ρ₀-y)²)                                            |
    θ = tan⁻¹(x/(ρ₀-y))                                                 |
                                                                        |
    φ = 2·tan⁻¹((F/ρ) ^ (1/n))-½π                                       |
    λ = λ₀ + θ/n                                                        |
| ===================================================================== */

typedef struct _lambert_proj_S {
	int proj_type;
    double F;
    double lambda0;
    double n;
    /* double phi0, phi1, phi2; */
    double rho0;
} lambert_proj;

double pi_itv(double nu) {
    int K = nu/M_PI;
    return nu-K*M_PI*2;
}

lambert_proj *_init_Lambert(double lambda0, double phi0, double phi1, double phi2) {
    lambert_proj *res = ALLOC(lambert_proj);
    res->proj_type = PROJ_LAMBERT_LCC;
    res->lambda0 = lambda0;
    /* res->phi0 = phi0; res->phi1 = phi1; res->phi2 = phi2; */
    res->n = log(cos(phi1)*sec(phi2))/log(tan(M_PI_4+phi2/2)*cot(M_PI_4+phi1/2));
    res->F = cos(phi1)*pow(tan(M_PI_4+phi1/2),res->n)/res->n;
    res->rho0 = res->F*pow(cot(M_PI_4+phi0/2),res->n);
    return res;
}

void Lambert(double *x, double *y, double phi, double lambda, lambert_proj *LCCP) {
    double rho = LCCP->F*pow(cot(M_PI_4 + phi/2),LCCP->n);
    double n_lambda_D = LCCP->n*(pi_itv(lambda - LCCP->lambda0));
    *x = rho*sin(n_lambda_D);
    *y = LCCP->rho0 - rho*cos(n_lambda_D);
}

proj *init_Lambert(double l0, double p0, double p1, double p2) {
    return (proj *)_init_Lambert(deg2rad(l0), deg2rad(p0), deg2rad(p1), deg2rad(p2));
}

void project(double *x, double *y, double phi, double lambda, proj *P) {
	if( P->proj_type == PROJ_LAMBERT_LCC ) {
        Lambert(x, y, phi, lambda, (lambert_proj *)P);
	}
	else {
		fprintf(stderr, "FATAL: unsupported projection type %i\n", P->proj_type);
		exit(-1);
	}
}

