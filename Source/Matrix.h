#pragma once
#define SIZE 10

struct Row;
struct Matrix;
Matrix inverse(Matrix matrix);
Matrix mult(Matrix& a, Matrix& b);
float* mult(Matrix m, float* v, float* result);
Matrix diag(float a[],int size);

struct Matrix
{
	float elem[9] = { 1, 5, 2, -2, 1, -4, 9, -10, 2 };
	int nRow, nCol;
	Matrix(int nRow, int nCol,float* initValue = nullptr) :nRow(nRow), nCol(nCol) {
		//elem = new float[nRow*nCol]; 
		if (initValue) {
			for (int i = 0; i < nRow * nCol; i++) {
				elem[i] = initValue[i];
			}
		}
		else {
			for (int i = 0; i < nRow * nCol; i++) {
				elem[i] = 0;
			}
		}
	}
	Row operator[](int index);
	
	Matrix operator*(Matrix& m) {
		return mult(*this, m);
	}
	Matrix operator~() {
		return inverse(*this);
	}
};

struct Row
{
	Matrix& mat;
	int rowNum;
	Row(Matrix& mat, int rowNum) :mat(mat), rowNum(rowNum) {}
	float& operator[](int index) {
		return mat.elem[rowNum * mat.nCol + index];
	}
};


