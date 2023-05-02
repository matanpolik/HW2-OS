#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "concurrent_list.h"

struct node {
  int value;
  node* next;
  pthread_mutex_t lock;
  
};

struct list {
  node* start;
  pthread_mutex_t lock;
};


void print_node(node* node)
{
  // DO NOT DELETE
  if(node)
  {
    pthread_mutex_lock(&(node->lock)); //locking the one node that we are printing
    printf("%d ", node->value);
    pthread_mutex_unlock(&(node->lock)); //unlocking the node
  }
}

list* create_list()
{
  list* temp;
  temp = malloc(sizeof(struct list)); //allocate new list ----ALLOCATE MEMORY !!!!
  if(temp == NULL){
      printf("Memory allocation failed");
      return;
    }
  temp->start = NULL;// make next point to NULL
  return temp;//return the new node
}

void delete_list(list* list)
{
  if(list==NULL) return;
  node* curr = list->start;
  node* temp;
  while(curr!=NULL){
    temp = curr->next;
    pthread_mutex_destroy(&(curr->lock)); //destroying every node lock
    free(curr); //locking relevant node inside print_node;
    curr = temp;
  }
  pthread_mutex_destroy(&(list->lock)); //destroying list lock
  free(list);
}

void insert_value(list* list, int value){
  if(list==NULL) return;
  node* new, *curr, *prev;
  new = malloc(sizeof(node)); //ALLOCATE MEMORY
  if(new == NULL){
    printf("Memory allocation failed");
    return;
  }
  new->value = value;
  new->next = NULL;
  pthread_mutex_init(&new->lock, NULL); //building new lock to the new node
  
  prev = list->start;
  if(list->start == NULL){ // add to first value
    list->start = new;
    return;
  }
  else if(list->start->value > value){ // add to secound node in order
    new->next = list->start;
    list->start = new;
    return;
  }
  else if(prev->next == NULL || prev->next->value > value){
    new->next = prev->next;
    prev->next = new;
    return;
  }
  else{ // WE WANT TO LOCK 2 NODES EACH ITERATION - PREVIOUS AND CURRENT
    pthread_mutex_lock(&(prev->lock)); //locking prev
    curr = prev->next;
    pthread_mutex_lock(&(curr->lock)); //locking current
    while(curr->next != NULL && curr->next->value <= value){
      pthread_mutex_unlock(&(prev->lock)); //unlock prev
      prev = curr; // prev = curr
      curr = curr->next;
      pthread_mutex_lock(&(curr->lock)); // lock new curr
    }
    new->next = curr->next;
    curr->next = new;
    pthread_mutex_unlock(&(prev->lock)); //unlock prev
    pthread_mutex_unlock(&(curr->lock));//unlock current
  }
  return;
} 


void remove_value(list* list, int value)
{
  //SAME LIKE INSERTING - WA WANT TO LOCK CURRENT AND PREVIOUS NODE EVERY TIME !
  if(list==NULL || list->start==NULL) return;
  node* curr, *pred;
  pred = list->start;
  if(pred->value==value){
    list->start = pred->next;
    pthread_mutex_destroy(&(pred->lock)); // if the one to remove is the first node
    free(pred);
    return;
  }
  pthread_mutex_lock(&pred->lock);
  curr = pred->next;
  if(curr!=NULL) pthread_mutex_lock(&curr->lock); // lock current
  while(curr!=NULL && curr->value<=value){
    if(value==curr->value){ // when we find the node to remove
      pred->next=curr->next;
      pthread_mutex_unlock(&(pred->lock)); // unlock prev
      pthread_mutex_unlock(&(curr->lock)); //unlock current
      pthread_mutex_destroy(&(curr->lock)); //destroy current
      free(curr);
      return;
    }
    pthread_mutex_unlock(&(pred->lock)); // unlock old previous
    pred=curr;
    curr=curr->next;
    if(curr!=NULL) pthread_mutex_lock(&(curr->lock)); // lock new curr
  }


}

void print_list(list* list)
{
  if(list==NULL) return;
  node* curr = list->start;
  while(curr!=NULL){
    print_node(curr); //locking relevant node inside print_node;
    curr = curr->next;
  }
  printf("\n"); // DO NOT DELETE
}

void count_list(list* list, int (*predicate)(int))
{
  int count = 0; // DO NOT DELETE
  struct node *curr, *q;
  curr = list->start;
  while(curr){
    if(predicate(curr->value)) count++; //checking predicate condition
    pthread_mutex_unlock(&(curr->lock)); //unlocking current
    curr = curr->next;
    if(curr!=NULL) pthread_mutex_lock(&(curr->lock)); //locking new current
    
  }
  printf("%d items were counted\n", count); // DO NOT DELETE
}
