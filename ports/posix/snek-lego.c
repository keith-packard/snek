/*
 * Copyright © 2025 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "snek.h"
#include "snek-70909.h"
#include <errno.h>
#include <dirent.h>
#include <ctype.h>

static uint16_t	current_output;

static dacta_t	**dactas;
static size_t	num_dactas;

static int
snek_lego_init(const char *lego)
{
	size_t    i;
	dacta_t	**new_dactas;
	dacta_t  *dacta;
	char	 name[16];

	for (i = 0; i < num_dactas; i++)
		if (!strcmp(lego, dactas[i]->name))
			return 1;

	new_dactas = malloc ((num_dactas + 1) * sizeof (dacta_t *));
	if (!new_dactas)
		return 0;

	dacta = calloc(1, sizeof(dacta_t) + strlen(lego) + 1);
	if (!dacta) {
		free(new_dactas);
		return 0;
	}
	strcpy(dacta->name, lego);

	current_output = 0;

	if (lego_start(dacta, dacta->name) != LEGO_OK) {
		printf("%s: %s\n", dacta->name, strerror(errno));
		free(dacta);
		free(new_dactas);
		return 0;
	}
	printf("%s: OUTPUTS", dacta->name);

	for (i = 0; i < LEGO_OUTPUTS; i++) {
		dacta->outputs[i].power = 8;
		dacta->outputs[i].left = 0;
		dacta->outputs[i].on = 0;
		dacta->active[i].power = 0xff;
		dacta->active[i].left = 0xff;
		dacta->active[i].on = 0xff;

		if (num_dactas == 0)
			sprintf(name, "%c", 'A' + (int) i);
		else
			sprintf(name, "%c%d", 'A' + (int) i, (int) num_dactas);

		printf(" %s", name);

		snek_id_t id;
		bool keyword;

		id = snek_name_id(name, &keyword);
		if (id != SNEK_ID_NONE) {
			snek_poly_t *ref = snek_global_ref(id, true);
			if (ref)
				*ref = snek_soffset_to_poly(num_dactas * LEGO_OUTPUTS + i + 1);
		}
	}
	printf("\n");

	printf("%s: INPUTS", dacta->name);
	for (i = 0; i < LEGO_SENSORS; i++)
		printf(" %zd", num_dactas * LEGO_SENSORS + i + 1);
	printf("\n");

	memcpy(new_dactas, dactas, num_dactas * sizeof (dacta_t *));
	new_dactas[num_dactas++] = dacta;
	free(dactas);
	dactas = new_dactas;

	return 1;
}

static inline lego_sensor_t *
sensor(uint16_t pin)
{
	return &(dactas[pin / LEGO_SENSORS]->sensors[pin % LEGO_SENSORS]);
}

static inline lego_output_t *
output(uint16_t pin)
{
	return &(dactas[pin / LEGO_OUTPUTS]->outputs[pin % LEGO_OUTPUTS]);
}

static inline lego_output_t *
active(uint16_t pin)
{
	return &(dactas[pin / LEGO_OUTPUTS]->active[pin % LEGO_OUTPUTS]);
}

static inline bool
is_on(uint16_t pin)
{
	return output(pin)->on;
}

static inline bool
is_left(uint16_t pin)
{
	return output(pin)->left;
}

static inline void
set_left(uint16_t pin)
{
	output(pin)->left = 1;
}

static inline void
set_right(uint16_t pin)
{
	output(pin)->left = 0;
}

static inline void
set_on(uint16_t pin)
{
	output(pin)->on = 1;
}

static inline void
set_off(uint16_t pin)
{
	output(pin)->on = 0;
}

static inline void
set_power(uint16_t pin, uint8_t pow)
{
	output(pin)->power = pow;
}

static inline bool
output_is_on(lego_output_t *o)
{
	return (o->on == 1) && o->power;
}

static snek_poly_t
set_out(uint16_t pin)
{
	dacta_t		*dacta = dactas[pin / LEGO_OUTPUTS];

	pin = pin % LEGO_OUTPUTS;

	lego_output_t	*o = &dacta->outputs[pin];
	lego_output_t	*a = &dacta->active[pin];

	if (output_is_on(a) && !output_is_on(o))
		lego_port_off(dacta, pin);
	if (o->left != a->left) {
		if (o->left)
			lego_set_left(dacta, pin);
		else
			lego_set_right(dacta, pin);
	}
	if (o->power != a->power && o->power != 0)
		lego_set_powers(dacta, o->power - 1, 1 << pin);
	if (!output_is_on(a) && output_is_on(o))
		lego_port_on(dacta, pin);
	a->on = o->on;
	a->left = o->left;
	a->power = o->power;
	return SNEK_NULL;
}

static uint16_t
snek_poly_get_output(snek_poly_t a)
{
	snek_offset_t p = snek_poly_get_soffset(a) - 1;
	if (num_dactas * LEGO_OUTPUTS <= p)
		snek_error_value(a);
	return p;
}

static uint16_t
snek_poly_get_input(snek_poly_t a)
{
	snek_offset_t p = snek_poly_get_soffset(a) - 1;
	if (num_dactas * LEGO_SENSORS <= p)
		snek_error_value(a);
	return p;
}

snek_poly_t
snek_builtin_talkto(snek_poly_t a)
{
	uint16_t	p = snek_poly_get_output(a);
	if (!snek_abort)
		current_output = p;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_setpower(snek_poly_t a)
{
	float p = snek_poly_get_float(a);
	if (p < 0.0f) p = 0.0f;
	if (p > 1.0f) p = 1.0f;
	set_power(current_output, (uint8_t) roundf(p * 8.0f));
	return set_out(current_output);
}

snek_poly_t
snek_builtin_setleft(void)
{
	if (!dactas)
		return SNEK_NULL;
	set_left(current_output);
	return set_out(current_output);
}

snek_poly_t
snek_builtin_setright(void)
{
	if (!dactas)
		return SNEK_NULL;
	set_right(current_output);
	return set_out(current_output);
}

snek_poly_t
snek_builtin_on(void)
{
	if (!dactas)
		return SNEK_NULL;
	set_on(current_output);
	return set_out(current_output);
}

snek_poly_t
snek_builtin_off(void)
{
	if (!dactas)
		return SNEK_NULL;
	set_off(current_output);
	return set_out(current_output);
}

snek_poly_t
snek_builtin_onfor(snek_poly_t a)
{
	if (!dactas)
		return SNEK_NULL;
	snek_builtin_on();
	snek_builtin_time_sleep(a);
	return snek_builtin_off();
}

snek_poly_t
snek_builtin_pullnone(snek_poly_t a)
{
	(void) a;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_pullup(snek_poly_t a)
{
	(void) a;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_read(snek_poly_t a)
{
	uint16_t p = snek_poly_get_input(a);
	if (snek_abort)
		return SNEK_NULL;

	dacta_t *dacta = dactas[p / LEGO_SENSORS];
	p = p % LEGO_SENSORS;
	lego_sensor_t	value = lego_sensor(dacta, p);
	return snek_float_to_poly((float) (1023 - value.value) / 1023.0f);
}

snek_poly_t
snek_builtin_stopall(void)
{
	uint16_t	p;
	for (p = 0; p < num_dactas * LEGO_OUTPUTS; p++) {
		set_off(p);
		set_out(p);
	}
	return SNEK_NULL;
}

#define USB_DEVICES	"/sys/bus/usb/devices"

static char *
fullname(const char *dir, const char *file)
{
	char *fullname = NULL;
	if (asprintf(&fullname, "%s/%s", dir, file) < 0)
		return NULL;
	return fullname;
}

static char *
load_string(const char *dir, const char *file)
{
	char	*full = fullname(dir, file);
	char	line[4096];
	char	*r;
	FILE	*f;
	int	rlen;

	f = fopen(full, "r");
	free(full);
	if (!f)
		return NULL;
	r = fgets(line, sizeof (line), f);
	fclose(f);
	if (!r)
		return NULL;
	rlen = strlen(r);
	if (r[rlen-1] == '\n')
		r[rlen-1] = '\0';
	return strdup(r);
}

static int
load_hex(const char *dir, const char *file)
{
	char	*line;
	char	*end;
	long	i;

	line = load_string(dir, file);
	if (!line)
		return -1;
	i = strtol(line, &end, 16);
	free(line);
	if (end == line)
		return -1;
	return i;
}

static int
dir_filter_dev(const struct dirent *d)
{
	const char	*n = d->d_name;
	char	c;

	while ((c = *n++)) {
		if (isdigit(c))
			continue;
		if (c == '-')
			continue;
		if (c == '.' && n != d->d_name + 1)
			continue;
		return 0;
	}
	return 1;
}

static int
dir_filter_tty_colon(const struct dirent *d)
{
	return strncmp(d->d_name, "tty:", 4) == 0;
}

static int
dir_filter_tty(const struct dirent *d)
{
	return strncmp(d->d_name, "tty", 3) == 0;
}

static char *
usb_tty(const char *sys)
{
	char *base;
	int num_configs;
	int config;
	struct dirent **namelist;
	int interface;
	int num_interfaces;
	char endpoint_base[32];
	char *endpoint_full;
	char *tty_dir;
	int ntty;
	char *tty;

	base = basename(sys);
	num_configs = load_hex(sys, "bNumConfigurations");
	num_interfaces = load_hex(sys, "bNumInterfaces");
	for (config = 1; config <= num_configs; config++) {
		for (interface = 0; interface < num_interfaces; interface++) {
			sprintf(endpoint_base, "%s:%d.%d",
				base, config, interface);
			endpoint_full = fullname(sys, endpoint_base);


			/* Check for tty:ttyACMx style names
			 */
			ntty = scandir(endpoint_full, &namelist,
				       dir_filter_tty_colon,
				       alphasort);
			if (ntty > 0) {
				free(endpoint_full);
				tty = fullname("/dev", namelist[0]->d_name + 4);
				free(namelist);
				return tty;
			}

			/* Check for tty/ttyACMx style names
			 */
			tty_dir = fullname(endpoint_full, "tty");
			ntty = scandir(tty_dir, &namelist,
				       dir_filter_tty,
				       alphasort);
			free (tty_dir);
			if (ntty > 0) {
				tty = fullname("/dev", namelist[0]->d_name);
				free(endpoint_full);
				free(namelist);
				return tty;
			}

			/* Check for ttyACMx style names
			 */
			ntty = scandir(endpoint_full, &namelist,
				       dir_filter_tty,
				       alphasort);
			free(endpoint_full);
			if (ntty > 0) {
				tty = fullname("/dev", namelist[0]->d_name);
				free(namelist);
				return tty;
			}

		}
	}
	return NULL;
}

static char *
snek_scan_device(const char *pathname)
{
	char *tty;

	tty= usb_tty(pathname);
	if (!tty)
		return NULL;
	return tty;
}

static bool
snek_lego_list(void (*func)(const char *devname, void *closure), void *closure)
{
	struct dirent **ents;
	int             e, n;

	n = scandir (USB_DEVICES, &ents,
		     dir_filter_dev,
		     alphasort);
	if (!n)
		return false;

	for (e = 0; e < n; e++) {
		char	*pathname = NULL;
		char	*devname;
		if (asprintf(&pathname, "%s/%s", USB_DEVICES, ents[e]->d_name) < 0)
			continue;

		devname = snek_scan_device(pathname);
		free(pathname);
		if (devname) {
			(*func)(devname, closure);
			free(devname);
		}
	}
	free(ents);
	return true;
}

static void
snek_list_collect(const char *devname, void *closure)
{
	snek_offset_t *sizep = closure;
	if (lego_detect(devname)) {
		snek_poly_t devstr = snek_string_build(devname);
		if (!snek_is_null(devstr)) {
			snek_stack_push(devstr);
			++(*sizep);
		}
	}
}

snek_poly_t
snek_builtin_lego_list(void)
{
	snek_offset_t size = 0;
	snek_lego_list(snek_list_collect, &size);
	return snek_list_imm(size, snek_list_tuple);
}

static snek_poly_t
snek_lego_open(snek_poly_t a)
{
	switch (snek_poly_type(a)) {
	case snek_string:
		const char *name = snek_poly_get_string(a);
		if (name) {
			if (snek_lego_init(name))
				return SNEK_NULL;
		}
		break;
	case snek_list:
#ifndef SNEK_NO_DICT
		snek_list_t *l = snek_poly_to_list(a);
		if (snek_list_type(l) == snek_list_dict)
			break;
#endif
		snek_offset_t o;
		snek_poly_t *data = snek_list_data(l);
		snek_poly_t ret = SNEK_NULL;
		for (o = 0; !snek_abort && o < l->size; o++)
			ret = snek_lego_open(data[o]);
		return ret;
	default:
		break;
	}
	snek_error_value(a);
	return SNEK_NULL;
}

static void
snek_open_collect(const char *devname, void *closure)
{
	snek_poly_t *ret = closure;
	if (snek_lego_init(devname))
		*ret = SNEK_ONE;
}

snek_poly_t
snek_builtin_lego_open(uint8_t nposition, uint8_t nnamed, snek_poly_t *args)
{
	snek_poly_t ret = SNEK_ZERO;
	(void) nnamed;
	if (nposition == 0) {
		snek_lego_list(snek_open_collect, &ret);
		if (!snek_poly_true(ret))
			snek_error_0("no lego devices found");
	} else {
		for (uint8_t i = 0; !snek_abort && i < nposition; i++)
			ret = snek_lego_open(args[i]);
	}
	return ret;
}

static bool
snek_lego_close(const char *name)
{
	size_t	d;

	for (d = 0; d < num_dactas; d++) {
		if (strcmp(name, dactas[d]->name) == 0) {
			lego_stop(dactas[d]);
			free(dactas[d]);
			num_dactas--;
			memmove(&dactas[d], &dactas[d+1], (num_dactas - d) * sizeof(dacta_t *));
			return true;
		}
	}
	return false;
}

snek_poly_t
snek_builtin_lego_close(uint8_t nposition, uint8_t nnamed, snek_poly_t *args)
{
	snek_poly_t ret = SNEK_ZERO;
	if (nposition == 0) {
		while (num_dactas)
			snek_lego_close(dactas[0]->name);
	} else {
		for (uint8_t i = 0; i < nnamed; i++) {
			const char *name = snek_poly_get_string(args[i]);
			if (name) {
				if (snek_lego_close(name))
					ret = SNEK_ONE;
			}
		}
	}
	return ret;
}
