#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <thread>
#include "randomVector.h"

using namespace std;


void getValues(int tid, int init, int end, char vector[]){
  for(int i = init; i < end; i++){
    //cout << "thread " << tid << ": " << (int)vector[i] << endl;
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

  long long n = atoi(argv[1]);
  cout << n << endl;
  char *vector = randomVector(n);


  for(long long i = 0; i < n ; i++) {
    cout << i << ": " << (int)vector[i]<< endl;
  }
  //Creating threads
  const int NUM_THREADS = atoi(argv[2]);
  thread t[NUM_THREADS];
  int reader = n / NUM_THREADS;
  cout << "reader:  " << reader << endl;
  int initial = 0;

  for(int i = 0; i < NUM_THREADS ; i++) {
    t[i] = thread(getValues, i, initial, initial+ reader, vector);
    initial += reader;
  }

  for(int i = 0; i < NUM_THREADS; i++) {
    t[i].join();
  }

  delete vector ;

  return 0;
}
