#include "kmalloc.h"
#include "kmalloc_dllist.h"

/****
 *  void kmalloc_dllist_add(kmchunk_ptr chunk, kmchunk_ptr head)
 * 
 *  Adds a chunk to the front of the list and modifies the head.
 *
 *  inputs
 *      kmchunk_ptr chunk  -  pointer to the new chunk to be added
 *      kmchunk_ptr *head  -  pointer to the headpointer
 * */
void kmalloc_dllist_add(kmchunk_ptr chunk, kmchunk_ptr *head) {
    kmchunk_ptr next;
    kmchunk_ptr prev;
    
    if(*head != NULL){
        next = *head;
        prev = next->prev;
        chunk->next = next;
        chunk->prev = prev;
        prev->next = chunk;
        next->prev = chunk;
    }else{
        chunk->next = chunk;
        chunk->prev = chunk;
    }

    *head = chunk;
}

/****
 *  void kmalloc_dllist_add_end(kmchunk_ptr chunk, kmchunk_ptr head)
 * 
 *  Adds a chunk to the end of a nonempty list.
 *
 *  inputs
 *      kmchunk_ptr chunk  -  pointer to the new chunk to be added
 *      kmchunk_ptr  head  -  pointer to the head chunk
 * */
void kmalloc_dllist_add_end(kmchunk_ptr chunk, kmchunk_ptr head) {
    kmchunk_ptr prev;
    
    if(head == NULL)
        return;

    prev = head->prev;
    chunk->prev = prev;
    chunk->next = head;
    prev->next = chunk;
    head->prev = chunk;
}

/****
 *  kmchunk_ptr kmalloc_dllist_remove_intern(kmchunk_ptr chunk, kmchunk_ptr *head);
 * 
 *  Removes the given chunk from the list and updates the head if needed.
 *
 *  inputs
 *      kmchunk_ptr chunk  -  pointer to the chunk to be removed
 *      kmchunk_ptr *head  -  pointer to the headpointer, headpointer will be updated
 *
 *  Returns:
 *      pointer to the removed chunk 
 * */

kmchunk_ptr kmalloc_dllist_remove_intern(kmchunk_ptr chunk, kmchunk_ptr *head) {
    
    kmchunk_ptr next;
    kmchunk_ptr prev;

    if(chunk == NULL)
        return NULL;
    
    next = chunk->next;
    prev = chunk->prev;

    if(next != chunk){
        prev->next = next;
        next->prev = prev;
        if(*head == chunk)
            *head = next;
    } else {
        *head = NULL;
    }


    return chunk;
}

/****
 *  kmchunk_ptr kmalloc_dllist_remove(kmchunk_ptr *head);
 * 
 *  Removes the given chunk from the frontof the list.
 *
 *  inputs
 *      kmchunk_ptr *head  -  pointer to the headpointer, , headpointer will be updated
 *
 *  Returns:
 *      pointer to the removed chunk 
 * */

kmchunk_ptr kmalloc_dllist_remove(kmchunk_ptr *head) {
    kmchunk_ptr next;
    kmchunk_ptr prev;
    kmchunk_ptr chunk;
    
    chunk = *head;

    if(chunk == NULL)
        return NULL;

    next = chunk->next;
    prev = chunk->prev;

    if(next != chunk){
        prev->next = next;
        next->prev = prev;
        *head = next;
    }else{
        *head = NULL;
    }
    return chunk;
}
