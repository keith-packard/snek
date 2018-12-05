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

typedef struct newt_name {
    struct newt_name *next;
    char name[0];
} newt_name_t;

static newt_name_t *names;

char *
newt_name_find(char *name)
{
    newt_name_t *n;
    for (n = names; n; n = n->next)
        if (!strcmp(n->name, name))
            return n->name;
    n = malloc(sizeof (newt_name_t) + strlen(name) + 1);
    strcpy(n->name, name);
    n->next = names;
    names = n;
    return n->name;
}
