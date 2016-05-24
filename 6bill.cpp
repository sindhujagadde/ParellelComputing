#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include<sys/time.h>
#include<ctime>
#include<iostream>
using namespace std;
#define SWAP(x,y) do {\
    __typeof__(x) tmp = x;\
    x = y;\
    y = tmp;\
} while(0)


struct qsort_starter
{
    int *array;
    int left;
    int right;
    int depth;
};

void parallel_quicksort(int *array, int left, int right, int depth);
int partition(int *array, int left, int right, int pivot);
void dump(int *array, int size);
void *quicksort_thread(void *init);
void quicksort(int *array, int left, int right);
int compare (const void * a, const void * b);


int main(int argc, char **argv)
{

    int depth = 7,numThreads,t2;
    struct timeval start, stop,strt,end;
	//double  startTime, endTime;

    if (argc > 1)
    {
        depth = strtol(argv[1], NULL, 10);
    }

   long int size = 6000000000;
    if (argc > 2)
    {
        size = strtol(argv[2], NULL, 10);
    }

	//for (numThreads = 1; numThreads <= 8; numThreads++) {


    srandom(time(NULL));
    int *values = (int*)calloc(size, sizeof(int));
    assert(values && "Allocation failed");

    for (long int i=0 ; i<size ; i++)
    {
        values[i] = random();
    }

    gettimeofday(&strt,NULL);

    qsort (values, size, sizeof(int), compare);

    gettimeofday(&end,NULL);

    gettimeofday(&start,NULL);

    parallel_quicksort(values, 0, size-1, depth);

	gettimeofday(&stop,NULL);
    double t=stop.tv_usec - start.tv_usec;
   t2=end.tv_usec - strt.tv_usec;

	cout << "time taken for qsort is:" <<t2<<endl;
	cout<<"time taken is:"<<t<<endl;
   // }

#ifdef DEBUG
    for (int i=0 ; i<size ; i++)
    {
        assert(values[i-1] <= values[i]);
    }


#endif
    return 0;

}



int partition(int *array, int left, int right, int pivot)
{
    int pivotValue = array[pivot];
    SWAP(array[pivot], array[right]);
    int storeIndex = left;
    for (int i=left ; i<right ; i++)
    {
        if (array[i] <= pivotValue)
        {
            SWAP(array[i], array[storeIndex]);
            storeIndex++;
        }
    }
    SWAP(array[storeIndex], array[right]);
    return storeIndex;
}


int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}
void dump(int *array, int size)
{
    for (int i=0 ; i<size ; i++)
    {
		cout<<array[i];

    }
}

void quicksort(int *array, int left, int right)
{
     if (right > left)
     {
        int pivotIndex = left + (right - left)/2;
        pivotIndex = partition(array, left, right, pivotIndex);
        quicksort(array, left, pivotIndex-1);
        quicksort(array, pivotIndex+1, right);
     }
}
void *quicksort_thread(void *init)
{

    struct qsort_starter *start= reinterpret_cast <qsort_starter *> (init);
	//*start = init;
    parallel_quicksort(start->array, start->left, start->right, start->depth);
    return NULL;
}

void parallel_quicksort(int *array, int left, int right, int depth)
{
    if (right > left)
    {
        int pivotIndex = left + (right - left)/2;
        pivotIndex = partition(array, left, right, pivotIndex);

        if (depth-- > 0)
        {

            struct qsort_starter arg = {array, left, pivotIndex-1, depth};
            pthread_t thread;
            int ret = pthread_create(&thread, NULL, quicksort_thread, &arg);
            assert((ret == 0) && "Thread creation failed");

            parallel_quicksort(array, pivotIndex+1, right, depth);

            pthread_join(thread, NULL);
        }
        else
        {
            quicksort(array, left, pivotIndex-1);
            quicksort(array, pivotIndex+1, right);
        }
    }
}
