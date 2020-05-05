
#ifndef __CUCKOO
#define __CUCKOO

//#include "memcached_define.h"

void cuckoo_init();

item * cuckoo_find(uint64_t *key);

int cuckoo_insert(item *item);

void cuckoo_delete(uint64_t *key);

bool cuckoo_upsert(item * it);

#endif