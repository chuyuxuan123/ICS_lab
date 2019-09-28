/**
 * name: Chu Yuxuan
 * ID: 517030910079
 */

/**
 * steps:
 * 1.read argus from command line argus, initialize vars and cache-lines
 * 2.read the trace file, parse per line, update the cache, record the usage of cache
 * 3.print the result
 * also need some variables and functions:
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>

#include "cachelab.h"

static int hits, misses, evicts;

/**
 * set_num - S
 * set_bits - s
 * lines_per_set - E
 * block_bits - b
 * block_size - B
 */
static int set_num, set_bits, lines_per_set, block_bits, block_size;
static char *tracefile;
static long long int *cache;


/** init_cache_line
 * read argus from command argus, and then initialize the cache simulator
 */
void init_cache_line(int argc, char *argv[])
{
    char opt;
    while ((opt = getopt(argc, argv, "s:E:b:t:")) != -1)
    {
        switch (opt)
        {
        case 's':
            set_bits = atoi(optarg);
            set_num = pow(2, set_bits);
            break;
        case 'E':
            lines_per_set = atoi(optarg);
            break;
        case 'b':
            block_bits = atoi(optarg);
            block_size = pow(2, block_bits);
            break;
        case 't':
            tracefile = optarg;
            break;
        default:
            printf("Wrong Args\n");
            break;
        }
    }
    hits = 0;
    misses = 0;
    evicts = 0;
    /**
     * cache 
     * use  memory to stall Tag , LRU counter and vaild bits
     * use 3 long long int space to represent a cache line
     */
    cache = (long long int *)calloc((int)block_size, 3 * lines_per_set * sizeof(long long int));
}

/**
 * cache_sim
 * read cache operation from tracefiles per line
 * simulate cache and record related data
 */
void cache_sim()
{
    FILE *file = fopen(tracefile, "r");
    if (!file)
    {
        printf("Trace file not exist");
        return;
    }
    char line[16];
    // int line_number = 1; //  DEBUG
    while (fgets(line, 16, file))
    {
        // printf("line %d :", line_number); //DEBUG
        if (line[0] == 'I')
        {
            continue;
        }

        long long int address;
        long long int index;
        long long int tag;

        char miss_flag = 1;
        char evict_flag = 1;

        long long int replace = 0;

        if (line[1] == 'L' || line[1] == 'S' || line[1] == 'M')
        {
            char str_addr[10];
            for (int i = 3; line[i] != ','; ++i)
            {
                str_addr[i - 3] = line[i];
                if(line[i+1]==',')
                {
                    str_addr[i-2] = '\0';
                }
            }
            address = strtoll(str_addr, NULL, 16);
            // printf("address:%llx",address); //DEBUG
            index = (address & ((1 << (block_bits + set_bits)) - (1 << block_bits))) >> block_bits;
            // printf("index:%llx",index); //DEBUG
            tag = address >> (block_bits + set_bits);
            // printf("tag:%llx",tag); //DEBUG
        }

        for (int it_set = 3 * lines_per_set * index; it_set < 3 * lines_per_set * (index + 1); it_set += 3)
        {
            if (tag == cache[it_set] && cache[it_set+2] !=0) //hit
            {
                for (int k = index * 3 * lines_per_set + 1; (cache[k] != 0 && k < (index + 1) * 3 * lines_per_set + 1); k += 3)
                {
                    cache[k] += 1;
                }
                cache[it_set + 1] = 1; //LRU counter
                hits++;
                miss_flag = 0;
                evict_flag = 0;
                // printf(" hit "); //DEBUG
                break;
            }
        }

        if (miss_flag)
        {
            misses++;
            for (int l = index * 3 * lines_per_set; l < (index + 1) * 3 * lines_per_set; l += 3)
            {
                if (0 == cache[l] && 0 == cache[l + 2]) // empty block
                {
                    cache[l] = tag;                                                 // put the cache line on
                    cache[l+2] = 1;
                    for (int m = index * 3 * lines_per_set + 1; m <= l + 1; m += 3) // change modify time
                    {
                        cache[m]++;
                    }
                    evict_flag = 0; //cold miss
                    // printf(" miss "); // DEBUG
                    break;
                }
                // record the "history" of replacement line in case eviction is needed
                if (replace < cache[l + 1])
                {
                    replace = cache[l + 1];
                }
            }
        }

        if (evict_flag)
        {
            evicts++;
            for (int n = index * 3 * lines_per_set + 1; n < (index + 1) * 3 * lines_per_set + 1; n += 3) // find out the victim block and replace it with the new block
            {
                if (cache[n] == replace)
                {
                    cache[n] = 0;
                    cache[n - 1] = tag;
                    break;
                }
            }
            for (int p = index * 3 * lines_per_set + 1; p < (index + 1) * 3 * lines_per_set + 1; p += 3) // change "history"
            {
                cache[p]++;
            }
            // printf(" evict"); //DEBUG
        }

        // modification have one more hit on store operation no matter its load operation hit or missed
        if ('M' == line[1])
        {
            // printf("modify\n");    //DEBUG
            hits++;
        }
        // line_number++; // DEBUG
        // printf("\n"); //DEBUG
    }
    free(cache);
    fclose(file);
    return;
}

int main(int argc, char *argv[])
{
    init_cache_line(argc, argv);
    cache_sim();
    printSummary(hits, misses, evicts);
    return 0;
}
