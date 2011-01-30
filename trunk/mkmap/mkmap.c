/************************************************************************
 *  Copyright (C) 2010 Tomas Kindahl                                    *
 ************************************************************************
 *  This file is part of mkmap                                          *
 *                                                                      *
 *  mkmap is free software: you can redistribute it and/or modify it    *
 *  under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or   *
 *  (at your option) any later version.                                 *
 *                                                                      *
 *  mkmap is distributed in the hope that it will be useful,            *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *  GNU General Public License for more details.                        *
 *                                                                      *
 *  You should have received a copy of the GNU General Public License   *
 *  along with mkmap. If not, see <http://www.gnu.org/licenses/>.       *
 ************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>        /* For time stamping the images in debug mode */
#include "usio.h"
#include "ucstr.h"
#include "token.h"
/*#include "parse.h"*/

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
    uchar *name;
    int width, height, dim;
    double scale;
    lambert_proj *proj;
} image_struct;

image_struct *new_image(uchar *name, int width, int height, double scale) {
    image_struct *res = (image_struct *)malloc(sizeof(image_struct));
    res->name = ucsdup(name);
    res->width = width;
    res->height = height;
    if(width > height) res->dim = width; else res->dim = height;
    res->scale = scale;
    return res;
}

image_struct *image_set_projection(image_struct *image, lambert_proj *proj) {
    image->proj = proj;
    return image;
}

typedef struct _star_T {
    int HIP;
    double RA, DE, vmag;
} star;

star *new_star(int HIP, double RA, double DE, double vmag) {
    star *res = (star *)malloc(sizeof(star));
    res->HIP = HIP; res->RA = RA; res->DE = DE; res->vmag = vmag;
    return res;
}

typedef struct _star_vec_S {
    int size;
    int next;
    star **S;
} star_vec;

star_vec *new_star_vec(int size) {
    star_vec *res = (star_vec *)malloc(sizeof(star_vec));
    res->size = size;
    res->next = 0;
    res->S = (star **)malloc(sizeof(star *)*size);
    return res;
}

int append_star(star_vec *SV, star *S) {
    if (SV->next == SV->size) {
        int newsize = SV->size<<1; /* double it */
        int ix;
        star **NS = (star **)malloc(sizeof(star *)*newsize);
        for(ix = 0; ix < SV->size; ix++) {
            NS[ix] = SV->S[ix];
        }
        free(SV->S);
        SV->S = NS;
        SV->size = newsize;
        SV->S[SV->next++] = S;
        return 1;
    }
    SV->S[SV->next++] = S;
    return 0;
}

void dump_star(FILE *stream, star *S) {
    fprintf(stream, "  (star: HIP =% 7i, α = %12.8f, δ = %12.8f, m = %4.2f)\n",
            S->HIP, S->RA, S->DE, S->vmag);
}

void dump_star_vec(FILE *stream, star_vec *SV) {
    int ix;
    fprintf(stream, "star_vector: size=%i, alloc=%i {\n", SV->next, SV->size);
    for(ix = 0; ix < SV->next; ix++) {
        dump_star(stream, SV->S[ix]);
    }
    fprintf(stream, "}\n");
}

typedef struct _program_state_S {
    /* hard-coded layers */
    star_vec *star_vector;
    /* hard-coded image data */
    image_struct *image;
    FILE *out_file;
    int debug;
} program_state;

program_state *new_program_state(int debug, FILE *debug_out) {
    program_state *res = (program_state *)malloc(sizeof(program_state));
    res->debug = debug;
    res->star_vector = new_star_vec(1024);
    return res;
}

image_struct *program_set_image(program_state *prog, image_struct *image) {
    prog->image = image;
    return image;
}

#define BETW(LB,X,UB) (((LB)<(X))&&((X)<(UB)))

int pos_in_frame(double *x, double *y, double X, double Y, image_struct *frame) {
    *x = frame->width/2-frame->dim*X*frame->scale;
    *y = frame->height/2-frame->dim*Y*frame->scale;
    return BETW(0,*x,frame->width) && BETW(0,*y,frame->height);
}

int open_file(char *fname, program_state *pstat) {
    pstat->out_file = fopen(fname, "wt");
    if (!pstat->out_file) return 0;
    return 1;
}

void head(program_state *pstat) {
    int ix, iy, H, W, H2, W2, dim;
    double ra, ras[24], de, des[17];
    double X, Y;
    double x, y;
    image_struct *image = pstat->image;
    lambert_proj *proj = image->proj;
    FILE *out = pstat->out_file;

    W = image->width; W2 = W/2;
    H = image->height; H2 = H/2;
    dim = image->dim;
    fprintf(out, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
    fprintf(out, "<svg width=\"%i\" height=\"%i\"\n"
                 "     xmlns=\"http://www.w3.org/2000/svg\"\n"
                 "     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
                 "     >\n", W, H);
    fprintf(out, "    <rect style=\"opacity:1;fill:#000033;fill-opacity:1;stroke:none;"
                 "stroke-width:0.2;stroke-linejoin:miter;stroke-miterlimit:4;"
                 "stroke-dasharray:none;stroke-opacity:1\"\n"
                 "          width=\"%i\" height=\"%i\"/>\n", W, H);
    if (pstat->debug == DEBUG) {
        /* Helper lines, paper coordinates: */
        /* vertical */
        for (ix = 100; ix < 560; ix += 100) {
            fprintf(out, "    <path style=\"stroke:#000000;stroke-width:1px\" "
                         "d=\"M %i,0 L %i,%i\"/>\n", ix, ix, H);
            fprintf(out, "    <path style=\"stroke:#080808;stroke-width:1px\" "
                         "d=\"M %i,0 L %i,%i\"/>\n", ix-50, ix-50, H);
        }
        /* horizontal */
        for (iy = 100; iy < 560; iy += 100) {
            fprintf(out, "    <path style=\"stroke:#000000;stroke-width:1px\" "
                         "d=\"M 0,%i L %i,%i\"/>\n", iy, W, iy);
            fprintf(out, "    <path style=\"stroke:#080808;stroke-width:1px\" "
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
            if (pos_in_frame(&x, &y, X, Y, image)) {
                fprintf(out, "    <circle cx=\"%.2f\" cy=\"%.2f\" r=\"1\"\n", x, y);
                fprintf(out, "            style=\"opacity:1;fill:#880088;");
                fprintf(out, "fill-opacity:1;\"/>\n");
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
            if (pos_in_frame(&x, &y, X, Y, image)) {
                fprintf(out, "    <circle cx=\"%.2f\" cy=\"%.2f\" r=\"1\"\n", x, y);
                fprintf(out, "            style=\"opacity:1;fill:#880088;");
                fprintf(out, "fill-opacity:1;\"/>\n");
            }
        }
    }
}

double next_dfield(uchar **pos) {
    *pos = ucschr(*pos,'|')+1;
    return ucstof(*pos);
}

int load_stars(char *fname, program_state *pstat) {
    /*
    DROP TABLE _cmap;
    SELECT hip, ra, de, vmag, _bv, _hvartype, _multflag, _sptype into _cmap 
           from _hipp where vmag < 6.5 order by vmag, ra, de;
    COPY _cmap TO '/home/rursus/Desktop/dumps/sternons/trunk/star.db'
         DELIMITER '|';
    */
    int HIP;
    double RA, DE, vmag;
    uchar line[1024], *pos;
    utf8_file *inf = u8fopen(fname);
    star *S = 0;

    if (!inf) return 0;
    /* LOAD THE STARS */
    while (fgetus(line, 1023, inf)) {
        line[ucslen(line)-1] = L'\0';
        pos = line;
        HIP = ucstoi(pos);
        RA =  next_dfield(&pos);
        DE =  next_dfield(&pos);
        vmag = next_dfield(&pos);
        S = new_star(HIP, RA, DE, vmag);
        append_star(pstat->star_vector, S);
    }
    return 1;
}

void draw_stars(program_state *pstat) {
    /*
    DROP TABLE _cmap;
    SELECT hip, ra, de, vmag, _bv, _hvartype, _multflag, _sptype into _cmap 
           from _hipp where vmag < 6.5 order by vmag, ra, de;
    COPY _cmap TO '/home/rursus/Desktop/dumps/sternons/trunk/star.db'
         DELIMITER '|';
    */
    int ix;
    int HIP;
    double RA, DE, vmag;
    double X, Y, size;
    double x, y;
    image_struct *image = pstat->image;
    lambert_proj *proj = image->proj;
    FILE *out = pstat->out_file;
    star *S = 0;

    /* DRAW THE STARS */
    for (ix = 0; ix < pstat->star_vector->next; ix++) {
        S = pstat->star_vector->S[ix];
        DE = S->DE; RA = S->RA; vmag = S->vmag;
        Lambert(&X, &Y, deg2rad(DE), deg2rad(RA), proj);
        if(pos_in_frame(&x, &y, X, Y, image)) {
            size = (6.8-vmag)*0.8*image->scale;
            fprintf(out, "    <circle title=\"HIP %i\" cx=\"%.2f\" cy=\"%.2f\" r=\"%g\"\n",
                             HIP, x, y, size);
            fprintf(out, "            style=\"opacity:1;fill:#FFFFFF;fill-opacity:1;"
                         "stroke:#666666;stroke-width:1px\"/>\n");
        }
    }
    /* dump_star_vec(stderr, pstat->star_vector); */
}

void write_version(program_state *pstat) {
    struct tm *TM;
    time_t T = time(NULL);
    int height = pstat->image->height;

    if (pstat->debug == DEBUG) {
        T = time(NULL);
        if (T == -1) return;
        TM = localtime(&T);
        fprintf(pstat->out_file, "    <text x=\"%i\" y=\"%i\" "
                "style=\"fill:#CC0000;font-size:12px\">generated by mkmap "
                "%04i-%02i-%02i, %02i:%02i"
                "</text>\n",
                10, height - 10,
                TM->tm_year+1900, TM->tm_mon+1, TM->tm_mday,
                TM->tm_hour, TM->tm_min
                );
    }
}

void foot(program_state *pstat) {
    /* printf("    <image y=\"200\" x=\"200\" height=\"100\" width=\"100\"\n"); */
    /* printf("           xlink:href=\"neptune.png\" />\n"); */
    fprintf(pstat->out_file, "</svg>\n");
}

int close_file(program_state *pstat) {
    fclose(pstat->out_file);
    return 0;
}

void usage_exit(void) {
    /* Usage text here when stabilized */
    exit(-1);
}

void tok_dump(int debug, token *tok) {
    char buf[1024], buf2[1024];

    switch (tok->type) {
      case TOK_STR:
        fprintf(stderr, "⟨%s⟩“%s”", tok_type_str(tok), tok_str(buf, tok, 1023));
        break;
      case TOK_NUM:
        fprintf(stderr, "⟨%s⟩%s(%s)", tok_type_str(tok),
                tok_str(buf, tok, 1023), tok_unit(buf2, tok, 1023));
        break;
      default:                    
        fprintf(stderr, "⟨%s⟩%s", tok_type_str(tok), tok_str(buf, tok, 1023));
    }
}

int main (int argc, char **argv) {
    /* dummy setup: */
    program_state *pstat;
    lambert_proj *proj;
    /*lambert_proj *proj = init_Lambert_deg(107.5, 0, 10, 20); Monoceros hack*/
    uchar _L_Orion[] = {'O','r','i','o','n',0};
    image_struct *image;

    if (argc != 3) usage_exit();
    /* init: */
    /*if (!parse_program(argv[1], pstat))
        usage_exit();*/

    pstat = new_program_state(DEBUG, stderr);
    load_stars(argv[2], pstat);

    proj = init_Lambert_deg(80, 0, 10, 20);
    image = new_image(_L_Orion, 500, 500, 1.4);
    program_set_image(pstat, image);
    image_set_projection(image, proj);

    /*>Arg handling here! */
    /*>---A₀:   mkmap /stardb/              -- star db only                          ---*/
    /*>---A₁:   mkmap /dummyprog/ /stardb/  -- prog loaded but unused                ---*/
    /*>   A₁,₅: mkmap /dummyprog/ /stardb/  -- prog loaded partially used            ---*/
    /*>   A₂:   mkmap /prog/ /stardb/       -- prog loaded and used for std setting     */
    /*>   A₃:   mkmap /prog/                -- prog also used for star db loading       */
    /*>   A₄:     /dismissed/                                                           */
    /*>   A₅:   mkmap /prog/ö /arg₁/ ...     -- make the 2++ arg real arguments         */

    /* generate one output map: */
    if (open_file("orion.svg", pstat)) {
        head(pstat);
        draw_stars(pstat);
        write_version(pstat);
        foot(pstat);
        close_file(pstat);
    }
    else {
        fprintf(stderr, "ERROR: couldn't write file 'orion.svg'\n");
    }

    return 0;
}
