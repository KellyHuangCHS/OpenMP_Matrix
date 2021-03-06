// openMP_mat.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>
#include <omp.h>
#include <string.h>

int MAT_SIZE = 512;
int THREAD_NUM = 8;

static void matmat(int n, float **mat_c, float **mat_a, float **mat_b)
{
	for (int i = 0; i < n; i++) {
		for (int k = 0; k < n; k++) {
			for (int j = 0; j < n; j++) {
				mat_c[i][j] += mat_a[i][k] * mat_b[k][j];
			}
		}
	}
}

static void randMat(float **mat)
{
	for (int i = 0; i < MAT_SIZE; i++) {
		for (int j = 0; j < MAT_SIZE; j++) {
			mat[i][j] = rand() % 10;
		}
	}
}

static void transMat(float **mat)
{
	for (int i = 0; i < MAT_SIZE - 1; i++) {
		for (int j = i; j < MAT_SIZE; j++) {
			float temp = mat[i][j];
			mat[i][j] = mat[j][i];
			mat[j][i] = temp;
		}
	}
}

void printMat(int n, float **mat)
{
	return;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			printf("%.0f,", mat[i][j]);
		}
		printf("\n");
	}
}

int equalMat(float **mat_a, float **mat_b)
{
	for (int i = 0; i < MAT_SIZE; i++) {
		for (int j = 0; j < MAT_SIZE; j++) {
			if (mat_a[i][j] != mat_b[i][j]) {
				return 0;
			}
		}
	}
	return 1;
}

static void multhreadMat(int n, float **mat_c, float **mat_a, float **mat_b)
{
	int i, j, k;
	transMat(mat_b);
	omp_set_num_threads(THREAD_NUM);
	#pragma omp parallel shared(mat_a, mat_b, mat_c) private(i, j, k)
	{
		#pragma omp for schedule(dynamic) 
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				int sum = 0;
				#pragma omp parallel for reduction(+:sum)
				for (k = 0; k < n; k++) {
					sum += mat_a[i][k] * mat_b[j][k];
				}
				mat_c[i][j] = sum;
			}
		}
	}

}

float **array(int row, int col)
{
	float **arr;
	arr = (float **)malloc(row * sizeof(float *));
	for (int i = 0; i < row; i++) {
		arr[i] = (float *)malloc(col * sizeof(float));
	}
	return arr;
}

void initArray(int n, float **mat)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			mat[i][j] = 0;
		}
	}
}

int main()
{
	int t1, t2;
	//printf("Enter a integer as matrix size: ");
	//scanf_s("%d", &MAT_SIZE);
	//printf("Enter a integer as thread count: ");
	//scanf_s("%d", &THREAD_NUM);
	float **mat_a = array(MAT_SIZE, MAT_SIZE);
	float **mat_b = array(MAT_SIZE, MAT_SIZE);
	float **mat_c = array(MAT_SIZE, MAT_SIZE);
	float **mat_d = array(MAT_SIZE, MAT_SIZE);
	initArray(MAT_SIZE, mat_c);
	initArray(MAT_SIZE, mat_d);

	randMat(mat_a);
	randMat(mat_b);

	LARGE_INTEGER num;
	LONGLONG start, end, freq;
	QueryPerformanceFrequency(&num);
	freq = num.QuadPart;
	QueryPerformanceCounter(&num);
	start = num.QuadPart;
	matmat(MAT_SIZE, mat_c, mat_a, mat_b);
	printMat(MAT_SIZE, mat_c);
	QueryPerformanceCounter(&num);
	end = num.QuadPart;
	t1 = (end - start) * 1000 / freq;
	printf("Matmat running time: %d\n", t1);
	printMat(MAT_SIZE, mat_c);

	QueryPerformanceCounter(&num);
	start = num.QuadPart;
	multhreadMat(MAT_SIZE, mat_d, mat_a, mat_b);
	QueryPerformanceCounter(&num);
	end = num.QuadPart;
	t2 = (end-start)*1000/freq;
	printf("Multi_mat running time: %d\n", t2);
	printMat(MAT_SIZE, mat_d);

	if(t1!=0&&t2!=0){
	    printf("T1/T2 = %.2f\n",(float)t1/(float)t2);
	}
	
	if(equalMat(mat_c, mat_d)==1){
	    printf("EQUAL!\n");
	}else{
	    printf("NOT EQUAL!\n");
	}

	system("pause");
	return 0;
}


