#include"Matrix.h"


// Inverse n*n matrix
//https://www.quora.com/How-do-I-make-a-C++-program-to-get-the-inverse-of-a-matrix-100-X-100
Matrix inverse(Matrix matrix)
{
	int i, j, k, n = matrix.nCol;
	float a[10][20], t;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			a[i][j] = matrix[i][j];
	for (i = 0; i < n; i++)
	{
		for (j = n; j < 2 * n; j++)
		{
			if (i == j - n)
				a[i][j] = 1;
			else
				a[i][j] = 0;
		}
	}
	for (i = 0; i < n; i++)
	{
		t = a[i][i];
		for (j = i; j < 2 * n; j++)
			a[i][j] = a[i][j] / t;
		for (j = 0; j < n; j++)
		{
			if (i != j)
			{
				t = a[j][i];
				for (k = 0; k < 2 * n; k++)
					a[j][k] = a[j][k] - t * a[i][k];
			}
		}
	}
	Matrix result(matrix.nRow, matrix.nCol);
	for (i = 0; i < n; i++)
	{
		for (j = n; j < 2 * n; j++)
			result[i][j-n] = a[i][j];
	}
	return result;
}

// https://www.programiz.com/cpp-programming/examples/matrix-multiplication
Matrix mult(Matrix& a, Matrix& b)
{
	int   i, j, k;
	int r1 = a.nRow, c1 = a.nCol, r2 = b.nRow, c2 = b.nCol;
	Matrix mult = Matrix(r1, c2);

	// Initializing elements of matrix mult to 0.
	for (i = 0; i < r1; ++i)
		for (j = 0; j < c2; ++j)
		{
			mult[i][j] = 0;
		}

	// Multiplying matrix a and b and storing in array mult.
	for (i = 0; i < r1; ++i)
		for (j = 0; j < c2; ++j)
			for (k = 0; k < c1; ++k)
			{
				mult[i][j] += a[i][k] * b[k][j];
			}
	return mult;
}

float* mult(Matrix& m, float* v, float* result) {
	for (int i = 0; i < m.nRow; i++) {
		result[i] = 0;
		for (int j = 0; j < m.nCol; j++) {
			result[i] += m[i][j] * v[j];
		}
	}
	return result;
}

Row Matrix::operator[](int index) {
	return Row(*this, index);
}

Matrix diag(float a[],int size) {
	Matrix m(size, size);
	while (size--) {
		m[size][size] = a[size];
	}
	return m;
}