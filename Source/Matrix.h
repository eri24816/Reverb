#pragma once
#define SIZE 10

struct Row;
struct Matrix;
Matrix inverse(Matrix matrix);
Matrix mult(Matrix& a, Matrix& b);
float* mult(Matrix& m, float* v, float* result);
Matrix diag(float a[],int size);

class Matrix
{
public:
	float elem[100] = { 0 };
	float multBuffer[10] = {0};
	int nRow, nCol;
	Matrix() {}
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
		//multBuffer = new float[nRow];
	}
	Row operator[](int index);
	
	Matrix operator*(Matrix& m) {
		return mult(*this, m);
	}
	float* operator*(float* m) {
		return mult(*this, m,multBuffer);
		//return new float[nRow];
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


