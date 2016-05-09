/* radix.c - implement a radix sort from Knuth's TAOCP (5.2.5) */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <getopt.h>

/* Barebones queue implementation following Knuth's for the sort. */
typedef struct NODE {
    unsigned int data;
    struct NODE *next;
} NODE;

/* Note that Knuth lets the rear pointer point at both nodes and the front
 * pointer. In that sense, these function more like void pointers than NODE
 * pointers. The pros and cons of assembly language, I suppose. */
typedef struct {
    NODE *rear;
    NODE *front;
} QUEUE;

QUEUE *queue_create(void)
{
    QUEUE *queue = calloc(1, sizeof(*queue));

    return queue;
}

/* Insert the given NODE* into the given queue.
 * Note that _technically_ this is not correct, as it does not set
 * insert->next = NULL. This is to match Knuth's algorithm. */
void queue_insert(QUEUE *queue, NODE *insert)
{
    /* Knuth assumes that rear = &front is valid, and that (&front)->next =
     * front. Because of types, this is not valid in C. But if it were, this
     * would allow us to say queue->rear->next = insert in all cases, because
     * if the queue was empty, then
     *      rear->next = insert
     * would be equivalent to
     *      front = insert
     *
     * But then we lose types. The pros and cons of assembly language. */
    if (queue->rear == NULL) {
        queue->front = insert;
    } else {
        queue->rear->next = insert;
    }

    queue->rear = insert;
}

/* Source: https://stackoverflow.com/questions/101439. */
unsigned int ipow(int base, int exp)
{
    unsigned int result = 1;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}

/* Hookup the list of given queues together into one linked list, where
 * queues[0] is first, then queues[1], etc. etc. This is Algorithm H in TAOCP
 * 5.2.5.
 *
 * After this algorithm is run, queues[0]->front points to the first element in
 * the list, or is NULL if the list was entirely empty. Then
 * queues[0]-front-next is the second element, and so on. */
void queue_hookup(QUEUE **queues, int n)
{
    NODE *P = NULL;
    int i = 0;

    while (1) {
        P = queues[i]->rear;

        /* Try to find the next non-empty queue. */
        while (1) {
            i++;

            /* No more queues, so linking is done. */
            if (i == n) {
                /* If every queue was empty, then P = NULL here. */
                if (P != NULL) {
                    P->next = NULL;
                }

                return;
            }

            /* Found a non-empty queue, so link it up. This is the only
             * place that can break, so queues[i] will never be empty after
             * this loop. */
            if (queues[i]->front != NULL) {
                /* P = NULL only on the first time the outer loop runs, meaning
                 * that queues[0] was empty and needs special treatment to
                 * link. */
                if (P != NULL) {
                    P->next = queues[i]->front;
                } else {
                    queues[0]->front = queues[i]->front;
                }

                break;
            }
        }
    }
}

/* Perform a radix sort on the given non-negative integer inputs. This
 * particular implementation is fairly low-level, and follows Algorithm R given
 * in TAOCP 5.2.5 as closely as possible. The main difficulty with recreating
 * the algorithm in C is that the steps are given fairly non-linearly, i.e.
 * with lots of implicit gotos, and Knuth assumes that there are no such things
 * as "types" or "higher-level constructs." Comments throughout indicate where
 * this differs from Algorithm R. */
NODE *radix_sort(unsigned int *inputs, int n, int radix, int length)
{
    /* Create the needed queues. */
    QUEUE **queues = calloc(radix, sizeof(*queues));

    for (int i = 0; i < radix; ++i) {
        queues[i] = queue_create();
    }

    NODE *P = NULL;
    /* Loop over every digits place, starting with place 0. */
    for (int place = 0; place < length; ++place) {
        if (place == 0) {
            /* Just started; place inputs into queues.
             * Note that Knuth assumes that the inputs are already NODEs, hence
             * his use of the pointer P here. */
            for (int i = 0; i < n; ++i) {
                NODE *insert = calloc(1, sizeof(*insert));
                insert->data = inputs[i];

                unsigned int digit = inputs[i] % radix;
                queue_insert(queues[digit], insert);
            }
        } else {
            /* Set queues empty. The pointer P is currently pointing to the
             * first element in the list. */
            for (int i = 0; i < radix; ++i) {
                /* Knuth sets queues[i]->front = &(queues[i]->rear), which is
                 * not valid in C. */
                queues[i]->front = NULL;
                queues[i]->rear = NULL;
            }

            while (P) {
                /* Insert into queues[digit] for the current place. */
                unsigned int digit = (P->data / ipow(radix, place)) % radix;

                /* Note that queue_insert does not modify P in any way until
                 * another insertion is performed, i.e. P->next is still valid
                 * here. */
                queue_insert(queues[digit], P);
                P = P->next;
            }
        }

        /* Hookup the queues, i.e. perform Algorithm H. */
        queue_hookup(queues, radix);

        /* P is now a pointer to the first element. */
        P = queues[0]->front;
    }

    /* Free the memory associated with all the queues.
     * If the algorithm ran correctly, then P should be the only bookkeeping
     * that we need, so we can safely free here. */
    for (int i = 0; i < radix; ++i) {
        free(queues[i]);
    }

    free(queues);

    return P;
}

/* Generate a random, unsigned integer in the range [start, stop). */
unsigned int rand_range(int start, int stop)
{
    return (rand() % (stop - 1 - start)) + start;
}

int int_compare(const void *a, const void *b)
{
    return *(int*)a < *(int*)b;
}

#define START 0

int main(int argc, char *argv[])
{
    int radix = 10;
    int count = 100000;
    int length = 5;
    int choice;

    while (1)
    {
        /* ":" denotes an option that has a required argument. */
        choice = getopt(argc, argv, "hr:n:l:");

        if (choice == -1)
            break;

        switch (choice)
        {
            case 'h':
                puts("Usage: radix [-h] [-l base-10 number length] [-n number of elements] [-r radix]");
                exit(EXIT_FAILURE);
                break;

            case '?':
                /* getopt_long will have already printed an error */
                break;

            case 'r':
                radix = atoi(optarg);
                break;

            case 'n':
                count = atoi(optarg);
                break;

            case 'l':
                length = atoi(optarg);
                break;

            default:
                /* Not sure how to get here... */
                return EXIT_FAILURE;
        }
    }

    unsigned int *to_sort = calloc(count, sizeof(*to_sort));
    int end = ipow(10, length);

    printf("radix: %d\nn: %d\nrange: [%d, %d)\n", radix, count, START, end);

    /* Create `count` pseudorandom non-negative integers. */
    srand(time(NULL));
    for (int i = 0; i < count; ++i) {
        to_sort[i] = rand_range(START, end);
        if (count <= 20) {
            printf("%u\n", to_sort[i]);
        }
    }

    /* Length of x in base d is floor(log_d(x)) + 1 = floor(log(x)/log(d)) + 1. */
    int end_radix_length = (int)floor(log(end - 1) / log(radix)) + 1;
    printf("length in base-%d: %d\n", radix, end_radix_length);

    clock_t radix_start = clock(), radix_diff;
    NODE *traverser = radix_sort(to_sort, count, radix, end_radix_length);
    radix_diff = clock() - radix_start;

    clock_t qsort_start = clock(), qsort_diff;
    qsort(to_sort, count, sizeof(*to_sort), int_compare);
    qsort_diff = clock() - qsort_start;

    if (count <= 20) {
        putchar('\n');
    }

    while (traverser) {
        if (count <= 20) {
            printf("%u\n", traverser->data);
        }

        NODE *save = traverser;
        traverser = traverser->next;

        free(save);
    }

    free(to_sort);

    printf("radix sort time: %Lfms\n", (long double)(radix_diff * 1000 / CLOCKS_PER_SEC));
    printf("qsort time: %Lfms\n", (long double)(qsort_diff * 1000 / CLOCKS_PER_SEC));

    return 0;
}
