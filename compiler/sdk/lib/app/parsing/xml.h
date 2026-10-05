#ifndef XML_H
#define XML_H

#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#else
#define	memchr(a, b, c)		ft_memchr(a, b, c)
#define	strlen(a)			ft_strlen(a)
#define	memcmp(a, b, c)		ft_memcmp(a, b, c)
#define	isspace(a)			ft_isspace(a)
#endif

typedef enum
{
	SXML_ERROR_XMLINVALID= -1,	/* Parser found invalid XML data - not much you can do beyond error reporting */
	SXML_SUCCESS= 0,			/* Parser has completed successfully - parsing of XML document is complete */
	SXML_ERROR_BUFFERDRY= 1,	/* Parser ran out of input data - refill buffer with more XML text to continue parsing */
	SXML_ERROR_TOKENSFULL= 2	/* Parser has filled all the supplied tokens with data - provide more tokens for further output */
}	sxmlerr_t;

/*
 You provide sxml_parse() with a buffer of XML text for parsing.
 The parser will handle text data encoded in ascii, latin-1 and utf-8.
 It should also work with other encodings that are acsii extensions.

 sxml_parse() is reentrant.
 In the case of return code SXML_ERROR_BUFFERDRY or SXML_ERROR_TOKENSFULL, you are expected to call the function again after resolving the problem to continue parsing.
 */

/** @brief Represents an XML element with its name, attributes, children, and sibling link. */
typedef	struct sxml_t sxml_t;
/** @brief Stores a token’s type, source range, and associated XML node. */
typedef	struct sxmltok_t sxmltok_t;
static inline sxmlerr_t	sxml_parse(sxml_t *parser, const char *buffer, unsigned bufferlen, sxmltok_t* tokens, unsigned num_tokens);

/*
 The sxml_t object stores all data required for SXML to continue from where it left of.

 After calling sxml_parse() 'ntokens' tells you how many output tokens have been filled with data.
 Depending on how you resolve SXML_ERROR_BUFFERDRY or SXML_ERROR_TOKENSFULL you may need to modifiy 'bufferpos' and 'ntokens' to correctly reflect the new buffer and tokens you provide.
*/

/** @brief Represents an XML element with its name, attributes, children, and sibling link. */
struct sxml_t
{
	unsigned bufferpos;	/* Current offset into buffer - all XML data before this position has been successfully parsed */
	unsigned ntokens;	/* Number of tokens filled with valid data by the parser */
	unsigned taglevel;	/* Used internally - keeps track of number of unclosed XML elements to detect start and end of document */
};

/*
 Before you call sxml_parse() for the first time, you have to initialize the parser object.
 You may easily do that with the provided function sxml_init().
*/

static inline void	sxml_init(sxml_t *parser);

/*
 Unlike most XML parsers, SXML does not use SAX callbacks or allocate a DOM tree.
 Instead you will have to interpret the XML structure through a table of tokens.

 A token can describe any of the following types:
*/

typedef enum
{
	SXML_STARTTAG,	/* Start tag describes the opening of an XML element */
	SXML_ENDTAG,	/* End tag is the closing of an XML element */

	SXML_CHARACTER,		/* Character data may be escaped - check if the first character is an ampersand '&' to identity a XML character reference */
	SXML_CDATA,			/* Character data should be read as is - it is not escaped */

	/* And some other token types you might be interested in: */
	SXML_INSTRUCTION,	/* Can be used to identity the text encoding */
	SXML_DOCTYPE,		/* If you'd like to interpret DTD data */
	SXML_COMMENT		/* Most likely you don't care about comments - but this is where you'll find them */
}	sxmltype_t;

/*
 If you are familiar with the structure of an XML document most of these type names should sound familiar.
 
 A token has the following data:
*/

/** @brief Stores a token’s type, source range, and associated XML node. */
struct sxmltok_t
{
	unsigned short type;	/* A token is one of the above sxmltype_t */
	unsigned short size;	/* The following number of tokens contain additional data related to this token - used for describing attributes */

	/* 'startpos' and 'endpos' together define a range within the provided text buffer - use these offsets with the buffer to extract the text value of the token */
	unsigned startpos;
	unsigned endpos;
};

static const char* str_findchr(const char* start, const char* end, int c)
{
	const char* it;
	assert(start <= end, 0);
	assert(0 <= c && c <= 127, 0);	/* CHAR_MAX - memchr implementation will only work when searching for ascii characters within a utf-8 string */
	
	it= (const char*)memchr(start, c, end - start);
	return (it != NULL) ? it : end;
}

static const char* str_findstr(const char* start, const char* end, const char* needle)
{
	size_t	needlelen;
	int		first;
	assert(start <= end, 0);
	
	needlelen= strlen(needle);
	assert(0 < needlelen, 0);
	first = (unsigned char)needle[0];

	while (start + needlelen <= end)
	{
		const char* it= (const char*)memchr(start, first, (end - start) - (needlelen - 1));
		if (it == NULL)
			break;

		if (memcmp(it, needle, needlelen) == 0)
			return it;

		start= it + 1;
	}

	return end;
}

static int str_startswith(const char* start, const char* end, const char* prefix)
{
	long nbytes;
	assert(start <= end, 0);
	
	nbytes= strlen(prefix);
	if (end - start < nbytes)
		return 0;
	
	return memcmp(prefix, start, nbytes) == 0;
}

static int NameStartChar(int c)
{
	/*
	 We don't perform utf-8 decoding - just accept all characters with hight bit set
	 (0xC0 <= c && c <= 0xD6) || (0xD8 <= c && c <= 0xF6) || (0xF8 <= c && c <= 0x2FF) ||
	 (0x370 <= c && c <= 0x37D) || (0x37F <= c && c <= 0x1FFF) || (0x200C <= c && c <= 0x200D) ||
	 (0x2070 <= c && c <= 0x218F) || (0x2C00 <= c && c <= 0x2FEF) || (0x3001 <= c && c <= 0xD7FF) ||
	 (0xF900 <= c && c <= 0xFDCF) || (0xFDF0 <= c && c <= 0xFFFD) || (0x10000 <= c && c <= 0xEFFFF);
	 */
	if (0x80 <= c)
		return 1;

	return c == ':' || ('A' <= c && c <= 'Z') || c == '_' || ('a' <= c && c <= 'z');
}

static int NameChar(int c)
{
	return NameStartChar (c) ||
		c == '-' || c == '.' || ('0'  <= c && c <= '9') ||
		c == 0xB7 || (0x0300 <= c && c <= 0x036F) || (0x203F <= c && c <= 0x2040);
}

#define ISSPACE(c)		(isspace(((unsigned char)(c))))
#define ISALPHA(c)		(NameStartChar(((unsigned char)(c))))
#define ISALNUM(c)		(NameChar(((unsigned char)(c))))

/* Left trim isspace */
static const char* str_ltrim(const char* start, const char* end)
{
	const char* it;
	assert(start <= end, 0);

	for (it= start; it != end && ISSPACE (*it); it++)
		;

	return it;
}

/* Right trim isspace */
static const char* str_rtrim(const char* start, const char* end)
{
	const char* it, *prev;
	assert(start <= end, 0);

	for (it= end; start != it; it= prev)
	{
		prev= it - 1;
		if (!ISSPACE (*prev))
			return it;
	}
	
	return start;
}

static const char* str_find_notalnum(const char* start, const char* end)
{
	const char* it;	
	assert(start <= end, 0);

	for (it= start; it != end && ISALNUM (*it); it++)
		;

	return it;
}

/* MARK: State */

/* Collect arguments in a structure for convenience */
typedef struct
{
	const char		*buffer;
	unsigned		bufferlen;
	sxmltok_t		*tokens;
	unsigned		num_tokens;
} sxml_args_t;

#define buffer_fromoffset(args,i)	((args)->buffer + (i))
#define buffer_tooffset(args,ptr)	(unsigned) ((ptr) - (args)->buffer)
#define buffer_getend(args)			((args)->buffer + (args)->bufferlen)

static int state_pushtoken(sxml_t* state, sxml_args_t* args, sxmltype_t type, const char* start, const char* end)
{
	sxmltok_t* token;
	unsigned i= state->ntokens++;
	if (args->num_tokens < state->ntokens)
		return 0;
	
	token= &args->tokens[i];
	token->type= type;
	token->startpos= buffer_tooffset (args, start);
	token->endpos= buffer_tooffset (args, end);
	token->size= 0;

	switch (type)
	{
		case SXML_STARTTAG:	state->taglevel++;	break;

		case SXML_ENDTAG:
			assert(0 < state->taglevel, 0);
			state->taglevel--;
			break;

		default:
			break;
	}

	return 1;
}

static sxmlerr_t state_setpos(sxml_t* state, const sxml_args_t* args, const char* ptr)
{
	state->bufferpos= buffer_tooffset(args, ptr);
	return (state->ntokens <= args->num_tokens) ? SXML_SUCCESS : SXML_ERROR_TOKENSFULL;
}

#define state_commit(dest,src) memcpy((dest), (src), sizeof (sxml_t))

/*
 MARK: Parse
 
 SXML does minimal validation of the input data.
 SXML_ERROR_XMLSTRICT is returned if some simple XML validation tests fail.
 SXML_ERROR_XMLINVALID is instead returned if the invalid XML data is serious enough to prevent the parser from continuing.
 We currently make no difference between these two - but they are marked differently in case we wish to do so in the future.
*/

#define SXML_ERROR_XMLSTRICT	SXML_ERROR_XMLINVALID

#define ENTITY_MAXLEN			8	/* &#x03A3; */
#define MIN(a,b)				((a) < (b) ? (a) : (b))

static sxmlerr_t parse_characters(sxml_t* state, sxml_args_t* args, const char* end)
{
	const char* start= buffer_fromoffset(args, state->bufferpos);
	const char* limit, *colon, *ampr= str_findchr(start, end, '&');
	assert(end <= buffer_getend (args), (sxmlerr_t)0);

	if (ampr != start)
		state_pushtoken(state, args, SXML_CHARACTER, start, ampr);

	if (ampr == end)
		return state_setpos(state, args, ampr);

	/* limit entity to search to ENTITY_MAXLEN */
	limit= MIN(ampr + ENTITY_MAXLEN, end);
	colon= str_findchr(ampr, limit, ';');
	if (colon == limit)
		return (limit == end) ? SXML_ERROR_BUFFERDRY : SXML_ERROR_XMLINVALID;
		
	start= colon + 1;
	state_pushtoken(state, args, SXML_CHARACTER, ampr, start);
	return state_setpos(state, args, start);
}

static sxmlerr_t parse_attrvalue(sxml_t* state, sxml_args_t* args, const char* end)
{
	while (buffer_fromoffset(args, state->bufferpos) != end)
	{
		sxmlerr_t err= parse_characters(state, args, end);
		if (err != SXML_SUCCESS)
			return err;
	}
	
	return SXML_SUCCESS;
}

static sxmlerr_t parse_attributes(sxml_t* state, sxml_args_t* args)
{
	const char* start= buffer_fromoffset(args, state->bufferpos);
	const char* end= buffer_getend(args);
	const char* name= str_ltrim(start, end);
	
	unsigned ntokens= state->ntokens;
	assert(0 < ntokens, (sxmlerr_t)0);

	while (name != end && ISALPHA(*name))
	{
		const char* eq, *space, *quot, *value;
		sxmlerr_t err;

		/* Attribute name */		
		eq= str_findchr(name, end, '=');
		if (eq == end)
			return SXML_ERROR_BUFFERDRY;

		space= str_rtrim(name, eq);
		state_pushtoken(state, args, SXML_CDATA, name, space);

		/* Attribute value */
		quot= str_ltrim(eq + 1, end);
		if (quot == end)
			return SXML_ERROR_BUFFERDRY;
		else if (*quot != '\'' && *quot != '"')
			return SXML_ERROR_XMLINVALID;

		value= quot + 1;
		quot= str_findchr(value, end, *quot);
		if (quot == end)
			return SXML_ERROR_BUFFERDRY;

		state_setpos (state, args, value);
		err= parse_attrvalue(state, args, quot);
		if (err != SXML_SUCCESS)
			return err;

		/* --- */
		
		name= str_ltrim(quot + 1, end);
	}

	{
		sxmltok_t* token= args->tokens + (ntokens - 1);
		token->size= (unsigned short) (state->ntokens - ntokens);
	}
	
	return state_setpos(state, args, name);
}

/* --- */

#define TAG_LEN(str)	(int)(sizeof (str) - 1)
#define TAG_MINSIZE	3

static sxmlerr_t parse_comment(sxml_t* state, sxml_args_t* args)
{
	static const char STARTTAG[]= "<!--";
	static const char ENDTAG[]= "-->";

	const char* dash;
	const char* start= buffer_fromoffset(args, state->bufferpos);
	const char* end= buffer_getend(args);
	if (end - start < TAG_LEN(STARTTAG))
		return SXML_ERROR_BUFFERDRY;

	if (!str_startswith(start, end, STARTTAG))
		return SXML_ERROR_XMLINVALID;

	start+= TAG_LEN(STARTTAG);
	dash= str_findstr(start, end, ENDTAG);
	if (dash == end)
		return SXML_ERROR_BUFFERDRY;

	state_pushtoken(state, args, SXML_COMMENT, start, dash);
	return state_setpos(state, args, dash + TAG_LEN (ENDTAG));
}

static sxmlerr_t parse_instruction(sxml_t* state, sxml_args_t* args)
{
	static const char STARTTAG[]= "<?";
	static const char ENDTAG[]= "?>";

	sxmlerr_t err;
	const char* quest, *space;
	const char* start= buffer_fromoffset(args, state->bufferpos);
	const char* end= buffer_getend(args);
	assert(TAG_MINSIZE <= end - start, (sxmlerr_t)0);

	if (!str_startswith(start, end, STARTTAG))
		return SXML_ERROR_XMLINVALID;

	start+= TAG_LEN(STARTTAG);
	space= str_find_notalnum(start, end);
	if (space == end)
		return SXML_ERROR_BUFFERDRY;

	state_pushtoken(state, args, SXML_INSTRUCTION, start, space);

	state_setpos(state, args, space);
	err= parse_attributes(state, args);
	if (err != SXML_SUCCESS)
		return err;

	quest= buffer_fromoffset(args, state->bufferpos);
	if (end - quest < TAG_LEN(ENDTAG))
		return SXML_ERROR_BUFFERDRY;

	if (!str_startswith(quest, end, ENDTAG))
		return SXML_ERROR_XMLINVALID;

	return state_setpos(state, args, quest + TAG_LEN (ENDTAG));
}

static sxmlerr_t parse_doctype(sxml_t* state, sxml_args_t* args)
{
	static const char STARTTAG[]= "<!DOCTYPE";
	static const char ENDTAG[]= "]>";

	const char* bracket;
	const char* start= buffer_fromoffset(args, state->bufferpos);
	const char* end= buffer_getend(args);
	if (end - start < TAG_LEN(STARTTAG))
		return SXML_ERROR_BUFFERDRY;

	if (!str_startswith(start, end, STARTTAG))
		return SXML_ERROR_BUFFERDRY;

	start+= TAG_LEN(STARTTAG);
	bracket= str_findstr(start, end, ENDTAG);
	if (bracket == end)
		return SXML_ERROR_BUFFERDRY;

	state_pushtoken(state, args, SXML_DOCTYPE, start, bracket);
	return state_setpos(state, args, bracket + TAG_LEN (ENDTAG));
}

static sxmlerr_t parse_start(sxml_t* state, sxml_args_t* args)
{	
	sxmlerr_t err;
	const char* gt, *name, *space;
	const char* start= buffer_fromoffset(args, state->bufferpos);
	const char* end= buffer_getend(args);
	assert(TAG_MINSIZE <= end - start, (sxmlerr_t)0);

	if (!(start[0] == '<' && ISALPHA(start[1])))
		return SXML_ERROR_XMLINVALID;

	/* --- */

	name= start + 1;
	space= str_find_notalnum(name, end);
	if (space == end)
		return SXML_ERROR_BUFFERDRY;

	state_pushtoken(state, args, SXML_STARTTAG, name, space);

	state_setpos(state, args, space);
	err= parse_attributes(state, args);
	if (err != SXML_SUCCESS)
		return err;

	/* --- */

	gt= buffer_fromoffset(args, state->bufferpos);
	
	if (gt != end && *gt == '/')
	{
		state_pushtoken(state, args, SXML_ENDTAG, name, space);
		gt++;
	}

	if (gt == end)
		return SXML_ERROR_BUFFERDRY;

	if (*gt != '>')
		return SXML_ERROR_XMLINVALID;

	return state_setpos(state, args, gt + 1);
}

static sxmlerr_t parse_end(sxml_t* state, sxml_args_t* args)
{
	const char* gt, *space;
	const char* start= buffer_fromoffset(args, state->bufferpos);
	const char* end= buffer_getend(args);
	assert(TAG_MINSIZE <= end - start, (sxmlerr_t)0);

	if (!(str_startswith(start, end, "</") && ISALPHA(start[2])))
		return SXML_ERROR_XMLINVALID;

	start+= 2;	
	gt= str_findchr(start, end, '>');
	if (gt == end)
		return SXML_ERROR_BUFFERDRY;

	/* Test for no characters beyond elem name */
	space= str_find_notalnum(start, gt);
	if (str_ltrim(space, gt) != gt)
		return SXML_ERROR_XMLSTRICT;

	state_pushtoken(state, args, SXML_ENDTAG, start, space);
	return state_setpos(state, args, gt + 1);
}

static sxmlerr_t parse_cdata(sxml_t* state, sxml_args_t* args)
{
	static const char STARTTAG[]= "<![CDATA[";
	static const char ENDTAG[]= "]]>";

	const char* bracket;
	const char* start= buffer_fromoffset(args, state->bufferpos);
	const char* end= buffer_getend(args);
	if (end - start < TAG_LEN(STARTTAG))
		return SXML_ERROR_BUFFERDRY;

	if (!str_startswith(start, end, STARTTAG))
		return SXML_ERROR_XMLINVALID;

	start+= TAG_LEN (STARTTAG);
	bracket= str_findstr(start, end, ENDTAG);
	if (bracket == end)
		return SXML_ERROR_BUFFERDRY;

	state_pushtoken(state, args, SXML_CDATA, start, bracket);
	return state_setpos(state, args, bracket + TAG_LEN (ENDTAG));
}

/*
 MARK: SXML
 Public API inspired by the JSON parser JSMN ( http://zserge.com/jsmn.html ).
*/

static inline void	sxml_init(sxml_t *state)
{
    state->bufferpos= 0;
    state->ntokens= 0;
	state->taglevel= 0;
}

#define ROOT_FOUND(state)	(0 < (state)->taglevel)
#define ROOT_PARSED(state)	((state)->taglevel == 0)

static inline sxmlerr_t sxml_parse(sxml_t *state, const char *buffer, unsigned bufferlen, sxmltok_t tokens[], unsigned num_tokens)
{
	sxml_t temp= *state;
	const char* end= buffer + bufferlen;
	
	sxml_args_t args;
	args.buffer= buffer;
	args.bufferlen= bufferlen;
	args.tokens= tokens;
	args.num_tokens= num_tokens;

	/* --- */

	while (!ROOT_FOUND(&temp))
	{
		sxmlerr_t err;
		const char* start= buffer_fromoffset(&args, temp.bufferpos);
		const char* lt= str_ltrim(start, end);
		state_setpos(&temp, &args, lt);
		state_commit(state, &temp);

		if (end - lt < TAG_MINSIZE)
			return SXML_ERROR_BUFFERDRY;

		/* --- */

		if (*lt != '<')
			return SXML_ERROR_XMLINVALID;

		switch (lt[1])
		{
			case '?':	err= parse_instruction(&temp, &args);	break;
			case '!':	err= parse_doctype(&temp, &args);	break;
			default:	err= parse_start(&temp, &args);	break;
		}

		if (err != SXML_SUCCESS)
			return err;

		state_commit(state, &temp);
	}

	/* --- */

	while (!ROOT_PARSED(&temp))
	{
		sxmlerr_t err;
		const char* start= buffer_fromoffset(&args, temp.bufferpos);
		const char* lt= str_findchr(start, end, '<');
		while (buffer_fromoffset(&args, temp.bufferpos) != lt)
		{
			sxmlerr_t err= parse_characters(&temp, &args, lt);
			if (err != SXML_SUCCESS)
				return err;

			state_commit(state, &temp);
		}

		/* --- */

		if (end - lt < TAG_MINSIZE)
			return SXML_ERROR_BUFFERDRY;

		switch (lt[1])
		{
			case '?':	err= parse_instruction(&temp, &args);		break;
			case '/':	err= parse_end(&temp, &args);	break;
			case '!':	err= (lt[2] == '-') ? parse_comment(&temp, &args) : parse_cdata (&temp, &args);	break;
			default:	err= parse_start(&temp, &args);	break;
		}

		if (err != SXML_SUCCESS)
			return err;

		state_commit(state, &temp);
	}

	return SXML_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif