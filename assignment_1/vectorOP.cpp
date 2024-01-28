#include "PPintrin.h"
#include <iostream>

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
	__pp_vec_float x;
	__pp_vec_float result;
	__pp_vec_float zero = _pp_vset_float(0.f);
	__pp_mask maskAll, maskIsNegative, maskIsNotNegative;

	//  Note: Take a careful look at this loop indexing.  This example
	//  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
	//  Why is that the case?
	for (int i = 0; i < N; i += VECTOR_WIDTH)
	{
		// All ones
		maskAll = _pp_init_ones();

		// All zeros
		maskIsNegative = _pp_init_ones(0);

		// Load vector of values from contiguous memory addresses
		_pp_vload_float(x, values + i, maskAll); // x = values[i];

		// Set mask according to predicate
		_pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) { ex. mask = 0010

		// Execute instruction using mask ("if" clause)
		_pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x; (abs the < 0 item -> mask=1 )

		// Inverse maskIsNegative to generate "else" mask
		maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else { ex. reversed mask = 1101

		// Execute instruction ("else" clause)
		_pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; (load the > 0 item -> mask=1) }

		// Write results back to memory
		_pp_vstore_float(output + i, result, maskAll);
	}
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
	__pp_vec_float val, result;
	__pp_vec_int exp;
	__pp_vec_int zero = _pp_vset_int(0), one = _pp_vset_int(1);
	__pp_vec_float bound = _pp_vset_float(9.999999f);
	__pp_mask maskAll, maskDone, maskTemp;

	//
	// PP STUDENTS TODO: Implement your vectorized version of
	// clampedExpSerial() here.
	//
	// Your solution should work for any value of
	// N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
	//

	for (int i = 0; i < N; i += VECTOR_WIDTH)
	{
		// All ones;
		maskAll = _pp_init_ones();

		if (i + VECTOR_WIDTH >= N)			// Check if i + VECTOR_WIDTH >= N:
			maskAll = _pp_init_ones(N - i); // Mask the redundant as 0;

		// All zeros;
		maskDone = _pp_init_ones(0);

		// Load values[i ... i + VECTOR_WIDTH] into val;
		_pp_vload_float(val, values + i, maskAll);
		// Load exponents[i ... i + VECTOR_WIDTH] into exp;
		_pp_vload_int(exp, exponents + i, maskAll);

		_pp_veq_int(maskDone, exp, zero, maskAll); // If exponents == 0;
		maskDone = _pp_mask_not(maskDone);		   // Mask the corresponding values as 0;

		if (i + VECTOR_WIDTH >= N)						// Combine maskDone and maskAll;
			maskDone = _pp_mask_and(maskDone, maskAll); // maskDone -> 0 means the calculation is finished (or doesn't need to do the calculation);

		// Initialize result = 1;
		_pp_vset_float(result, 1, maskAll);

		PPLogger.addLog("maskDone", maskDone, VECTOR_WIDTH);

		while (_pp_cntbits(maskDone) != 0)
		{
			_pp_vmult_float(result, val, result, maskDone); // Do N * N operation;
			_pp_vsub_int(exp, exp, one, maskDone);			// Exp = Exp - 1;
			_pp_veq_int(maskTemp, exp, zero, maskDone);		// if Exp == 0: Corresponding maskDone = 0 (means it has finished the calculation);

			maskTemp = _pp_mask_not(maskTemp);
			maskDone = _pp_mask_and(maskDone, maskTemp); // Update maskDone;
		}

		maskTemp = _pp_init_ones(0);
		_pp_vgt_float(maskTemp, result, bound, maskAll); // If result > 9.999999f:
		_pp_vstore_float(output + i, bound, maskTemp);	 //  Load result = 9.999999f into outputs;
		maskTemp = _pp_mask_not(maskTemp);

		if (i + VECTOR_WIDTH >= N)
		{
			maskTemp = _pp_mask_and(maskTemp, maskAll);
		}

		_pp_vstore_float(output + i, result, maskTemp); // Load others results into outputs;
	}
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{
	__pp_vec_float Vsum;
	__pp_mask maskAll;
	float sum_vector[VECTOR_WIDTH];
	float sum = 0;

	maskAll = _pp_init_ones();

	for (int i = 0; i < N; i += VECTOR_WIDTH)
	{	
		// Load values[i ... i + VECTOR_WIDTH] into Vsum;
		_pp_vload_float(Vsum, values + i, maskAll);

		int k = VECTOR_WIDTH - 1;
		while (k > 0) // Do "log(VECTOR_WIDTH)" operations;
		{
			_pp_hadd_float(Vsum, Vsum);       // Do adjacent pairs adding operation;
			_pp_interleave_float(Vsum, Vsum); // Performs an even-odd interleaving operation.
			k /= 2;
		}

		sum += Vsum.value[0]; // After operations, every elements in Vsum are the same.
	}

	return sum;
}