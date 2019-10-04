#ifndef __KMALLOC_TREE
#define __KMALLOC_TREE

#include "kmalloc_dllist.h"
#include "kmalloc.h"

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

#endif // !__KMALLOC_TREE
