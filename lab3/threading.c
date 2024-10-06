#include "threading.h"



#include <stdlib.h>
#include <stdio.h>     
#include <string.h>     
#include <ucontext.h>  
#include <stdint.h>     

void t_start(intptr_t foo_ptr, int32_t arg1, int32_t arg2);
void t_finish();



void t_init()
{
        // TODO

        // printf("initializing contexts   \n");
        for (int i = 0; i < NUM_CTX; i++) {
                contexts[i].state = INVALID; // invalid state
            }
            current_context_idx = NUM_CTX; // invalid index



}

int32_t t_create(fptr foo, int32_t arg1, int32_t arg2)
{
        // TODO



        volatile uint8_t idx;


        for (idx = 0; idx < NUM_CTX; idx++) {
            if (contexts[idx].state == INVALID) {
                // printf("creating context for %d to %d   \n", arg1, arg2);
                

                if (getcontext(&contexts[idx].context) == -1) {
                    perror("getcontext failed");
                    return -1; //  error if getcontext fails
                }

                
                contexts[idx].context.uc_stack.ss_sp = malloc(STK_SZ);
                if (contexts[idx].context.uc_stack.ss_sp == NULL) {
                    perror("malloc failed");
                    return -1; //  error if malloc fails
                }
                contexts[idx].context.uc_stack.ss_size = STK_SZ;
                contexts[idx].context.uc_stack.ss_flags = 0;
                contexts[idx].context.uc_link = NULL; // do this in tfinsih

                makecontext(&contexts[idx].context, (void (*)()) t_start, 3, (intptr_t)foo, arg1, arg2);
                contexts[idx].state = VALID; // context is VALID
                return 0; // yay
            }
        }
        return -1;
}

int32_t t_yield()
{
        // TODO


        // uint8_t previous_context_idx = current_context_idx;

        if (current_context_idx == NUM_CTX) { // no current context
            for (volatile uint8_t idx = 0; idx < NUM_CTX; idx++) {
                if (contexts[idx].state == VALID) {
                    current_context_idx = idx;
                    // printf("starting context %d   \n", idx);
                    setcontext(&contexts[idx].context);

                    perror("setcontext failed"); // errorif fails
                    exit(1);
                }
            }
            return -1;
        }

    
        return -1;



}

void t_finish()
{
        // TODO



            // printf("finishing context %d   \n", current_context_idx);
            free(contexts[current_context_idx].context.uc_stack.ss_sp);
            contexts[current_context_idx].state = INVALID; 

            // check if others valid
            for (volatile uint8_t idx = 0; idx < NUM_CTX; idx++) {
                if (contexts[idx].state == VALID) {
                    current_context_idx = idx;
                    // printf("switching to context %d   \n", idx);
                    setcontext(&contexts[idx].context);
               
                }
            }

            // exit if no  contexts
            exit(0);

}


void t_start(intptr_t foo_ptr, int32_t arg1, int32_t arg2)
{
    fptr foo = (fptr)foo_ptr;
    foo(arg1, arg2);
    t_finish();
}
