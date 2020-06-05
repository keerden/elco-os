#ifndef __KMALLOC_DLLIST
#define __KMALLOC_DLLIST

#include "kmalloc.h"


/****
 *  void kmalloc_dllist_add(kmchunk_ptr chunk, kmchunk_ptr *head)
 * 
 *  Adds a chunk to the front of the list and modifies the head.
 *
 *  inputs
 *      kmchunk_ptr chunk  -  pointer to the new chunk to be added
 *      kmchunk_ptr *head  -  pointer to the headpointer
 * */
void kmalloc_dllist_add(kmchunk_ptr chunk, kmchunk_ptr *head);

/****
 *  void kmalloc_dllist_add_end(kmchunk_ptr chunk, kmchunk_ptr head)
 * 
 *  Adds a chunk to the end of a nonempty list.
 *
 *  inputs
 *      kmchunk_ptr chunk  -  pointer to the new chunk to be added
 *      kmchunk_ptr  head  -  pointer to the head chunk
 * */
void kmalloc_dllist_add_end(kmchunk_ptr chunk, kmchunk_ptr head);

/****
 *  kmchunk_ptr kmalloc_dllist_remove(kmchunk_ptr *head);
 * 
 *  Removes the chunk from the front of the list.
 *
 *  inputs
 *      kmchunk_ptr *head  -  pointer to the headpointer
 *
 *  Returns:
 *      pointer to the removed chunk 
 * */

kmchunk_ptr kmalloc_dllist_remove(kmchunk_ptr *head);

/****
 *  kmchunk_ptr kmalloc_dllist_remove_intern(kmchunk_ptr chunk, kmchunk_ptr *head);
 * 
 *  Removes the given chunk from the list and updates the head if needed.
 *
 *  inputs
 *      kmchunk_ptr chunk  -  pointer to the chunk to be removed
 *      kmchunk_ptr *head  -  pointer to the headpointer
 *
 *  Returns:
 *      pointer to the removed chunk 
 * */

kmchunk_ptr kmalloc_dllist_remove_intern(kmchunk_ptr chunk, kmchunk_ptr *head);


#endif // !__KMALLOC_DLLIST
