/*
 * Copyright © 2019 Keith Packard <keithp@keithp.com>
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
#include "ao.h"
#include "ao_pwmin.h"

volatile __xdata struct ao_data	ao_data_ring[AO_DATA_RING];
volatile __data uint8_t		ao_data_head;

#ifdef TELESCIENCE
const uint8_t	adc_channels[AO_LOG_TELESCIENCE_NUM_ADC] = {
	0x00,
	0x01,
	0x04,
	0x05,
	0x06,
	0x07,
	0x20,
	0x21,
	0x22,
	0x23,
	0x24,
	0x25,
};
#endif

#ifdef TELEPYRO
const uint8_t	adc_channels[AO_TELEPYRO_NUM_ADC] = {
	0x00,	/* ADC0  v_batt */
	0x04,	/* ADC4  sense_a */
	0x05,	/* ADC5  sense_b */
	0x06,	/* ADC6  sense_c */
	0x07,	/* ADC7  sense_d */
	0x23,	/* ADC11 sense_e */
	0x22,	/* ADC10 sense_f */
	0x21,	/* ADC9 sense_g */
	0x20,	/* ADC8 sense_h */
};
#endif

#define NUM_ADC	(sizeof (adc_channels) / sizeof (adc_channels[0]))

static uint8_t	ao_adc_channel;

#define ADC_CHANNEL_LOW(c)	(((c) & 0x1f) << MUX0)
#define ADC_CHANNEL_HIGH(c)	((((c) & 0x20) >> 5) << MUX5)

#define ADCSRA_INIT	((1 << ADEN) |		/* Enable ADC */ 		\
			 (0 << ADATE) |		/* No auto ADC trigger */ 	\
			 (1 << ADIF) |		/* Clear interrupt */		\
			 (0 << ADIE) |		/* Enable interrupt */		\
			 (6 << ADPS0))		/* Prescale clock by 64 */

#define ADCSRB_INIT	((0 << ADHSM) |		/* No high-speed mode */ \
			 (0 << ACME) |		/* Some comparitor thing */ \
			 (0 << ADTS0))		/* Free running mode (don't care) */

static void
ao_adc_start(void)
{
	uint8_t	channel = adc_channels[ao_adc_channel];
	ADMUX = ((0 << REFS1) |				/* AVcc reference */
		 (1 << REFS0) |				/* AVcc reference */
		 (1 << ADLAR) |				/* Left-shift results */
		 (ADC_CHANNEL_LOW(channel)));		/* Select channel */

	ADCSRB = (ADCSRB_INIT |
		  ADC_CHANNEL_HIGH(channel));		/* High channel bit */

	ADCSRA = (ADCSRA_INIT |
		  (1 << ADSC) |
		  (1 << ADIE));				/* Start conversion */
}

ISR(ADC_vect)
{
	uint16_t	value;

	/* Must read ADCL first or the value there will be lost */
	value = ADCL;
	value |= (ADCH << 8);
	ao_data_ring[ao_data_head].adc.adc[ao_adc_channel] = value;
	if (++ao_adc_channel < NUM_ADC - HAS_ICP3_COUNT)
		ao_adc_start();
	else {
#if HAS_ICP3_COUNT
		/* steal last adc channel for pwm input */
		ao_data_ring[ao_data_head].adc.adc[ao_adc_channel] = ao_icp3_count;
#endif
		ADCSRA = ADCSRA_INIT;
		ao_data_ring[ao_data_head].tick = ao_time();
		ao_data_head = ao_data_ring_next(ao_data_head);
		ao_wakeup((void *) &ao_data_head);
		ao_cpu_sleep_disable = 0;
	}
}

void
ao_adc_poll(void)
{
	ao_cpu_sleep_disable = 1;
	ao_adc_channel = 0;
	ao_adc_start();
}

void
ao_data_get(__xdata struct ao_data *packet)
{
	uint8_t	i = ao_data_ring_prev(ao_data_head);
	memcpy(packet, (void *) &ao_data_ring[i], sizeof (struct ao_data));
}

static void
ao_adc_dump(void) __reentrant
{
	static __xdata struct ao_data	packet;
	uint8_t i;
	ao_data_get(&packet);
	printf("tick: %5u",  packet.tick);
	for (i = 0; i < NUM_ADC; i++)
		printf (" %2d: %5u", i, packet.adc.adc[i]);
	printf("\n");
}

__code struct ao_cmds ao_adc_cmds[] = {
	{ ao_adc_dump,	"a\0ADC" },
	{ 0, NULL },
};

void
ao_adc_init(void)
{
	PRR0 &= ~(1 << PRADC);
	DIDR0 = 0xf3;
	DIDR2 = 0x3f;
	ADCSRB = ADCSRB_INIT;
	ADCSRA = ADCSRA_INIT;
	ao_cmd_register(&ao_adc_cmds[0]);
}
