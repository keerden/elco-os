#ifndef __KMALLOC_UTIL
#define __KMALLOC_UTIL

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
 *  void kmalloc_chunk_iterate(kmchunk_ptr *chunk);
 * 
 *  Used for iterating through all chunks in heap.
 *
 *  inputs
 *      kmchunk_ptr *chunk  -  pointer to current chunk address
 *
 *  outputs
 *      kmchunk_ptr *chunk  -  points to next chunk, 
 *                             equal to NULL when the end of heap is reached
 * */

void kmalloc_chunk_iterate(kmchunk_ptr *chunk);

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


/****
 *  void kmalloc_tree_insert(ktchunk_ptr chunk, ktchunk_ptr *root, int depth);
 * 
 *  Insert a chunk in a tree. Updates root if it is NULL
 *
 *  inputs
 *      kmchunk_ptr chunk  -  pointer to chunk to be inserted
 *      kmchunk_ptr *root  -  pointer to the root address of the tree
 *      int depth          -  max depth of tree 
 * 
 * */

void kmalloc_tree_insert(ktchunk_ptr chunk, ktchunk_ptr *root, int depth);

/****
 *  void kmalloc_tree_remove(ktchunk_ptr chunk, ktchunk_ptr *root);
 * 
 *  Removes a chunk from a tree. Updates root if needed
 *  if the chunk is not a leave. It is replaced by the first left-most leave below.
 * 
 *
 *  inputs
 *      kmchunk_ptr chunk  -  pointer to chunk to be inserted
 *      kmchunk_ptr *root  -  pointer to the root address of the tree
 * 
 * */

void kmalloc_tree_remove(ktchunk_ptr chunk, ktchunk_ptr *root);

/****
 *  ktchunk_ptr kmalloc_tree_get_best_fit(size_t size, ktchunk_ptr root, int depth);
 * 
 *  Removes the best fitting chunk from the tree.
 *
 *  inputs
 *      size_t size        -  wanted chunksize
 *      kmchunk_ptr root  -  pointer to the root of the tree
 *      int depth          -  max depth of tree
 * 
 *  returns
 *      a ktchunk_ptr pointing to the best fitting chunk
 * 
 * */

ktchunk_ptr kmalloc_tree_get_best_fit(size_t size, ktchunk_ptr root, int depth);

/****
 *  ktchunk_ptr kmalloc_tree_get_smallest(ktchunk_ptr root);
 * 
 *  Removes the smallest chunk from the tree.
 *
 *  inputs
 *      kmchunk_ptr root  -  pointer to the root of the tree
 * 
 *  returns
 *      a ktchunk_ptr pointing to the smallest chunk
 * 
 * */

ktchunk_ptr kmalloc_tree_get_smallest(ktchunk_ptr root);

#endif // !__KMALLOC_UTIL
