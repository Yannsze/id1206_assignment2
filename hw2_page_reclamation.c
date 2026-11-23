
#include <pthread.h> 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
          
typedef struct page { 
     int page_id;
     int reference_bit;
     int total_reference_bit;
     struct page *next;
     // @other auxiliary 
} Node;


// @create an active list
Node *active_list_frontend = NULL;
Node *active_list_rearend = NULL;

// @create an inactive list
Node *inactive_list_frontend = NULL;
Node *inactive_list_rearend = NULL;

int count = 0;
pthread_mutex_t lock;
int player_finished = 0;


void *player_thread_func(void *arg) { 
     // @Add code for player
     int *reference = (int *)arg;
     int N = 64;
     int watermark = (int)(0.7 * N);

     for (int i = 0; i < 10000; i++) {
          int pid = reference[i];
          pthread_mutex_lock(&lock);

          //Search in Active list
          Node *current = active_list_frontend;
          Node *previous = NULL;
          while (current != NULL && current->page_id != pid) {
               previous = current;
               current = current->next;
          }

          if (current != NULL) {
               current->reference_bit = 1; //found in active list: set reference bit to 1

               //Move to the rear end IF NOT ALREADY AT THE END
               if (current->next != NULL) {
                    if (previous == NULL) {
                         active_list_frontend = current->next;
                    } else {
                         previous->next = current->next;
                    }

                    //Find tail and attach
                    Node *tail = active_list_frontend;
                    while (tail->next != NULL) tail = tail->next;
                    tail->next = current;
                    current->next = NULL;
               }
          } else {

               //Search in inactive list
               current = inactive_list_frontend;
               previous = NULL;
               while (current != NULL && current ->page_id != pid) {
                     previous = current;
                    current = current->next;
               }

               if (current != NULL) {
                    //remove from inactive list
                    if (previous == NULL) {
                         inactive_list_frontend = current->next;
                    } else {
                         previous->next = current->next;
                    }

                    //Add to rear of active list
                    current->reference_bit = 1; 
                    current->next = NULL;

                    if (active_list_frontend == NULL) {
                         active_list_frontend = current;
                    } else {
                          Node *tail = active_list_frontend;
                          while (tail->next != NULL) tail = tail->next;
                         tail->next = current;
                    }     
                    count++;

               } else {
                    // If coudt find anywhere , create a new one at rear of active list
                    Node *new_node = malloc(sizeof(Node));
                    new_node->page_id = pid;
                    new_node->reference_bit = 1;
                    new_node ->total_reference_bit = 0;
                    new_node->next = NULL;

                    if(active_list_frontend == NULL) {
                         active_list_frontend = new_node;
                    } else {
                         Node *tail = active_list_frontend;
                         while (tail->next != NULL) tail = tail->next;
                         tail->next = new_node;
                    }     
                    count++;
                    }
                   
               }
           
               //Check watermark
               while (count > watermark && active_list_frontend != NULL) {
                    Node *demoted = active_list_frontend;
                    active_list_frontend = active_list_frontend->next;
                    count--;

                    //Add demoted node to inactive
                    demoted->next = inactive_list_frontend;
                    inactive_list_frontend = demoted;
               }

               pthread_mutex_unlock(&lock);
               usleep(10);
          }

          pthread_mutex_unlock(&lock);
          player_finished = 1;
          pthread_mutex_unlock(&lock);

          return NULL;


   
}


void *checker_thread_func(void *arg) { 
     // @Add code for checker
     int m = *(int *)arg;

     while(1) {
          usleep(m);

          pthread_mutex_lock(&lock);
          if(player_finished && active_list_frontend == NULL) {
               pthread_mutex_unlock(&lock);
               break;
          }

          //Traverse active list and reset refernce bits
          Node *current = active_list_frontend;
          while (current != NULL) {
               if (current->reference_bit == 1) {
                    current->total_reference_bit++;
                    current->reference_bit = 0;
               }

               current = current->next;
          }

          int finished = player_finished;
         pthread_mutex_unlock(&lock);

         if(finished) break;
     }

     return NULL;

}


int main(int argc, char *argv[])
{
     int n = atoi(argv[1]);
     int m = atoi(argv[2]);

    // @create a random reference string of N
    int *reference = malloc(10000 *sizeof(int));
    for (int i = 0; i < 10000; i++) {
     reference[i] = rand() % (n +1);
    }
   
    pthread_mutex_init(&lock, NULL);
     /* Create two workers */ 
     pthread_t player;   
     pthread_t checker;    

     pthread_create(&player, NULL, player_thread_func, reference);
     pthread_create(&checker, NULL, checker_thread_func, &m);

     pthread_join(player, NULL);
     pthread_join(checker, NULL);

     printf("Page ID, Total References\n");
     //@Print out the statistics of page references
     Node *current = active_list_frontend;
     while (current != NULL) {
          printf("%d, %d\n", current->page_id, current->total_reference_bit);
          current = current->next;
     }

     current = inactive_list_frontend;
     while (current != NULL) {
          printf("%d, %d\n", current->page_id, current->total_reference_bit);
          current = current->next;
     }
     
     printf("Pages in active list: ");
     //@Print out the list of pages in active list
     current = active_list_frontend;
     while (current != NULL) {
          printf("[%d] ", current->page_id);
          current = current->next;
     }
     printf("\n");

     printf("Pages in inactive list: ");
     //@Print out the list of pages in inactive list
     current = inactive_list_frontend;
     while (current != NULL) {
          printf("[%d] ", current->page_id);
          current = current->next;
     }
     printf("\n");

     /*free up resources properly */
     current = active_list_frontend;
     while (current != NULL) {
          Node *tempor = current;
          current = current->next;
          free(tempor);
     }

     current = inactive_list_frontend;
     while (current != NULL) {
          Node *tempor = current;
          current = current->next;
          free(tempor);
     }

     pthread_mutex_destroy(&lock);
     free(reference);
     return 0;
}
