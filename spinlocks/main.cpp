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

void getValues(int tid, int init, int end, char vector[]){
  for(int i = init; i < end; i++){
    int value = (int)vector[i];
    agg_accumulator(value);
  }
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
  cout << n << endl;
  char *vector = randomVector(n);
  long test = 0;
  for(long long i = 0; i < n; i++) {
    test += (int)vector[i];
  }

  cout << "test : " << test << endl;

  //Creating threads
  const int NUM_THREADS = atoi(argv[2]);
  thread t[NUM_THREADS];
  //vector<thread> t;
  int reader = n / NUM_THREADS;
  int rest = n % NUM_THREADS;
  int initial = 0;


  for(int i = 0; i < NUM_THREADS ; i++) {
    int end = initial + reader + ((rest-- <= 0) ? 0 : 1);
    t[i] = thread(getValues, i, initial, end, vector);
    initial = end;
  }

  for(int i = 0; i < NUM_THREADS; i++) {
    t[i].join();
  }

  delete vector;

  cout << "accumulador : " << accumulator << endl;

  return 0;
}
