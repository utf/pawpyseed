# cython : profile=True
# cython : language_level=3

## Cython file for interfacing
# pawpyseed classes with C code.
# Do not directly use any classes
# or utilities from the module; it
# is unsafe because some of the error
# catching is in Python.

from pawpyseed.core cimport pawpyc
from pawpyseed.core cimport pawpyc_extern as ppc

from cpython cimport array
from libc.stdlib cimport malloc, free
from libc.stdio cimport FILE
from pymatgen.core.structure import Structure
from pymatgen.io.vasp.outputs import Vasprun
from monty.io import zopen
import numpy as np
from numpy.testing import assert_almost_equal
cimport numpy as np
import time
import sys
from libc.stdint cimport uintptr_t
from pawpyseed.core.symmetry import *

###################
#  TIMER SECTION  #
###################

class Timer:
	@staticmethod
	def setup_time(t):
		Timer.ALL_SETUP_TIME += t
	@staticmethod
	def overlap_time(t):
		Timer.ALL_OVERLAP_TIME += t
	@staticmethod
	def augmentation_time(t):
		Timer.ALL_AUGMENTATION_TIME += t
	ALL_SETUP_TIME = 0
	ALL_OVERLAP_TIME = 0
	ALL_AUGMENTATION_TIME = 0

################################
#  MISCELLANEOUS PYTHON UTILS  #
################################  

def el(site):
	"""
	Return the element symbol of a pymatgen
	site object
	"""
	return site.specie.symbol

OPSIZE = 9

def make_c_ops(op_nums, symmops):
	ops = np.zeros(OPSIZE*len(op_nums), dtype = np.float64)
	for i in range(len(op_nums)):
		ops[OPSIZE*i:OPSIZE*(i+1)] = symmops[op_nums[i]].rotation_matrix.flatten()
	drs = np.zeros(3*len(op_nums), dtype = np.float64)
	for i in range(len(op_nums)):
		drs[3*i:3*(i+1)] = symmops[op_nums[i]].translation_vector
	return ops, drs

#################################
#  C UTILS INTERFACE FUNCTIONS  #
#################################

cpdef double legendre(int l, int m, double x):
	return ppc.legendre(l, m, x)

cpdef double complex Ylm(int l, int m, double theta, double phi):
	return ppc.Ylm(l, m, theta, phi)

cpdef double complex Ylm2(int l, int m, double costheta, double phi):
	return ppc.Ylm2(l, m, costheta, phi)

cpdef frac_to_cartesian(np.ndarray[double, ndim=1] coord,
	np.ndarray[double, ndim=2] lattice):

	if not coord.flags['C_CONTIGUOUS']:
		coord = np.ascontiguousarray(coord)
	latticef = lattice.flatten()
	if not latticef.flags['C_CONTIGUOUS']:
		latticef = np.ascontiguousarray(latticef)
	cdef double[::1] coordv = coord
	cdef double[::1] latticev = latticef
	ppc.frac_to_cartesian(&coordv[0], &latticev[0])

cpdef cartesian_to_frac(np.ndarray[double, ndim=1] coord,
	np.ndarray[double, ndim=2] reclattice):

	if not coord.flags['C_CONTIGUOUS']:
		coord = np.ascontiguousarray(coord)
	reclatticef = reclattice.flatten()
	if not reclatticef.flags['C_CONTIGUOUS']:
		reclatticef = np.ascontiguousarray(reclatticef)
	cdef double[::1] coordv = coord
	cdef double[::1] latticev = reclatticef
	ppc.cartesian_to_frac(&coordv[0], &latticev[0])

cpdef interpolate(np.ndarray[double, ndim=1] res,
	np.ndarray[double, ndim=1] tst,
	np.ndarray[double, ndim=1] x,
	np.ndarray[double, ndim=1] y,
	double rmax,
	int size, int tstsize):

	cdef double[::1] xv = x
	cdef double[::1] yv = y
	cdef double** coef = ppc.spline_coeff(&xv[0], &yv[0], size)

	for i in range(tstsize):
		res[i] = ppc.proj_interpolate(tst[i], rmax,
			size, &xv[0], &yv[0], coef)

	free(coef[0])
	free(coef[1])
	free(coef[2])
	free(coef)

cpdef spherical_bessel_transform(double encut, int l,
	np.ndarray[double, ndim=1] r, np.ndarray[double, ndim=1] f):

	size = r.shape[0]
	if not r.flags['C_CONTIGUOUS']:
		r = np.ascontiguousarray(r)
	if not f.flags['C_CONTIGUOUS']:
		f = np.ascontiguousarray(f)
	cdef np.ndarray k = np.zeros(size, dtype=np.float64, order='C')
	cdef np.ndarray fk = np.zeros(size, dtype=np.float64, order='C')
	cdef double[::1] rv = r
	cdef double[::1] kv = k
	cdef double[::1] fv = f
	cdef ppc.sbt_descriptor_t* sbtd = ppc.spherical_bessel_transform_setup(encut, 0,
					l, size, &rv[0], &kv[0])
	cdef double* fkv = ppc.wave_spherical_bessel_transform(sbtd, &fv[0], l)
	for i in range(size):
		fk[i] = fkv[i]
	free(fkv)
	return k, fk

cpdef reciprocal_offsite_wave_overlap(np.ndarray[double, ndim=1] dcoord,
	np.ndarray[double, ndim=1] r1, np.ndarray[double, ndim=1] f1,
	np.ndarray[double, ndim=1] r2, np.ndarray[double, ndim=1] f2,
	int l1, int m1, int l2, int m2):

	cdef int size1 = r1.shape[0]
	cdef int size2 = r2.shape[0]

	cdef double[::1] r1v = r1
	cdef double[::1] r2v = r2
	cdef double[::1] f1v = f1
	cdef double[::1] f2v = f2

	cdef double encut = 1e5
	k1, fk1 = spherical_bessel_transform(encut, l1, r1, f1)
	k2, fk2 = spherical_bessel_transform(encut, l2, r2, f2)

	cdef double[::1] dcoordv = dcoord
	cdef double[::1] k1v = k1
	cdef double[::1] fk1v = fk1
	cdef double[::1] k2v = k2
	cdef double[::1] fk2v = fk2

	cdef double** s1 = ppc.spline_coeff(&k1v[0], &fk1v[0], size1)
	cdef double** s2 = ppc.spline_coeff(&k2v[0], &fk2v[0], size2)

	res = ppc.reciprocal_offsite_wave_overlap(&dcoordv[0],
		&k1v[0], &fk1v[0], s1, size1,
		&k2v[0], &fk2v[0], s2, size2,
		NULL, l1, m1, l2, m2)

	free(s1[0])
	free(s1[1])
	free(s1[2])
	free(s1)
	free(s2[0])
	free(s2[1])
	free(s2[2])
	free(s2)

	return res

############################
#  PAWPYSEED BASE CLASSES  #
############################

cdef class PWFPointer:
	"""
	Container class for a pointer to a pswf_t* object
	in C, which can be used to initialize CWavefunction.
	Also contains a function for get copies of PWFPointer
	objects without symmetry-reduced k-point sampling.
	"""

	def __init__(self, filename = None, vr = None):
		cdef double[::1] kws
		if filename == None or vr == None:
			self.ptr = NULL
		else:
			filename = str(filename)
			if type(vr) == str:
				vr = Vasprun(vr) 
			self.weights = np.array(vr.actual_kpoints_weights, dtype=np.float64)
			self.kpts = np.array(vr.actual_kpoints, dtype=np.float64)
			self.band_props = np.array(vr.eigenvalue_band_properties)
			kws = self.weights
			if '.gz' in filename or '.bz2' in filename:
				f = zopen(filename, 'rb')
				contents = f.read()
				f.close()
				self.ptr = ppc.read_wavefunctions_from_str(
					contents, &kws[0])
			else:
				self.ptr = ppc.read_wavefunctions(filename.encode('utf-8'), &kws[0])
			print ("INITIALIZED PWFP")
			sys.stdout.flush()

	@staticmethod
	cdef PWFPointer from_pointer_and_kpts(ppc.pswf_t* ptr,
		structure, kpts, band_props, allkpts, weights):

		return_kpts_and_weights = False
		if (allkpts is None) or (weights is None):
			return_kpts_and_weights = True
			allkpts, orig_kptnums, op_nums, symmops, trs = get_nosym_kpoints(kpts, structure)
			weights = np.ones(allkpts.shape[0], dtype=np.float64, order='C')
			# need to change this if spin orbit coupling is added in later
			for i in range(allkpts.shape[0]):
				if np.linalg.norm(allkpts[i]) < 1e-10:
					weights[i] *= 0.5
			weights /= np.sum(weights)
		else:
			orig_kptnums, op_nums, symmops, trs = get_kpt_mapping(allkpts, kpts, structure)

		ops, drs = make_c_ops(op_nums, symmops)

		kpts = np.array(allkpts, np.float64, order='C', copy=False)
		weights = np.array(weights, np.float64, order='C', copy=False)
		print(kpts, weights, "hihi")
		sys.stdout.flush()
		cdef double[::1] weights_v = weights
		cdef int[::1] orig_kptnums_v = np.array(orig_kptnums, np.int32, order='C', copy=False)
		cdef int[::1] op_nums_v = np.array(op_nums, np.int32, order='C', copy=False)
		cdef double[::1] ops_v = np.array(ops, np.float64, order='C', copy=False)
		cdef double[::1] drs_v = np.array(drs, np.float64, order='C', copy=False)
		cdef int[::1] trs_v = np.array(trs, np.int32, order='C', copy=False)

		cdef ppc.pswf_t* new_ptr = ppc.expand_symm_wf(ptr, len(orig_kptnums),
				&orig_kptnums_v[0], &ops_v[0], &drs_v[0], &weights_v[0], &trs_v[0])

		cdef PWFPointer pwfp = PWFPointer()
		pwfp.ptr = new_ptr
		pwfp.kpts = kpts
		pwfp.weights = weights
		pwfp.band_props = np.array(band_props)
		return pwfp


cdef class PseudoWavefunction:
	"""
	THIS CLASS IS NOT USEFUL ON ITS OWN. IF YOU WANT TO WORK WITH
	PSEUDOWAVEFUNCTIONS, INITIALIZE A Wavefunction OBJECT WITH
	setup_projectors=False.

	Class for storing pseudowavefunction from WAVECAR file. Most important attribute
	is wf_ptr, a C pointer used in the C portion of the program for storing
	plane wave coefficients.

	Attributes:
		kpts (np.array): nx3 array of fractional kpoint vectors,
			where n is the number of kpoints
		kws (np.array): weight of each kpoint
		wf_ptr (ctypes POINTER): c pointer to pswf_t object
		ncl (bool): Whether the pseudowavefunction is from a noncollinear
			VASP calculation
		band_props (list): [band gap, conduction band minimum,
			valence band maximum, whether the band gap is direct]
	"""

	def __init__(self, PWFPointer pwf):
		"""
		Initializes a PseudoWavefunction from a PWFPointer
		"""
		print("START INIT PWF")
		sys.stdout.flush()
		if pwf.ptr is NULL:
			raise Exception("NULL PWFPointer ptr!")
		self.wf_ptr = pwf.ptr
		self.kpts = pwf.kpts.copy(order='C')
		self.kws = pwf.weights.copy(order='C')
		self.ncl = ppc.is_ncl(self.wf_ptr) > 0
		self.nband = ppc.get_nband(self.wf_ptr)
		self.nwk = ppc.get_nwk(self.wf_ptr)
		self.nspin = ppc.get_nspin(self.wf_ptr)
		self.encut = ppc.get_encut(self.wf_ptr)
		print("INITIALIZED PWF")
		sys.stdout.flush()

	def __dealloc__(self):
		ppc.free_pswf(self.wf_ptr)

	def pseudoprojection(self, band_num, PseudoWavefunction basis):
		"""
		Computes <psibt_n1k|psit_n2k> for all n1 and k
		and a given n2, where psibt are basis structures
		pseudowavefunctions and psit are self pseudowavefunctions

		Arguments:
			band_num (int): n2 (see description)
			basis (Pseudowavefunction): pseudowavefunctions onto whose bands
				the band of self is projected
		"""
		res = np.zeros(basis.nband * basis.nwk * basis.nspin, dtype = np.complex128)
		cdef double complex[::1] resv = res
		ppc.pseudoprojection(&resv[0], basis.wf_ptr, self.wf_ptr, band_num)
		return res


cdef class CWavefunction(PseudoWavefunction):
	"""
	Base class for pawpyseed.core.wavefunction.Wavefunction.
	Implements functions that call C routines and access
	C objects

	Attributes:
		int[::1] dimv: FFT grid dimensions
		int gridsize: dimv[0]*dimv[1]*dimv[2]
		int[::1] nums: element label of each site in the structure
		double[::1] coords: flattened list of coordinates for each site
		int number_projector_elements: number of elements in the structure
		readonly int projector_owner: Whether projector functions have
			been initialized
	"""
	
	def __init__(self, PWFPointer pwf):
		"""
		Initializes a CWavefunction from a PWFPointer
		"""
		self.projector_owner = 0
		super(CWavefunction, self).__init__(pwf)

	def _c_projector_setup(self, int num_elems, int num_sites,
							double grid_encut, nums, coords, dim, pps):
		"""
		Sets up the projector functions for AE components.
		"""

		start = time.monotonic()
		clabels = np.array([], np.intc)
		ls = np.array([], np.intc)
		projectors = np.array([], np.double)
		aewaves = np.array([], np.double)
		pswaves = np.array([], np.double)
		wgrids = np.array([], np.double)
		augs = np.array([], np.double)
		rmaxs = np.array([], np.double)

		for num in sorted(pps.keys()):
			pp = pps[num]
			clabels = np.append(clabels, [num, len(pp.ls), pp.ndata, len(pp.grid)])
			rmaxs = np.append(rmaxs, pp.rmax)
			ls = np.append(ls, pp.ls)
			wgrids = np.append(wgrids, pp.grid)
			augs = np.append(augs, pp.augs)
			for i in range(len(pp.ls)):
				proj = pp.realprojs[i]
				aepw = pp.aewaves[i]
				pspw = pp.pswaves[i]
				projectors = np.append(projectors, proj)
				aewaves = np.append(aewaves, aepw)
				pswaves = np.append(pswaves, pspw)

		cdef int[::1] clabels_v = clabels.astype(np.intc)
		cdef int[::1] ls_v = ls.astype(np.intc)
		cdef double[::1] wgrids_v = wgrids.astype(np.double)
		cdef double[::1] projectors_v = projectors.astype(np.double)
		cdef double[::1] aewaves_v = aewaves.astype(np.double)
		cdef double[::1] pswaves_v = pswaves.astype(np.double)
		cdef double[::1] rmaxs_v = rmaxs.astype(np.double)

		print ("GRID ENCUT", grid_encut)
		cdef ppc.ppot_t* projector_list = ppc.get_projector_list(
							num_elems, &clabels_v[0], &ls_v[0], &wgrids_v[0],
							&projectors_v[0], &aewaves_v[0], &pswaves_v[0],
							&rmaxs_v[0], grid_encut)
		end = time.monotonic()
		print('--------------\nran get_projector_list in %f seconds\n---------------' % (end-start))

		self.number_projector_elements = num_elems
		self.nums = np.array(nums, dtype = np.int32, copy = True)
		self.coords = np.array(coords, dtype = np.float64, copy = True)
		self.update_dimv(dim)

		print("STARTING PROJSETUP")
		sys.stdout.flush()
		ppc.setup_projections(
			self.wf_ptr, projector_list,
			num_elems, num_sites, &self.dimv[0],
			&self.nums[0], &self.coords[0]
			)

		self.projector_owner = 1

	def update_dimv(self, dim):
		dim = np.array(dim, dtype = np.int32, order = 'C', copy = False)
		self.dimv = dim
		self.gridsize = np.cumprod(dim)[-1]

	#-----------------------------------------------------#
	# HELPER FUNCTION ROUTINES FOR REAL SPACE PROJECTIONS #
	#-----------------------------------------------------#

	def _get_realspace_state(self, int b, int k, int s):
		res = np.zeros(self.gridsize, dtype = np.complex128, order='C')
		cdef double complex[::1] resv = res
		ppc.realspace_state(&resv[0], b, k+s*self.nwk,
			self.wf_ptr, &self.dimv[0], &self.nums[0], &self.coords[0])
		res.shape = self.dimv
		return res

	def _get_realspace_density(self):
		res = np.zeros(self.gridsize, dtype = np.float64, order='C')
		cdef double[::1] resv = res
		ppc.ae_chg_density(&resv[0], self.wf_ptr,
			&self.dimv[0], &self.nums[0], &self.coords[0])
		res.shape = self.dimv
		return res

	def _write_realspace_state(self, filename1, filename2, double scale, int b, int k, int s):
		filename1 = bytes(filename1.encode('utf-8'))
		filename2 = bytes(filename2.encode('utf-8'))
		res = self._get_realspace_state(b, k, s)
		res2 = res.view()
		res2.shape = self.gridsize
		resr = np.ascontiguousarray(np.real(res2))
		resi = np.ascontiguousarray(np.imag(res2))
		cdef double[::1] resv = resr
		ppc.write_volumetric(filename1, &resv[0], &self.dimv[0], scale)
		resv = resi
		ppc.write_volumetric(filename2, &resv[0], &self.dimv[0], scale)
		return res

	def _write_realspace_density(self, filename, double scale):
		filename = bytes(filename.encode('utf-8'))
		res = self._get_realspace_density()
		res2 = res.view()
		res2.shape = self.gridsize
		cdef double[::1] resv = res2
		ppc.write_volumetric(filename, &resv[0], &self.dimv[0], scale);
		return res

	def _desymmetrized_pwf(self, structure, band_props, allkpts = None, weights = None):
		return PWFPointer.from_pointer_and_kpts(<ppc.pswf_t*> self.wf_ptr, structure,
							self.kpts, band_props, allkpts, weights)

	def _get_occs(self):
		nk = self.nwk * self.nspin
		res = np.zeros(self.nband * nk, dtype=np.float64, order='C')
		for k in range(nk):
			for b in range(self.nband):
				res[b * nk + k] = self.wf_ptr.kpts[k].bands[b].occ
		return res

	def _get_energy_list(self, bands):
		"""
		Helper function to get a list of energy levels for a given list
		of bands. Used by defect_band_analysis in the Projector class.
		"""
		energy_list = {}
		for b in bands:
			energy_list[b] = []
			for k in range(self.nwk):
				for s in range(self.nspin):
					energy_list[b].append([ppc.get_energy(self.wf_ptr, b, k, s),\
										ppc.get_occ(self.wf_ptr, b, k, s)])
		return energy_list


cdef class CNCLWavefunction(CWavefunction):
	#-----------------------------------------------------#
	# HELPER FUNCTION ROUTINES FOR REAL SPACE PROJECTIONS #
	#-----------------------------------------------------#

	def _get_realspace_state(self, int b, int k, int s):
		res = np.zeros(self.gridsize * 2, dtype = np.complex128, order='C')
		cdef double complex[::1] resv = res
		ppc.ncl_realspace_state(&resv[0], b, k+s*self.nwk,
			self.wf_ptr, &self.dimv[0], &self.nums[0], &self.coords[0])
		res0, res1 = res[:self.gridsize], res[self.gridsize:]
		res0.shape = self.dimv
		res1.shape = self.dimv
		return res0, res1

	def _get_realspace_density(self):
		res = np.zeros(self.gridsize, dtype = np.float64, order='C')
		cdef double[::1] resv = res
		ppc.ncl_ae_chg_density(&resv[0], self.wf_ptr,
			&self.dimv[0], &self.nums[0], &self.coords[0])
		res.shape = self.dimv
		return res

	def _write_realspace_state(self, filename1, filename2, filename3, filename4,
								double scale, int b, int k, int s):
		filename1 = bytes(filename1.encode('utf-8'))
		filename2 = bytes(filename2.encode('utf-8'))
		filename3 = bytes(filename3.encode('utf-8'))
		filename4 = bytes(filename4.encode('utf-8'))
		res0, res1 = self._get_realspace_state(b, k, s)

		res2 = res0.view()
		res2.shape = self.gridsize
		resr = np.ascontiguousarray(np.real(res2))
		resi = np.ascontiguousarray(np.imag(res2))
		cdef double[::1] resv = resr
		ppc.write_volumetric(filename1, &resv[0], &self.dimv[0], scale)
		resv = resi
		ppc.write_volumetric(filename2, &resv[0], &self.dimv[0], scale)

		res2 = res1.view()
		res2.shape = self.gridsize
		resr = np.ascontiguousarray(np.real(res2))
		resi = np.ascontiguousarray(np.imag(res2))
		resv = resr
		ppc.write_volumetric(filename3, &resv[0], &self.dimv[0], scale)
		resv = resi
		ppc.write_volumetric(filename4, &resv[0], &self.dimv[0], scale)

		return res0, res1

	def _write_realspace_density(self, filename, double scale):
		filename = bytes(filename.encode('utf-8'))
		res = self._get_realspace_density()
		res2 = res.view()
		res2.shape = self.gridsize
		cdef double[::1] resv = res2
		ppc.write_volumetric(filename, &resv[0], &self.dimv[0], scale);
		return res


cdef class CProjector:
	"""
	Parent class for pawpyseed.core.projector.Projector,
	used for abstracting the Python-C interface.
	"""

	def __init__(self, wf, basis):
		"""
		Initialize a C projector form two Wavefunction objects
		"""
		self.wf = wf
		self.basis = basis

	#-------------------------------------------------#
	# HELPER FUNCTION ROUTINES FOR OVERLAP EVALUATION #
	#-------------------------------------------------#

	def _setup_overlap(self, site_cat, recip):

		# set up site lists
		self.M_R = np.array(site_cat[0], dtype=np.int32, order = 'C')
		self.M_S = np.array(site_cat[1], dtype=np.int32, order = 'C')
		self.N_R = np.array(site_cat[2], dtype=np.int32, order = 'C')
		self.N_S = np.array(site_cat[3], dtype=np.int32, order = 'C')
		self.N_RS_R = np.array(site_cat[4], dtype=np.int32, order = 'C')
		self.N_RS_S = np.array(site_cat[5], dtype=np.int32, order = 'C')

		# specify lengths of site lists
		self.num_M_R, self.num_M_S = len(site_cat[0]), len(site_cat[1])
		self.num_N_R, self.num_N_S = len(site_cat[2]), len(site_cat[3])
		self.num_N_RS_R, self.num_N_RS_S = len(site_cat[4]), len(site_cat[5])

		cdef int* M_R = NULL if self.num_M_R == 0 else &self.M_R[0]
		cdef int* M_S = NULL if self.num_M_S == 0 else &self.M_S[0]
		cdef int* N_R = NULL if self.num_N_R == 0 else &self.N_R[0]
		cdef int* N_S = NULL if self.num_N_S == 0 else &self.N_S[0]
		cdef int* N_RS_R = NULL if self.num_N_RS_R == 0 else &self.N_RS_R[0]
		cdef int* N_RS_S = NULL if self.num_N_RS_S == 0 else &self.N_RS_S[0]
		
		# choose function
		if recip:
			ppc.overlap_setup_recip(self.basis.wf_ptr, self.wf.wf_ptr,
				&self.basis.nums[0], &self.wf.nums[0], &self.basis.coords[0], &self.wf.coords[0],
				N_R, N_S, N_RS_R, N_RS_S,
				self.num_N_R, self.num_N_S, self.num_N_RS_R)
		else:
			ppc.overlap_setup_real(self.basis.wf_ptr, self.wf.wf_ptr,
				&self.basis.nums[0], &self.wf.nums[0], &self.basis.coords[0], &self.wf.coords[0],
				N_R, N_S, N_RS_R, N_RS_S,
				self.num_N_R, self.num_N_S, self.num_N_RS_R)

	def _add_augmentation_terms(self, np.ndarray[double complex, ndim=1] res, band_num):
		
		cdef double complex[::1] resv = res

		# set up site lists
		cdef int* M_R = NULL if self.num_M_R == 0 else &self.M_R[0]
		cdef int* M_S = NULL if self.num_M_S == 0 else &self.M_S[0]
		cdef int* N_R = NULL if self.num_N_R == 0 else &self.N_R[0]
		cdef int* N_S = NULL if self.num_N_S == 0 else &self.N_S[0]
		cdef int* N_RS_R = NULL if self.num_N_RS_R == 0 else &self.N_RS_R[0]
		cdef int* N_RS_S = NULL if self.num_N_RS_S == 0 else &self.N_RS_S[0]

		# call compensation terms C routine
		ppc.compensation_terms(&resv[0], band_num, self.wf.wf_ptr, self.basis.wf_ptr,
			self.num_M_R, self.num_N_R, self.num_N_S, self.num_N_RS_R,
			M_R, M_S, N_R, N_S, N_RS_R, N_RS_S,
			&self.wf.nums[0], &self.wf.coords[0], &self.basis.nums[0], &self.basis.coords[0],
			&self.wf.dimv[0])

	def _projection_recip(self, np.ndarray[double complex, ndim=1] res, band_num):
		
		cdef double complex[::1] resv = res

		# set up site lists
		cdef int* M_R = NULL if self.num_M_R == 0 else &self.M_R[0]
		cdef int* M_S = NULL if self.num_M_S == 0 else &self.M_S[0]
		cdef int* N_R = NULL if self.num_N_R == 0 else &self.N_R[0]
		cdef int* N_S = NULL if self.num_N_S == 0 else &self.N_S[0]
		cdef int* N_RS_R = NULL if self.num_N_RS_R == 0 else &self.N_RS_R[0]
		cdef int* N_RS_S = NULL if self.num_N_RS_S == 0 else &self.N_RS_S[0]

		# call compensation terms C routine
		ppc.compensation_terms_recip(&resv[0], band_num, self.wf.wf_ptr, self.basis.wf_ptr,
			self.num_M_R, self.num_N_R, self.num_N_S, self.num_N_RS_R,
			M_R, M_S, N_R, N_S, N_RS_R, N_RS_S,
			&self.wf.nums[0], &self.wf.coords[0], &self.basis.nums[0], &self.basis.coords[0],
			&self.wf.dimv[0])

	def _realspace_projection(self, int band_num, np.ndarray dim):
		res = np.zeros(self.basis.nband * self.basis.nwk * self.basis.nspin,
			dtype=np.complex128, order='C')
		cdef double complex[::1] resv = res
		cdef int[::1] dimv
		if dim == None:
			dimv = self.wf.dimv
		else:
			dimv = np.array(dim, dtype=np.float64, order='C', copy=False)
		ppc.project_realspace_state(&resv[0], 
			band_num, self.wf.wf_ptr, self.basis.wf_ptr,
			&dimv[0], &self.wf.nums[0], &self.wf.coords[0],
			&self.basis.nums[0], &self.basis.coords[0])
		return res
