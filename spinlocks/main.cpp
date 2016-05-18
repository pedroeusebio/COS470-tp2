#include <cstdio>
#include <sstream>
#include <iostream>
#include <thread>
#include "randomVector.h"
#include "spinLock.h"
#include <random>
#include <unistd.h>
#include <atomic>


using namespace std;

SpinLock lck;
long long accumulator  = 0;

void agg_accumulator(int agg) {
  lck.lock();
  accumulator += agg;
  lck.unlock();
}

void getValues(int tid, long n, int NUM_THREADS, double acc[],char vector[]){
  for(int i = tid; i < n; i+= NUM_THREADS){
    double value = (int)vector[i];
    acc[tid] += value;
  }
  agg_accumulator(acc[tid]);
}


int main (int argc, char const *argv[]) {

  if (argc < 2)
    {
      cout<<"Usage is:\n"<<argv[0]<<
        " <amount of numbers to generate>"<<
        endl;
      return 0;
    }

  if (!isdigit(argv[1][0]))
    {
      cerr<<"Amount of numbers to generate should be a number!"<<endl;
      return 0;
  }

  //generating array of random values

  long long n = atol(argv[1]);


  char *vector = randomVector(n);

  cout << "end Fill" << endl;
  //long test = 0;
  //for(long long i = 0; i < n; i++) {
  //  test += (int)vector[i];
  //}
  //cout << "test : " << test << endl;


  //Creating threads
  const int NUM_THREADS = atoi(argv[2]);
  thread t[NUM_THREADS];

  int reader = n / NUM_THREADS;
  int rest = n % NUM_THREADS;
  int initial = 0;

  double *acc = new double[NUM_THREADS];
  for( int  i = 0; i < NUM_THREADS; i ++) {
    acc[i] = 0;
  }

  for(int i = 0; i < NUM_THREADS ; i++) {
    t[i] = thread(getValues, i, n, NUM_THREADS, acc, vector);
  }

  for(int i = 0; i < NUM_THREADS; i++) {
    t[i].join();
  }

  cout << "accumulador : " << accumulator << endl;

  delete [] vector;


  return 0;
}
