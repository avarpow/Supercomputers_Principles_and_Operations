/* File:      hw4_openmp.c
 * Purpose:   Implementation of parallel count sort
 *
 * Compile:   gcc -g -Wall -fopenmp -o hw4_openmp hw4_openmp.c
 *
 * Run:      ./hw4_openmp <thread_count> <n>
 *
 * Input:   none
 * Output:  runtime of serial count sort
 *          runtime of parallel count sort
 *          runtime of qsort library function
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <immintrin.h>
#include <sys/time.h>


#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}

void Usage(char prog_name[]);
void Get_args(char* argv[], int* thread_count_p, int* n_p);
void Gen_data(int a[], int n);
void Count_sort_serial(int a[], int n);
void Count_sort_parallel(int a[], int n, int thread_count);
void Count_sort_parallel_loop_unrooling(int a[], int n, int thread_count);
void Count_sort_parallel_using_avx512(int a[],int n,int thread_count);
void Library_qsort(int a[], int n);
int  My_compare(const void* a, const void* b);
void Print_data(int a[], int n, char msg[]);
int  Check_sort(int a[], int n);
#define N_COUNT 6
#define THREAD_COUNT 7
int main(int argc, char* argv[]) {
   int n, thread_count;
   int *a, *copy;
   double start, stop;

   /* please choose terms 'n', and the threads 'thread_count' here. */
   int ns[] = {1e3,1e4,1e5,1e6,2e6,4e6};
   int thread_counts[] = {2,4,8,18,36,54,72};

   /* You can also get number of threads from command line */
   //if (argc != 3) Usage(argv[0]);
   //Get_args(argv, &thread_count, &n);

   /* Allocate storage and generate data for a */
   a = malloc(1e7*sizeof(int));
   Gen_data(a, 1e7);

   /* Allocate storage for copy */
   copy = malloc(1e7*sizeof(int));

fprintf(stderr,"start parallel\n");
for(int i=0;i<N_COUNT;i++){
   for(int j=0;j<THREAD_COUNT;j++){
   printf("==== parllar sort start n %d threads %d=====\n",ns[i],thread_counts[j]);
   fprintf(stderr,"==== parllar sort start n %d threads %d=====\n",ns[i],thread_counts[j]);
   /* Parallel count sort */
   memcpy(copy, a, ns[i]*sizeof(int));
#  ifdef DEBUG
   Print_data(copy, n, "Original: Parallel qsort a");
#  endif
   GET_TIME(start);
   Count_sort_parallel(copy, ns[i], thread_counts[j]);
   GET_TIME(stop);
#  ifdef DEBUG
   Print_data(copy, n, "Sorted: Parallel sort a");
#  endif
   if (!Check_sort(copy, ns[i]))
      printf("Parallel sort failed\n");
   printf("Parallel run time: %e\n\n", stop-start);
   }
}

fprintf(stderr,"start avx512 parallel\n");
for(int i=0;i<N_COUNT;i++){
   for(int j=0;j<THREAD_COUNT;j++){
   printf("==== AVX512 parallel sort start n %d threads %d=====\n",ns[i],thread_counts[j]);
   fprintf(stderr,"==== AVX512 parallel sort start n %d threads %d=====\n",ns[i],thread_counts[j]);
   /* Parallel avx count sort */
   memcpy(copy, a, ns[i]*sizeof(int));
#  ifdef DEBUG
   Print_data(copy, n, "Original: Parallel qsort a");
#  endif
   GET_TIME(start);
   Count_sort_parallel_using_avx512(copy, ns[i], thread_counts[j]);
   GET_TIME(stop);
#  ifdef DEBUG
   Print_data(copy, n, "Sorted: Parallel sort a");
#  endif
   if (!Check_sort(copy, ns[i]))
      printf("AVX512 Parallel sort failed\n");
   printf("AVX512 Parallel run time: %e\n\n", stop-start);
   }
}
// fprintf(stderr,"start loop unrooling parallel\n");
// for(int i=0;i<N_COUNT;i++){
//    for(int j=0;j<THREAD_COUNT;j++){
//    printf("==== loop unrooling parallel sort start n %d threads %d=====\n",ns[i],thread_counts[j]);
//    fprintf(stderr,"==== loop unrooling parallel sort start n %d threads %d=====\n",ns[i],thread_counts[j]);
//    memcpy(copy, a, ns[i]*sizeof(int));
// #  ifdef DEBUG
//    Print_data(copy, n, "Original: Parallel qsort a");
// #  endif
//    GET_TIME(start);
//    Count_sort_parallel_loop_unrooling(copy, ns[i], thread_counts[j]);
//    GET_TIME(stop);
// #  ifdef DEBUG
//    Print_data(copy, n, "Sorted: Parallel sort a");
// #  endif
//    if (!Check_sort(copy, ns[i]))
//       printf("loop unrooling Parallel sort failed\n");
//    printf("loop unrooling Parallel run time: %e\n\n", stop-start);
//    }
// }

fprintf(stderr,"start serial \n");
for(int i=0;i<N_COUNT;i++){
   /* Serial count sort */
   printf("==== serial sort start n %d=====\n",ns[i]);
   fprintf(stderr,"==== serial sort start n %d=====\n",ns[i]);
   memcpy(copy, a, ns[i]*sizeof(int));
#  ifdef DEBUG
   Print_data(copy, n, "Original: Serial sort a");
#  endif
   GET_TIME(start);
   Count_sort_serial(copy, ns[i]);
   GET_TIME(stop);
#  ifdef DEBUG
   Print_data(copy, n, "Sorted: Serial sort a");
#  endif
   if (!Check_sort(copy, ns[i]))
      printf("Serial sort failed\n");
   printf("Serial run time: %e\n\n", stop-start);
}

fprintf(stderr,"start qsort \n");
for(int i=0;i<N_COUNT;i++){
   /* qsort library */
   printf("====  qsort start n %d=====\n",ns[i]);
   fprintf(stderr,"====  qsort start n %d=====\n",ns[i]);
   memcpy(copy, a, ns[i]*sizeof(int));
#  ifdef DEBUG
   Print_data(copy, n, "Original: Library qsort a");
#  endif
   GET_TIME(start);
   Library_qsort(copy, ns[i]);
   GET_TIME(stop);
#  ifdef DEBUG
   Print_data(copy, n, "Sorted: Library qsort a");
#  endif
   if (!Check_sort(copy, ns[i]))
      printf("Library sort failed\n");
   printf("qsort run time: %e\n", stop-start);
}
   free(a);
   free(copy);

   return 0;
}  /* main */

/*---------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message showing how to run the program and quit
 * In arg:    prog_name:  the name of the program from the command line
 */
void Usage(char prog_name[]) {
   fprintf(stderr, "usage: %s <thread_count> <n>\n", prog_name);
   exit(0);
}  /* Usage */


/*---------------------------------------------------------------------
 * Function:  Get_args
 * Purpose:   Get the command line arguments
 * In arg:    argv:  strings from command line
 * Out args:  thread_count_p: number of threads
 *            n_p: number of elements
 */
void Get_args(char* argv[], int* thread_count_p, int* n_p) {
   *thread_count_p = strtol(argv[1], NULL, 10);
   *n_p = strtol(argv[2], NULL, 10);
}  /* Get_args */


/*---------------------------------------------------------------------
 * Function:  Gen_data
 * Purpose:   Generate random ints in the range 1 to n
 * In args:   n: number of elements
 * Out arg:   a: array of elements
 */

void Gen_data(int a[], int n) {
   int i;

   for (i = 0; i < n; i++)
      a[i] = random() % n + 1; // (double) RAND_MAX;

#  ifdef DEBUG
   Print_data(a, n, "a");
#  endif
}  /* Gen_data */


/*---------------------------------------------------------------------
 * Function:     Count_sort_serial
 * Purpose:      sort elements in an array using count sort
 * In args:      n: number of elements
 * In/out arg:   a: array of elements
 */

void Count_sort_serial(int a[], int n) {
   int i, j, count;
   int* temp = malloc(n*sizeof(int));

   for (i = 0; i < n; i++) {
      count = 0;
      for (j = 0; j < n; j++)
         if (a[j] < a[i])
            count++;
         else if (a[j] == a[i] && j < i)
            count++;
      temp[count] = a[i];
   }

   memcpy(a, temp, n*sizeof(int));
   free(temp);
}  /* Count_sort_serial */


void Count_sort_parallel(int a[], int n, int thread_count) {
   //
   //
   int i, j, count;
   int* temp = malloc(n*sizeof(int));
#pragma omp parallel for num_threads(thread_count) private(i,j,count)
   for (i = 0; i < n; i++) {
      count = 0;
      for (j = 0; j < n; j++)
         if (a[j] < a[i])
            count++;
         else if (a[j] == a[i] && j < i)
            count++;
      temp[count] = a[i];
   }

   memcpy(a, temp, n*sizeof(int));
   free(temp);   //
   //
}

void Count_sort_parallel_loop_unrooling(int a[], int n, int thread_count) {
   //
   //
   int i, j, count;
   int* temp = malloc(n*sizeof(int));
#pragma omp parallel for num_threads(thread_count) private(i,j,count)
   for (i = 0; i < n; i++) {
      count = 0;
      for (j = 0; j+15 < n; j+=16){
         if (a[j+0] < a[i] || (a[j+0] == a[i] && j+0 < i)) count++;
         if (a[j+1] < a[i] || (a[j+1] == a[i] && j+1 < i)) count++;
         if (a[j+2] < a[i] || (a[j+2] == a[i] && j+2 < i)) count++;
         if (a[j+3] < a[i] || (a[j+3] == a[i] && j+3 < i)) count++;
         if (a[j+4] < a[i] || (a[j+4] == a[i] && j+4 < i)) count++;
         if (a[j+5] < a[i] || (a[j+5] == a[i] && j+5 < i)) count++;
         if (a[j+6] < a[i] || (a[j+6] == a[i] && j+6 < i)) count++;
         if (a[j+7] < a[i] || (a[j+7] == a[i] && j+7 < i)) count++;
         if (a[j+8] < a[i] || (a[j+8] == a[i] && j+8 < i)) count++;
         if (a[j+9] < a[i] || (a[j+9] == a[i] && j+9 < i)) count++;
         if (a[j+10] < a[i] || (a[j+10] == a[i] && j+10 < i)) count++;
         if (a[j+11] < a[i] || (a[j+11] == a[i] && j+11 < i)) count++;
         if (a[j+12] < a[i] || (a[j+12] == a[i] && j+12 < i)) count++;
         if (a[j+13] < a[i] || (a[j+13] == a[i] && j+13 < i)) count++;
         if (a[j+14] < a[i] || (a[j+14] == a[i] && j+14 < i)) count++;
         if (a[j+15] < a[i] || (a[j+15] == a[i] && j+15 < i)) count++;
      }
      for (; j < n; j++){
         if (a[j] < a[i] || (a[j] == a[i] && j < i)) count++;
      }
      temp[count] = a[i];
   }

   memcpy(a, temp, n*sizeof(int));
   free(temp);   //
   //
}

void Count_sort_parallel_using_avx512(int a[], int n, int thread_count) {
   //
   //
   int i=0, j=0, count;
   int* temp = malloc(n*sizeof(int));
#pragma omp parallel for num_threads(thread_count) private(i,j,count)
//#pragma omp parallel for
   for (i = 0; i < n; i++) {
      __mmask16 res=0;
      __m512i ai,aj;
      count = 0;
      __mmask16 load = _cvtu32_mask16(-1);
      //int *pa=a+i;
      ai= _mm512_maskz_set1_epi32(load,a[i]);
      //-------
      for(j=0;j+15<i;j+=16){
         aj= _mm512_maskz_expandloadu_epi32(load,a+j);
         res = _mm512_cmpge_epi32_mask(ai,aj);//a[i]>a[j]
         count+=__builtin_popcount(res);
         //avx imp less equal
      }
      for(;j<i;j++){
         if(a[j]<=a[i]){
            count++;
         }
      }
      for(j=i;j+15<n;j+=16){
         aj= _mm512_maskz_expandloadu_epi32(load,a+j);
         res = _mm512_cmpgt_epi32_mask(ai,aj);//a[i]>=a[j]
         count+=__builtin_popcount(res);
         //axv imp less than
      }
      for(;j<n;j++){
         if (a[j] < a[i])
            count++;
      }
      //-------
      temp[count] = a[i];
   }
   memcpy(a, temp, n*sizeof(int));
   free(temp);   //
   //
}

/*---------------------------------------------------------------------
 * Function:     Library_qsort
 * Purpose:      sort elements in an array using qsort library function
 * In args:      n: number of elements
 * In/out arg:   a: array of elements
 */

void Library_qsort(int a[], int n) {
   qsort(a, n, sizeof(int), My_compare);
}  /* Library_qsort */

/*---------------------------------------------------------------------
 * Function:     My_compare
 * Purpose:      compare integer elements for use with qsort function
 * In args:      element a, element b
 * Return val:   positive if a > b, negative if b > a, 0 if equal
 */
int My_compare(const void* a, const void* b) {
   const int* int_a = (const int*) a;
   const int* int_b = (const int*) b;

   return (*int_a - *int_b);
}  /* My_compare */


/*---------------------------------------------------------------------
 * Function:  Print_data
 * Purpose:   print an array
 * In args:   a: array of elements
 *            n: number of elements
 *            msg: name of array
 */

void Print_data(int a[], int n, char msg[]) {
   int i;

   printf("%s = ", msg);
   for (i = 0; i < n; i++)
      printf("%d ", a[i]);
   printf("\n");
}  /* Print_data */


/*---------------------------------------------------------------------
 * Function:  Check_sort
 * Purpose:   Determine whether an array is sorted
 * In args:   a: array of elements
 *            n: number of elements
 * Ret val:   true if sorted, false if not sorted
 */

int  Check_sort(int a[], int n) {
   int i;

   for (i = 1; i < n; i++)
      if (a[i-1] > a[i]) return 0;
   return 1;
}  /* Check_sort */