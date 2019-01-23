#include <avr/pgmspace.h>
#define NEWT_POOL	1024
#define NEWT_DEBUG	0
#define strtof(a,b) strtod(a,b)
#define VALUE_STACK_SIZE	8
#define PARSE_STACK_SIZE	64
#define NEWT_STACK		16
#define PARSE_TABLE_DECLARATION(t) 	PROGMEM t
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

#define NEWT_BUILTIN_NAMES_DECLARE(n) 	PROGMEM n
#define NEWT_BUILTIN_NAMES(a)		((uint8_t) pgm_read_byte(&newt_builtin_names[a]))
#define NEWT_BUILTIN_NAMES_CMP(a,b)	strcmp_P(a,b)

#define NEWT_BUILTIN_DECLARE(n)	PROGMEM n
#define NEWT_BUILTIN_NFORMAL(b) ((int8_t) pgm_read_byte(&(b)->nformal))
#define NEWT_BUILTIN_FUNCV(b)	((newt_poly_t(*)(newt_offset_t, newt_poly_t *)) pgm_read_word(&(b)->funcv))
#define NEWT_BUILTIN_FUNC0(b) 	((newt_poly_t(*)(void)) pgm_read_word(&(b)->func0))
#define NEWT_BUILTIN_FUNC1(b) 	((newt_poly_t(*)(newt_poly_t)) pgm_read_word(&(b)->func1))
#define NEWT_BUILTIN_FUNC2(b) 	((newt_poly_t(*)(newt_poly_t, newt_poly_t)) pgm_read_word(&(b)->func2))
#define NEWT_BUILTIN_FUNC3(b) 	((newt_poly_t(*)(newt_poly_t, newt_poly_t, newt_poly_t)) pgm_read_word(&(b)->func3))
#define NEWT_BUILTIN_FUNC4(b) 	((newt_poly_t(*)(newt_poly_t, newt_poly_t, newt_poly_t, newt_poly_t)) pgm_read_word(&(b)->func4))

#define NEWT_ROOT_DECLARE(n)	PROGMEM n
#define NEWT_ROOT_TYPE(n) 	((const newt_mem_t *) pgm_read_word(&(n)->type))
#define NEWT_ROOT_ADDR(n) 	((void **) pgm_read_word(&(n)->addr))

static inline const char *
avr_newt_builtin_names_return(const uint8_t *bits)
{
	static char ret[NEWT_BUILTIN_NAMES_MAX_LEN + 1];
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

#define NEWT_MEM_DECLARE(n) 	PROGMEM n
#define NEWT_MEM_SIZE(m)	((newt_offset_t (*)(void *addr)) pgm_read_word(&(m)->size))
#define NEWT_MEM_MARK(m)	((void (*)(void *addr)) pgm_read_word(&(m)->mark))
#define NEWT_MEM_MOVE(m)	((void (*)(void *addr)) pgm_read_word(&(m)->move))
