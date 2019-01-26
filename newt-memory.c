/*
 * Copyright © 2018 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */


#include "newt.h"

#ifdef NEWT_DYNAMIC
uint8_t 	*newt_pool  __attribute__((aligned(NEWT_ALLOC_ROUND)));
uint32_t	newt_pool_size;
#else
uint8_t	newt_pool[NEWT_POOL + NEWT_POOL_EXTRA] __attribute__((aligned(NEWT_ALLOC_ROUND)));
#endif

struct newt_root {
	const newt_mem_t	*type;
	void			**addr;
};

#define NEWT_NUM_STASH	5
static newt_poly_t		stash_poly[NEWT_NUM_STASH];
static uint8_t			stash_poly_ptr;
static newt_code_t		*stash_code;

#ifndef NEWT_ROOT_DECLARE
#define NEWT_ROOT_DECLARE(n) n
#define NEWT_ROOT_TYPE(n) ((n)->type)
#define NEWT_ROOT_ADDR(n) ((n)->addr)
#endif

static const struct newt_root	NEWT_ROOT_DECLARE(newt_root)[] = {
	{
		.type = &newt_name_mem,
		.addr = (void **) (void *) &newt_names,
	},
	{
		.type = &newt_frame_mem,
		.addr = (void **) (void *) &newt_globals,
	},
	{
		.type = &newt_frame_mem,
		.addr = (void **) (void *) &newt_frame,
	},
	{
		.type = &newt_range_mem,
		.addr = (void **) (void *) &newt_ranges,
	},
	{
		.type = &newt_in_mem,
		.addr = (void **) (void *) &newt_ins,
	},
	{
		.type = &newt_code_mem,
		.addr = (void **) (void *) &stash_code,
	},
	{
		.type = &newt_compile_mem,
		.addr = (void **) (void *) &newt_compile,
	},
	{
		.type = NULL,
		.addr = (void **) (void *) &stash_poly[0]
	},
	{
		.type = NULL,
		.addr = (void **) (void *) &stash_poly[1]
	},
	{
		.type = NULL,
		.addr = (void **) (void *) &stash_poly[2]
	},
	{
		.type = NULL,
		.addr = (void **) (void *) &stash_poly[3]
	},
	{
		.type = NULL,
		.addr = (void **) (void *) &stash_poly[4]
	},
};

#define NEWT_ROOT	(sizeof (newt_root) / sizeof (newt_root[0]))

#define NEWT_BUSY_SIZE		((NEWT_POOL + 31) / 32)
#define NEWT_NCHUNK_EST(pool)	((pool) / 64)

struct newt_chunk {
	newt_offset_t		old_offset;
	union {
		newt_offset_t	size;
		newt_offset_t	new_offset;
	};
};

#ifdef NEWT_DYNAMIC
static uint8_t	*newt_busy;
static struct newt_chunk *newt_chunk;
static newt_offset_t	NEWT_NCHUNK;

typedef newt_soffset_t newt_chunk_t;

bool
newt_mem_alloc(newt_poly_t pool_size)
{
	newt_poly_t	busy_size = (pool_size + 31) / 32;

	newt_pool = malloc(pool_size + NEWT_POOL_EXTRA +
				busy_size +
				busy_size +
				busy_size +
				NEWT_NCHUNK_EST(pool_size) * sizeof (struct newt_chunk));
	if (!newt_pool)
		return false;
	newt_busy = newt_pool + pool_size + NEWT_POOL_EXTRA;
	newt_chunk = (struct newt_chunk *) (((uintptr_t)(newt_busy + busy_size) + 7) & ~7);
	newt_pool_size = pool_size;
	NEWT_NCHUNK = NEWT_NCHUNK_EST(pool_size);
	return true;
}

#else

#define NEWT_NCHUNK NEWT_NCHUNK_EST(NEWT_POOL)

static uint8_t			newt_busy[NEWT_BUSY_SIZE];
static struct newt_chunk	newt_chunk[NEWT_NCHUNK];

#if NEWT_NCHUNK < 127
typedef int8_t newt_chunk_t;
typedef uint8_t newt_uchunk_t;
#else
typedef newt_soffset_t newt_chunk_t;
typedef newt_offset_t newt_uchunk_t;
#endif

#endif

static newt_offset_t	newt_note_list;

static newt_offset_t	newt_top;

/* Offset of an address within the pool. */
static inline newt_offset_t pool_offset(void *addr) {
	return ((uint8_t *) addr) - newt_pool;
}

/* Address of an offset within the pool */
static inline void *pool_addr(newt_offset_t offset) {
	return newt_pool + offset;
}

static inline newt_offset_t tag_byte(newt_offset_t offset) {
	return offset >> (NEWT_ALLOC_SHIFT + 3);
}

static inline uint8_t tag_bit(newt_offset_t offset) {
	return (offset >> NEWT_ALLOC_SHIFT) & 7;
}

static inline void mark(newt_offset_t offset) {
	newt_busy[tag_byte(offset)] |= (1 << tag_bit(offset));
}

static inline void clear(newt_offset_t offset) {
	newt_busy[tag_byte(offset)] &= ~(1 << tag_bit(offset));
}

static inline bool busy(newt_offset_t offset) {
	return (newt_busy[tag_byte(offset)] >> tag_bit(offset)) & 1;
}

static inline int min(int a, int b) { return a < b ? a : b; }
static inline int max(int a, int b) { return a > b ? a : b; }

static inline bool
note_list(newt_list_t *list_old, newt_list_t *list_new)
{
	if (!newt_list_noted(list_new)) {
		debug_memory("\tnote list %d -> %d\n", pool_offset(list_old), pool_offset(list_new));
		newt_list_set_note_next(list_new, newt_note_list);
		newt_list_set_noted(list_new, true);
		newt_note_list = newt_pool_offset(list_old);
		return false;
	}
	debug_memory("\tnote list %d -> %d already noted (noted %d busy %d)\n",
		     pool_offset(list_old), pool_offset(list_new),
		     newt_list_noted(list_new), busy(pool_offset(list_old)));
	return true;
}

static newt_offset_t	chunk_low, chunk_high;
static newt_chunk_t	chunk_first, chunk_last;

static newt_chunk_t
find_chunk(newt_offset_t offset)
{
	newt_chunk_t l, r;

	/* Binary search for the location */
	l = chunk_first;
	r = chunk_last - 1;
	while (l <= r) {
		newt_chunk_t m = ((newt_uchunk_t) (l + r)) >> 1;
		if (newt_chunk[m].old_offset < offset)
			l = m + 1;
		else
			r = m - 1;
	}
	return l;
}

static void
note_chunk(newt_offset_t offset, newt_offset_t size)
{
	newt_chunk_t chunk;
	newt_offset_t end;

	if (offset < chunk_low || chunk_high <= offset)
		return;

	chunk = find_chunk(offset);

#if NEWT_DEBUG
	if (chunk >= NEWT_NCHUNK)
		newt_error("note chunk failed");
#endif

	/*
	 * The correct location is always in 'l', with r = l-1 being
	 * the entry before the right one
	 */

	/* Shuffle existing entries right */
	end = min(NEWT_NCHUNK, chunk_last + 1);

	memmove(&newt_chunk[chunk+1],
		&newt_chunk[chunk],
		(end - (chunk+1)) * sizeof (struct newt_chunk));

	/* Add new entry */
	newt_chunk[chunk].old_offset = offset;
	newt_chunk[chunk].size = size;

	/* Increment the number of elements up to the size of the array */
	if (chunk_last < NEWT_NCHUNK)
		chunk_last++;

	debug_memory("add chunk %d offset %d size %d chunk_last %d\n", chunk, offset, size, chunk_last);

	/* Set the top address if the array is full */
	if (chunk_last == NEWT_NCHUNK) {
		chunk_high = newt_chunk[NEWT_NCHUNK-1].old_offset +
			newt_chunk[NEWT_NCHUNK-1].size;
		debug_memory("reset chunk_high to %d\n", chunk_high);
	}
}

static void
reset_chunks(void)
{
	chunk_high = newt_top;
	chunk_last = 0;
	chunk_first = 0;
}

/*
 * Walk all referenced objects calling functions on each one
 */

static void
walk(bool (*visit_addr)(const struct newt_mem *type, void **addr),
     bool (*visit_poly)(newt_poly_t *p),
     void (*visit_run)(void))
{
	newt_offset_t i;

	memset(newt_busy, '\0', NEWT_BUSY_SIZE);
	newt_note_list = 0;
	visit_run();
	for (i = 0; i < (int) NEWT_ROOT; i++) {
		if (NEWT_ROOT_TYPE(&newt_root[i])) {
			void **a = NEWT_ROOT_ADDR(&newt_root[i]), *v;
			if (a == NULL || (v = *a) != NULL) {
				visit_addr(NEWT_ROOT_TYPE(&newt_root[i]), a);
			}
		} else {
			newt_poly_t *a = (newt_poly_t *) NEWT_ROOT_ADDR(&newt_root[i]), p;
			if (a && !newt_is_null(p = *a)) {
				visit_poly(a);
			}
		}
	}
	while (newt_note_list) {
		newt_offset_t note = newt_note_list;
		newt_note_list = 0;
		debug_memory("processing list %d\n", pool_offset(newt_pool_ref(note)));
		while (note) {
			newt_list_t *list = newt_pool_ref(note);
			debug_memory("\tprocess list %d\n", pool_offset(list));
			visit_addr(&newt_list_mem, (void **) &list);
			note = newt_list_note_next(list);
			debug_memory("\t\tprocessed, list is now %d next now %d\n",
				     pool_offset(list), note);
			newt_list_set_note_next(list, 0);
			newt_list_set_noted(list, false);
		}
		debug_memory("done procesing list\n");
	}
}


static const struct newt_mem * const newt_mems[4] = {
	[newt_list] = &newt_list_mem,
	[newt_string] = &newt_string_mem,
	[newt_func] = &newt_func_mem,
	[newt_builtin] = &newt_null_mem,
};

static bool
newt_mark_ref(const struct newt_mem *type, void **ref)
{
	return newt_mark_addr(type, *ref);
}

bool
newt_poly_mark_ref(newt_poly_t *p)
{
	return newt_poly_mark(*p);
}

newt_offset_t newt_last_top;
uint8_t newt_collect_counts;

#ifdef DEBUG_MEMORY
static void dump_busy(void)
{
	newt_offset_t i;

	for (i = 0; i < newt_top; i += 4)
		if (busy(i))
			debug_memory("\tBusy %d\n", i);
}
#else
#define dump_busy()
#endif

newt_offset_t
newt_collect(uint8_t style)
{
	newt_offset_t	i;
	newt_offset_t	top;

	debug_memory("Collect...\n");
	/* The first time through, we're doing a full collect */
	if (newt_last_top == 0)
		style = NEWT_COLLECT_FULL;

	/* One in a while, just do a full collect */

	if (newt_collect_counts >= 128)
		style = NEWT_COLLECT_FULL;

	if (style == NEWT_COLLECT_FULL)
		newt_collect_counts = 0;

	if (style == NEWT_COLLECT_FULL) {
		chunk_low = top = 0;
	} else {
		chunk_low = top = newt_last_top;
	}
	for (;;) {
		/* Find the sizes of the first chunk of objects to move */
		reset_chunks();
		debug_memory("mark\n");
		walk(newt_mark_ref, newt_poly_mark_ref, newt_run_mark);
		dump_busy();
		debug_memory("done\n");

		debug_memory("chunk_last %d\n", chunk_last);
		/* Find the first moving object */
		for (i = 0; i < chunk_last; i++) {
			newt_offset_t	size = newt_chunk[i].size;

			debug_memory("size %d old_offset %d top %d\n", size, newt_chunk[i].old_offset, top);
			if (newt_chunk[i].old_offset > top)
				break;

			top += size;
		}

		/* Short-circuit the rest of the loop when all of the
		 * found objects aren't moving. This isn't strictly
		 * necessary as the rest of the loop is structured to
		 * work in this case, but GCC 7.2.0 with optimization
		 * greater than 2 generates incorrect code for this...
		 */
		if (i == NEWT_NCHUNK) {
			chunk_low = chunk_high;
			continue;
		}

		/*
		 * Limit amount of chunk array used in mapping moves
		 * to the active region
		 */
		chunk_first = i;
		chunk_low = newt_chunk[i].old_offset;

		/* Copy all of the objects */
		for (; i < chunk_last; i++) {
			newt_offset_t	size = newt_chunk[i].size;

			debug_memory("  moving %d -> %d (%d)\n",
			       newt_chunk[i].old_offset, top, size);

			newt_chunk[i].new_offset = top;

			memmove(&newt_pool[top],
				&newt_pool[newt_chunk[i].old_offset],
				size);

			top += size;
		}

		if (chunk_first < chunk_last) {
			/* Relocate all references to the objects */
			debug_memory("move\n");
			walk(newt_move_addr, newt_poly_move, newt_run_move);
			debug_memory("done\n");
		}

		/* If we ran into the end of the heap, then
		 * there's no need to keep walking
		 */
		if (chunk_last != NEWT_NCHUNK)
			break;

		/* Next loop starts right above this loop */
		chunk_low = chunk_high;
	}

	newt_top = top;
	if (style == NEWT_COLLECT_FULL)
		newt_last_top = top;

	debug_memory("%d free\n", NEWT_POOL - newt_top);
	return NEWT_POOL - newt_top;
}

/*
 * Mark interfaces for objects
 */


/*
 * Note a reference to memory and collect information about a few
 * object sizes at a time
 */

bool
newt_mark_blob(void *addr, newt_offset_t size)
{
	newt_offset_t offset;

#if NEWT_DEBUG
	if (!newt_is_pool_addr(addr))
		return true;
#endif

	offset = pool_offset(addr);
	if (busy(offset))
		return true;
	debug_memory("\tmark %d size %d\n", offset, size);
	mark(offset);
	note_chunk(offset, size);
	return false;
}

bool
newt_mark_block_addr(const struct newt_mem *type, void *addr)
{
	bool ret;
	ret = newt_mark_blob(addr, newt_size(type, addr));
	if (!ret)
		debug_memory("\tmark %s %d %d\n", type->name, pool_offset(addr), newt_size(type, addr));
	return ret;
}

bool
newt_mark_block_offset(const struct newt_mem *type, newt_offset_t offset)
{
	return newt_mark_block_addr(type, newt_pool_ref(offset));
}

/*
 * Mark an object and all that it refereces
 */
bool
newt_mark_addr(const struct newt_mem *type, void *addr)
{
	bool ret;
	ret = newt_mark_block_addr(type, addr);
	if (!ret)
		NEWT_MEM_MARK(type)(addr);
	return ret;
}

bool
newt_mark_offset(const struct newt_mem *type, newt_offset_t offset)
{
	return newt_mark_addr(type, newt_pool_ref(offset));
}

/*
 * Mark an object, unless it is a list. In that case, just set a bit
 * in the list note array; those will be marked in a separate pass to
 * avoid deep recursion in the collector
 */

bool
newt_poly_mark(newt_poly_t p)
{
	newt_type_t	type;
	void		*addr;
	bool		ret;

	type = newt_poly_type(p);

	if (type == newt_float || type == newt_builtin)
		return true;

	addr = newt_ref(p);

	if (type == newt_list)
		debug_memory("\tmark list %d\n", pool_offset(addr));

#if NEWT_DEBUG
	if (!newt_is_pool_addr(addr))
		newt_panic("non-pool addr in heap");
#endif

	const struct newt_mem *mem;

	mem = newt_mems[type];
	ret = newt_mark_block_addr(mem, addr);
	if (!ret) {
		NEWT_MEM_MARK(mem)(addr);
		if (type == newt_list)
			note_list(addr, addr);
	}
	return ret;
}

/*
 * Find the current location of an object
 * based on the original location. For unmoved
 * objects, this is simple. For moved objects,
 * go search for it
 */

static newt_offset_t
move_map(newt_offset_t offset)
{
	newt_chunk_t	chunk;

	if (offset < chunk_low || chunk_high <= offset)
		return offset;

	chunk = find_chunk(offset);

#if DBG_MEM
	if (newt_chunk[chunk].old_offset != offset)
		abort();
#endif
	return newt_chunk[chunk].new_offset;
}

bool
newt_move_block_offset(newt_offset_t *ref)
{
	newt_offset_t	orig_offset = *ref - 1;
	newt_offset_t	offset;

	offset = move_map(orig_offset);
	if (offset != orig_offset) {
		debug_memory("\tmove %d -> %d\n", orig_offset, offset);
		*ref = offset + 1;
	}

	if (busy(offset))
		return true;

	mark(offset);
	return false;
}

bool
newt_move_block_addr(void **ref)
{
	void		*addr = *ref;
	newt_offset_t	offset, orig_offset;
	bool		ret;

#if NEWT_DEBUG
	if (!newt_is_pool_addr(addr))
		newt_panic("non-pool address");
#endif

	orig_offset = newt_pool_offset(addr);
	offset = orig_offset;
	ret = newt_move_block_offset(&offset);
	if (offset != orig_offset)
		*ref = newt_pool_ref(offset);

	return ret;
}

bool
newt_move_addr(const struct newt_mem *type, void **ref)
{
	bool ret;
	ret = newt_move_block_addr(ref);
	if (!ret)
		NEWT_MEM_MOVE(type)(*ref);

	return ret;
}

bool
newt_move_offset(const struct newt_mem *type, newt_offset_t *ref)
{
	bool ret;
	ret = newt_move_block_offset(ref);
	if (!ret)
		NEWT_MEM_MOVE(type)(newt_pool_ref(*ref));
	return ret;
}

bool
newt_poly_move(newt_poly_t *ref)
{
	newt_poly_t	p = *ref;
	bool		ret;
	void		*addr;
	newt_offset_t	offset, orig_offset;
	newt_type_t	type = newt_poly_type(p);

	if (type == newt_float || type == newt_builtin)
		return true;

	addr = newt_ref(p);

	if (type == newt_list)
		debug_memory("\tmove list %d\n", pool_offset(addr));

#if NEWT_DEBUG
	if (!newt_is_pool_addr(addr))
		newt_panic("non-pool address");
#endif

	orig_offset = pool_offset(addr);
	offset = move_map(orig_offset);

	const struct newt_mem *mem;

	mem = newt_mems[type];

	/* inline newt_move to save stack space */
	ret = newt_move_block_addr(&addr);
	if (!ret) {
		NEWT_MEM_MOVE(mem)(addr);
		if (type == newt_list)
			note_list(pool_addr(orig_offset), addr);
	}

	/* Re-write the poly value */
	if (offset != orig_offset) {
		newt_poly_t np = newt_poly(pool_addr(offset), newt_poly_type(p));
		*ref = np;
	}
	return ret;
}

void *
newt_alloc(newt_offset_t size)
{
	void	*addr;

	size = newt_size_round(size);
	if (NEWT_POOL - newt_top < size &&
	    newt_collect(NEWT_COLLECT_INCREMENTAL) < size &&
	    newt_collect(NEWT_COLLECT_FULL) < size)
	{
		newt_error("out of memory");
		return NULL;
	}
	addr = pool_addr(newt_top);
	memset(addr, '\0', size);
	debug_memory("Alloc %d size %d\n", newt_top, size);
	newt_top += size;
	return addr;
}

void
newt_poly_stash(newt_poly_t p)
{
	stash_poly[stash_poly_ptr++] = p;
}

newt_poly_t
newt_poly_fetch(void)
{
	newt_poly_t	p;

	p = stash_poly[--stash_poly_ptr];
	stash_poly[stash_poly_ptr] = NEWT_NULL;
	return p;
}

void
newt_string_stash(const char *s)
{
	newt_poly_stash(newt_string_to_poly((char *) s));
}

char *
newt_string_fetch(void)
{
	return newt_poly_to_string(newt_poly_fetch());
}

void
newt_code_stash(newt_code_t *code)
{
	stash_code = code;
}

newt_code_t *
newt_code_fetch(void)
{
	newt_code_t	*code;
	code = stash_code;
	stash_code = 0;
	return code;
}
