/************************************************************************
 *  Copyright (C) 2010 Tomas Kindahl                                    *
 ************************************************************************
 *  This file is part of mkmap                                          *
 *                                                                      *
 *  mkmap is free software: you can redistribute it and/or modify it    *
 *  under the terms of the GNU Lesser General Public License as         *
 *  published by the Free Software Foundation, either version 3 of the  *
 *  License, or (at your option) any later version.                     *
 *                                                                      *
 *  mkmap is distributed in the hope that it will be useful,            *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *  GNU Lesser General Public License for more details.                 *
 *                                                                      *
 *  You should have received a copy of the GNU Lesser General Public    *
 *  License along with mkmap. If not, see                               *
 *  <http://www.gnu.org/licenses/>.                                     *
 ************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "usio.h"
#include "ucstr.h"
#include "scan.h"

/* ==================================================================== |
     Known bugs and features:                                           |
       F1: a λ₀ outside [0,360[ generates black declination sections in |
           the map - avoid (in 2010-07-26)                              |
       F2: a φ₁ or φ₂ outside [-90,90] generates a black map            |
       F3: the declination circle dots become too tight ±70° and        |
           polewards (in 2010-07-26)                                    |
       F4:                                                              |
   ==================================================================== */

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

double cot(double x) { return 1/tan(x); }
double sec(double x) { return 1/cos(x); }
double deg2rad(double x) { return M_PI * x / 180.0; }
double rad2deg(double x) { return 180.0 * x / M_PI; }

#define NO_DEBUG 0
#define DEBUG 1

typedef struct _program_state_S {
    int debug;
} program_state;

program_state *new_program_state(int debug) {
    program_state *res = (program_state *)malloc(sizeof(program_state));
    res->debug = debug;
    return res;
}

typedef struct _lambert_proj_S {
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

lambert_proj *init_Lambert(double lambda0, double phi0, double phi1, double phi2) {
    lambert_proj *res = (lambert_proj *)malloc(sizeof(lambert_proj));
    res->lambda0 = lambda0;
    /* res->phi0 = phi0; res->phi1 = phi1; res->phi2 = phi2; */
    res->n = log(cos(phi1)*sec(phi2))/log(tan(M_PI_4+phi2/2)*cot(M_PI_4+phi1/2));
    res->F = cos(phi1)*pow(tan(M_PI_4+phi1/2),res->n)/res->n;
    res->rho0 = res->F*pow(cot(M_PI_4+phi0/2),res->n);
    return res;
}

lambert_proj *init_Lambert_deg(double l0, double p0, double p1, double p2) {
    return init_Lambert(deg2rad(l0), deg2rad(p0), deg2rad(p1), deg2rad(p2));
}

void Lambert(double *x, double *y, double phi, double lambda, lambert_proj *LCCP) {
    double rho = LCCP->F*pow(cot(M_PI_4 + phi/2),LCCP->n);
    double n_lambda_D = LCCP->n*(pi_itv(lambda - LCCP->lambda0));
    *x = rho*sin(n_lambda_D);
    *y = LCCP->rho0 - rho*cos(n_lambda_D);
}

typedef struct _image_struct_S {
    int width, height, dim;
    double aspect;
} image_struct;

image_struct *new_image(int width, int height, double aspect) {
    image_struct *res = (image_struct *)malloc(sizeof(image_struct));
    res->width = width;
    res->height = height;
    if(width > height) res->dim = width; else res->dim = height;
    res->aspect = aspect;
    return res;
}

#define BETW(LB,X,UB) (((LB)<(X))&&((X)<(UB)))

int pos_in_frame(int *x, int *y, double X, double Y, image_struct *frame) {
    *x = frame->width/2-frame->dim*X*frame->aspect;
    *y = frame->height/2-frame->dim*Y*frame->aspect;
    return BETW(0,*x,frame->width) && BETW(0,*y,frame->height);
}

void head(lambert_proj *proj, image_struct *frame, program_state *progstate) {
    int ix, iy, H, W, H2, W2, dim;
    double ra, ras[24], de, des[17];
    double X, Y, A;
    int x, y;

    W = frame->width; W2 = W/2;
    H = frame->height; H2 = H/2;
    dim = frame->dim;
    A = frame->aspect;
    printf("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
    printf("<svg width=\"%i\" height=\"%i\"\n"
           "     xmlns=\"http://www.w3.org/2000/svg\"\n"
           "     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
           "     >\n", W, H);
    printf("<rect style=\"opacity:1;fill:#000033;fill-opacity:1;stroke:none;"
           "stroke-width:0.2;stroke-linejoin:miter;stroke-miterlimit:4;"
           "stroke-dasharray:none;stroke-opacity:1\"\n"
           "      width=\"%i\" height=\"%i\"/>\n", W, H);
    if (progstate->debug == DEBUG) {
        /* Helper lines, paper coordinates: */
        /* vertical */
        for (ix = 100; ix < 560; ix += 100) {
            printf("<path style=\"stroke:#000000;stroke-width:1px\" "
                   "d=\"M %i,0 L %i,%i\"/>\n", ix, ix, H);
            printf("<path style=\"stroke:#080808;stroke-width:1px\" "
                   "d=\"M %i,0 L %i,%i\"/>\n", ix-50, ix-50, H);
        }
        /* horizontal */
        for (iy = 100; iy < 560; iy += 100) {
            printf("<path style=\"stroke:#000000;stroke-width:1px\" "
                   "d=\"M 0,%i L %i,%i\"/>\n", iy, W, iy);
            printf("<path style=\"stroke:#080808;stroke-width:1px\" "
                   "d=\"M 0,%i L %i,%i\"/>\n", iy-50, W, iy-50);
        }
    }
    /* Declination lines: */
    #define NUM_DE 17

    for (iy = 0; iy < NUM_DE; iy++) {
        des[iy] = deg2rad(iy*10-80);
    }
    for (ra = 0; ra < 360; ra+=0.5) {
        for (ix = 0; ix < NUM_DE; ix++) {
            Lambert(&X, &Y, des[ix], deg2rad(ra), proj);
            if (pos_in_frame(&x, &y, X, Y, frame)) {
                printf("    <circle cx=\"%i\" cy=\"%i\" r=\"1\"\n", x, y);
                printf("            style=\"opacity:1;fill:#880088;");
                printf("fill-opacity:1;\"/>\n");
                /*       W2-X*dim*A, H2-Y*dim*A); */
            }
        }
    }
    /* Right Ascension lines: */
    #define NUM_RA 24
    for (ix = 0; ix < NUM_RA; ix++) {
        ras[ix] = deg2rad(ix*15);
    }
    for (de = -80; de <= 80; de+=0.5) {
        for (iy = 0; iy < NUM_RA; iy++) {
            Lambert(&X, &Y, deg2rad(de), ras[iy], proj);
            if (pos_in_frame(&x, &y, X, Y, frame)) {
                printf("    <circle cx=\"%i\" cy=\"%i\" r=\"1\"\n", x, y);
                printf("            style=\"opacity:1;fill:#880088;");
                printf("fill-opacity:1;\"/>\n");
            }
        }
    }
}

int draw_stars(char *fname, lambert_proj *proj, image_struct *frame) {
    int HIP;
    double RA, DE, mag;
    double X, Y, size;
    int x, y;
    uchar line[1024], *pos;
    utf8_file *inf = u8fopen(fname);

    if (!inf) return 0;
    while (fgetus(line, 1023, inf)) {
        line[ucslen(line)-1] = L'\0';
        /*
        DROP TABLE _cmap;
        SELECT hip, ra, de, vmag, _bv, _hvartype, _multflag, _sptype into _cmap 
               from _hipp where vmag < 6.5 order by vmag, ra, de;
        */
        HIP = ucstoi(&line[0]);
        pos = ucschr(line,'|')+1; RA = ucstof(pos);
        pos = ucschr(pos,'|')+1; DE = ucstof(pos);
        pos = ucschr(pos,'|')+1; mag = ucstof(pos);
        /*
        printf("%c HIP =% 7i, α = %12.8f, δ = %12.8f, m = %4.2f ⟨%s⟩\n",
               nl?'+':' ', HIP, RA, DE, mag, ucstombs(buf,line,1023));
        */
        Lambert(&X, &Y, deg2rad(DE), deg2rad(RA), proj);
        if(pos_in_frame(&x, &y, X, Y, frame)) {
            size = (6.8-mag)*0.8*frame->aspect;
            printf("    <circle cx=\"%i\" cy=\"%i\" r=\"%g\"\n", x, y, size);
            printf("            style=\"opacity:1;fill:#FFFFFF;fill-opacity:1;"
                   "stroke:#666666;stroke-width:1px\"/>\n");
        }
    }
    return 1;
}

void foot(void) {
    /* printf("    <image y=\"200\" x=\"200\" height=\"100\" width=\"100\"\n"); */
    /* printf("           xlink:href=\"neptune.png\" />\n"); */
    printf("</svg>\n");
}

void usage_exit(void) {
    /* Usage text here when stabilized */
    exit(-1);
}

int read_program( char *program, 
                  lambert_proj *projection,
                  image_struct *frame,
                  program_state *progstate )
{
    token_file *pfile;
    token *tok;
    char buf[1024], buf2[1024];

    if (!(pfile = tokfopen(program))) {
        fprintf(stderr, "ERROR: program '%s' not found\n", program);
        return 0;
    }
    fprintf(stderr, "INFO: program '%s' opened\n", program);
    if (progstate->debug == DEBUG) {
        tok = scan(pfile);
        while (!tokfeof(pfile)) {
            switch (tok->type) {
              case TOK_STR:
                  fprintf(stderr, "⟨%s⟩“%s”\n", tok_type_str(tok), tok_str(buf, tok, 1023));
                  break;
                case TOK_NUM:
                  fprintf(stderr, "⟨%s⟩%s(%s)\n", tok_type_str(tok),
                          tok_str(buf, tok, 1023), tok_unit(buf2, tok, 1023));
                  break;
              default:                    
                  fprintf(stderr, "⟨%s⟩%s\n", tok_type_str(tok), tok_str(buf, tok, 1023));
            }
            tok_free(tok);
            tok = scan(pfile);
        }
    }

    fprintf(stderr, "INFO: program '%s' read\n", program);
    tokfclose(pfile);
    return 1;
}

int main (int argc, char **argv) {
    /* dummy setup: */
    program_state *progstate = new_program_state(DEBUG);
    image_struct *frame = new_image(500, 500, 1.4);
    lambert_proj *projection = init_Lambert_deg(80, 0, 10, 20);

    /*>Arg handling here! */
    /*>---A₀: mkmap /stardb/              -- star db only                        ---*/
    /*>   A₁: mkmap /dummyprog/ /stardb/  -- prog loaded but unused                 */
    /*>   A₂: mkmap /prog/ /stardb/       -- prog loaded and used for std setting   */
    /*>   A₃: mkmap /prog/                -- prog also used for star db loading     */
    /*>   A₄: mkmap /prog/ /out/          -- output spec'd and generated acc'2      */
    /*>                                      file type                              */
    /*>   A₅: mkmap /prog/ /arg₁/ ...     -- make the 2++ arg real arguments        */

    if (argc != 3) usage_exit();

    /* init: */
    if (!read_program(argv[1], projection, frame, progstate)) usage_exit();

    /*  */
    head(projection, frame, progstate);
    draw_stars(argv[2], projection, frame);
    foot();

    return 0;
}
