#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* Exploring B-Trees.  */
/* Code from: http://www.cs.yale.edu/homes/aspnes/pinewiki/BTrees.html */
/* Slight modifications by Ryan Yates */

//default size is: 1024
//--------------------------------------
#define MAX_KEYS (2)
//#define MAX_KEYS (4)
//#define MAX_KEYS (8)
//#define MAX_KEYS (16)
//#define MAX_KEYS (32)
//#define MAX_KEYS (64)
//#define MAX_KEYS (128)
//#define MAX_KEYS (256)
//#define MAX_KEYS (512)
//#define MAX_KEYS (1024)

/* type for keys */
typedef int KEY;

/* B-Tree node structure */
typedef struct node node;
struct node {
    int   isLeaf;     /* is this a leaf node? */
    int   numKeys;    /* how many keys does this node contain? */
    KEY   keys[MAX_KEYS];
    node* kids[MAX_KEYS+1];  /* kids[i] holds nodes < keys[i] */
};

/* implementation of a B-tree */
typedef struct node* btree;

/* create a new empty tree */
btree create()
{
    btree b;

    b = malloc(sizeof(*b));
    assert(b);

    b->isLeaf = 1;
    b->numKeys = 0;

    return b;
}

/* free a tree */
void destroy(btree b)
{
    int i;

    if(!b->isLeaf) {
        for(i = 0; i < b->numKeys + 1; i++) {
            destroy(b->kids[i]);
        }
    }

    free(b);
}

/* return smallest index i in sorted array such that key <= a[i] */
/* (or n if there is no such index) */
static int searchKey(int n, const KEY* a, KEY key)
{
    int lo;
    int hi;
    int mid;

    /* invariant: a[lo] < key <= a[hi] */
    lo = -1;
    hi = n;

    while(lo + 1 < hi) {
        mid = (lo+hi)/2;
        if(a[mid] == key) {
            return mid;
        } else if(a[mid] < key) {
            lo = mid;
        } else {
            hi = mid;
        }
    }

    return hi;
}

/* return nonzero if key is present in tree */
int search(btree b, KEY key)
{
    int pos;

    /* have to check for empty tree */
    if(b->numKeys == 0) {
        return 0;
    }

    /* look for smallest position that key fits below */
    pos = searchKey(b->numKeys, b->keys, key);

    if(pos < b->numKeys && b->keys[pos] == key) {
        return 1;
    } else {
        return(!b->isLeaf && search(b->kids[pos], key));
    }
}

/***********************************/
/* PUT YOUR VERSION OF search HERE */
/***********************************/

/* insert a new key into a tree */
/* returns new right sibling if the node splits */
/* and puts the median in *median */
/* else returns 0 */

//write a helper function to call

int searchCount(btree b, KEY key)
{
    int pos;
    int layerCount = 1;

    /* have to check for empty tree */
    if(b->numKeys == 0) {
        return 1;
    }

    /* look for smallest position that key fits below */
    // doesnt change the nuber of nodes looked at
    pos = searchKey(b->numKeys, b->keys, key);

    if(pos < b->numKeys && b->keys[pos] == key) {
        return 1;
    } else {
        if (b->isLeaf)
        {
            return 1;
        }
        return 1 + searchCount(b->kids[pos], key);
    }
}


btree insertInternal(btree b, KEY key, KEY* median)
{
    int pos;
    int mid;
    btree b2;

    pos = searchKey(b->numKeys, b->keys, key);

    if(pos < b->numKeys && b->keys[pos] == key) {
        /* nothing to do */
        return 0;
    }

    if(b->isLeaf) {

        /* everybody above pos moves up one space */
        memmove(&b->keys[pos+1], &b->keys[pos], sizeof(*(b->keys)) * (b->numKeys - pos));
        b->keys[pos] = key;
        b->numKeys++;

    } else {

        /* insert in child */
        b2 = insertInternal(b->kids[pos], key, &mid);
        
        /* maybe insert a new key in b */
        if(b2) {

            /* every key above pos moves up one space */
            memmove(&b->keys[pos+1], &b->keys[pos], sizeof(*(b->keys)) * (b->numKeys - pos));
            /* new kid goes in pos + 1*/
            memmove(&b->kids[pos+2], &b->kids[pos+1], sizeof(*(b->keys)) * (b->numKeys - pos));

            b->keys[pos] = mid;
            b->kids[pos+1] = b2;
            b->numKeys++;
        }
    }

    /* we waste a tiny bit of space by splitting now
     * instead of on next insert */
    if(b->numKeys >= MAX_KEYS) {
        mid = b->numKeys/2;

        *median = b->keys[mid];

        /* make a new node for keys > median */
        /* picture is:
         *
         *      3 5 7
         *      A B C D
         *
         * becomes
         *          (5)
         *      3        7
         *      A B      C D
         */
        b2 = malloc(sizeof(*b2));

        b2->numKeys = b->numKeys - mid - 1;
        b2->isLeaf = b->isLeaf;

        memmove(b2->keys, &b->keys[mid+1], sizeof(*(b->keys)) * b2->numKeys);
        if(!b->isLeaf) {
            memmove(b2->kids, &b->kids[mid+1], sizeof(*(b->kids)) * (b2->numKeys + 1));
        }

        b->numKeys = mid;

        return b2;
    } else {
        return 0;
    }
}

/* insert a new element into a tree */
void insert(btree b, KEY key)
{
    btree b1;   /* new left child */
    btree b2;   /* new right child */
    KEY median;

    b2 = insertInternal(b, key, &median);

    if(b2) {
        /* basic issue here is that we are at the root */
        /* so if we split, we have to make a new root */

        b1 = malloc(sizeof(*b1));
        assert(b1);

        /* copy root to b1 */
        memmove(b1, b, sizeof(*b));

        /* make root point to b1 and b2 */
        b->numKeys = 1;
        b->isLeaf = 0;
        b->keys[0] = median;
        b->kids[0] = b1;
        b->kids[1] = b2;
    }
}

/**************************************/
/* TEST code                          */
/**************************************/
int main(int argc, char **argv)
{
    btree b;
    int i;

    /* Put your code to find the size of each node here: */
    /* printf(... */
      int btree_size = 10000000;

    b = create();
    assert(b);

    assert(search(b, 12) == 0);
    insert(b, 12);
    assert(search(b, 12) == 1);
    destroy(b);

    b = create();
    for(i = 0; i < 100; i++) {
        assert(search(b, i) == 0);
        insert(b, i);
        assert(search(b, i) == 1);
    }

    for(i = 0; i < 100; i++) {
        assert(search(b, i) == 1);
        insert(b, i);
        assert(search(b, i) == 1);
    }
      
    destroy(b);

    b = create();
    for(i = 0; i < btree_size; i += 2) {
        assert(search(b, i) == 0);
        insert(b, i);
        assert(search(b, i+1) == 0);
        assert(search(b, i) == 1);
    }

    /*******************************************************/
    /* At this point the tree `b` has 5 million even keys. */
    /* Put your test for height here.                      */
    /*******************************************************/
    int height = 0;
    int size = 0;
    printf("Max Keys: ");
    printf("%d\n", MAX_KEYS);
    printf("Items in tree: ");
    printf("%d\n", btree_size/2);
    printf("Searching... \n");
    height = searchCount(b, 13);
    printf("Height of tree: ");
    printf("%d\n", height);
    printf("Size (double nibbles) of Node: ");
    printf("%ld\n", sizeof(node));
    printf("Size of Tree in memory: ");
    printf("%ld\n", sizeof(node)*((btree_size/2)/MAX_KEYS));

    destroy(b);

    return 0;
}