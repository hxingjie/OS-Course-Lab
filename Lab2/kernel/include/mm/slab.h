/*
 * Copyright (c) 2023 Institute of Parallel And Distributed Systems (IPADS), Shanghai Jiao Tong University (SJTU)
 * Licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *     http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#ifndef MM_SLAB_H
#define MM_SLAB_H

#include <common/list.h>

/*
 * order range: [SLAB_MIN_ORDER, SLAB_MAX_ORDER]
 * ChCore prepares the slab for each order in the range.
 */
#define SLAB_MIN_ORDER (5)
#define SLAB_MAX_ORDER (11)
// 表示SLab分配器可以操作的内存块大小，从2^5到2^11字节

/* The size of one slab is 128K. */
#define SIZE_OF_ONE_SLAB (128*1024)
// 表示每个slab的大小，在Chcore中是128KB

/* slab_header resides in the beginning of each slab (i.e., occupies the first slot). */
struct slab_header {
        /* The list of free slots, which can be converted to struct slab_slot_list. */
        void *free_list_head; // 内部空闲slot的链表
        /* Partial slab list. */
        struct list_head node; // partial中表示自身的节点

        int order; // cur slab's size fo mem, slot's sz == 2^order
        // 总的空闲块数 & 当前空闲块数
        unsigned short total_free_cnt; /* MAX: 65536 */ // 2^17 / 2^5 == 2^12 2^16
        unsigned short current_free_cnt;
};

/* Each free slot in one slab is regarded as slab_slot_list. */
struct slab_slot_list {
        void *next_free;
};

// 即Chcore中表示slab池的数据结构，这个也可以在下面对 slab_pool 的定义中看见。
// 其数据成员即我们之前介绍的current和partial指针，在这里以 slab_header 和 list_head 的形式出现
struct slab_pointer {
        struct slab_header *current_slab;
        struct list_head partial_slab_list;
};

/* All interfaces are kernel/mm module internal interfaces. */
void init_slab(void);
void *alloc_in_slab(unsigned long, size_t *);
void free_in_slab(void *addr);
unsigned long get_free_mem_size_from_slab(void);

#endif /* MM_SLAB_H */
