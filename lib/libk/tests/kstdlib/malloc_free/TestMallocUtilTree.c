#include "unity.h"
#include "testsuite.h"
#include <string.h>
#include <stdio.h>

#include <kstdlib.h>
#include "kstdlib/malloc_free/kmalloc.h"
#include "kstdlib/malloc_free/kmalloc_util.h"


#define testTreeChunk(chunk, prnt,lft,rght,nxt,pre) { \
    TEST_ASSERT_EQUAL_PTR_MESSAGE((prnt), (chunk).parent, "parent field mismatch"); \
    TEST_ASSERT_EQUAL_PTR_MESSAGE((lft), (chunk).left, "left field mismatch");\
    TEST_ASSERT_EQUAL_PTR_MESSAGE((rght), (chunk).right, "right field mismatch");\
    TEST_ASSERT_EQUAL_PTR_MESSAGE((nxt), (chunk).next, "next field mismatch");\
    TEST_ASSERT_EQUAL_PTR_MESSAGE((pre), (chunk).prev, "prev field mismatch");\
}

void test_TreeInsertSingle(void)
{
    ktchunk_ptr root = NULL;
    ktchunk chunk1;

    chunk1.header=0x1AB;
    kmalloc_tree_insert(&chunk1, &root, 8);

    TEST_ASSERT_EQUAL_PTR(&chunk1, root);
    testTreeChunk(chunk1, NULL, NULL, NULL, &chunk1, &chunk1);
}

void test_TreeAddSame3(void)
{
    ktchunk_ptr root = NULL;
    ktchunk chunk1;
    chunk1.header = 0x120;
    
    ktchunk chunk2 = chunk1, chunk3 = chunk1;
 
    kmalloc_tree_insert(&chunk1, &root, 8);
    kmalloc_tree_insert(&chunk2, &root, 8);
    kmalloc_tree_insert(&chunk3, &root, 8);

    TEST_ASSERT_EQUAL_PTR(&chunk1, root);
    testTreeChunk(chunk1, NULL, NULL, NULL, &chunk2, &chunk3);
    testTreeChunk(chunk2, NULL, NULL, NULL, &chunk3, &chunk1);
    testTreeChunk(chunk3, NULL, NULL, NULL, &chunk1, &chunk2);
}

void test_TreeInsert4(void)
{
    /*      8bit depth

            0x128
        0x108   0x1F0
                    0x1C8        

    */

    ktchunk chunk[4];
    ktchunk_ptr root = NULL;

    chunk[0].header = 0x128;
    chunk[1].header = 0x1F0;
    chunk[2].header = 0x188;
    chunk[3].header = 0x108;

    kmalloc_tree_insert(&chunk[0], &root, 8);
    kmalloc_tree_insert(&chunk[1], &root, 8);
    kmalloc_tree_insert(&chunk[2], &root, 8);
    kmalloc_tree_insert(&chunk[3], &root, 8);

    TEST_ASSERT_EQUAL_PTR(&chunk[0], root);
    testTreeChunk(chunk[0], NULL, &chunk[3], &chunk[1], &chunk[0], &chunk[0]);
    testTreeChunk(chunk[1], &chunk[0], &chunk[2], NULL, &chunk[1], &chunk[1]);
    testTreeChunk(chunk[2], &chunk[1], NULL, NULL, &chunk[2], &chunk[2]);    
    testTreeChunk(chunk[3], &chunk[0], NULL, NULL, &chunk[3], &chunk[3]);
}

void test_TreeInsert5Double(void)
{
    /*      8bit depth

            0x128
        0x108   0x1F0 --- 0x1F0
                    0x1C8        

    */

    ktchunk chunk[5];
    ktchunk_ptr root = NULL;

    chunk[0].header = 0x128;
    chunk[1].header = 0x1F0;
    chunk[2].header = 0x188;
    chunk[3].header = 0x108;
    chunk[4].header = 0x1F0;

    kmalloc_tree_insert(&chunk[0], &root, 8);
    kmalloc_tree_insert(&chunk[1], &root, 8);
    kmalloc_tree_insert(&chunk[2], &root, 8);
    kmalloc_tree_insert(&chunk[3], &root, 8);
    kmalloc_tree_insert(&chunk[4], &root, 8);

    TEST_ASSERT_EQUAL_PTR(&chunk[0], root);
    testTreeChunk(chunk[0], NULL, &chunk[3], &chunk[1], &chunk[0], &chunk[0]);
    testTreeChunk(chunk[1], &chunk[0], &chunk[2], NULL, &chunk[4], &chunk[4]);
    testTreeChunk(chunk[2], &chunk[1], NULL, NULL, &chunk[2], &chunk[2]);
    testTreeChunk(chunk[3], &chunk[0], NULL, NULL, &chunk[3], &chunk[3]);
    testTreeChunk(chunk[4], NULL, NULL, NULL, &chunk[1], &chunk[1]);
}

void test_TreeRemoveRoot(void){
  ktchunk_ptr root = NULL;
    ktchunk chunk1;
    chunk1.header = 0x120;
 
    kmalloc_tree_insert(&chunk1, &root, 8);
    kmalloc_tree_remove(&chunk1, &root);


    TEST_ASSERT_EQUAL_PTR(NULL, root);
}

void test_TreeRemoveRootListHead(void){
    ktchunk_ptr root = NULL;
    ktchunk chunk1;
    chunk1.header = 0x120;
    
    ktchunk chunk2 = chunk1, chunk3 = chunk1;
 
    kmalloc_tree_insert(&chunk1, &root, 8);
    kmalloc_tree_insert(&chunk2, &root, 8);
    kmalloc_tree_insert(&chunk3, &root, 8);
    kmalloc_tree_remove(&chunk1, &root);


    TEST_ASSERT_EQUAL_PTR(&chunk2, root);
    testTreeChunk(chunk2, NULL, NULL, NULL, &chunk3, &chunk3);
    testTreeChunk(chunk3, NULL, NULL, NULL, &chunk2, &chunk2);
}

void test_TreeRemoveRootList(void){
    ktchunk_ptr root = NULL;
    ktchunk chunk1;
    chunk1.header = 0x120;
    
    ktchunk chunk2 = chunk1, chunk3 = chunk1;
 
    kmalloc_tree_insert(&chunk1, &root, 8);
    kmalloc_tree_insert(&chunk2, &root, 8);
    kmalloc_tree_insert(&chunk3, &root, 8);
    kmalloc_tree_remove(&chunk2, &root);


    TEST_ASSERT_EQUAL_PTR(&chunk1, root);
    testTreeChunk(chunk1, NULL, NULL, NULL, &chunk3, &chunk3);
    testTreeChunk(chunk3, NULL, NULL, NULL, &chunk1, &chunk1);
}

void test_TreeRemoveLeave(void){
    /*
            1:0x120
        2:0x100   3:0x1F8

    */
    ktchunk chunk1, chunk2, chunk3;
    ktchunk_ptr root = NULL;
    chunk1.header = 0x120;
    chunk2.header = 0x100;
    chunk3.header = 0x1f8;


    kmalloc_tree_insert(&chunk1, &root, 8);
    kmalloc_tree_insert(&chunk2, &root, 8);
    kmalloc_tree_insert(&chunk3, &root, 8);
    kmalloc_tree_remove(&chunk2, &root);  

    TEST_ASSERT_EQUAL_PTR(&chunk1, root);

    testTreeChunk(chunk1, NULL, NULL, &chunk3, &chunk1, &chunk1);
    testTreeChunk(chunk3, &chunk1, NULL, NULL, &chunk3, &chunk3);
}

void test_TreeRemoveLeaveList(void){
    /*
            1:0x120
        2-4-5:0x100   3:0x1F8

    */
    ktchunk chunk1, chunk2, chunk3, chunk4, chunk5;
    ktchunk_ptr root = NULL;
    chunk1.header = 0x120;
    chunk2.header = 0x100;
    chunk3.header = 0x1f8;
    chunk5 = chunk4 = chunk2;

    kmalloc_tree_insert(&chunk1, &root, 8);
    kmalloc_tree_insert(&chunk2, &root, 8);
    kmalloc_tree_insert(&chunk3, &root, 8);
    kmalloc_tree_insert(&chunk4, &root, 8);
    kmalloc_tree_insert(&chunk5, &root, 8);
    kmalloc_tree_remove(&chunk4, &root);  

    TEST_ASSERT_EQUAL_PTR(&chunk1, root);

    testTreeChunk(chunk1, NULL, &chunk2, &chunk3, &chunk1, &chunk1);
    testTreeChunk(chunk2, &chunk1, NULL, NULL, &chunk5, &chunk5);
    testTreeChunk(chunk3, &chunk1, NULL, NULL, &chunk3, &chunk3);
    testTreeChunk(chunk5, NULL, NULL, NULL, &chunk2, &chunk2);

}

void test_TreeRemoveLeaveListHead(void){
 /*
            1:0x120
        2-4-5:0x100   3:0x1F8

    */
    ktchunk chunk1, chunk2, chunk3, chunk4, chunk5;
    ktchunk_ptr root = NULL;
    chunk1.header = 0x120;
    chunk2.header = 0x100;
    chunk3.header = 0x1f8;
    chunk5 = chunk4 = chunk2;



    kmalloc_tree_insert(&chunk1, &root, 8);
    kmalloc_tree_insert(&chunk2, &root, 8);
    kmalloc_tree_insert(&chunk3, &root, 8);
    kmalloc_tree_insert(&chunk4, &root, 8);
    kmalloc_tree_insert(&chunk5, &root, 8);
    kmalloc_tree_remove(&chunk2, &root);  

    TEST_ASSERT_EQUAL_PTR(&chunk1, root);

    testTreeChunk(chunk1, NULL, &chunk4, &chunk3, &chunk1, &chunk1);
    testTreeChunk(chunk3, &chunk1, NULL, NULL, &chunk3, &chunk3);
    testTreeChunk(chunk4, &chunk1, NULL, NULL, &chunk5, &chunk5);
    testTreeChunk(chunk5, NULL, NULL, NULL, &chunk4, &chunk4);
}

void test_TreeRemoveInner(void){
    /*   start tree 
                                0:0x120
                  1:(0x100)                    2:0x1F8  
           3:0x108          4:0x140
      5:0x118    6:0x130  

        expected result 
                                0:0x120
                  5:(0x118)                    2:0x1F8  
           3:0x108          4:0x140
        ()   6:0x130 


    */

    ktchunk chunk[7];
    ktchunk_ptr root = NULL;
    chunk[0].header = 0x120; //1 0010 0000
    chunk[1].header = 0x100; //1 0000 0000
    chunk[2].header = 0x1F8; //1 1111 1000
    chunk[3].header = 0x108; //1 0000 1000
    chunk[4].header = 0x140; //1 0100 0000
    chunk[5].header = 0x118; //1 0001 1000
    chunk[6].header = 0x130; //1 0011 0000

    for(int i = 0; i < 7; i++)
        kmalloc_tree_insert(&(chunk[i]), &root, 8);
    kmalloc_tree_remove(&chunk[1], &root);

    TEST_ASSERT_EQUAL_PTR(&chunk[0], root);
    testTreeChunk(chunk[0], NULL, &chunk[5], &chunk[2], &chunk[0], &chunk[0]);
    testTreeChunk(chunk[2], &chunk[0], NULL, NULL, &chunk[2], &chunk[2]);
    testTreeChunk(chunk[3], &chunk[5], NULL, &chunk[6], &chunk[3], &chunk[3]);
    testTreeChunk(chunk[4], &chunk[5], NULL, NULL, &chunk[4], &chunk[4]);
    testTreeChunk(chunk[5], &chunk[0], &chunk[3], &chunk[4], &chunk[5], &chunk[5]);
    testTreeChunk(chunk[6], &chunk[3], NULL, NULL, &chunk[6], &chunk[6]);


}
void test_TreeRemoveInnerList(void){
    /*   start tree 
                                0:0x120
                1-7-8:(0x100)                    2:0x1F8  
           3:0x108          4:0x140
      5:0x118    6:0x130  

        expected result 
                                0:0x120
                1-7:(0x100)                    2:0x1F8  
           3:0x108          4:0x140
      5:0x118    6:0x130    


    */

    ktchunk chunk[9];
    ktchunk_ptr root = NULL;
    chunk[0].header = 0x120; //1 0010 0000
    chunk[1].header = 0x100; //1 0000 0000
    chunk[2].header = 0x1F8; //1 1111 1000
    chunk[3].header = 0x108; //1 0000 1000
    chunk[4].header = 0x140; //1 0100 0000
    chunk[5].header = 0x118; //1 0001 1000
    chunk[6].header = 0x130; //1 0011 0000
    chunk[7].header = 0x100; //1 0000 0000
    chunk[8].header = 0x100; //1 0000 0000

    for(int i = 0; i < 9; i++)
        kmalloc_tree_insert(&(chunk[i]), &root, 8);
    kmalloc_tree_remove(&chunk[8], &root);

    TEST_ASSERT_EQUAL_PTR(&chunk[0], root);
    testTreeChunk(chunk[0], NULL, &chunk[1], &chunk[2], &chunk[0], &chunk[0]);
    testTreeChunk(chunk[1],  &chunk[0], &chunk[3], &chunk[4], &chunk[7], &chunk[7]);
    testTreeChunk(chunk[2], &chunk[0], NULL, NULL, &chunk[2], &chunk[2]);
    testTreeChunk(chunk[3], &chunk[1], &chunk[5], &chunk[6], &chunk[3], &chunk[3]);
    testTreeChunk(chunk[4], &chunk[1], NULL, NULL, &chunk[4], &chunk[4]);
    testTreeChunk(chunk[5], &chunk[3],NULL, NULL, &chunk[5], &chunk[5]);
    testTreeChunk(chunk[6], &chunk[3], NULL, NULL, &chunk[6], &chunk[6]);
    testTreeChunk(chunk[7],  NULL, NULL, NULL, &chunk[1], &chunk[1]);
}
void test_TreeRemoveInnerListHead(void){
    /*   start tree 
                                0:0x120
                1-7-8:(0x100)                    2:0x1F8  
           3:0x108          4:0x140
      5:0x118    6:0x130  

        expected result 
                                0:0x120
                7-8:(0x100)                    2:0x1F8  
           3:0x108          4:0x140
      5:0x118    6:0x130    


    */

    ktchunk chunk[9];
    ktchunk_ptr root = NULL;
    chunk[0].header = 0x120; //1 0010 0000
    chunk[1].header = 0x100; //1 0000 0000
    chunk[2].header = 0x1F8; //1 1111 1000
    chunk[3].header = 0x108; //1 0000 1000
    chunk[4].header = 0x140; //1 0100 0000
    chunk[5].header = 0x118; //1 0001 1000
    chunk[6].header = 0x130; //1 0011 0000
    chunk[7].header = 0x100; //1 0000 0000
    chunk[8].header = 0x100; //1 0000 0000

    for(int i = 0; i < 9; i++)
        kmalloc_tree_insert(&(chunk[i]), &root, 8);
    kmalloc_tree_remove(&chunk[1], &root);

    TEST_ASSERT_EQUAL_PTR(&chunk[0], root);
    testTreeChunk(chunk[0], NULL, &chunk[7], &chunk[2], &chunk[0], &chunk[0]);
    testTreeChunk(chunk[2], &chunk[0], NULL, NULL, &chunk[2], &chunk[2]);
    testTreeChunk(chunk[3], &chunk[7], &chunk[5], &chunk[6], &chunk[3], &chunk[3]);
    testTreeChunk(chunk[4], &chunk[7], NULL, NULL, &chunk[4], &chunk[4]);
    testTreeChunk(chunk[5], &chunk[3],NULL, NULL, &chunk[5], &chunk[5]);
    testTreeChunk(chunk[6], &chunk[3], NULL, NULL, &chunk[6], &chunk[6]);
    testTreeChunk(chunk[7],  &chunk[0], &chunk[3], &chunk[4], &chunk[8], &chunk[8]);
    testTreeChunk(chunk[8],  NULL, NULL, NULL, &chunk[7], &chunk[7]);
}

void test_TreeRemoveInnerLeaveList(void){
 /*   start tree 
                                0:0x120
                  1:(0x100)                    2:0x1F8  
           3:0x108          4:0x140
      5-7-8:0x118    6:0x130  

        expected result 
                                0:0x120
               5-7-8:(0x118)                    2:0x1F8  
           3:0x108          4:0x140
        ()   6:0x130 


    */

    ktchunk chunk[9];
    ktchunk_ptr root = NULL;
    chunk[0].header = 0x120; //1 0010 0000
    chunk[1].header = 0x100; //1 0000 0000
    chunk[2].header = 0x1F8; //1 1111 1000
    chunk[3].header = 0x108; //1 0000 1000
    chunk[4].header = 0x140; //1 0100 0000
    chunk[5].header = 0x118; //1 0001 1000
    chunk[6].header = 0x130; //1 0011 0000
    chunk[7].header = 0x118; //1 0001 1000
    chunk[8].header = 0x118; //1 0001 1000

    for(int i = 0; i < 9; i++)
        kmalloc_tree_insert(&(chunk[i]), &root, 8);
    kmalloc_tree_remove(&chunk[1], &root);

    TEST_ASSERT_EQUAL_PTR(&chunk[0], root);
    testTreeChunk(chunk[0], NULL, &chunk[5], &chunk[2], &chunk[0], &chunk[0]);
    testTreeChunk(chunk[2], &chunk[0], NULL, NULL, &chunk[2], &chunk[2]);
    testTreeChunk(chunk[3], &chunk[5], NULL, &chunk[6], &chunk[3], &chunk[3]);
    testTreeChunk(chunk[4], &chunk[5], NULL, NULL, &chunk[4], &chunk[4]);
    testTreeChunk(chunk[5], &chunk[0], &chunk[3], &chunk[4], &chunk[7], &chunk[8]);
    testTreeChunk(chunk[6], &chunk[3], NULL, NULL, &chunk[6], &chunk[6]);
    testTreeChunk(chunk[7], NULL, NULL, NULL, &chunk[8], &chunk[5]);
    testTreeChunk(chunk[8], NULL, NULL, NULL, &chunk[5], &chunk[7]);
}

void test_TreeBestFitExact(void){

     /*   start tree 
                                0:0x120
                  1:0x100                    2:0x1F8  
           3:0x108          4:0x140
      5:0x118    6:0x130  

        expected result= 3:0x108

    */

    ktchunk chunk[7];
    ktchunk_ptr result, root = NULL;
    chunk[0].header = 0x120; //1 0010 0000
    chunk[1].header = 0x100; //1 0000 0000
    chunk[2].header = 0x1F8; //1 1111 1000
    chunk[3].header = 0x108; //1 0000 1000
    chunk[4].header = 0x140; //1 0100 0000
    chunk[5].header = 0x118; //1 0001 1000
    chunk[6].header = 0x130; //1 0011 0000


    for(int i = 0; i < 7; i++)
        kmalloc_tree_insert(&(chunk[i]), &root, 8);
    result = kmalloc_tree_get_best_fit(0x108, root, 8);

    TEST_ASSERT_EQUAL_PTR(&chunk[3], result);
}

void test_TreeBestFitExactList(void){
     /*   start tree 
                                0:0x120
                  1:0x100                    2:0x1F8  
           3-7-8:0x108          4:0x140
      5:0x118    6:0x130  

        expected result= 3:0x108
    */

    ktchunk chunk[9];
    ktchunk_ptr result, root = NULL;
    chunk[0].header = 0x120; //1 0010 0000
    chunk[1].header = 0x100; //1 0000 0000
    chunk[2].header = 0x1F8; //1 1111 1000
    chunk[3].header = 0x108; //1 0000 1000
    chunk[4].header = 0x140; //1 0100 0000
    chunk[5].header = 0x118; //1 0001 1000
    chunk[6].header = 0x130; //1 0011 0000
    chunk[7].header = 0x108; //1 0000 1000
    chunk[8].header = 0x108; //1 0000 1000

    for(int i = 0; i < 9; i++)
        kmalloc_tree_insert(&(chunk[i]), &root, 8);
    result = kmalloc_tree_get_best_fit(0x108, root, 8);

    TEST_ASSERT_EQUAL_PTR(&chunk[3], result);

    
}


void test_TreeBestFitLargerSamePath(void){
 /*   start tree 
                                0:0x120
                  1:0x100                    2:0x180  
                                       3:0x1B0       4:0x1D8
                                   5:0x190  6:0x1B8   

        expected result for 0x1AB= 3:0x1B0 
    */

    ktchunk chunk[7];
    ktchunk_ptr result, root = NULL;
    chunk[0].header = 0x120; //1 0010 0000
    chunk[1].header = 0x100; //1 0000 0000
    chunk[2].header = 0x180; //1 1000 0000
    chunk[3].header = 0x1B0; //1 1011 0000
    chunk[4].header = 0x1D8; //1 1101 1000
    chunk[5].header = 0x190; //1 1001 0000
    chunk[6].header = 0x1B8; //1 1011 1000


    for(int i = 0; i < 7; i++)
        kmalloc_tree_insert(&(chunk[i]), &root, 8);
    result = kmalloc_tree_get_best_fit(0x1AB, root, 8);

    TEST_ASSERT_EQUAL_PTR(&chunk[3], result);

}

void test_TreeBestFitLargerOtherPath(void){
 /*   start tree 
                                0:0x120
                  1:0x100                    2:0x180  
                                       3:0x188       4:0x1F0
                                   5:0x190        6:0x1C8
                                                        7:0x1D8
                                                     8:0x1D0

        expected result for 0x198= 6:0x1C8 
                                                      
    */

    ktchunk chunk[9];
    ktchunk_ptr result, root = NULL;
    chunk[0].header = 0x120; //1 0010 0000
    chunk[1].header = 0x100; //1 0000 0000
    chunk[2].header = 0x180; //1 1000 0000
    chunk[3].header = 0x188; //1 1000 1000
    chunk[4].header = 0x1F0; //1 1111 0000
    chunk[5].header = 0x190; //1 1001 0000
    chunk[6].header = 0x1C8; //1 1100 1000
    chunk[7].header = 0x1D8; //1 1101 1000
    chunk[8].header = 0x1D0; //1 1101 0000


    for(int i = 0; i < 9; i++)
        kmalloc_tree_insert(&(chunk[i]), &root, 8);
    result = kmalloc_tree_get_best_fit(0x1AB, root, 8);

    TEST_ASSERT_EQUAL_PTR(&chunk[6], result);
}

void test_TreeBestFitNone(void){
 /*   start tree 
                                0:0x120
                  1:0x100                    2:0x180  
                                       3:0x1B0       4:0x1D8
                                   5:0x190  6:0x1B8   

        expected result for 0x1F8= NULL 

    */

    ktchunk chunk[7];
    ktchunk_ptr result, root = NULL;
    chunk[0].header = 0x120; //1 0010 0000
    chunk[1].header = 0x100; //1 0000 0000
    chunk[2].header = 0x180; //1 1000 0000
    chunk[3].header = 0x1B0; //1 1011 0000
    chunk[4].header = 0x1D8; //1 1101 1000
    chunk[5].header = 0x190; //1 1001 0000
    chunk[6].header = 0x1B8; //1 1011 1000


    for(int i = 0; i < 7; i++)
        kmalloc_tree_insert(&(chunk[i]), &root, 8);
    result = kmalloc_tree_get_best_fit(0x1F8, root, 8);

    TEST_ASSERT_EQUAL_PTR(NULL, result);
}

void test_TreeSmallestList(void){
/*   start tree 
                                0:0x120
                1-7-8:(0x100)                    2:0x1F8  
           3:0x108          4:0x140
      5:0x118    6:0x130  

        expected result  1:0x100
     */

    ktchunk chunk[9];
    ktchunk_ptr result, root = NULL;
    chunk[0].header = 0x120; //1 0010 0000
    chunk[1].header = 0x100; //1 0000 0000
    chunk[2].header = 0x1F8; //1 1111 1000
    chunk[3].header = 0x108; //1 0000 1000
    chunk[4].header = 0x140; //1 0100 0000
    chunk[5].header = 0x118; //1 0001 1000
    chunk[6].header = 0x130; //1 0011 0000
    chunk[7].header = 0x100; //1 0000 0000
    chunk[8].header = 0x100; //1 0000 0000

    for(int i = 0; i < 9; i++)
        kmalloc_tree_insert(&(chunk[i]), &root, 8);
    result = kmalloc_tree_get_smallest(root);

    TEST_ASSERT_EQUAL_PTR(&chunk[1], result);
}

void test_TreeSmallest (void) {
    /*
                              0:0x120
                  1:(0x100)                    2:0x1F8  
           3:0x108          4:0x140
      5:0x118    6:0x130  

        expected result= 1:0x100 

    */

    ktchunk chunk[7];
    ktchunk_ptr result, root = NULL;
    chunk[0].header = 0x120; //1 0010 0000
    chunk[1].header = 0x100; //1 0000 0000
    chunk[2].header = 0x1F8; //1 1111 1000
    chunk[3].header = 0x108; //1 0000 1000
    chunk[4].header = 0x140; //1 0100 0000
    chunk[5].header = 0x118; //1 0001 1000
    chunk[6].header = 0x130; //1 0011 0000

    for(int i = 0; i < 7; i++)
        kmalloc_tree_insert(&(chunk[i]), &root, 8);
    result = kmalloc_tree_get_smallest(root);

    TEST_ASSERT_EQUAL_PTR(&chunk[1], result);

}