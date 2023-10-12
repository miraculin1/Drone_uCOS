/*
 * svd.c
 *
 *  Created on: 8 feb. 2019
 *      Author: Daniel Mårtensson
 */

#include "declareFunctions.h"

/*
 * Do SVD on the matrix A who has the size row x column. Get the U: size row x row, S: size row x column, V: size column x column
 */
void svd(float* A, float* U, float* S, float* V, int row, int column) {

	integer M = row;
	integer N = column;
	integer LDA = row;
	integer LDU = row;
	integer LDVT = column;
	integer INFO;
	integer LWORK;
	floatreal WKOPT;
	floatreal S_[column];
	floatreal A_[row * column];

	// Pre-work
	memcpy(A_, A, row * column * sizeof(float)); // Copy from A to A_
	tran(A_, row, column); // Important to do transpose of A, due to this FORTRAN library
	memset(S_, 0, column * sizeof(float)); // Need to set all the S to zeros

	// Find optimal solution
	LWORK = -1;
	dgesvd_("All", "All", &M, &N, A_, &LDA, S_, U, &LDU, V, &LDVT, &WKOPT, &LWORK, &INFO);
	LWORK = WKOPT;
	floatreal WORK[LWORK];
	memset(WORK, 0, LWORK * sizeof(float));

	// Solve
	dgesvd_("All", "All", &M, &N, A_, &LDA, S_, U, &LDU, V, &LDVT, WORK, &LWORK, &INFO);

	// Ok! Solve done! Now do transpose of U;
	tran(U, row, row);

	// Turn S into a diagonal matrix
	diag(S_, S, row, column);

}
