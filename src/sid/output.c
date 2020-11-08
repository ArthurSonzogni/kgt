/*
 * Copyright 2014-2017 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

/*
 * SID Output.
 *
 * TODO: fprintf(fout), instead of stdout
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <ctype.h>

#include "../txt.h"
#include "../ast.h"
#include "../xalloc.h"
#include "../context.h"

#include "io.h"

static void output_alt(struct context* context, const struct ast_alt *);

static void
output_section(struct context* context, const char *section)
{
	fprintf(context->out,"\n%%%s%%\n\n", section);
}

static void
output_literal(struct context* context, const struct txt *t)
{
	char c;

	assert(t != NULL);
	assert(t->p != NULL);

	c = memchr(t->p, '\"', t->n) ? '\'' : '\"';
	fprintf(context->out,"%c%.*s%c; ", c, (int) t->n, t->p, c);
}

static void
output_basic(struct context* context, const struct ast_term *term)
{
	assert(term != NULL);
	assert(!term->invisible);

	switch (term->type) {
	case TYPE_EMPTY:
		fputs("$$; ", stdout);
		break;

	case TYPE_RULE:
		fprintf(context->out,"%s; ", term->u.rule->name);
		break;

	case TYPE_CI_LITERAL:
    context->reached_unimplemented = true;
    return;

	case TYPE_CS_LITERAL:
		output_literal(context, &term->u.literal);
		break;

	case TYPE_TOKEN:
		fprintf(context->out,"%s; ", term->u.token);
		break;

	case TYPE_PROSE:
    context->reached_unimplemented = true;
    return;

	case TYPE_GROUP:
		fputs("{ ", stdout);
		output_alt(context, term->u.group);
		fputs("}; ", stdout);
	}
}

static void
output_term(struct context* context, const struct ast_term *term)
{
	assert(term != NULL);
	assert(!term->invisible);

	/* SID cannot express term repetition; TODO: semantic checks for this */
	/* TODO: can output repetition as [ ... ] local rules with a stub to call them X times? */
	assert(term->min <= 1);
	assert(term->max <= 1);

	if (term->min == 0) {
		fputs("{ $$; || ", stdout);
	}

	output_basic(context, term);

	if (term->min == 0) {
		fputs("}; ", stdout);
	}
}

static void
output_alt(struct context* context, const struct ast_alt *alt)
{
	const struct ast_term *term;

	assert(alt != NULL);
	assert(!alt->invisible);

	for (term = alt->terms; term != NULL; term = term->next) {
		output_term(context, term);
	}
}

static void
output_rule(struct context* context, const struct ast_rule *rule)
{
	const struct ast_alt *alt;

	fprintf(context->out,"\t%s = {\n\t\t", rule->name);

	for (alt = rule->alts; alt != NULL; alt = alt->next) {
		output_alt(context, alt);

		fprintf(context->out,"\n");

		if (alt->next != NULL) {
			fprintf(context->out,"\t||\n\t\t");
		}
	}

	fprintf(context->out,"\t};\n\n");
}

static int
is_equal(struct context* context, const struct ast_term *a, const struct ast_term *b)
{
	if (a->type != b->type) {
		return 0;
	}

	switch (a->type) {
	case TYPE_EMPTY:      return 1;
	case TYPE_RULE:       return 0 == strcmp(a->u.rule->name,    b->u.rule->name);
	case TYPE_CI_LITERAL: return 0 == txtcasecmp(&a->u.literal, &b->u.literal);
	case TYPE_CS_LITERAL: return 0 == txtcmp(&a->u.literal,     &b->u.literal);
	case TYPE_TOKEN:      return 0 == strcmp(a->u.token,         b->u.token);
	case TYPE_PROSE:      return 0 == strcmp(a->u.prose,         b->u.prose);

	case TYPE_GROUP:
    context->reached_unimplemented = true;
    return false;
	}

	assert(!"unreached");
}

static void
output_terminals(struct context* context, const struct ast_rule *grammar)
{
	const struct ast_rule *p;
	struct ast_term *found = NULL;

	/* List terminals */
	for (p = grammar; p != NULL; p = p->next) {
		struct ast_alt *alt;

		for (alt = p->alts; alt != NULL; alt = alt->next) {
			const struct ast_term *term;
			struct ast_term *t;

			assert(alt!= NULL);
			assert(!alt->invisible);

			for (term = alt->terms; term != NULL; term = term->next) {
				assert(term!= NULL);
				assert(!term->invisible);

				switch (term->type) {
				case TYPE_EMPTY:
				case TYPE_GROUP:
					continue;

				case TYPE_RULE:
				case TYPE_TOKEN:
				case TYPE_PROSE:
					continue;

				case TYPE_CI_LITERAL:
				case TYPE_CS_LITERAL:
					break;
				}

				for (t = found; t != NULL; t = t->next) {
					if (is_equal(context, t, term)) {
						break;
					}
				}

				if (t != NULL) {
					continue;
				}

				t = xmalloc(sizeof *t);
				t->u.literal = term->u.literal;
				t->type = term->type;
				t->invisible = 0;
				t->next = found;
				found = t;
			}
		}
	}

	/* Output list */
	{
		struct ast_term *next;
		struct ast_term *t;

		for (t = found; t != NULL; t = next) {
			next = t->next;
			fprintf(context->out,"\t");
			output_basic(context, t);
			fprintf(context->out,"\n");
			free(t);
		}
	}
}

void
sid_output(struct context* context, const struct ast_rule *grammar)
{
	const struct ast_rule *p;

	output_section(context, "types");

	output_section(context, "terminals");

	output_terminals(context, grammar);

	output_section(context, "rules");

	/* TODO list rule declartations */

	for (p = grammar; p != NULL; p = p->next) {
		output_rule(context, p);
	}

	output_section(context, "entry");

	fprintf(context->out,"\t%s;\n\n", grammar->name);
}

