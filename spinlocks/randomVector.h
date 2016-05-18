#include <random>
#include <unistd.h>
#include <thread>

using namespace std;

#define numthreads 8

thread th[numthreads];


void fillArray(int tid, long n, char element[]) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(-100,100);
  for ( int i = tid; i < n; i +=(4*numthreads)) {
    element[i + tid*4 + 0] = dis(gen);
    element[i + tid*4 + 1] = dis(gen);
    element[i + tid*4 + 2] = dis(gen);
    element[i + tid*4 + 3] = dis(gen);
  }
}

char* randomVector(long n) {
  char *element = new char[n];

  for( int i = 0; i < numthreads ; i++) {
    th[i] = thread(fillArray,i,n, element);
  }

  for(int i = 0; i < numthreads; i++) {
    th[i].join();
  }

  return element;
}


