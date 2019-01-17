#include <avr/pgmspace.h>
#define NEWT_POOL	640
#define NEWT_DEBUG	0
#define strtof(a,b) strtod(a,b)
#define VALUE_STACK_SIZE	8
#define PARSE_STACK_SIZE	64
#define NEWT_STACK		16
#define PARSE_TABLE_DECLARATION(t) PROGMEM t
#define PARSE_TABLE_FETCH_KEY(a) 	((parse_key_t) pgm_read_word(a))
#define PARSE_TABLE_FETCH_TOKEN(a)	((token_t) pgm_read_byte(a))
#define PARSE_TABLE_FETCH_PRODUCTION(a)	((uint8_t) pgm_read_byte(a))
void
newt_internal_error(char *format, ...);
#define newt_error_name newt_internal_error
#define newt_error(fmt, ...) newt_internal_error("error")
