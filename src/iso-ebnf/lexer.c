/* Generated by lx */

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>

#include LX_HEADER

static enum lx_iso_ebnf_token z0(struct lx_iso_ebnf_lx *lx);
static enum lx_iso_ebnf_token z1(struct lx_iso_ebnf_lx *lx);
static enum lx_iso_ebnf_token z2(struct lx_iso_ebnf_lx *lx);
static enum lx_iso_ebnf_token z3(struct lx_iso_ebnf_lx *lx);
static enum lx_iso_ebnf_token z4(struct lx_iso_ebnf_lx *lx);

#if __STDC_VERSION__ >= 199901L
inline
#endif
static int
lx_getc(struct lx_iso_ebnf_lx *lx)
{
	int c;

	assert(lx != NULL);
	assert(lx->lgetc != NULL);

	if (lx->c != EOF) {
		c = lx->c, lx->c = EOF;
	} else {
		c = lx->lgetc(lx);
		if (c == EOF) {
			return EOF;
		}
	}

	lx->end.byte++;
	lx->end.col++;

	if (c == '\n') {
		lx->end.line++;
		lx->end.saved_col = lx->end.col - 1;
		lx->end.col = 1;
	}

	return c;
}

#if __STDC_VERSION__ >= 199901L
inline
#endif
static void
lx_iso_ebnf_ungetc(struct lx_iso_ebnf_lx *lx, int c)
{
	assert(lx != NULL);
	assert(lx->c == EOF);

	lx->c = c;


	lx->end.byte--;
	lx->end.col--;

	if (c == '\n') {
		lx->end.line--;
		lx->end.col = lx->end.saved_col;
	}
}

int
lx_iso_ebnf_dynpush(void *buf_opaque, char c)
{
	struct lx_dynbuf *t = buf_opaque;

	assert(t != NULL);

	if (t->p == t->a + t->len) {
		size_t len;
		ptrdiff_t off;
		char *tmp;

		if (t->len == 0) {
			assert(LX_DYN_LOW > 0);
			len = LX_DYN_LOW;
		} else {
			len = t->len * LX_DYN_FACTOR;
			if (len < t->len) {
				errno = ERANGE;
				return -1;
			}
		}

		off = t->p - t->a;
		tmp = realloc(t->a, len);
		if (tmp == NULL) {
			return -1;
		}

		t->p   = tmp + off;
		t->a   = tmp;
		t->len = len;
	}

	assert(t->p != NULL);
	assert(t->a != NULL);

	*t->p++ = c;

	return 0;
}

int
lx_iso_ebnf_dynclear(void *buf_opaque)
{
	struct lx_dynbuf *t = buf_opaque;

	assert(t != NULL);

	if (t->len > LX_DYN_HIGH) {
		size_t len;
		char *tmp;

		len = t->len / LX_DYN_FACTOR;

		tmp = realloc(t->a, len);
		if (tmp == NULL) {
			return -1;
		}

		t->a   = tmp;
		t->len = len;
	}

	t->p = t->a;

	return 0;
}

void
lx_iso_ebnf_dynfree(void *buf_opaque)
{
	struct lx_dynbuf *t = buf_opaque;

	assert(t != NULL);

	free(t->a);
}
static enum lx_iso_ebnf_token
z0(struct lx_iso_ebnf_lx *lx)
{
	int c;

	enum {
		S0, S1, S2, NONE
	} state;

	assert(lx != NULL);

	if (lx->clear != NULL) {
		lx->clear(lx->buf_opaque);
	}

	state = NONE;

	lx->start = lx->end;

	while (c = lx_getc(lx), c != EOF) {
		if (state == NONE) {
			state = S0;
		}

		switch (state) {
		case S0: /* start */
			switch ((unsigned char) c) {
			case '?': state = S2; break;
			default: state = S1; break;
			}
			break;

		case S1: /* e.g. "a" */
			lx_iso_ebnf_ungetc(lx, c); return lx->z(lx);

		case S2: /* e.g. "?" */
			lx_iso_ebnf_ungetc(lx, c); return lx->z = z4, lx->z(lx);

		default:
			; /* unreached */
		}

		switch (state) {
		case S0:
		case S1:
		case S2:
			break;

		default:
			if (lx->push != NULL) {
				if (-1 == lx->push(lx->buf_opaque, c)) {
					return TOK_ERROR;
				}
			}
			break;

		}
	}

	lx->lgetc = NULL;

	switch (state) {
	case NONE: return TOK_EOF;
	case S1: return TOK_EOF;
	case S2: return TOK_EOF;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

static enum lx_iso_ebnf_token
z1(struct lx_iso_ebnf_lx *lx)
{
	int c;

	enum {
		S0, S1, S2, NONE
	} state;

	assert(lx != NULL);

	if (lx->clear != NULL) {
		lx->clear(lx->buf_opaque);
	}

	state = NONE;

	lx->start = lx->end;

	while (c = lx_getc(lx), c != EOF) {
		if (state == NONE) {
			state = S0;
		}

		switch (state) {
		case S0: /* start */
			switch ((unsigned char) c) {
			case '"': state = S2; break;
			default: state = S1; break;
			}
			break;

		case S1: /* e.g. "a" */
			lx_iso_ebnf_ungetc(lx, c); return TOK_CHAR;

		case S2: /* e.g. "\"" */
			lx_iso_ebnf_ungetc(lx, c); return lx->z = z4, TOK_CS_LITERAL;

		default:
			; /* unreached */
		}

		if (lx->push != NULL) {
			if (-1 == lx->push(lx->buf_opaque, c)) {
				return TOK_ERROR;
			}
		}
	}

	lx->lgetc = NULL;

	switch (state) {
	case NONE: return TOK_EOF;
	case S1: return TOK_CHAR;
	case S2: return TOK_CS_LITERAL;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

static enum lx_iso_ebnf_token
z2(struct lx_iso_ebnf_lx *lx)
{
	int c;

	enum {
		S0, S1, S2, NONE
	} state;

	assert(lx != NULL);

	if (lx->clear != NULL) {
		lx->clear(lx->buf_opaque);
	}

	state = NONE;

	lx->start = lx->end;

	while (c = lx_getc(lx), c != EOF) {
		if (state == NONE) {
			state = S0;
		}

		switch (state) {
		case S0: /* start */
			switch ((unsigned char) c) {
			case '\'': state = S2; break;
			default: state = S1; break;
			}
			break;

		case S1: /* e.g. "a" */
			lx_iso_ebnf_ungetc(lx, c); return TOK_CHAR;

		case S2: /* e.g. "'" */
			lx_iso_ebnf_ungetc(lx, c); return lx->z = z4, TOK_CS_LITERAL;

		default:
			; /* unreached */
		}

		if (lx->push != NULL) {
			if (-1 == lx->push(lx->buf_opaque, c)) {
				return TOK_ERROR;
			}
		}
	}

	lx->lgetc = NULL;

	switch (state) {
	case NONE: return TOK_EOF;
	case S1: return TOK_CHAR;
	case S2: return TOK_CS_LITERAL;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

static enum lx_iso_ebnf_token
z3(struct lx_iso_ebnf_lx *lx)
{
	int c;

	enum {
		S0, S1, S2, S3, NONE
	} state;

	assert(lx != NULL);

	if (lx->clear != NULL) {
		lx->clear(lx->buf_opaque);
	}

	state = NONE;

	lx->start = lx->end;

	while (c = lx_getc(lx), c != EOF) {
		if (state == NONE) {
			state = S0;
		}

		switch (state) {
		case S0: /* start */
			switch ((unsigned char) c) {
			case '*': state = S2; break;
			default: state = S1; break;
			}
			break;

		case S1: /* e.g. "a" */
			lx_iso_ebnf_ungetc(lx, c); return lx->z(lx);

		case S2: /* e.g. "*" */
			switch ((unsigned char) c) {
			case ')': state = S3; break;
			default:  lx_iso_ebnf_ungetc(lx, c); return lx->z(lx);
			}
			break;

		case S3: /* e.g. "*)" */
			lx_iso_ebnf_ungetc(lx, c); return lx->z = z4, lx->z(lx);

		default:
			; /* unreached */
		}

		switch (state) {
		case S0:
		case S1:
		case S2:
		case S3:
			break;

		default:
			if (lx->push != NULL) {
				if (-1 == lx->push(lx->buf_opaque, c)) {
					return TOK_ERROR;
				}
			}
			break;

		}
	}

	lx->lgetc = NULL;

	switch (state) {
	case NONE: return TOK_EOF;
	case S1: return TOK_EOF;
	case S2: return TOK_EOF;
	case S3: return TOK_EOF;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

static enum lx_iso_ebnf_token
z4(struct lx_iso_ebnf_lx *lx)
{
	int c;

	enum {
		S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, 
		S10, S11, S12, S13, S14, S15, S16, S17, S18, S19, 
		S20, S21, S22, S23, NONE
	} state;

	assert(lx != NULL);

	if (lx->clear != NULL) {
		lx->clear(lx->buf_opaque);
	}

	state = NONE;

	lx->start = lx->end;

	while (c = lx_getc(lx), c != EOF) {
		if (state == NONE) {
			state = S0;
		}

		switch (state) {
		case S0: /* start */
			switch ((unsigned char) c) {
			case '*': state = S7; break;
			case '\'': state = S4; break;
			case ')': state = S6; break;
			case '=': state = S14; break;
			case '\t':
			case '\n':
			case '\v':
			case '\f':
			case '\r':
			case ' ': state = S1; break;
			case '?': state = S15; break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z': state = S16; break;
			case ']': state = S18; break;
			case '[': state = S17; break;
			case '}': state = S20; break;
			case '{': state = S19; break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': state = S12; break;
			case ',': state = S8; break;
			case '-': state = S9; break;
			case '.':
			case ';': state = S10; break;
			case '/': state = S11; break;
			case ':': state = S13; break;
			case '!':
			case '|': state = S2; break;
			case '"': state = S3; break;
			case '(': state = S5; break;
			default:  lx->lgetc = NULL; return TOK_UNKNOWN;
			}
			break;

		case S1: /* e.g. "\t" */
			switch ((unsigned char) c) {
			case '\t':
			case '\n':
			case '\v':
			case '\f':
			case '\r':
			case ' ': break;
			default:  lx_iso_ebnf_ungetc(lx, c); return lx->z(lx);
			}
			break;

		case S2: /* e.g. "!" */
			lx_iso_ebnf_ungetc(lx, c); return TOK_ALT;

		case S3: /* e.g. "\"" */
			lx_iso_ebnf_ungetc(lx, c); return lx->z = z1, lx->z(lx);

		case S4: /* e.g. "'" */
			lx_iso_ebnf_ungetc(lx, c); return lx->z = z2, lx->z(lx);

		case S5: /* e.g. "(" */
			switch ((unsigned char) c) {
			case '*': state = S23; break;
			case '/': state = S17; break;
			case ':': state = S19; break;
			default:  lx_iso_ebnf_ungetc(lx, c); return TOK_STARTGROUP;
			}
			break;

		case S6: /* e.g. ")" */
			lx_iso_ebnf_ungetc(lx, c); return TOK_ENDGROUP;

		case S7: /* e.g. "*" */
			lx_iso_ebnf_ungetc(lx, c); return TOK_STAR;

		case S8: /* e.g. "," */
			lx_iso_ebnf_ungetc(lx, c); return TOK_CAT;

		case S9: /* e.g. "-" */
			lx_iso_ebnf_ungetc(lx, c); return TOK_EXCEPT;

		case S10: /* e.g. "." */
			lx_iso_ebnf_ungetc(lx, c); return TOK_SEP;

		case S11: /* e.g. "\057" */
			switch ((unsigned char) c) {
			case ')': state = S18; break;
			default:  lx_iso_ebnf_ungetc(lx, c); return TOK_ALT;
			}
			break;

		case S12: /* e.g. "0" */
			switch ((unsigned char) c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': break;
			default:  lx_iso_ebnf_ungetc(lx, c); return TOK_COUNT;
			}
			break;

		case S13: /* e.g. ":" */
			switch ((unsigned char) c) {
			case ')': state = S20; break;
			default:  lx->lgetc = NULL; return TOK_UNKNOWN;
			}
			break;

		case S14: /* e.g. "=" */
			lx_iso_ebnf_ungetc(lx, c); return TOK_EQUALS;

		case S15: /* e.g. "?" */
			lx_iso_ebnf_ungetc(lx, c); return lx->z = z0, lx->z(lx);

		case S16: /* e.g. "a" */
			switch ((unsigned char) c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z': break;
			case '\t':
			case '\n':
			case '\v':
			case '\f':
			case '\r':
			case ' ': state = S21; break;
			case '-': state = S22; break;
			default:  lx_iso_ebnf_ungetc(lx, c); return TOK_IDENT;
			}
			break;

		case S17: /* e.g. "[" */
			lx_iso_ebnf_ungetc(lx, c); return TOK_STARTOPT;

		case S18: /* e.g. "]" */
			lx_iso_ebnf_ungetc(lx, c); return TOK_ENDOPT;

		case S19: /* e.g. "{" */
			lx_iso_ebnf_ungetc(lx, c); return TOK_STARTSTAR;

		case S20: /* e.g. "}" */
			lx_iso_ebnf_ungetc(lx, c); return TOK_ENDSTAR;

		case S21: /* e.g. "a\t" */
			switch ((unsigned char) c) {
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z': state = S16; break;
			case '\t':
			case '\n':
			case '\v':
			case '\f':
			case '\r':
			case ' ': break;
			default:  lx_iso_ebnf_ungetc(lx, c); return TOK_IDENT;
			}
			break;

		case S22: /* e.g. "a-" */
			switch ((unsigned char) c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z': state = S16; break;
			default:  lx->lgetc = NULL; return TOK_UNKNOWN;
			}
			break;

		case S23: /* e.g. "(*" */
			lx_iso_ebnf_ungetc(lx, c); return lx->z = z3, lx->z(lx);

		default:
			; /* unreached */
		}

		switch (state) {
		case S1:
		case S3:
		case S4:
		case S15:
		case S23:
			break;

		default:
			if (lx->push != NULL) {
				if (-1 == lx->push(lx->buf_opaque, c)) {
					return TOK_ERROR;
				}
			}
			break;

		}
	}

	lx->lgetc = NULL;

	switch (state) {
	case NONE: return TOK_EOF;
	case S1: return TOK_EOF;
	case S2: return TOK_ALT;
	case S3: return TOK_EOF;
	case S4: return TOK_EOF;
	case S5: return TOK_STARTGROUP;
	case S6: return TOK_ENDGROUP;
	case S7: return TOK_STAR;
	case S8: return TOK_CAT;
	case S9: return TOK_EXCEPT;
	case S10: return TOK_SEP;
	case S11: return TOK_ALT;
	case S12: return TOK_COUNT;
	case S14: return TOK_EQUALS;
	case S15: return TOK_EOF;
	case S16: return TOK_IDENT;
	case S17: return TOK_STARTOPT;
	case S18: return TOK_ENDOPT;
	case S19: return TOK_STARTSTAR;
	case S20: return TOK_ENDSTAR;
	case S21: return TOK_IDENT;
	case S23: return TOK_EOF;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

const char *
lx_iso_ebnf_name(enum lx_iso_ebnf_token t)
{
	switch (t) {
	case TOK_COUNT: return "COUNT";
	case TOK_IDENT: return "IDENT";
	case TOK_STAR: return "STAR";
	case TOK_EXCEPT: return "EXCEPT";
	case TOK_ENDGROUP: return "ENDGROUP";
	case TOK_STARTGROUP: return "STARTGROUP";
	case TOK_EQUALS: return "EQUALS";
	case TOK_CAT: return "CAT";
	case TOK_ENDSTAR: return "ENDSTAR";
	case TOK_STARTSTAR: return "STARTSTAR";
	case TOK_ENDOPT: return "ENDOPT";
	case TOK_STARTOPT: return "STARTOPT";
	case TOK_SEP: return "SEP";
	case TOK_ALT: return "ALT";
	case TOK_CHAR: return "CHAR";
	case TOK_CS_LITERAL: return "CS_LITERAL";
	case TOK_EOF:     return "EOF";
	case TOK_ERROR:   return "ERROR";
	case TOK_UNKNOWN: return "UNKNOWN";
	default: return "?";
	}
}

const char *
lx_iso_ebnf_example(enum lx_iso_ebnf_token (*z)(struct lx_iso_ebnf_lx *), enum lx_iso_ebnf_token t)
{
	assert(z != NULL);

	if (z == z0) {
		switch (t) {
		default: goto error;
		}
	} else
	if (z == z1) {
		switch (t) {
		case TOK_CHAR: return "a";
		case TOK_CS_LITERAL: return "\"";
		default: goto error;
		}
	} else
	if (z == z2) {
		switch (t) {
		case TOK_CHAR: return "a";
		case TOK_CS_LITERAL: return "'";
		default: goto error;
		}
	} else
	if (z == z3) {
		switch (t) {
		default: goto error;
		}
	} else
	if (z == z4) {
		switch (t) {
		case TOK_COUNT: return "0";
		case TOK_IDENT: return "a";
		case TOK_STAR: return "*";
		case TOK_EXCEPT: return "-";
		case TOK_ENDGROUP: return ")";
		case TOK_STARTGROUP: return "(";
		case TOK_EQUALS: return "=";
		case TOK_CAT: return ",";
		case TOK_ENDSTAR: return "}";
		case TOK_STARTSTAR: return "{";
		case TOK_ENDOPT: return "]";
		case TOK_STARTOPT: return "[";
		case TOK_SEP: return ".";
		case TOK_ALT: return "!";
		default: goto error;
		}
	}

error:

	errno = EINVAL;
	return NULL;
}

void
lx_iso_ebnf_init(struct lx_iso_ebnf_lx *lx)
{
	static const struct lx_iso_ebnf_lx lx_default;

	assert(lx != NULL);

	*lx = lx_default;

	lx->c = EOF;
	lx->z = z4;

	lx->end.byte = 0;
	lx->end.line = 1;
	lx->end.col  = 1;
}

enum lx_iso_ebnf_token
lx_iso_ebnf_next(struct lx_iso_ebnf_lx *lx)
{
	enum lx_iso_ebnf_token t;

	assert(lx != NULL);
	assert(lx->z != NULL);

	if (lx->lgetc == NULL) {
		return TOK_EOF;
	}

	t = lx->z(lx);

	if (lx->push != NULL) {
		if (-1 == lx->push(lx->buf_opaque, '\0')) {
			return TOK_ERROR;
		}
	}

	return t;
}

