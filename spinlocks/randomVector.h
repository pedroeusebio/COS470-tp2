#include <random>
#include <unistd.h>
#include <thread>

using namespace std;

#define numthreads 8

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

void fillArrayCorrect(int tid, long n, char* element) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(-100,100);

  int index = 0;
  for ( int i = tid; i < n; i +=(4*numthreads)) {
    for (int j = 0; j < 4; ++j)
    {
      index = i + tid*4 + j;
      if(index < n)
        element[index] = dis(gen);
      else
        break;
    }
  }

}

char* randomVector(long n) {
  thread th[numthreads];

  char *element = new char[n];

  for( int i = 0; i < numthreads ; i++) {
    th[i] = thread(fillArray,i,n, element);
  }

  for(int i = 0; i < numthreads; i++) {
    th[i].join();
  }

  return element;
}

void randomAllocatedVector (char* element, long n) {
  thread th[numthreads];

  for( int i = 0; i < numthreads ; i++) {
    th[i] = thread(fillArrayCorrect,i,n,element);
  }

  for(int i = 0; i < numthreads; i++) {
    th[i].join();
  }
}

void threadlessRandom (char* element, long n) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(-100,100);
  
  for (int i = 0; i < n; ++i)
  {
    element[i] = dis(gen);
  }
}


