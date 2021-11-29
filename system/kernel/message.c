#include "kernel.h"

#include <libk.h>
#include <kstring.h>
#include <kstdlib.h>


int message_send(task_t *caller, int32_t dest_pid, message_t *message)
{
    //TODO: check if caller and message buffer is valid


    task_t *dest= find_task(dest_pid);
    if(dest == NULL)
        return -1;  //destination not found

    //check if destination is waiting for this message
    if((dest->message_flags & MESSAGE_FLAGS_RECEIVING) && (dest->message_rec_pid == caller->pid || dest->message_rec_pid == PID_ALL_TASKS)){
        kmemcpy(&dest->message_buffer, message, sizeof(message_t));
        dest->message_flags &= (message_flags_t) ~MESSAGE_FLAGS_RECEIVING;
        dest->message_flags |= MESSAGE_FLAGS_DELIVERING;
        dest->flags |= TASK_FLAGS_AFTER_SWITCH; //when receiver is scheduled, 

        unblock_task(dest);

    }else{
        caller->message_flags |= MESSAGE_FLAGS_SENDING;
        caller->message_send_to = dest;
        kmemcpy(&caller->message_buffer, message, sizeof(message_t));

        task_t *tsk = dest->message_q_head;

        //add sender to end of destination message Q
        if(tsk == NULL){
            dest->message_q_head = caller;
            caller->message_q_next = NULL;
        }else{
            while(tsk->message_q_next != NULL)
                tsk = tsk->message_q_next;
            tsk->message_q_next = caller;
            caller->message_q_next = NULL;

        }
        block_task(caller);
    }
    return 0;
}

int message_receive(task_t *caller, int32_t origin_pid, message_t *message)
{
    if(origin_pid == PID_NO_TASK)
        return -1;

    //TODO: check if caller and message buffer is valid

    //check if wanted message is available
    task_t *sender = caller->message_q_head;
    task_t **prev = &caller->message_q_head;

    while(sender != NULL){
        if(origin_pid == PID_ALL_TASKS || origin_pid == sender->pid)
            break;

        prev = &sender->message_q_next;
        sender = sender->message_q_next;
    }

    if(sender != NULL){
        kmemcpy(message, &sender->message_buffer, sizeof(message_t));
        *prev = sender->message_q_next;
        sender->message_q_next = NULL;
        sender->message_send_to = NULL;
        sender->message_flags &= (message_flags_t) ~MESSAGE_FLAGS_SENDING;
        unblock_task(sender);

    }else{
        //block and wait for message
        caller->message_ptr = message;
        caller->message_rec_pid = origin_pid;
        caller->message_flags |= MESSAGE_FLAGS_RECEIVING;
        block_task(caller);
    } 
    return 0;
}

bool message_deliver(task_t *caller){
    if((caller->message_flags & MESSAGE_FLAGS_DELIVERING) == 0)
        return false;
    
    kmemcpy(caller->message_ptr, &caller->message_buffer, sizeof(message_t));
    kmemset(&caller->message_buffer,0,  sizeof(message_t));
    caller->message_flags &= (message_flags_t) ~MESSAGE_FLAGS_DELIVERING;
    caller->message_rec_pid = PID_NO_TASK;
    caller->message_ptr = NULL;

    return true;
}