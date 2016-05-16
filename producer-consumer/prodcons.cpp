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
#include <atomic>
#include <random>
#include <time.h>
#include "semaphore.h"
#include "../utility/isPrime.h"
 
std::mutex mtx;             // mutex for buffer access
std::mutex c_mtx;
bool ready = false;         // Tell threads to run
bool produce = true;        // Producers try to produce while true
semaphore empty;            //
semaphore full;
std::atomic_uint a_consumerCounter;
int productsConsumed = 0;

void addProduct (int &pid, int product, int &buffersize, int *buffer);

void producer(int pid, int buffersize, int *buffer) {
  
  std::random_device r;
  std::seed_seq seed{r(),r(),r(),r(),r(),r(),r(),r()};
  std::mt19937 gen(seed);
  //std::mt19937 gen(rd() + std::clock() + std::hash<std::thread::id>()(std::this_thread::get_id()));
  std::uniform_int_distribution<int> dis(LOWER, UPPER);
  
  {
    std::unique_lock<std::mutex> ulock(c_mtx);
    std::cout<<"\nProducer "<<pid<<" has awaken!"<<std::endl;
  }
  int product = dis(gen);
  while (true) {
    product = dis(gen);//5;
    if(empty.waitOrShutdown())
    {
      {
        std::unique_lock<std::mutex> ulock(c_mtx);
        std::cout<<"\nNo need for producers. Producer ";
        std::cout<<pid<<" exiting..."<<std::endl;
      }
      return;
    }
    addProduct(pid, product, buffersize, buffer);
    full.signal();
  }

}

void addProduct (int &pid, int product, int &buffersize, int *buffer) {
  std::unique_lock<std::mutex> ulock(mtx);
  for (int i = 0; i < buffersize; ++i)
  {
    if (buffer[i] == 0)
    {
      //{
      //  std::unique_lock<std::mutex> ulock(c_mtx);
      //  std::cout<<pid<<" is adding product! "<<i<<" = ";
      //  std::cout<<product;
      //  std::cout<<std::endl;
      //}
      buffer[i] = product;
      return;
    }
  }
}

bool removeProduct (int &cid, int &product, int &buffersize, int *buffer, int &limit);

void consumer(int cid, int buffersize, int *buffer, int limit) {
  {
    std::unique_lock<std::mutex> ulock(c_mtx);
    std::cout<<"\nConsumer "<<cid<<" has awaken!"<<std::endl;
  }
  int product = 0;
  bool hasRemoved = false;
  //for (int i = 0; i < limit; ++i)
  while(true)
  {

    if(full.waitOrShutdown())
    {
      {
        std::unique_lock<std::mutex> ulock(c_mtx);
        std::cout<<"Consumer "<<cid<<" finished its job of consuming his share of ";
        std::cout<<limit<<" products. Exiting..."<<std::endl;
      }
      return;
    }
    hasRemoved = removeProduct(cid, product, buffersize, buffer, limit);
    empty.signal();

    if (hasRemoved)
    {
      if (isPrime(product))
      {
        std::unique_lock<std::mutex> ulock(c_mtx);
        std::cout<<product<<" is prime"<<std::endl;//processProduct(product);
      }
      else
      {
        std::unique_lock<std::mutex> ulock(c_mtx);
        std::cout<<product<<" is NOT prime"<<std::endl;
      }
    }

  }
}

bool removeProduct (int &cid, int &product, int &buffersize, int *buffer, int &limit) {
  std::unique_lock<std::mutex> ulock(mtx);
  if (productsConsumed >= limit)
  {
    if (empty.isShutdown() || empty.isShutdown())
    {
      return false;
    }
    {
      std::unique_lock<std::mutex> ulock(c_mtx);
      std::cout<<cid<<" is shutting down!";
      std::cout<<std::endl;
    }
    empty.doShutdown();
    full.doShutdown();
    return false;
  }

  for (int i = 0; i < buffersize; ++i)
  {
    if (buffer[i] != 0)
    {
      product = buffer[i];
      buffer[i] = 0;
      productsConsumed++;
      return true;
    }
  }
  {
    std::unique_lock<std::mutex> ulock(c_mtx);
    std::cout<<"Did not find any product!"<<std::endl;
  }
  return false;
}

//inline void processProduct(int &product) {}
 
int main (int argc, char const *argv[]){

  if (argc < 4)
  {
    std::cout<<"Usage is: "<<argv[0];
    std::cout<<" <buffer size> <# producers> <# consumers>";
    std::cout<<std::endl;
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
  //Number of products to be consumed by each consumer
  int limit = 100000;

  //Array with 'products'
  int *buffer = new int[buffersize];
  //Array with producer threads
  std::thread *producers = new std::thread[prodnum];
  //Array with consumer threads
  std::thread *consumers = new std::thread[consnum];

  //Mark all buffer positions as empty;
  empty.setCounter(buffersize);
  a_consumerCounter.exchange(consnum);

  for (int i = 0; i < buffersize; ++i)
  {
    buffer[i] = 0;
  }

  //create prodnum producers  and consnum consumers
  for (int p = 0; p < prodnum; p++)
    producers[p] = std::thread(producer, p, buffersize, buffer);
  for (int c = 0; c < consnum; c++)
    consumers[c] = std::thread(consumer, c, buffersize, buffer, limit);

  // Block main thread until all consumers finish
  for (int p = 0; p < prodnum; p++)
    producers[p].join();
  for (int c = 0; c < consnum; c++)
    consumers[c].join();

  {
    std::unique_lock<std::mutex> ulock(c_mtx);
    std::cout << "\nCompleted produce consumer example!\n";
    std::cout << std::endl;
  }

  return 0;
}