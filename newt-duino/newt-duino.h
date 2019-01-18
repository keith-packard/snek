#include <avr/pgmspace.h>
#define NEWT_POOL	900
#define NEWT_DEBUG	0
#define strtof(a,b) strtod(a,b)
#define VALUE_STACK_SIZE	8
#define PARSE_STACK_SIZE	64
#define NEWT_STACK		16
#define PARSE_TABLE_DECLARATION(t) PROGMEM t
#define PARSE_TABLE_FETCH_KEY(a) 	((parse_key_t) pgm_read_word(a))
#define PARSE_TABLE_FETCH_TOKEN(a)	((token_t) pgm_read_byte(a))
#define PARSE_TABLE_FETCH_PRODUCTION(a)	((uint8_t) pgm_read_byte(a))
#define ERROR_FETCH_FORMAT_CHAR(a)	((char) pgm_read_byte(a))

#define newt_error_name newt_internal_error
#define newt_error(fmt, args...) do {				\
		static const char PROGMEM __fmt__[] = (fmt);	\
		newt_internal_error(__fmt__, ## args);		\
	} while (0)

#define fprintf(file, fmt, args...) do {			\
		static const char PROGMEM __fmt__[] = (fmt);	\
		fprintf_P(file, __fmt__, ## args);		\
	} while(0)

#define sprintf_const(dst, fmt, args...) do {			\
		static const char PROGMEM __fmt__[] = (fmt);	\
		sprintf_P(dst, __fmt__, ##args);		\
	} while(0)

#define NEWT_BUILTIN_NAMES_DECLARATION(n) PROGMEM n
#define NEWT_BUILTIN_NAMES(a)	((uint8_t) pgm_read_byte(&newt_builtin_names[a]))
#define NEWT_BUILTIN_NAMES_CMP(a,b)	strcmp_P(a,b)

static inline const char *
avr_newt_builtin_names_return(const uint8_t *bits)
{
	static char ret[32];
	int i;
	char c;
	char *r = ret;

	while ((*r++ = (char) pgm_read_byte(bits++)))
		;
	return ret;
}

static inline int
avr_newt_builtin_names_len(const uint8_t *a)
{
	int len = 0;

	while (pgm_read_byte(a++))
		len++;
	return len;
}

#define newt_builtin_names_return(a) avr_newt_builtin_names_return(a)
#define newt_builtin_names_len(a) avr_newt_builtin_names_len(a)
