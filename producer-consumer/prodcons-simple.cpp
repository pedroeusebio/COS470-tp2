/**
 * Semaphore example, written in C++ May 4, 2014
 * Compiled on OSX 10.9, using:
 * g++ -std=c++11 semaphore.cpp
 **/

#define LOWER 1
#define UPPER 10000000

#include <iostream>
#include <sstream>      
#include <thread>        
#include <condition_variable>
//#include <atomic>
#include <random>
#include <time.h>
#include "../utility/isPrime.h"

#include <mutex>         

#ifdef MYSEMAPHORE
  #include "semaphore.hpp"
#else
  #include <semaphore.h>
#endif
 
#ifdef DEBUG
  std::mutex c_mtx;
#endif
#ifdef PRIME
  #ifdef MYSEMAPHORE
    std::mutex p_mtx;
  #else
    sem_t p_mtx;
  #endif
#endif

#ifdef MYSEMAPHORE
  std::mutex mtx;             // mutex for buffer access
  semaphore empty;            //
  semaphore full;
#else
  sem_t mtx;
  sem_t empty;
  sem_t full;
#endif

int productsConsumed = 0;
const int limit = 10000;    //Number of products to be consumed by all consumers together

inline void addProduct (int &pid, int product, int &buffersize, int *buffer);

void producer(int pid, int buffersize, int *buffer, int amount) {
  
  std::random_device r;
  std::seed_seq seed{r(),r(),r(),r(),r(),r(),r(),r()};
  std::mt19937 gen(seed);
  std::uniform_int_distribution<int> dis(LOWER, UPPER);

  #ifdef DEBUG
    {
      std::unique_lock<std::mutex> ulock(c_mtx);
      std::cout<<"\nProducer "<<pid<<" has awaken!"<<"\n";
      std::cout<<"Will produce "<<amount<<" items"<<"\n";
    }
  #endif

  int product = 0;//dis(gen);

  for(int i = 0; i < amount; i++)
  {
    product = dis(gen);

    #ifdef MYSEMAPHORE
      empty.wait();
    #else
      sem_wait(&empty);
    #endif

    addProduct(pid, product, buffersize, buffer);

    #ifdef MYSEMAPHORE
      full.signal();
    #else
      sem_post(&full);
    #endif

  }

  #ifdef DEBUG
    {
      std::unique_lock<std::mutex> ulock(c_mtx);
      std::cout<<"\nNo need for producers. Producer ";
      std::cout<<pid<<" exiting..."<<"\n";
    }
  #endif

}

inline void addProduct (int &pid, int product, int &buffersize, int *buffer) {
  #ifdef MYSEMAPHORE
    std::unique_lock<std::mutex> ulock(mtx);
  #else
    sem_wait(&mtx);
  #endif

  for (int i = 0; i < buffersize; ++i)
  {
    if (buffer[i] == 0)
    {
      #ifdef DEBUG
        {
          std::unique_lock<std::mutex> ulock(c_mtx);
          std::cout<<pid<<" is adding product! "<<i<<" = ";
          std::cout<<product;
          std::cout<<"\n";
        }
      #endif
      
      buffer[i] = product;

      #ifdef MYSEMAPHORE

      #else
        sem_post(&mtx);
      #endif
      return;
    }
  }
}

inline void removeProduct (int &cid, int &product, int &buffersize, int *buffer);

void consumer(int cid, int buffersize, int *buffer, int amount) {
  #ifdef DEBUG
    {
      std::unique_lock<std::mutex> ulock(c_mtx);
      std::cout<<"\nConsumer "<<cid<<" has awaken!"<<"\n";
      std::cout<<"Will consume "<<amount<<" items"<<"\n";
    }
  #endif
  int product = 0;
  
  for(int i = 0; i < amount; i++)
  {
    #ifdef MYSEMAPHORE
      full.wait();
    #else
      sem_wait(&full);
    #endif

    removeProduct(cid, product, buffersize, buffer);

    #ifdef MYSEMAPHORE
      empty.signal();
    #else
      sem_post(&empty);
    #endif

    #ifdef PRIME
      #ifdef MYSEMAPHORE
        if (isPrime(product))
        {
          std::unique_lock<std::mutex> ulock(p_mtx);
          std::cout<<product<<" is prime"<<"\n";//processProduct(product);
        }
        else
        {
          std::unique_lock<std::mutex> ulock(p_mtx);
          std::cout<<product<<" is NOT prime"<<"\n";
        }
      #else
        if (isPrime(product))
        {
          sem_wait(&p_mtx);
          std::cout<<product<<" is prime"<<"\n";//processProduct(product);
          sem_post(&p_mtx);
        }
        else
        {
          sem_wait(&p_mtx);
          std::cout<<product<<" is NOT prime"<<"\n";
          sem_post(&p_mtx);
        }
      #endif
    #else
      isPrime(product);
    #endif

  }

  #ifdef DEBUG
    {
      std::unique_lock<std::mutex> ulock(c_mtx);
      std::cout<<"\nConsumer "<<cid<<" will exit!"<<"\n";
    }
  #endif
}

inline void removeProduct (int &cid, int &product, int &buffersize, int *buffer) {
  #ifdef MYSEMAPHORE
    std::unique_lock<std::mutex> ulock(mtx);
  #else
    sem_wait(&mtx);
  #endif

  for (int i = 0; i < buffersize; ++i)
  {
    if (buffer[i] != 0)
    {
      product = buffer[i];
      buffer[i] = 0;

      #ifdef MYSEMAPHORE

      #else
        sem_post(&mtx);
      #endif

      return;
    }
  }
  #ifdef DEBUG
    {
      std::unique_lock<std::mutex> ulock(c_mtx);
      std::cout<<"Did not find any product!"<<"\n";
    }
  #endif

  #ifdef MYSEMAPHORE

  #else
    sem_post(&mtx);
  #endif

  return;
}
 
int main (int argc, char const *argv[]){

  if (argc < 4)
  {
    std::cout<<"Usage is: "<<argv[0];
    std::cout<<" <buffer size> <# producers> <# consumers>";
    std::cout<<"\n";
    return 0;
  }
  std::stringstream tempss;

  //Size of product buffer
  int buffersize = 1;
  tempss<<argv[1];
  tempss>>buffersize;
  //Number of producers
  int prodnum = 1;
  tempss.str("");
  tempss.clear();
  tempss<<argv[2];
  tempss>>prodnum;
  //Number of consumers
  int consnum = 1;
  tempss.str("");
  tempss.clear();
  tempss<<argv[3];
  tempss>>consnum;

  //Array with 'products'
  int *buffer = new int[buffersize];
  //Array with producer threads
  std::thread *producers = new std::thread[prodnum];
  //Array with consumer threads
  std::thread *consumers = new std::thread[consnum];

  //Amount of values each producer has to produce
  int prodShare = limit/prodnum;
  int prodLast = limit - prodShare*(prodnum-1);

  //Amount of values to process on each consumer
  int consShare = limit/consnum;
  int consLast = limit - consShare*(consnum-1);

  //Mark all buffer positions as empty;
  #ifdef MYSEMAPHORE
    empty.setCounter(buffersize);
  #else
    sem_init(&mtx,0,1);
    sem_init(&full,0,0);
    sem_init(&empty,0,buffersize);
    #ifdef PRIME
      sem_init(&p_mtx,0,1);
    #endif
  #endif

  for (int i = 0; i < buffersize; ++i)
  {
    buffer[i] = 0;
  }

  //create prodnum producers  and consnum consumers
  int prodLimit = prodnum-1;
  for (int p = 0; p < prodLimit; p++)
    producers[p] = std::thread(producer, p, buffersize, buffer, prodShare);
  producers[prodLimit] = std::thread(producer, prodLimit, buffersize, buffer, prodLast);

  int consLimit = consnum-1;
  for (int c = 0; c < consLimit; c++)
    consumers[c] = std::thread(consumer, c, buffersize, buffer, consShare);
  consumers[consLimit] = std::thread(consumer, consLimit, buffersize, buffer, consLast);

  // Block main thread until all consumers finish
  for (int p = 0; p < prodnum; p++)
    producers[p].join();
  for (int c = 0; c < consnum; c++)
    consumers[c].join();

  {
    #ifdef DEBUG
      std::unique_lock<std::mutex> ulock(c_mtx);
    #endif
    std::cout << "\nCompleted produce consumer example!\n";
    std::cout << std::endl;
  }

  return 0;
}