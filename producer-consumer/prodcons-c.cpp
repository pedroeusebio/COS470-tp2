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
#include <mutex>         
#include <condition_variable>
//#include <atomic>
#include <random>
#include <time.h>
#include <semaphore.h>
#include "../utility/isPrime.h"
 
sem_t mtx;             // mutex for buffer access
#ifdef DEBUG
  sem_t c_mtx;
#endif
#ifdef PRIME
  sem_t p_mtx;
#endif

sem_t empty;            //
sem_t full;

int productsConsumed = 0;
const int limit = 10000;    //Number of products to be consumed by all consumers together

inline bool addProduct (int &pid, int product, int &buffersize, int *buffer);

void producer(int pid, int buffersize, int *buffer) {
  
  std::random_device r;
  std::seed_seq seed{r(),r(),r(),r(),r(),r(),r(),r()};
  std::mt19937 gen(seed);
  std::uniform_int_distribution<int> dis(LOWER, UPPER);
  bool produce = true;

  #ifdef DEBUG
    {
      sem_wait(&c_mtx);
      std::cout<<"\nProducer "<<pid<<" has awaken!"<<"\n";
      sem_post(&c_mtx);
    }
  #endif

  int product = dis(gen);

  while (produce) {
    product = dis(gen);
    sem_wait(&empty);
    produce = addProduct(pid, product, buffersize, buffer);
    sem_post(&full);
  }

  #ifdef DEBUG
    {
      sem_wait(&c_mtx);
      std::cout<<"Producer "<<pid<<" exiting..."<<"\n";
      sem_post(&c_mtx);
    }
  #endif

}

inline bool addProduct (int &pid, int product, int &buffersize, int *buffer) {
  sem_wait(&mtx);  
  if (productsConsumed >= limit)
  {
    #ifdef DEBUG
    {
      sem_wait(&c_mtx);
      std::cout<<"Producer"<<pid<<" is shutting down!";
      std::cout<<"\n";
      sem_post(&c_mtx);
    }
    #endif
    sem_post(&mtx);
    sem_post(&empty);
    return false;
  }
  for (int i = 0; i < buffersize; ++i)
  {
    if (buffer[i] == 0)
    {
      #ifdef DEBUG
        {
          sem_wait(&c_mtx);
          std::cout<<pid<<" is adding product! "<<i<<" = ";
          std::cout<<product;
          std::cout<<"\n";
          sem_post(&c_mtx);
        }
      #endif
      buffer[i] = product;
      sem_post(&mtx);
      return true;
    }
  }
  //The function should never reach here, but if it does...
  sem_post(&mtx);
  sem_post(&empty);
  return false;
}

inline bool removeProduct (int &cid, int &product, int &buffersize, int *buffer);

void consumer(int cid, int buffersize, int *buffer) {
  #ifdef DEBUG
    {
      sem_wait(&c_mtx);
      std::cout<<"\nConsumer "<<cid<<" has awaken!"<<"\n";
      sem_post(&c_mtx);
    }
  #endif
  int product = 0;
  bool hasRemoved = true;
  
  while(hasRemoved)
  {
    sem_wait(&full);
    hasRemoved = removeProduct(cid, product, buffersize, buffer);
    sem_post(&empty);
    if (hasRemoved)
    {
      #ifdef PRIME
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
      #else
        isPrime(product);
      #endif
    }
  }

  #ifdef DEBUG
    {
      sem_wait(&c_mtx);
      std::cout<<"Consumer "<<cid<<" finished its job of consuming his share of ";
      std::cout<<limit<<" products. Exiting..."<<"\n";
      sem_post(&c_mtx);
    }
  #endif
}

inline bool removeProduct (int &cid, int &product, int &buffersize, int *buffer) {
  sem_wait(&mtx);
  if (productsConsumed >= limit)
  {
    #ifdef DEBUG
      {
        sem_wait(&c_mtx);
        std::cout<<"Consumer"<<cid<<" is shutting down!";
        std::cout<<"\n";
        sem_post(&c_mtx);
      }
    #endif
    sem_post(&mtx);
    sem_post(&full);
    return false;
  }

  for (int i = 0; i < buffersize; ++i)
  {
    if (buffer[i] != 0)
    {
      product = buffer[i];
      buffer[i] = 0;
      productsConsumed++;
      sem_post(&mtx);
      return true;
    }
  }
  //The function should never reach here, but if it does...
  #ifdef DEBUG
    {
      sem_wait(&c_mtx);
      std::cout<<"Did not find any product!"<<"\n";
      sem_post(&c_mtx);
    }
  #endif
  sem_post(&mtx);
  sem_post(&full);
  return false;
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

  //Mark all buffer positions as empty;
  sem_init(&mtx,0,1);
  sem_init(&full,0,0);
  sem_init(&empty,0,buffersize);
  #ifdef PRIME
    sem_init(&p_mtx,0,1);
  #endif
  #ifdef DEBUG
    sem_init(&c_mtx,0,1);
  #endif

  for (int i = 0; i < buffersize; ++i)
  {
    buffer[i] = 0;
  }

  //create prodnum producers  and consnum consumers
  for (int p = 0; p < prodnum; p++)
    producers[p] = std::thread(producer, p, buffersize, buffer);
  for (int c = 0; c < consnum; c++)
    consumers[c] = std::thread(consumer, c, buffersize, buffer);

  // Block main thread until all consumers finish
  for (int p = 0; p < prodnum; p++)
    producers[p].join();
  for (int c = 0; c < consnum; c++)
    consumers[c].join();

  {
    #ifdef DEBUG
      sem_wait(&c_mtx);
    #endif
    std::cout << "\nCompleted produce consumer example!\n";
    std::cout << std::endl;
    #ifdef DEBUG
      sem_post(&c_mtx);
    #endif
  }

  return 0;
}