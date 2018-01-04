#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include <mkl.h>
#include <mkl_types.h>
#include "utils.h"
#include "projector.h"

#define PI 3.14159265359
#define c 0.262465831

void vc_pseudoprojection(pswf_t* wf_ref, pswf_t* wf_proj, int BAND_NUM, double* results) {

	clock_t start = clock();
	kpoint_t** kpts = wf_ref->kpts;
	kpoint_t** kptspro = wf_proj->kpts;
	int NUM_KPTS = wf_ref->nwk * wf_ref->nspin;
	int NUM_BANDS = wf_ref->nband;

	double* cband = (double*) calloc(NUM_KPTS, sizeof(double));
	double* vband = (double*) calloc(NUM_KPTS, sizeof(double));

	#pragma omp parallel for 
	for (int b = 0; b < NUM_BANDS; b++)
	{
		for (int kpt_num = 0; kpt_num < NUM_KPTS; kpt_num++)
		{
			float complex curr_overlap = 0;
			float complex* C1s = kptspro[kpt_num]->bands[0]->Cs;
			float complex* C2s = kpts[kpt_num]->bands[b]->Cs;
			int num_waves = kpts[kpt_num]->bands[b]->num_waves;
			for (int w = 0; w < num_waves; w++)
			{
				curr_overlap += C1s[w] * conj(C2s[w]);
			}
			#pragma omp critical
			{
				if (kpts[kpt_num]->bands[b]->occ > 0.5)
					vband[kpt_num] += creal((double) (curr_overlap * conj(curr_overlap)));
				else
					cband[kpt_num] += creal((double) (curr_overlap * conj(curr_overlap)));
			}
		}
	}

	double ctotal = 0.0;
	double vtotal = 0.0;
	for (int kpt_num = 0; kpt_num < NUM_KPTS; kpt_num++) {
		ctotal += cband[kpt_num] * kpts[kpt_num]->weight;
		vtotal += vband[kpt_num] * kpts[kpt_num]->weight;
	}

	printf("%lf\n", creal(kptspro[0]->bands[0]->energy));
	printf("c %lf\n", ctotal);
	printf("v %lf\n", vtotal);

	free(vband);
	free(cband);
	results[0] = vtotal;
	results[1] = ctotal;

	clock_t end = clock();
	printf("%lf seconds for band projection\n", (double)(end - start) / CLOCKS_PER_SEC);

}

float* pseudoprojection(pswf_t* wf_ref, pswf_t* wf_proj, int BAND_NUM) {

	kpoint_t** kpts = wf_ref->kpts;
	kpoint_t** kptspro = wf_proj->kpts;
	int NUM_KPTS = wf_ref->nwk * wf_ref->nspin;
	int NUM_BANDS = wf_ref->nband;

	float* projections = (float*) malloc(2*NUM_BANDS*NUM_KPTS*sizeof(float));

	#pragma omp parallel for 
	for (int b = 0; b < NUM_BANDS; b++)
	{
		for (int kpt_num = 0; kpt_num < NUM_KPTS; kpt_num++)
		{
			float complex curr_overlap = 0;
			float complex* C1s = kptspro[kpt_num]->bands[BAND_NUM]->Cs;
			float complex* C2s = kpts[kpt_num]->bands[b]->Cs;
			int num_waves = kpts[kpt_num]->bands[b]->num_waves;
			for (int w = 0; w < num_waves; w++)
			{
				curr_overlap += C1s[w] * conj(C2s[w]);
			}
			projections[2*(b*NUM_KPTS+kpt_num)] = creal(curr_overlap);
			projections[2*(b*NUM_KPTS+kpt_num)+1] = cimag(curr_overlap);
		}
	}

	return projections;
}

ppot_t* get_projector_list(int num_els, int* labels, int* ls, double* proj_grids, double* wave_grids,
	double* projectors, double* aewaves, double* pswaves) {
	ppot_t* pps = (ppot_t*) malloc(num_els * sizeof(ppot_t));
	int wt = 0;
	int pt = 0;
	int wgt = 0;
	int pgt = 0;
	int l_num = 0;
	for (int i = 0; i < num_els; i++) {
		pps[i].num_projs = labels[4*i+1];
		pps[i].proj_gridsize = labels[4*i+2];
		pps[i].wave_gridsize = labels[4*i+3];
		pps[i].wave_grid = (double*) malloc((pps[i].wave_gridsize+1)*sizeof(double));
		for (int j = 0; j < pps[i].wave_gridsize + 1; j++) {
			pps[i].wave_grid[j] = wave_grids[wgt];
			wgt++;
		}
		pps[i].proj_grid = (double*) malloc(pps[i].proj_gridsize*sizeof(double));
		for (int j = 0; j < pps[i].proj_gridsize; j++) {
			pps[i].proj_grid[j] = proj_grids[pgt];
			pgt++;
		}
		funcset_t* funcs = (funcset_t*) malloc(pps[i].num_projs*sizeof(funcset_t));
		for (int k = 0; k < pps[i].num_projs; k++) {
			funcs[k].proj = (double*) malloc(sizeof(double)*pps[i].proj_gridsize);
			funcs[k].aewave = (double*) malloc(sizeof(double)*pps[i].wave_gridsize);
			funcs[k].pswave = (double*) malloc(sizeof(double)*pps[i].wave_gridsize);
			funcs[k].l = ls[l_num];
			l_num++;
			for (int j = 0; j < pps[i].wave_gridsize; j++) {
				funcs[k].aewave[j] = aewaves[wt];
				funcs[k].pswave[j] = pswaves[wt];
				wt++;
			}
			for (int j = 0; j < pps[i].proj_gridsize; j++) {
				funcs[k].proj[j] = projectors[pt];
				pt++;
			}
		}
		pps[i].funcs = funcs;
		printf("%d\n", pps[i].funcs[0].l);
	}
	return pps;
}

real_proj_site_t* projector_values(int num_sites, int* labels, double* coords,
	double* lattice, ppot_t* pps, int* fftg) {

	int t_projs = 0;
	for (int i = 0; i < num_M; i++) {
		for (int j = 0; j < pps[labels[M[i]]].num_projs; j++)
			t_projs += 2 * pps[labels[M[i]]].funcs[j].l + 1;
	}

	double intervals[3] = {mag(lattice)/fftg[0], mag(lattice+3)/fftg[1], mag(lattice+6)/fftg[2]};
	double vol = determinant(lattice);
	int num_pts = fftg[0] * fftg[1] * fftg[2];

	real_proj_site_t* sites = (real_proj_site_t*) malloc(num_sites * sizeof(real_proj_site_t));
	for (int i = 0; i < num_sites; i++) {
		sites[i].index = i;
		sites[i].elem = labels[i];
		sites[i].num_projs = pps[labels[i]].num_projs;
		sites[i].rmax = pps[labels[i]].rmax;
		sites[i].total_projs = 0;
		sites[i].num_indices = 0;
		for (int j = 0; j < sites[i].num_projs; j++)
			sites[i].total_projs += 2 * pps[labels[i]].funcs[j].l + 1;
		sites[i].indices = calloc(pps[labels[i]].num_cart_gridpts, sizeof(int));
		sites[i].projs = (real_proj_t*) malloc(sites[i].total_projs * sizeof(real_proj_t));
		int p = 0;
		for (int j = 0; j < sites[i].num_projs; j++) {
			for (int m = -pps[labels[i]].funcs[j].l; m <= pps[labels[i]].funcs[j].l; m++) {
				sites[i].projs[p].l = pps[labels[i]].funcs[j].l;
				sites[i].projs[p].m = m;
				sites[i].projs[p].values = calloc(pps[labels[i]].num_cart_gridpts, sizeof(double complex));
				p++;
			}
		}
	}

	for (int i = 0; i < fftg[0]; i++) {
		for (int j = 0; j < fftg[1]; j++) {
			for (int k = 0; k  < fftg[2]; k++) {
				double frac[3] = {(double)i/fftg[0], (double)j/fftg[1], (double)k/fftg[2]};
				for (int p = 0; p < num_sites; p++) {
					if (dist_from_frac(coords+3*p, frac, lattice) < sites[p].rmax) {
						sites[p].indices[sites[p].total_projs] = i*fftg[1]*fftg[2] + j*fftg[2] + k;
						for (int n = 0; n < sites[p].total_projs; n++) {
							sites[p].projs[n].values[sites[p].total_projs] = 
								proj_value(pp.funcs[n], m, pp.rmax, coords+3*p, frac, lattice);
						}
						sites[p].total_projs++;
						break;
					}
				}
			}
		}
	}

	return sites;
}

double complex* onto_projector(int* labels, double* coords, int* G_bounds, double* lattice,
	double* kpt, int* Gs, float complex* Cs, int num_waves, int num_M, int* M, ppot_t* pps, int* fftg) {
	
	MKL_LONG status = 0;
	DFTI_DESCRIPTOR_HANDLE handle = 0;
	MKL_Complex16* x = (MKL_Complex16*) mkl_calloc(fftg[0]*fftg[1]*fftg[2], sizeof(MKL_Complex16), 64);

	for (int w = 0; w < num_waves; w++) {
		int g1 = Gs[3*w]-G_bounds[0], g2 = Gs[3*w+1]-G_bounds[2], g3 = Gs[3*w+2]-G_bounds[4];
		x[g1*fftg[1]*fftg[2] + g2*fftg[2] + g3].real = creal(Cs[w]);
		x[g1*fftg[1]*fftg[2] + g2*fftg[2] + g3].imag = cimag(Cs[w]);
	}

	DftiCreateDescriptor(&handle, DFTI_DOUBLE, DFTI_COMPLEX, 3, fftg);
	DftiCommitDescriptor(handle);
	DftiComputeBackward(handle, x);

	double kmins[3] = {G_bounds[0] + kpt[0], G_bounds[2] + kpt[1], G_bounds[4] + kpt[2]};
	double dv = determinant(lattice) / fftg[0] / fftg[1] / fftg[2];
	inv_sqrt_vol = pow(determinant(lattice), -0.5);

	int t_projs = 0;
	for (int i = 0; i < num_M; i++) {
		for (int j = 0; j < pps[labels[M[i]]].num_projs; j++)
			t_projs += 2 * pps[labels[M[i]]].funcs[j].l + 1;
	}

	double complex* overlap = (double complex*) calloc(t_projs, sizeof(double complex));
	double frac[3];
	double kdotr;

	for (int i = 0; i < fftg[0]; i++) {
		for (int j = 0; j < fftg[1]; j++) {
			for (int k = 0; k  < fftg[2]; k++) {
				frac[0] = (double)i/fftg[0];
				frac[1] = (double)j/fftg[1];
				frac[2] = (double)k/fftg[2];
				kdotr = 2 * PI * dot(kmins, frac);
				x[i*fftg[1]*fftg[2] + j*fftg[2] + k].real *= inv_sqrt_vol * cos(kdotr);
				x[i*fftg[1]*fftg[2] + j*fftg[2] + k].imag *= inv_sqrt_vol * sin(kdotr);
			}
		}
	}

	int s, num_indices, index, t=0;
	for (int m = 0; m < num_M; m++) {
		s = M[m];
		num_indices = sites[s].num_indices;
		int* indices = sites[s].indices;
		for (int p = 0; p < sites[s].total_projs; p++) {
			double complex* values = sites[s].projs[p].values;
			for (int i = 0; i < num_indices; i++) {
				index = indices[i];
				overlap[t] += conj(values[i]) * (x[index].real + I*x[index].imag) * dV;
			}
			t++;
		}
	}

	mkl_free(x);

	return overlap;
}

void add_num_cart_gridpts(ppot_t pp, double* lattice, int* fftg) {

	double maga1 = mag(lattice+0) / fftg[0];
	double maga2 = mag(lattice+3) / fftg[1];
	double maga3 = mag(lattice+6) / fftg[2];

	double vtemp[3];
	double vmag, sinphi123;
	
	double phi12 = acos(dot(lattice+0, lattice+3) / (maga1 * maga2));
	vcross(vtemp, lattice+0, lattice+3);
	vmag = mag(vtemp);
	sinphi123 = dot(lattice+6, vtemp) / (vmag * maga3);
	double na1maxA = sites[i].rmax / (maga1 * fabs(sin(phi12))) + 1;
	double na2maxA = sites[i].rmax / (maga2 * fabs(sin(phi12))) + 1;
	double na3maxA = sites[i].rmax / (maga3 * fabs(sinphi123)) + 1;
	int npmaxA = (int) round(4.0/3.0*PI*na1maxA*na2maxA*na3maxA);

	double phi13 = acos(dot(lattice+0, lattice+6) / (maga1 * maga3));
	vcross(vtemp, lattice+0, lattice+6);
	vmag = mag(vtemp);
	sinphi123 = dot(lattice+3, vtemp) / (vmag * maga2);
	double na1maxB = sites[i].rmax / (maga1 * fabs(sin(phi13))) + 1;
	double na2maxB = sites[i].rmax / (maga2 * fabs(sinphi123)) + 1;
	double na3maxB = sites[i].rmax / (maga3 * fabs(sin(phi13))) + 1;
	int npmaxB = (int) round(4.0/3.0*PI*na1maxB*na2maxB*na3maxB);

	double phi23 = acos(dot(lattice+3, lattice+6) / (maga2 * maga3));
	vcross(vtemp, lattice+3, lattice+6);
	vmag = mag(vtemp);
	sinphi123 = dot(lattice, vtemp) / (vmag * maga1);
	double na1maxC = sites[i].rmax / (maga1 * fabs(sinphi123)) + 1;
	double na2maxC = sites[i].rmax / (maga2 * fabs(sin(phi23))) + 1;
	double na3maxC = sites[i].rmax / (maga3 * fabs(sin(phi23))) + 1;
	int npmaxC = (int) round(4.0/3.0*PI*na1maxC*na2maxC*na3maxC);

	int npmax = npmaxA;
	if (npmaxB < npmax) npmax = npmaxB;
	if (npmaxC < npmax) npmax = npmaxC;

	pp.num_cart_gridpts = npmax;
}

void make_pwave_overlap_matrices(ppot_t pp) {
	int size = pp.num_projs * pp.num_projs;
	double* psov = (double*) calloc(size, sizeof(double));
	double* aeov = (double*) calloc(size, sizeof(double));
	double* diov = (double*) calloc(size, sizeof(double));

	for (int i = 0; i < pp.num_projs; i++) {
		for (int j = i; j < pp.num_projs; j++) {
			if (pp.funcs[i].l == pp.funcs[j].l) {
				double* ps1 = pp.funcs[i].pswave;
				double* ps2 = pp.funcs[j].pswave;
				double* ae1 = pp.funcs[i].aewave;
				double* ae2 = pp.funcs[j].aewave;
				double dr = pp.wave_grid[0];
				double r = pp.wave_grid[0];
				for (int k = 0; k < pp.wave_gridsize; k++) {
					r = pp.wave_grid[k];
					dr = pp.wave_grid[k+1] - pp.wave_grid[k];
					psov[pp.num_projs*i+j] += r * r * ps1[k] * ps2[k] * dr;
					aeov[pp.num_projs*i+j] += r * r * ae1[k] * ae2[k] * dr;
					diov[pp.num_projs*i+j] += r * r * (ae1[k]-ps1[k]) * (ae2[k]-ps2[k]) * dr;
				}
			}
		}
	}
	for (int i = 1; i < pp.num_projs; i++) {
		for (int j = 0; j < i; j++) {
			psov[pp.num_projs*i+j] = conj(psov[pp.num_projs*j+i]);
			aeov[pp.num_projs*i+j] = conj(aeov[pp.num_projs*j+i]);
			diov[pp.num_projs*i+j] = conj(diov[pp.num_projs*j+i]);
		}
	}
	
	pp.pspw_overlap_matrix = psov;
	pp.aepw_overlap_matrix = aeov;
	pp.diff_overlap_matrix = diov;
}

double complex* compensation_terms(int BAND_NUM, pswf_t* wf_proj, pswf_t* wf_ref, ppot_t* pps,
	int num_elems, int num_M, int num_N_R, int num_N_S, int num_N_RS,
	int* M, int* N_R, int* N_S, int* N_RS,
	int* proj_labels, double* proj_coords, int* ref_labels, double* ref_coords,
	int* fft_grid) {

	printf("%d %d %d %d %d %d\n", BAND_NUM, num_elems, num_M, num_N_R, num_N_S, num_N_RS);
	printf("%d %lf %d %lf %d\n", proj_labels[0], proj_coords[0], ref_labels[0], ref_coords[0], fft_grid[1]);
	printf("%d %d %d %d %d %d %d\n", wf_proj->nband, wf_ref->nband, pps[0].funcs[0].l, M[0], N_R[0], N_S[0], N_RS[0]);
	
	int NUM_KPTS = wf_proj->nwk * wf_proj->nspin;
	int NUM_BANDS = wf_proj->nband;

	//#pragma omp parallel for 
	for (int p = 0; p < num_elems; p++) {
		make_pwave_overlap_matrices(pps[p]);
		add_num_cart_gridpts(pps[p], wf_ref->lattice, fftg);
	}

	double complex* overlap = (double complex*) calloc(NUM_BANDS * NUM_KPTS, sizeof(double complex));

	double complex** lst_proj_projs = (double complex**) malloc(NUM_KPTS*sizeof(double complex*));
	#pragma omp parallel for 
	for (int kpt_num = 0; kpt_num < NUM_KPTS; kpt_num++) {
		lst_proj_projs[kpt_num] = onto_projector(proj_labels, proj_coords,
			wf_proj->G_bounds, wf_proj->lattice, wf_proj->kpts[kpt_num]->k,
			wf_proj->kpts[kpt_num]->Gs, wf_proj->kpts[kpt_num]->bands[BAND_NUM]->Cs,
			wf_proj->kpts[kpt_num]->bands[BAND_NUM]->num_waves, num_M, N_RS, pps, fft_grid);
	}

	#pragma omp parallel for
	for (int w = 0; w < NUM_BANDS * NUM_KPTS; w++) {

		double complex* proj_projs = lst_proj_projs[w%NUM_KPTS];
		double complex* ref_projs = onto_projector(ref_labels, ref_coords,
			wf_ref->G_bounds, wf_ref->lattice, wf_ref->kpts[w%NUM_KPTS]->k,
			wf_ref->kpts[w%NUM_KPTS]->Gs, wf_ref->kpts[w%NUM_KPTS]->bands[w/NUM_KPTS]->Cs,
			wf_ref->kpts[w%NUM_KPTS]->bands[w/NUM_KPTS]->num_waves, num_M, M, pps, fft_grid);

		int t = 0;
		for (int s = 0; s < num_M; s++) {
			ppot_t pp = pps[ref_labels[M[s]]];
			int ti = 0;
			int ttemp = 0;
			for (int i = 0; i < pp.num_projs; i++) {
				ttemp += 2*pp.funcs[i].l+1;
				for (int temp1 = 0; temp1 < 2*pp.funcs[i].l+1; temp1++) {
					int tj = 0;
					for (int j = 0; j < pp.num_projs; j++) {
						for (int temp2 = 0; temp2 < 2*pp.funcs[j].l+1; temp2++) {
							overlap[w] += conj(ref_projs[t+tj])
								* (pp.aepw_overlap_matrix[4*i+j] - pp.pspw_overlap_matrix[4*i+j])
								* proj_projs[t+i];
							tj++;
						}
					}
					ti++;
				}
			}
			t += ttemp;
		}

		free(ref_projs);
	}

	for (int kpt_num = 0; kpt_num < NUM_KPTS; kpt_num++) {
		free(lst_proj_projs[kpt_num]);
	}
	free(lst_proj_projs);

	return overlap;
}
