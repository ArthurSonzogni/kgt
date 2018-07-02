/* Generated by lx */

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>

#include LX_HEADER

static enum lx_wsn_token z0(struct lx_wsn_lx *lx);
static enum lx_wsn_token z1(struct lx_wsn_lx *lx);

#if __STDC_VERSION__ >= 199901L
inline
#endif
static int
lx_getc(struct lx_wsn_lx *lx)
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
		lx->end.col = 1;
	}

	return c;
}

#if __STDC_VERSION__ >= 199901L
inline
#endif
static void
lx_wsn_ungetc(struct lx_wsn_lx *lx, int c)
{
	assert(lx != NULL);
	assert(lx->c == EOF);

	lx->c = c;


	lx->end.byte--;
	lx->end.col--;

	if (c == '\n') {
		lx->end.line--;
		lx->end.col = 0; /* XXX: lost information */
	}
}

int
lx_wsn_dynpush(void *buf_opaque, char c)
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
lx_wsn_dynclear(void *buf_opaque)
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
lx_wsn_dynfree(void *buf_opaque)
{
	struct lx_dynbuf *t = buf_opaque;

	assert(t != NULL);

	free(t->a);
}
static enum lx_wsn_token
z0(struct lx_wsn_lx *lx)
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
			case '"': state = S2; break;
			default: state = S1; break;
			}
			break;

		case S1: /* e.g. "a" */
			lx_wsn_ungetc(lx, c); return TOK_CHAR;

		case S2: /* e.g. """ */
			switch ((unsigned char) c) {
			case '"': state = S3; break;
			default:  lx_wsn_ungetc(lx, c); return lx->z = z1, TOK_LITERAL;
			}
			break;

		case S3: /* e.g. """" */
			lx_wsn_ungetc(lx, c); return TOK_ESC;

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
	case S2: return TOK_LITERAL;
	case S3: return TOK_ESC;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

static enum lx_wsn_token
z1(struct lx_wsn_lx *lx)
{
	int c;

	enum {
		S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, 
		S10, S11, S12, S13, NONE
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
			case '\t':
			case '\n':
			case '\v':
			case '\f':
			case '\r': state = S1; break;
			case ' ': state = S1; break;
			case '"': state = S2; break;
			case '(': state = S3; break;
			case ')': state = S4; break;
			case '-': state = S5; break;
			case '.': state = S6; break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': state = S5; break;
			case '=': state = S7; break;
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
			case 'Z': state = S5; break;
			case '[': state = S8; break;
			case ']': state = S9; break;
			case '_': state = S5; break;
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
			case 'z': state = S5; break;
			case '{': state = S10; break;
			case '|': state = S11; break;
			case '}': state = S12; break;
			default:  lx->lgetc = NULL; return TOK_UNKNOWN;
			}
			break;

		case S1: /* e.g. "\t" */
			switch ((unsigned char) c) {
			case '\t':
			case '\n':
			case '\v':
			case '\f':
			case '\r': break;
			case ' ': break;
			default:  lx_wsn_ungetc(lx, c); return lx->z(lx);
			}
			break;

		case S2: /* e.g. """ */
			switch ((unsigned char) c) {
			case '"': state = S13; break;
			default:  lx_wsn_ungetc(lx, c); return lx->z = z0, lx->z(lx);
			}
			break;

		case S3: /* e.g. "(" */
			lx_wsn_ungetc(lx, c); return TOK_STARTGROUP;

		case S4: /* e.g. ")" */
			lx_wsn_ungetc(lx, c); return TOK_ENDGROUP;

		case S5: /* e.g. "a" */
			switch ((unsigned char) c) {
			case '-': break;
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
			case 'Z': break;
			case '_': break;
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
			default:  lx_wsn_ungetc(lx, c); return TOK_IDENT;
			}
			break;

		case S6: /* e.g. "." */
			lx_wsn_ungetc(lx, c); return TOK_SEP;

		case S7: /* e.g. "=" */
			lx_wsn_ungetc(lx, c); return TOK_EQUALS;

		case S8: /* e.g. "[" */
			lx_wsn_ungetc(lx, c); return TOK_STARTOPT;

		case S9: /* e.g. "]" */
			lx_wsn_ungetc(lx, c); return TOK_ENDOPT;

		case S10: /* e.g. "{" */
			lx_wsn_ungetc(lx, c); return TOK_STARTSTAR;

		case S11: /* e.g. "|" */
			lx_wsn_ungetc(lx, c); return TOK_ALT;

		case S12: /* e.g. "}" */
			lx_wsn_ungetc(lx, c); return TOK_ENDSTAR;

		case S13: /* e.g. """" */
			lx_wsn_ungetc(lx, c); return TOK_EMPTY;

		default:
			; /* unreached */
		}

		switch (state) {
		case S1:
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
	case S3: return TOK_STARTGROUP;
	case S4: return TOK_ENDGROUP;
	case S5: return TOK_IDENT;
	case S6: return TOK_SEP;
	case S7: return TOK_EQUALS;
	case S8: return TOK_STARTOPT;
	case S9: return TOK_ENDOPT;
	case S10: return TOK_STARTSTAR;
	case S11: return TOK_ALT;
	case S12: return TOK_ENDSTAR;
	case S13: return TOK_EMPTY;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

const char *
lx_wsn_name(enum lx_wsn_token t)
{
	switch (t) {
	case TOK_EMPTY: return "EMPTY";
	case TOK_IDENT: return "IDENT";
	case TOK_ENDGROUP: return "ENDGROUP";
	case TOK_STARTGROUP: return "STARTGROUP";
	case TOK_ENDOPT: return "ENDOPT";
	case TOK_STARTOPT: return "STARTOPT";
	case TOK_ENDSTAR: return "ENDSTAR";
	case TOK_STARTSTAR: return "STARTSTAR";
	case TOK_CHAR: return "CHAR";
	case TOK_ESC: return "ESC";
	case TOK_LITERAL: return "LITERAL";
	case TOK_SEP: return "SEP";
	case TOK_ALT: return "ALT";
	case TOK_EQUALS: return "EQUALS";
	case TOK_EOF:     return "EOF";
	case TOK_ERROR:   return "ERROR";
	case TOK_UNKNOWN: return "UNKNOWN";
	default: return "?";
	}
}

const char *
lx_wsn_example(enum lx_wsn_token (*z)(struct lx_wsn_lx *), enum lx_wsn_token t)
{
	assert(z != NULL);

	if (z == z0) {
		switch (t) {
		case TOK_CHAR: return "a";
		case TOK_ESC: return "\"\"";
		case TOK_LITERAL: return "\"";
		default: goto error;
		}
	} else
	if (z == z1) {
		switch (t) {
		case TOK_EMPTY: return "\"\"";
		case TOK_IDENT: return "a";
		case TOK_ENDGROUP: return ")";
		case TOK_STARTGROUP: return "(";
		case TOK_ENDOPT: return "]";
		case TOK_STARTOPT: return "[";
		case TOK_ENDSTAR: return "}";
		case TOK_STARTSTAR: return "{";
		case TOK_SEP: return ".";
		case TOK_ALT: return "|";
		case TOK_EQUALS: return "=";
		default: goto error;
		}
	}

error:

	errno = EINVAL;
	return NULL;
}

void
lx_wsn_init(struct lx_wsn_lx *lx)
{
	static const struct lx_wsn_lx lx_default;

	assert(lx != NULL);

	*lx = lx_default;

	lx->c = EOF;
	lx->z = z1;

	lx->end.byte = 0;
	lx->end.line = 1;
	lx->end.col  = 1;
}

enum lx_wsn_token
lx_wsn_next(struct lx_wsn_lx *lx)
{
	enum lx_wsn_token t;

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

