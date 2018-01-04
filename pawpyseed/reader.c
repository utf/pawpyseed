#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include "utils.h"
#include "reader.h"

#define PI 3.14159265359
#define c 0.262465831

void setup(char* filename, int* pnrecl, int* pnspin, int* pnwk, int* pnband,
	double* nb1, double* nb2, double* nb3, double* ecut,
	double* lattice, double* reclattice) {
	
	double readin[3];
	FILE* fp = fopen(filename, "rb");
	double* ptr = readin;
	fread(ptr,24,1,fp);
	int nrecl = (int) round(readin[0]);
	int nspin = (int) round(readin[1]);
	int nprec = (int) round(readin[2]);
	fclose(fp);
	fp = fopen(filename, "rb");
	double readarr[nrecl/8];
	ptr = readarr;
	FILE* fp0 = fp;
	fseek(fp0,1*nrecl,0);
	fread(ptr,nrecl,1,fp0);
	int nwk = (int) round(readarr[0]);
	int nband = (int) round(readarr[1]);
	double encut = readarr[2];
	printf("encut %lf\n", encut);
	for (int i = 0; i < 9; i++) {
		lattice[i] = readarr[i+3];
	}
	vcross(reclattice+0, lattice+3, lattice+6);
	vcross(reclattice+3, lattice+6, lattice+0);
	vcross(reclattice+6, lattice+0, lattice+3);
	double* b1 = reclattice;
	double* b2 = reclattice+3;
	double* b3 = reclattice+6;
	double Vcell = determinant(lattice);
	for (int i = 0; i < 9; i++) {
		reclattice[i] *= 2.0 * PI / Vcell;
	}
	double magb1 = mag(reclattice+0);
	double magb2 = mag(reclattice+3);
	double magb3 = mag(reclattice+6);
	double vtemp[3];
	double vmag, sinphi123;
	
	double phi12 = acos(dot(reclattice+0, reclattice+3) / (magb1 * magb2));
	vcross(vtemp, reclattice+0, reclattice+3);
	vmag = mag(vtemp);
	sinphi123 = dot(reclattice+6, vtemp) / (vmag * magb3);
	double nb1maxA = pow(encut*c,0.5) / (magb1 * fabs(sin(phi12))) + 1;
	double nb2maxA = pow(encut*c,0.5) / (magb2 * fabs(sin(phi12))) + 1;
	double nb3maxA = pow(encut*c,0.5) / (magb3 * fabs(sinphi123)) + 1;
	printf("%lf %lf %lf %lf %lf %lf\n", sin(phi12), pow(encut*c,0.5), phi12, vmag, sinphi123, magb3*abs(sinphi123));
	int npmaxA = (int) round(4.0/3.0*PI*nb1maxA*nb2maxA*nb3maxA);
	
	double phi13 = acos(dot(reclattice+0, reclattice+6) / (magb1 * magb3));
	vcross(vtemp, reclattice+0, reclattice+6);
	vmag = mag(vtemp);
	sinphi123 = dot(reclattice+3, vtemp) / (vmag * magb2);
	double nb1maxB = pow(encut*c,0.5) / (magb1 * fabs(sin(phi13))) + 1;
	double nb2maxB = pow(encut*c,0.5) / (magb2 * fabs(sinphi123)) + 1;
	double nb3maxB = pow(encut*c,0.5) / (magb3 * fabs(sin(phi13))) + 1;
	int npmaxB = (int) round(4.0/3.0*PI*nb1maxB*nb2maxB*nb3maxB);

	double phi23 = acos(dot(reclattice+6, reclattice+3) / (magb3 * magb2));
	vcross(vtemp, reclattice+3, reclattice+6);
	vmag = mag(vtemp);
	sinphi123 = dot(reclattice+0, vtemp) / (vmag * magb1);
	double nb1maxC = pow(encut*c,0.5) / (magb1 * fabs(sinphi123)) + 1;
	double nb2maxC = pow(encut*c,0.5) / (magb2 * fabs(sin(phi23))) + 1;
	double nb3maxC = pow(encut*c,0.5) / (magb3 * fabs(sin(phi23))) + 1;
	int npmaxC = (int) round(4.0/3.0*PI*nb1maxC*nb2maxC*nb3maxC);

	double nb1max = fmax(nb1maxA, fmax(nb1maxB, nb1maxC));
	double nb2max = fmax(nb2maxA, fmax(nb2maxB, nb2maxC));
	double nb3max = fmax(nb3maxA, fmax(nb3maxB, nb3maxC));
	printf("%lf %lf %lf\n", nb1maxA, nb2maxA, nb3maxA);
	printf("%lf %lf %lf\n", nb1maxB, nb2maxB, nb3maxB);
	printf("%lf %lf %lf\n", nb1maxC, nb2maxC, nb3maxC);

	int npmax = npmaxA;
	if (npmaxB < npmax) npmax = npmaxB;
	if (npmaxC < npmax) npmax = npmaxC;

	printf("spin k band %d %d %d\n", nspin, nwk, nband);
	printf(" %lf %lf %lf\n", lattice[0], lattice[1], lattice[2]);
	printf(" %lf %lf %lf\n", lattice[3], lattice[4], lattice[5]);
	printf(" %lf %lf %lf\n", lattice[6], lattice[7], lattice[8]);
	printf(" %lf %lf %lf\n", reclattice[0], reclattice[1], reclattice[2]);
	printf(" %lf %lf %lf\n", reclattice[3], reclattice[4], reclattice[5]);
	printf(" %lf %lf %lf\n", reclattice[6], reclattice[7], reclattice[8]);
	printf("\n %lf %lf %lf %d %d %d\n", nb1max, nb2max, nb3max, npmaxA, npmaxB, npmaxC);

	*pnrecl = nrecl;
	*pnspin = nspin;
	*pnwk = nwk;
	*pnband = nband;
	*nb1 = nb1max;
	*nb2 = nb2max;
	*nb3 = nb3max;
	*ecut = encut;

	fclose(fp);
}

pswf_t* read_wavefunctions(char* filename, double* kpt_weights) {
	int nrecli, nspin, nwk, nband;
	double nb1max, nb2max, nb3max, encut;
	double* lattice = (double*) malloc(9*sizeof(double));
	double* reclattice = (double*) malloc(9*sizeof(double));
	setup(filename, &nrecli, &nspin, &nwk, &nband, &nb1max, &nb2max, &nb3max,
		&encut, lattice, reclattice);
	double* b1 = reclattice;
	double* b2 = reclattice+3;
	double* b3 = reclattice+6;

	long nrecl = (long)nrecli;

	double* occ = malloc(nband*sizeof(double));
	double* cener = malloc(nband*sizeof(double));
	if (occ == NULL || cener == NULL) {
		ALLOCATION_FAILED();
	}

	pswf_t* wf = (pswf_t*) malloc(sizeof(pswf_t));

	wf->nspin = nspin;
	wf->nwk = nwk;
	wf->nband = nband;

	kpoint_t** kpts = (kpoint_t**) malloc(nwk*nspin*sizeof(kpoint_t*));
	if (kpts == NULL) {
		ALLOCATION_FAILED();
	}

	wf->kpts = kpts;
	wf->lattice = lattice;
	wf->reclattice = reclattice;
	wf->G_bounds = (int*) calloc(6, sizeof(int));

	double* kptr = (double*) malloc(nrecl);
	float complex* cptr = (float complex*) malloc(nrecl);
	//#pragma omp parallel for
	for (int iwk = 0; iwk < nwk * nspin; iwk++) {
		long irec = iwk * (long)(1 + nband);
		FILE* pfp = fopen(filename, "rb");
		kpoint_t* kpt = (kpoint_t*) malloc(sizeof(kpoint_t));
		kpt->num_bands = nband;
		band_t** bands = (band_t**) malloc(nband*sizeof(band_t*));
		kpt->bands = bands;
		if (kpt == NULL || bands == NULL) {
		    	ALLOCATION_FAILED();
		}
		fseek(pfp, irec*nrecl+2*nrecl, SEEK_SET);
		fread(kptr, 8, nrecl/8, pfp);
		
		int nplane = (int) round(kptr[0]);
		int* igall = malloc(3*nplane*sizeof(int));
		if (igall == NULL) {
		    	ALLOCATION_FAILED();
		}
		kpt->k = (double*) malloc(3*sizeof(double));
		kpt->k[0] = kptr[1];
		kpt->k[1] = kptr[2];
		kpt->k[2] = kptr[3];
		double kx = kpt->k[0], ky = kpt->k[1], kz = kpt->k[2];
		for (int i = 0; i < nband; i++) {
			cener[i] = kptr[4+i*3];
			occ[i] = kptr[6+i*3];
			band_t* band = (band_t*) malloc(sizeof(band_t));
			band->num_waves = nplane;
			band->energy = kptr[4+i*3];
			band->occ = kptr[6+i*3];
			kpt->bands[i] = band;
		}

		int ncnt = -1;
		for (int ig3 = 0; ig3 <= 2 * nb3max; ig3++) {
			int ig3p = ig3;
			if (ig3 > nb3max) ig3p = ig3 - 2 * nb3max - 1;
			for (int ig2 = 0; ig2 <= 2 * nb2max; ig2++) {
				int ig2p = ig2;
				if (ig2 > nb2max) ig2p = ig2 - 2 * nb2max - 1;
				for (int ig1 = 0; ig1 <= 2 * nb1max; ig1++) {
					int ig1p = ig1;
					if (ig1 > nb1max) ig1p = ig1 - 2 * nb1max - 1;
					double sumkg[3];
					for (int j = 0; j < 3; j++) {
						sumkg[j] = (kx+ig1p) * b1[j]
									+ (ky+ig2p) * b2[j]
									+ (kz+ig3p) * b3[j];
					}
					double gtot = mag(sumkg);
					double etot = pow(gtot,2.0) / c;
					//printf("%lf %lf\n", etot, gtot);
					if (etot <= encut) {
						ncnt++;
						igall[ncnt*3+0] = ig1p;
						igall[ncnt*3+1] = ig2p;
						igall[ncnt*3+2] = ig3p;
						if (ig1p < wf->G_bounds[0]) wf->G_bounds[0] = ig1p;
						else if (ig1p > wf->G_bounds[1]) wf->G_bounds[1] = ig1p;
						if (ig2p < wf->G_bounds[2]) wf->G_bounds[2] = ig2p;
						else if (ig2p > wf->G_bounds[3]) wf->G_bounds[3] = ig2p;
						if (ig3p < wf->G_bounds[4]) wf->G_bounds[4] = ig3p;
						else if (ig3p > wf->G_bounds[5]) wf->G_bounds[5] = ig3p;
					}
				}
			}
		}
		if (ncnt != nplane - 1) printf("ERROR %lf %lf %lf %lf", kx,ky,kz, c);
		//if (ncnt > npmax) printf("BIG ERROR");
		//printf("%d %d\n", ncnt, npmax);

		for (int iband = 0; iband < nband; iband++) {
			irec++;
			fseek(pfp, (long)irec*nrecl+2*(long)nrecl, SEEK_SET);
			fread(cptr, 8, nrecl/8, pfp);
			float complex* coeff = malloc(nplane*sizeof(float complex));
			for (int iplane = 0; iplane < nplane; iplane++) {
				coeff[iplane] = cptr[iplane];
			}
			kpt->bands[iband]->Cs = coeff;
		}
		
		//printf("iwk %d\n", iwk);
		kpt->weight = kpt_weights[iwk%nwk];
		kpt->Gs = igall;
		kpts[iwk] = kpt;
	}

	free(kptr);
	free(cptr);

	return wf;
}

kpoint_t** read_one_band(int* G_bounds, double* kpt_weights, int* ns, int* nk, int* nb, int BAND_NUM, char* filename) {
	clock_t start = clock();

	int nrecli=0, nspin=0, nwk=0, nband=0;
	double nb1max=0, nb2max=0, nb3max=0, encut=0;
	double lattice[9] = {0};
	double reclattice[9] = {0};
	setup(filename, &nrecli, &nspin, &nwk, &nband, &nb1max, &nb2max, &nb3max,
		&encut, lattice, reclattice);
	double* b1 = reclattice;
	double* b2 = reclattice+3;
	double* b3 = reclattice+6;

	long nrecl = (long) nrecli;

	double* kptr = (double*) malloc(nrecl);
	float complex* cptr = (float complex*) malloc(nrecl);

	kpoint_t** kpts = (kpoint_t**) malloc(nwk*nspin*sizeof(kpoint_t*));

	//#pragma omp parallel for
	for (int iwk = 0; iwk < nwk * nspin; iwk++) {
		int irec = iwk * (1 + nband);
		FILE* pfp = fopen(filename, "rb");
		kpoint_t* kpt = (kpoint_t*) malloc(sizeof(kpoint_t));
		kpt->num_bands = 1;
		band_t** bands = (band_t**) malloc(sizeof(band_t*));
		kpt->bands = bands;
		//void* karr = malloc(nrecl);
		fseek(pfp, (long)irec*nrecl+(long)2*nrecl, SEEK_SET);
		//printf("reading dat %d\n", fread(kptr, 8, nrecl/8, pfp));
		fread(kptr, 8, nrecl/8, pfp);

		int nplane = (int) round(kptr[0]);
		int* igall = malloc(3*nplane*sizeof(int));
		double kx = kptr[1];
		double ky = kptr[2];
		double kz = kptr[3];

		band_t* band = (band_t*) malloc(sizeof(band_t));
		band->num_waves = nplane;
		band->energy = kptr[4+BAND_NUM*3];
		band->occ = kptr[6+BAND_NUM*3];
		kpt->bands[0] = band;

		//printf("nplane %d %d\n", nplane, npmax);
		/*#pragma omp critical
		{
		printf("hm %lf %lf %lf\n", kptr[6+BAND_NUM*3], kptr[7+BAND_NUM*3], band->occ);
		printf("eek,iwk nrecl np e o kpt %d %d %d %lf %lf %lf %lf\n", iwk, nrecl, nplane, kx,ky,kz, band->energy);
		
		printf("got some max %lf %lf %lf\n\n", nb1max, nb2max, nb3max);
		}
		continue;*/
		int ncnt = -1;
		for (int ig3 = 0; ig3 <= 2 * nb3max; ig3++) {
			int ig3p = ig3;
			if (ig3 > nb3max) ig3p = ig3 - 2 * nb3max - 1;
			for (int ig2 = 0; ig2 <= 2 * nb2max; ig2++) {
				int ig2p = ig2;
				if (ig2 > nb2max) ig2p = ig2 - 2 * nb2max - 1;
				for (int ig1 = 0; ig1 <= 2 * nb1max; ig1++) {
					int ig1p = ig1;
					if (ig1 > nb1max) ig1p = ig1 - 2 * nb1max - 1;
					double sumkg[3];
					for (int j = 0; j < 3; j++) {
						sumkg[j] = (kx+ig1p) * b1[j]
									+ (ky+ig2p) * b2[j]
									+ (kz+ig3p) * b3[j];
					}
					double gtot = mag(sumkg);
					double etot = pow(gtot,2.0) / c;
					//printf("%lf %lf\n", etot, gtot);
					if (etot <= encut) {
						ncnt++;
						igall[ncnt*3+0] = ig1p;
						igall[ncnt*3+1] = ig2p;
						igall[ncnt*3+2] = ig3p;
					}
				}
			}
		}
		if (ncnt != nplane - 1) printf("ERROR %d %d", ncnt, nplane);

		int iband = BAND_NUM;
		irec += 1 + BAND_NUM;
		fseek(pfp,(long) irec*nrecl+2*(long)nrecl, SEEK_SET);
		fread(cptr, 8, nrecl/8, pfp);
		float complex* coeff = malloc(nplane*sizeof(float complex));
		for (int iplane = 0; iplane < nplane; iplane++) {
			coeff[iplane] = cptr[iplane];
		}
		kpt->bands[0]->Cs = coeff;
		kpt->weight = kpt_weights[iwk%nwk];
		kpt->Gs = igall;
		kpts[iwk] = kpt;
	}
	clock_t end = clock();
	free(kptr);
	free(cptr);
	printf("%lf seconds to read defect\n", (double)(end-start) / CLOCKS_PER_SEC);

	return kpts;
}