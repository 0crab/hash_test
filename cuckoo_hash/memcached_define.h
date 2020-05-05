#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>

#ifndef __MEMCACHED_DEFINE
#define __MEMCACHED_DEFINE
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long int uint64_t;
typedef unsigned int rel_time_t;

#define ITEM_CFLAGS 256
#define ITEM_CAS 2

#define ITEM_key(item) (((char*)&((item)->data)) \
         + (((item)->it_flags & ITEM_CAS) ? sizeof(uint64_t) : 0))

#define ITEM_data(item) ((char*) &((item)->data) + (item)->nkey + 1 \
         + (((item)->it_flags & ITEM_CFLAGS) ? sizeof(uint32_t) : 0) \
         + (((item)->it_flags & ITEM_CAS) ? sizeof(uint64_t) : 0))


#define refcount_incr(it) ++(it->refcount)
#define refcount_decr(it) --(it->refcount)

typedef struct _stritem {
    /* Protected by LRU locks */
    struct _stritem *next;
    struct _stritem *prev;
    /* Rest are protected by an item lock */
    struct _stritem *h_next;    /* hash chain next */
    rel_time_t time;       /* least recent access */
    rel_time_t exptime;    /* expire time */
    int nbytes;     /* size of data */
    unsigned short refcount;
    uint16_t it_flags;   /* ITEM_* above */
    uint8_t slabs_clsid;/* which slab class we're in */
    uint8_t nkey;       /* key length, w/terminating null and padding */
    /* this odd type prevents type-punning issues when we do
     * the little shuffle to save space when not using CAS. */
    union {
        uint64_t cas;
        char end;
    } data[];

    /* if it_flags & ITEM_CAS we have 8 bytes CAS */
    /* then null-terminated key */
    /* then " flags length\r\n" (no terminating null) */
    /* then data with terminating \r\n (no terminating null; it's binary!) */
} item;

static item *item_alloc(char *key, const size_t nkey, const unsigned int flags,
                    const rel_time_t exptime, const int nbytes) {
    item *it;
    it = (item *) malloc(80+nkey+nbytes);
    memset(it, 0, 80+nkey+nbytes);
    it->nbytes = nbytes;
    memcpy(ITEM_key(it), key, nkey);
    it->nkey = nkey;
    it->exptime = exptime;
    it->it_flags = flags;
    refcount_incr(it);
    return it;
}

#endif