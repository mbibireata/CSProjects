#include "func.h"
#include "util.h"

void func0(double *weights, double *arrayX, double *arrayY, int xr, int yr, int n)
{
	int i;
	//#pragma omp parallel for private(i)
	omp_set_num_threads(15);
	#pragma omp simd
	for(i = 0; i < n-1; i+=2){		
		/* try to have less compare statements with loop optimization */	
		//#pragma omp parallel
		  //{
			weights[i] = 1/((double)(n));
			weights[i+1]=1/((double)(n));
			arrayX[i] = xr;
			arrayX[i+1] = xr;
			arrayY[i] = yr;
			arrayY[i+1] = yr;	
		  //}
	}
}

void func1(int *seed, int *array, double *arrayX, double *arrayY,
			double *probability, double *objxy, int *index,
			int Ones, int iter, int X, int Y, int Z, int n)
{
	int i, j;
   	int index_X, index_Y;
	int max_size = X*Y*Z;
	//double repl = 1/(double) Ones;
	#pragma omp parallel for private(i)
	//#pragma omp simd
   	for(i = 0; i < n; ++i){
		//a = rand2(seed, i); 
   		arrayX[i] += 1 + 5*rand2(seed,i);
   		arrayY[i] += -2 + 2*rand2(seed,i);
   	}
	#pragma omp parallel for private(i)
	//#pragma omp private(i)
   	for(i = 0; i<n; ++i){
		#pragma omp simd
   		for(j = 0; j < Ones; ++j){  
   			index_X = round(arrayX[i]) + objxy[j*2 + 1];
  			index_Y = round(arrayY[i]) + objxy[j*2];
   			index[i*Ones + j] = fabs(index_X*Y*Z + index_Y*Z + iter);
   			if(index[i*Ones + j] >= max_size)
   				index[i*Ones + j] = 0; 
   		}
   		probability[i] = 0;
		//#pragma omp parallel for private(j) 
   		for(j = 0; j < Ones; ++j) {
   			probability[i] += (pow((array[index[i*Ones + j]] - 100),2) -
   							  pow((array[index[i*Ones + j]]-228),2))/50.0;
   		}
   		probability[i] = probability[i]/((double) Ones);
   	}
}

void func2(double *weights, double *probability, int n)
{
	int i;
	double sumWeights=0;
	//#pragma omp for private(i)
	for(i = 0; i < n; ++i)
		//#pragma omp parallel
		  //{
   			weights[i] = weights[i] * exp(probability[i]);
			//weights[i+1] = weights[i+1] * exp(probability[i+1]); 
			//weights[i+2] = weights[i+2] * exp(probability[i+2]);
			//weights[i+3] = weights[i+3] * exp(probability[i+3]);
		  //}
	//#pragma omp for private(i)
   	for(i = 0; i < n; ++i)
   		sumWeights += weights[i];
	
	//#pragma omp parallel for
	//#pragma omp for private(i)
	#pragma omp simd
	for(i = 0; i < n-3; i+=4) {
   		weights[i] /= sumWeights;
		weights[i+1] /= sumWeights; 
		weights[i+2] /= sumWeights; 
		weights[i+3] /= sumWeights; 
	}
}

void func3(double *arrayX, double *arrayY, double *weights, double *x_e, double *y_e, int n)
{
	double estimate_x=0.0;
	double estimate_y=0.0;
    int i;
	//#pragma omp parallel for reduction(+:estimate_x, estimate_y)
	//#pragma omp simd reduction(+:estimate_x, estimate_y)
	for(i = 0; i < n; ++i){
		//#pragma omp parallel reduction(+:estimate_x, estimate_y) 
		  //{
   			estimate_x += arrayX[i] * weights[i];
   			estimate_y += arrayY[i] * weights[i];
		  //}
   	}

	*x_e = estimate_x;
	*y_e = estimate_y;

}

void func4(double *u, double u1, int n)
{
	int i;
	double repl = 1/((double) (n)); 
	//#pragma omp private(i)
	//#pragma omp parallel for private(i) 
	#pragma omp simd
	for(i = 0; i < n-1; i+=2){
   		u[i] = u1 + i*repl;
		u[i+1] = u1 + (i+1)*repl; 
   	}
}

void func5(double *x_j, double *y_j, double *arrayX, double *arrayY, double *weights, double *cfd, double *u, int n)
{
	int i, j;
	double repl = 1/((double) n);
	#pragma omp private(j) 
	//#pragma omp simd
	for(j = 0; j < n; ++j){
   		//i = findIndex(cfd, n, u[j]);
   		i = findIndexBin(cfd, 0, n, u[j]);
   		if(i == -1)
   			i = n-1;  
   		x_j[j] = arrayX[i];
   		y_j[j] = arrayY[i];
		  
   	}
	#pragma omp simd
	//#pragma omp parallel for private(i)
	for(i = 0; i < n-1; i+=1){
		arrayX[i] = x_j[i];
		arrayX[i+1] = x_j[i+1];
		arrayY[i] = y_j[i];
		arrayY[i+1] = y_j[i+1];
		weights[i] = 1*repl;
		weights[i+1] = 1*repl;
	}
}
