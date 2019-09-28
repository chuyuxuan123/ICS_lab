/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1 << 12)

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp) ((char *)(bp)-WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp)-WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE)))

#define PRED(bp) ((char *)(bp))
#define SUCC(bp) ((char *)(bp + DSIZE))

#define GETADDR(p) (*(unsigned long *)(p))
#define PUTADDR(p, val) (*(unsigned long *)(p) = (val))

static void *coalesce(void *bp);
static void *extend_heap(size_t words);
static void *find_fit(size_t size);
static void place(void *bp, size_t asize);

static char *heap_listp;

static char *blk_headp = NULL;

static char *mem_heap;
static char *mem_brk;
static char *mem_max_addr;

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));
    heap_listp += (2 * WSIZE);
    blk_headp = NULL;
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;
    return 0;
}

static void *extend_heap(size_t words)
{

    char *bp;
    size_t size;

    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    printf("extending...size:%d\n", size); //DEBUG

    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    // printf("entended successfully before coalesce\n"); //DEBUG

    return coalesce(bp);
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extendsize;
    char *bp;

    printf("before alloc list head:%ld\n", (unsigned long)blk_headp - (unsigned long)heap_listp); //DEBUG
    if (size == 0)
    {
        return NULL;
    }

    if (size <= 2 * DSIZE)
    {
        asize = 3 * DSIZE;
    }
    else
    {
        asize = DSIZE * ((size + (2 * DSIZE) + (DSIZE - 1)) / DSIZE);
    }
    if ((bp = find_fit(asize)) != NULL)
    {
        place(bp, asize);
        printf("after alloc list head:%ld\n", (unsigned long)blk_headp - (unsigned long)heap_listp); //DEBUG

        return bp;
    }

    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
    {
        return NULL;
    }
    place(bp, asize);
    printf("after alloc list head:%ld\n", (unsigned long)blk_headp - (unsigned long)heap_listp); //DEBUG

    return bp;
}

static void *find_fit(size_t size)
{
    void *bp = blk_headp;

    while (bp)
    {
        /* code */
        if (size <= GET_SIZE(HDRP(bp)))
        {
            return bp;
        }
        bp = GETADDR(SUCC(bp));
    }

    return NULL;
}

static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) >= (3 * DSIZE))
    {
        unsigned long pred_bp = GETADDR(bp);
        unsigned long succ_bp = GETADDR(SUCC(bp));
        if (pred_bp && succ_bp) //not head not tail
        {
            PUT(HDRP(bp), PACK(asize, 1));
            PUT(FTRP(bp), PACK(asize, 1));
            bp = NEXT_BLKP(bp);
            PUT(HDRP(bp), PACK(csize - asize, 0));
            PUT(FTRP(bp), PACK(csize - asize, 0));

            PUTADDR(bp, pred_bp);
            PUTADDR(SUCC(bp), succ_bp);
            PUTADDR(SUCC(pred_bp), bp);
            PUTADDR(succ_bp, bp);
        }
        else if (!pred_bp && succ_bp) //head ;update next free block pred
        {
            PUT(HDRP(bp), PACK(asize, 1));
            PUT(FTRP(bp), PACK(asize, 1));
            bp = NEXT_BLKP(bp);
            PUT(HDRP(bp), PACK(csize - asize, 0));
            PUT(FTRP(bp), PACK(csize - asize, 0));

            PUTADDR(bp, 0);
            PUTADDR(SUCC(bp), succ_bp);
            PUTADDR(succ_bp, bp);
            blk_headp = bp;
        }
        else if (pred_bp && !succ_bp) //tail ; update previous free block succ
        {
            PUT(HDRP(bp), PACK(asize, 1));
            PUT(FTRP(bp), PACK(asize, 1));
            bp = NEXT_BLKP(bp);
            PUT(HDRP(bp), PACK(csize - asize, 0));
            PUT(FTRP(bp), PACK(csize - asize, 0));

            PUTADDR(SUCC(bp), 0);
            PUTADDR(bp, pred_bp);
            PUTADDR(SUCC(pred_bp), bp);
        }
        else //empty list ; create a new node
        {
            PUT(HDRP(bp), PACK(asize, 1));
            PUT(FTRP(bp), PACK(asize, 1));
            bp = NEXT_BLKP(bp);
            PUT(HDRP(bp), PACK(csize - asize, 0));
            PUT(FTRP(bp), PACK(csize - asize, 0));

            PUTADDR(SUCC(bp), 0);
            PUTADDR(bp, 0);
            blk_headp = bp;
        }
    }
    else
    {
        unsigned long pred_bp = GETADDR(bp);
        unsigned long succ_bp = GETADDR(SUCC(bp));
        if (pred_bp && succ_bp) //not head not tail
        {
            PUTADDR(SUCC(pred_bp), succ_bp);
            PUTADDR(succ_bp, pred_bp);
        }
        else if (!pred_bp && succ_bp) //head ;update next free block pred
        {
            PUTADDR(succ_bp, 0);
            blk_headp = succ_bp;
        }
        else if (pred_bp && !succ_bp) //tail ; update previous free block succ
        {
            PUTADDR(SUCC(pred_bp), 0);
        }
        else //empty list ; create a new node
        {
            blk_headp = NULL;
            printf("oopS!===\n");
        }
        //TODO
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc)
    {
        printf("#####-------######coalesce happen\n"); //DEBUG
        PUTADDR(bp, 0);
        PUTADDR(SUCC(bp), blk_headp);
        blk_headp = bp;
        return bp;
    }

    else if (prev_alloc && !next_alloc)
    {
        printf("######------- -------coalesce\n");
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        if (!GETADDR(NEXT_BLKP(bp))) //next block is head
        {
            blk_headp = bp;
        }
        PUTADDR(bp, GETADDR(NEXT_BLKP(bp)));
        PUTADDR(SUCC(bp), GETADDR(SUCC(NEXT_BLKP(bp))));
    }

    else if (!prev_alloc && next_alloc)
    {
        printf("-------- ------- #########coalesce\n");
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    else
    {
        printf("full coalesce\n");
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        unsigned long t1 = GETADDR(NEXT_BLKP(bp));
        unsigned long t2 = GETADDR(SUCC(NEXT_BLKP(bp)));
        if (t1 && t2)
        {
            PUTADDR(SUCC(t1), t2);
            PUTADDR(t2, t1);
        }
        else if (t1 && !t2)
        {
            PUTADDR(SUCC(t1), 0);
        }
        else if (!t1 && t2)
        {
            PUTADDR(t2, 0);
            blk_headp = t2;
        }
        else
        {
            printf("list head:%ld\n", (unsigned long)blk_headp - (unsigned long)heap_listp); //DEBUG

            printf("coalesce oops\n");

            printf("%ld\n", bp-(unsigned long)heap_listp);
            printf("%ld\n", NEXT_BLKP(bp)-(unsigned long)heap_listp);
        }

        bp = PREV_BLKP(bp);
    }

    return bp;
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
        copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}
