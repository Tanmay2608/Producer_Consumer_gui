/*
 * ============================================================================
 * Project: Producer-Consumer Bounded-Buffer Simulator
 * Subject: Operating System (Skill-Based Lab / SLA-3)
 * Institute: Shah & Anchor Kutchhi Engineering College (Autonomous, Univ. of Mumbai)
 * 
 * Authors:
 *   1. Tanmay Agrawal  (Roll No: 02 | PRN: 125BTCM1102)
 *   2. Pruthvi Kalel   (Roll No: 23 | PRN: 125BTCM1123)
 * 
 * Mentor: Prof. Vaishali Kosamkar
 * Academic Year: 2026-2027
 * ============================================================================
 * Compilation:
 *   gcc -Wall -Wextra -pthread main.c -o producer_consumer_sim
 * 
 * Execution:
 *   ./producer_consumer_sim
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE 5
#define MAX_NAME_LEN 32

/* Bounded Buffer State Structure */
typedef struct {
    char buffer[BUFFER_SIZE];
    int in;                      /* Producer write index */
    int out;                     /* Consumer read index */
    int empty;                   /* Counting semaphore: free slots */
    int full;                    /* Counting semaphore: filled slots */
    int mutex;                   /* Binary semaphore: mutual exclusion */
    int total_produced;
    int total_consumed;
    int blocked_producers;
    int blocked_consumers;
} BoundedBufferSimulator;

/* Function Prototypes */
void initSimulator(BoundedBufferSimulator *sim);
void printDivider(char ch, int length);
void displayState(const BoundedBufferSimulator *sim);
bool produceItem(BoundedBufferSimulator *sim, char item);
bool consumeItem(BoundedBufferSimulator *sim, char *consumed_item);
void runComprehensiveBatch(BoundedBufferSimulator *sim);
void runBurstOverflowScenario(BoundedBufferSimulator *sim);
void runColdStartUnderflowScenario(BoundedBufferSimulator *sim);

/* Initialize the circular queue and synchronization semaphores */
void initSimulator(BoundedBufferSimulator *sim) {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        sim->buffer[i] = '-';
    }
    sim->in = 0;
    sim->out = 0;
    sim->empty = BUFFER_SIZE;    /* All slots initially free */
    sim->full = 0;               /* Zero items ready initially */
    sim->mutex = 1;              /* Binary mutex unlocked */
    sim->total_produced = 0;
    sim->total_consumed = 0;
    sim->blocked_producers = 0;
    sim->blocked_consumers = 0;
}

/* Helper to render formatted horizontal dividers */
void printDivider(char ch, int length) {
    for (int i = 0; i < length; i++) {
        putchar(ch);
    }
    putchar('\n');
}

/* Display the visual memory buffer, index arrows, semaphores, and invariant */
void displayState(const BoundedBufferSimulator *sim) {
    printDivider('-', 70);
    printf("BUFFER MEMORY SLOTS: [ ");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (sim->buffer[i] == '-') {
            printf("[   ] ");
        } else {
            printf("[ %c ] ", sim->buffer[i]);
        }
    }
    printf("]\n");

    printf("Indices    : in = %d (Next write), out = %d (Next read)\n", sim->in, sim->out);
    printf("Semaphores : empty = %d | full = %d | mutex = %d\n", 
           sim->empty, sim->full, sim->mutex);
    printf("Telemetry  : Produced = %d, Consumed = %d, Blocked P = %d, Blocked C = %d\n",
           sim->total_produced, sim->total_consumed, 
           sim->blocked_producers, sim->blocked_consumers);
    printf("Invariant  : empty + full = %d + %d = %d (Expected: %d) -> %s\n",
           sim->empty, sim->full, sim->empty + sim->full, BUFFER_SIZE,
           (sim->empty + sim->full == BUFFER_SIZE) ? "VERIFIED [OK]" : "VIOLATION [FAIL]");
    printDivider('-', 70);
}

/* Producer critical section and condition coordination */
bool produceItem(BoundedBufferSimulator *sim, char item) {
    printf("\n>>> Action: PRODUCE '%c'\n", item);

    /* Condition Check: Check for free slots */
    if (sim->empty == 0) {
        printf("STATUS: [BLOCKED] Buffer Full! (empty == 0). Producer must wait.\n");
        printf("ACTION: Thread suspended on sem_wait(&empty). Overwrite prevented.\n");
        sim->blocked_producers++;
        return false;
    }

    /* Wait operations (Atomic modeling) */
    sim->empty--;
    sim->mutex--;

    /* CRITICAL SECTION BEGIN */
    int target_slot = sim->in;
    sim->buffer[target_slot] = item;
    printf("SUCCESS: Placed item '%c' into Slot [%d].\n", item, target_slot);
    sim->in = (sim->in + 1) % BUFFER_SIZE;   /* Circular pointer advance */
    sim->total_produced++;
    /* CRITICAL SECTION END */

    /* Signal operations */
    sim->mutex++;
    sim->full++;

    return true;
}

/* Consumer critical section and condition coordination */
bool consumeItem(BoundedBufferSimulator *sim, char *consumed_item) {
    printf("\n>>> Action: CONSUME\n");

    /* Condition Check: Check for available items */
    if (sim->full == 0) {
        printf("STATUS: [BLOCKED] Buffer Empty! (full == 0). Consumer must wait.\n");
        printf("ACTION: Thread suspended on sem_wait(&full). Underflow prevented.\n");
        sim->blocked_consumers++;
        return false;
    }

    /* Wait operations (Atomic modeling) */
    sim->full--;
    sim->mutex--;

    /* CRITICAL SECTION BEGIN */
    int target_slot = sim->out;
    *consumed_item = sim->buffer[target_slot];
    sim->buffer[target_slot] = '-';
    printf("SUCCESS: Removed item '%c' from Slot [%d].\n", *consumed_item, target_slot);
    sim->out = (sim->out + 1) % BUFFER_SIZE; /* Circular pointer advance */
    sim->total_consumed++;
    /* CRITICAL SECTION END */

    /* Signal operations */
    sim->mutex++;
    sim->empty++;

    return true;
}

/* Scenario A & Batch Evaluation */
void runComprehensiveBatch(BoundedBufferSimulator *sim) {
    printf("\n======================================================================\n");
    printf("              AUTOMATED COMPREHENSIVE BENCHMARK EVALUATION            \n");
    printf("======================================================================\n");
    initSimulator(sim);
    displayState(sim);

    char item_ret;

    printf("\n--- Test Phase 1: Verify Initial Underflow Defense ---\n");
    consumeItem(sim, &item_ret);
    displayState(sim);

    printf("\n--- Test Phase 2: Satiate Buffer to Max Capacity ---\n");
    char batch[] = {'A', 'B', 'C', 'D', 'E'};
    for (int i = 0; i < BUFFER_SIZE; i++) {
        produceItem(sim, batch[i]);
    }
    displayState(sim);

    printf("\n--- Test Phase 3: Intercept Illegal Buffer Overflow ---\n");
    produceItem(sim, 'X');
    displayState(sim);

    printf("\n--- Test Phase 4: Circular Handoff and Pointers Wrap-Around ---\n");
    consumeItem(sim, &item_ret);
    consumeItem(sim, &item_ret);
    displayState(sim);

    produceItem(sim, 'F');
    produceItem(sim, 'G');
    displayState(sim);
}

/* Main Interactive Execution Loop */
int main(void) {
    BoundedBufferSimulator sim;
    initSimulator(&sim);
    int choice;
    char item_in, item_out;

    do {
        printf("\n======================================================================\n");
        printf("      PRODUCER-CONSUMER BOUNDED-BUFFER SIMULATOR (POSIX C99)          \n");
        printf("      Department of Computer Engineering - SAKEC (2026-2027)          \n");
        printf("======================================================================\n");
        printf("1. Produce Single Item\n");
        printf("2. Consume Single Item\n");
        printf("3. Display Current Buffer & Semaphore Telemetry\n");
        printf("4. Run Automated Comprehensive Test Batch (All Scenarios)\n");
        printf("5. Reset Buffer to Initial State\n");
        printf("6. Exit Simulator\n");
        printDivider('-', 70);
        printf("Enter your choice (1-6): ");

        if (scanf("%d", &choice) != 1) {
            printf("\n[ERROR] Invalid input encountered. Terminating.\n");
            break;
        }

        switch (choice) {
            case 1:
                printf("Enter single character data item to produce: ");
                scanf(" %c", &item_in);
                produceItem(&sim, item_in);
                displayState(&sim);
                break;
            case 2:
                if (consumeItem(&sim, &item_out)) {
                    printf("[INFO] Consumer received payload: '%c'\n", item_out);
                }
                displayState(&sim);
                break;
            case 3:
                displayState(&sim);
                break;
            case 4:
                runComprehensiveBatch(&sim);
                break;
            case 5:
                initSimulator(&sim);
                printf("\n[INFO] Buffer and semaphores successfully reset.\n");
                displayState(&sim);
                break;
            case 6:
                printf("\nExiting Producer-Consumer Simulator. Execution complete.\n");
                break;
            default:
                printf("\n[WARNING] Invalid selection! Please enter a numeric option 1 to 6.\n");
        }
    } while (choice != 6);

    return 0;
}