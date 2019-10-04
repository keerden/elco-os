#include "kmalloc.h"
#include "kmalloc_tree.h"
#include "kmalloc_dllist.h"


#define ktchunk_is_leave(chunk) (((chunk)->left == NULL) && ((chunk)->right == NULL))
#define ktchunk_is_in_list(chunk) (((chunk)->next != (chunk)) && ((chunk)->prev != (chunk)))

//note following macro does not work for a tree of 1 item
#define ktchunk_is_in_tree(chunk) (((chunk)->left != NULL) || ((chunk)->right != NULL) || ((chunk)->parent != NULL))

#define ktchunk_replace_head(old, new) {\
    (new)->parent = (old)->parent; \
    (new)->left = (old)->left;\
    (new)->right = (old)->right;\
    (new)->prev = (old)->prev;\
    ((old)->prev)->next = (new);\
    if((new)->left != NULL) \
        (new)->left->parent = (new);\
    if((new)->right != NULL) \
        (new)->right->parent = (new); \
}

#define ktchunk_update_parent(chunk, newchild) {\
    if(((chunk)->parent)->left == (chunk)) \
        ((chunk)->parent)->left = (newchild); \
    else \
        ((chunk)->parent)->right = (newchild);\
}

/****
 *  void kmalloc_tree_insert(ktchunk_ptr chunk, ktchunk_ptr *root, int depth);
 * 
 *  Insert a chunk in a tree based on its size. Updates root if it is NULL
 *  If there already exists a node with the same size, they will form a list.
 *
 *  inputs
 *      kmchunk_ptr chunk  -  pointer to chunk to be inserted
 *      kmchunk_ptr *root  -  pointer to the root address of the tree
 *      int depth          -  max depth of tree 
 * 
 * */

void kmalloc_tree_insert(ktchunk_ptr chunk, ktchunk_ptr *root, int depth) {
    ktchunk_ptr parent = NULL;
    ktchunk_ptr cur;
    size_t size, isright;

    if(depth <= 0 || chunk == NULL || root == NULL)
        return;

    chunk->next = chunk->prev = chunk;
    chunk->left = chunk->right = NULL;

    size = GETCHUNKSIZE(chunk);
    if(*root == NULL) { //just add
        *root = chunk;
        chunk->parent = chunk->left = chunk->right = NULL;
    } else {
        cur = *root;
        while(cur != NULL) {
            if(size == GETCHUNKSIZE(cur)){
                chunk->parent = NULL;
                kmalloc_dllist_add_end((kmchunk_ptr) chunk, (kmchunk_ptr) cur);
                return;
            }
            parent = cur;
            isright = size & (SIZE_T_ONE << (depth - 1));
            cur = isright?(cur->right):(cur->left);
            depth--;
            if(depth < 0)   //this should not happen!
                return;
        }
        chunk->parent = parent;
        if(isright)
            parent->right = chunk;
        else
            parent->left = chunk;

    }

}

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

void kmalloc_tree_remove(ktchunk_ptr chunk, ktchunk_ptr *root) {
    ktchunk_ptr replacement;

    if(chunk == NULL || root == NULL)
        return;

    if(ktchunk_is_in_list(chunk)) {
        if(chunk == *root) {
            replacement = chunk->next;
            ktchunk_replace_head(chunk, replacement);
            *root = replacement;
        }else if(ktchunk_is_in_tree(chunk)) {
            replacement = chunk->next;
            ktchunk_replace_head(chunk, replacement);
            ktchunk_update_parent(chunk, replacement);
        }else{
            (chunk->prev)->next = chunk->next;
            (chunk->next)->prev = chunk->prev;
        }

    } else if(ktchunk_is_leave(chunk)) {
        if(chunk->parent != NULL){
           ktchunk_update_parent(chunk, NULL); 
        } else {
            *root = NULL;
        }
    } else {
        //replace node with first left-most leave below
        replacement = chunk;
        while(!ktchunk_is_leave(replacement)) {
            if(replacement->left != NULL)
                replacement = replacement->left;
            else
                replacement = replacement->right;
        }
        ktchunk_update_parent(replacement, NULL); 
        replacement->left = chunk->left;
        if(replacement->left != NULL)
            replacement->left->parent = replacement;
        
        replacement->right = chunk->right;
        if(replacement->right != NULL)
            replacement->right->parent = replacement;
        
        replacement->parent = chunk->parent;
        if(replacement->parent != NULL) {
            ktchunk_update_parent(chunk, replacement); 
        } else {
            *root = replacement;
        }


    }
}

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

ktchunk_ptr kmalloc_tree_get_best_fit(size_t size, ktchunk_ptr root, int depth){

    ktchunk_ptr cur, parent, smallest_chunk = NULL;
    size_t smallest_size = MAX_SIZE_T;    

    if(depth <= 0 || root == NULL)
        return NULL;

    cur = root;

    while(cur != NULL) {
        if(size == GETCHUNKSIZE(cur)){
            return cur;
        }

        if(GETCHUNKSIZE(cur) > size){
            if(smallest_chunk == NULL || GETCHUNKSIZE(cur) < smallest_size){
                smallest_chunk = cur;
                smallest_size = GETCHUNKSIZE(cur);
            }
        }
        parent = cur;
        if(size & (SIZE_T_ONE << (depth - 1)))
            cur = cur->right;
        else
            cur = cur->left;
        depth--;
        if(depth < 0)   
            break;
    }

    //if we end up here, we didn find an exact fit. so letś find the best fit

    //first step up until we either found the smallest node, or until we could go right
    while(parent->right == NULL || parent->right == cur) {
        if(parent == smallest_chunk){
            return parent;
        }
        if(parent->parent == NULL)
            return NULL;
        cur = parent;
        parent = parent->parent;
    }

    //if we are here, parent->right holds a subtree with fitting nodes. get the smallest.
    cur = kmalloc_tree_get_smallest(parent->right);
    return cur;
}

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


ktchunk_ptr kmalloc_tree_get_smallest(ktchunk_ptr chunk) {
    ktchunk_ptr cur, smallest_chunk = NULL;
    size_t smallest_size;
    
    if(chunk == NULL)
        return NULL;
    
    cur = chunk;
    smallest_chunk = cur;
    smallest_size = GETCHUNKSIZE(cur);

    while(!ktchunk_is_leave(cur)){
        cur = (cur->left != NULL) ? cur->left : cur->right;
        if(GETCHUNKSIZE(cur) < smallest_size){
            smallest_chunk = cur;
            smallest_size = GETCHUNKSIZE(cur);
        }
    }

    return smallest_chunk;
}