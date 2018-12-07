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

#define NEWT_NUM_STASH	6
static newt_poly_t		stash_poly[NEWT_NUM_STASH];
static int			stash_poly_ptr;
static newt_name_t		*stash_name;
static newt_code_t		*stash_code;

static const struct newt_root	newt_root[] = {
	{
		.type = &newt_stack_mem,
		.addr = NULL,
	},
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
		.type = &newt_name_mem,
		.addr = (void **) (void *) &stash_name,
	},
	{
		.type = &newt_code_mem,
		.addr = (void **) (void *) &stash_code,
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

static int	newt_printing, newt_print_cleared;

struct newt_chunk {
	newt_offset_t		old_offset;
	union {
		newt_offset_t	size;
		newt_offset_t	new_offset;
	};
};

#ifdef NEWT_DYNAMIC
static uint8_t	*newt_busy;
static uint8_t	*newt_list_note;
static uint8_t	*newt_list_last;
static struct newt_chunk *newt_chunk;
static newt_poly_t NEWT_NCHUNK;

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
	newt_list_note = newt_busy + busy_size;
	newt_list_last = newt_list_note + busy_size;
	newt_chunk = (struct newt_chunk *) (((uintptr_t)(newt_list_last + busy_size) + 7) & ~7);
	newt_pool_size = pool_size;
	NEWT_NCHUNK = NEWT_NCHUNK_EST(pool_size);
	return true;
}

#else

#define NEWT_NCHUNK NEWT_NCHUNK_EST(NEWT_POOL)

static uint8_t	newt_busy[NEWT_BUSY_SIZE];
static uint8_t	newt_list_note[NEWT_BUSY_SIZE];
static uint8_t	newt_list_last[NEWT_BUSY_SIZE];
static struct newt_chunk newt_chunk[NEWT_NCHUNK];

#endif

static uint8_t	newt_list_noted;

newt_offset_t		newt_top;

/* Offset of an address within the pool. */
static inline newt_offset_t pool_offset(void *addr) {
	return ((uint8_t *) addr) - newt_pool;
}

static inline int tag_byte(int offset) {
	return offset >> (NEWT_ALLOC_SHIFT + 3);
}

static inline int tag_bit(int offset) {
	return (offset >> NEWT_ALLOC_SHIFT) & 7;
}

static inline void mark(uint8_t *tag, int offset) {
	tag[tag_byte(offset)] |= (1 << tag_bit(offset));
}

static inline void clear(uint8_t *tag, int offset) {
	tag[tag_byte(offset)] &= ~(1 << tag_bit(offset));
}

static inline int busy(uint8_t *tag, int offset) {
	return (tag[tag_byte(offset)] >> tag_bit(offset)) & 1;
}

static inline int min(int a, int b) { return a < b ? a : b; }
static inline int max(int a, int b) { return a > b ? a : b; }

static inline int limit(int offset) {
	return min(NEWT_POOL, max(offset, 0));
}

static inline void
note_list(newt_offset_t offset)
{
	newt_list_noted = 1;
	mark(newt_list_note, offset);
}

static newt_offset_t	chunk_low, chunk_high;
static newt_offset_t	chunk_first, chunk_last;

static int
find_chunk(newt_offset_t offset)
{
	int l, r;
	/* Binary search for the location */
	l = chunk_first;
	r = chunk_last - 1;
	while (l <= r) {
		int m = (l + r) >> 1;
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
	newt_offset_t l;
	newt_offset_t end;

	if (offset < chunk_low || chunk_high <= offset)
		return;

	l = find_chunk(offset);

	/*
	 * The correct location is always in 'l', with r = l-1 being
	 * the entry before the right one
	 */

	/* Shuffle existing entries right */
	end = min(NEWT_NCHUNK, chunk_last + 1);

	memmove(&newt_chunk[l+1],
		&newt_chunk[l],
		(end - (l+1)) * sizeof (struct newt_chunk));

	/* Add new entry */
	newt_chunk[l].old_offset = offset;
	newt_chunk[l].size = size;

	/* Increment the number of elements up to the size of the array */
	if (chunk_last < NEWT_NCHUNK)
		chunk_last++;

	/* Set the top address if the array is full */
	if (chunk_last == NEWT_NCHUNK)
		chunk_high = newt_chunk[NEWT_NCHUNK-1].old_offset +
			newt_chunk[NEWT_NCHUNK-1].size;
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
walk(int (*visit_addr)(const struct newt_mem *type, void **addr),
     int (*visit_poly)(newt_poly_t *p, uint8_t do_note_list))
{
	newt_offset_t i;

	memset(newt_busy, '\0', NEWT_BUSY_SIZE);
	memset(newt_list_note, '\0', NEWT_BUSY_SIZE);
	newt_list_noted = 0;
	for (i = 0; i < (int) NEWT_ROOT; i++) {
		if (newt_root[i].type) {
			void **a = newt_root[i].addr, *v;
			if (a && (v = *a)) {
				visit_addr(newt_root[i].type, a);
			}
		} else {
			newt_poly_t *a = (newt_poly_t *) newt_root[i].addr, p;
			if (a && !newt_is_null(p = *a)) {
				visit_poly(a, 0);
			}
		}
	}
	while (newt_list_noted) {
		memcpy(newt_list_last, newt_list_note, NEWT_BUSY_SIZE);
		memset(newt_list_note, '\0', NEWT_BUSY_SIZE);
		newt_list_noted = 0;
		for (i = 0; i < NEWT_POOL; i += NEWT_ALLOC_ROUND) {
			if (busy(newt_list_last, i)) {
				void *v = newt_pool + i;
				visit_addr(&newt_list_mem, &v);
			}
		}
	}
}


static const struct newt_mem * const newt_mems[4] = {
	[newt_list] = &newt_list_mem,
	[newt_string] = &newt_string_mem,
	[newt_func] = &newt_func_mem,
};

static int
newt_mark_ref(const struct newt_mem *type, void **ref)
{
	return newt_mark(type, *ref);
}

static int
newt_poly_mark_ref(newt_poly_t *p, uint8_t do_note_list)
{
	return newt_poly_mark(*p, do_note_list);
}

int newt_last_top;
int newt_collect_counts;

newt_offset_t
newt_collect(uint8_t style)
{
	newt_offset_t	i;
	newt_offset_t	top;

	/* The first time through, we're doing a full collect */
	if (newt_last_top == 0)
		style = NEWT_COLLECT_FULL;

	/* One in a while, just do a full collect */

	if (newt_collect_counts >= 128)
		style = NEWT_COLLECT_FULL;

	if (style == NEWT_COLLECT_FULL)
		newt_collect_counts = 0;

#if 0
	/* Clear references to all caches */
	for (i = 0; i < NEWT_CACHE; i++)
		*newt_cache[i] = NULL;
#endif
	if (style == NEWT_COLLECT_FULL) {
		chunk_low = top = 0;
	} else {
		chunk_low = top = newt_last_top;
	}
	for (;;) {
		/* Find the sizes of the first chunk of objects to move */
		reset_chunks();
		walk(newt_mark_ref, newt_poly_mark_ref);

		/* Find the first moving object */
		for (i = 0; i < chunk_last; i++) {
			newt_offset_t	size = newt_chunk[i].size;

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

			newt_chunk[i].new_offset = top;

			memmove(&newt_pool[top],
				&newt_pool[newt_chunk[i].old_offset],
				size);

			top += size;
		}

		if (chunk_first < chunk_last) {
			/* Relocate all references to the objects */
			walk(newt_move, newt_poly_move);
//			newt_atom_move();
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

	return NEWT_POOL - newt_top;
}

/*
 * Mark interfaces for objects
 */


/*
 * Note a reference to memory and collect information about a few
 * object sizes at a time
 */

int
newt_mark_memory(const struct newt_mem *type, void *addr)
{
	newt_offset_t offset;

	if (!newt_is_pool_addr(addr))
		return 1;

	offset = pool_offset(addr);
	if (busy(newt_busy, offset))
		return 1;
	mark(newt_busy, offset);
	note_chunk(offset, newt_size(type, addr));
	return 0;
}

/*
 * Mark an object and all that it refereces
 */
int
newt_mark(const struct newt_mem *type, void *addr)
{
	int ret;
	ret = newt_mark_memory(type, addr);
	if (!ret)
		type->mark(addr);
	return ret;
}

/*
 * Mark an object, unless it is a cons cell and
 * do_note_list is set. In that case, just
 * set a bit in the cons note array; those
 * will be marked in a separate pass to avoid
 * deep recursion in the collector
 */
int
newt_poly_mark(newt_poly_t p, uint8_t do_note_list)
{
	newt_type_t	type;
	void		*addr;
	int		ret;

	type = newt_poly_type(p);

	if (type == newt_float)
		return 1;

	addr = newt_ref(p);
	if (!newt_is_pool_addr(addr))
		return 1;

	if (type == newt_list && do_note_list) {
		note_list(pool_offset(addr));
		return 1;
	} else {
		const struct newt_mem *mem;

		mem = newt_mems[type];
		ret = newt_mark_memory(mem, addr);
		if (!ret)
			mem->mark(addr);

		return ret;
	}
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
	int		l;

	if (offset < chunk_low || chunk_high <= offset)
		return offset;

	l = find_chunk(offset);

#if DBG_MEM
	if (newt_chunk[l].old_offset != offset)
		newt_abort();
#endif
	return newt_chunk[l].new_offset;
}

int
newt_move_offset(const struct newt_mem *type, newt_offset_t *ref)
{
	newt_offset_t	orig_offset = *ref;
	newt_offset_t	offset;

	(void) type;

	offset = move_map(orig_offset);
	if (offset != orig_offset)
		*ref = offset;

	if (busy(newt_busy, offset))
		return 1;

	mark(newt_busy, offset);
	return 0;
}

int
newt_move_memory(const struct newt_mem *type, void **ref)
{
	void		*addr = *ref;
	newt_offset_t	offset, orig_offset;
	int		ret;

	if (!newt_is_pool_addr(addr))
		return 1;

	(void) type;

	orig_offset = pool_offset(addr);
	offset = orig_offset;
	ret = newt_move_offset(type, &offset);
	if (offset != orig_offset)
		*ref = newt_pool + offset;

	return ret;
}

int
newt_move(const struct newt_mem *type, void **ref)
{
	int ret;
	ret = newt_move_memory(type, ref);
	if (!ret)
		type->move(*ref);

	return ret;
}

int
newt_poly_move(newt_poly_t *ref, uint8_t do_note_list)
{
	newt_poly_t	p = *ref;
	int		ret;
	void		*addr;
	newt_offset_t	offset, orig_offset;

	if (newt_is_float(p))
		return 1;

	addr = newt_ref(p);
	if (!newt_is_pool_addr(addr))
		return 1;

	orig_offset = pool_offset(addr);
	offset = move_map(orig_offset);

	if (newt_poly_type(p) == newt_list && do_note_list) {
		note_list(orig_offset);
		ret = 1;
	} else {
		newt_type_t type = newt_poly_type(p);
		const struct newt_mem *mem;

		mem = newt_mems[type];

		/* inline newt_move to save stack space */
		ret = newt_move_memory(mem, &addr);
		if (!ret)
			mem->move(addr);
	}

	/* Re-write the poly value */
	if (offset != orig_offset) {
		newt_poly_t np = newt_poly(newt_pool + offset, newt_poly_type(p));
		*ref = np;
	}
	return ret;
}

int
newt_marked(void *addr)
{
	if (!newt_is_pool_addr(addr))
		return 1;
	return busy(newt_busy, pool_offset(addr));
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
//		newt_error(NEWT_OOM, "out of memory");
		return NULL;
	}
	addr = newt_pool + newt_top;
	newt_top += size;
	return addr;
}

void
newt_poly_stash(newt_poly_t p)
{
	assert(stash_poly_ptr < NEWT_NUM_STASH);
	stash_poly[stash_poly_ptr++] = p;
}

newt_poly_t
newt_poly_fetch(void)
{
	newt_poly_t	p;

	assert (stash_poly_ptr > 0);
	p = stash_poly[--stash_poly_ptr];
	stash_poly[stash_poly_ptr] = NEWT_NULL;
	return p;
}

void
newt_name_stash(newt_name_t *name)
{
	stash_name = name;
}

newt_name_t *
newt_name_fetch(void)
{
	newt_name_t	*name;
	name = stash_name;
	stash_name = 0;
	return name;
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

int
newt_print_mark_addr(void *addr)
{
	int	offset;

#if DBG_MEM
	if (newt_collecting)
		newt_abort();
#endif

	if (!newt_is_pool_addr(addr))
		return 0;

	if (!newt_print_cleared) {
		newt_print_cleared = 1;
		memset(newt_busy, '\0', NEWT_BUSY_SIZE);
	}
	offset = pool_offset(addr);
	if (busy(newt_busy, offset))
		return 1;
	mark(newt_busy, offset);
	return 0;
}

void
newt_print_clear_addr(void *addr)
{
	int	offset;

	if (!newt_is_pool_addr(addr))
		return;

	if (!newt_print_cleared)
		return;
	offset = pool_offset(addr);
	clear(newt_busy, offset);
}

/* Notes that printing has started */
void
newt_print_start(void)
{
	newt_printing++;
}

/* Notes that printing has ended. Returns 1 if printing is still going on */
int
newt_print_stop(void)
{
	newt_printing--;
	if (newt_printing != 0)
		return 1;
	newt_print_cleared = 0;
	return 0;
}
