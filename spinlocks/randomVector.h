#include <random>
#include <unistd.h>

using namespace std;


char* randomVector(long n) {
  char element[n];
  for(long i = 0; i <n; i++) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(-100,100);
    element[i] = dis(gen);
  }
  return element;
}
