/*
 * Copyright 2014-2017 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

/*
 * Wirth Syntax Notation Output.
 *
 * TODO: fprintf(fout), instead of stdout
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../txt.h"
#include "../ast.h"
#include "../context.h"

#include "io.h"

static void output_alt(struct context* context, const struct ast_alt *alt);

static void
output_term(struct context* context, const struct ast_term *term)
{
	const char *s, *e;
	size_t i;

	struct {
		unsigned int min;
		unsigned int max;
		const char *s;
		const char *e;
	} a[] = {
		{ 1, 1, " ( ", " )" },
		{ 1, 1, "",    ""   },
		{ 0, 1, " [ ", " ]" },
		{ 0, 0, " { ", " }" }
	};

	assert(term != NULL);
	assert(!term->invisible);

	s = NULL;
	e = NULL;

	for (i = 0; i < sizeof a / sizeof *a; i++) {
		if (i == 0 && term->type != TYPE_GROUP) {
			continue;
		}

		if (term->min == a[i].min && term->min == a[i].min) {
			s = a[i].s;
			e = a[i].e;
			break;
		}
	}

	/* TODO: for {1,0} output first term inline */

	assert(s != NULL && e != NULL);

	/* EBNF cannot express minimum term repetition; TODO: semantic checks for this */
	assert(term->min <= 1);
	assert(term->max <= 1);

	fprintf(context->out,"%s", s);

	switch (term->type) {
	case TYPE_EMPTY:
		fputs(" \"\"", stdout);
		break;

	case TYPE_RULE:
		fprintf(context->out," %s", term->u.rule->name);
		break;

	case TYPE_CI_LITERAL:
    context->reached_unimplemented = true;
    return;

	case TYPE_CS_LITERAL: {
			size_t i;

			fputs(" \"", stdout);
			for (i = 0; i < term->u.literal.n; i++) {
				if (term->u.literal.p[i] == '\"') {
					fputs("\"\"", stdout);
				} else {
					putc(term->u.literal.p[i], stdout);
				}
			}
			putc('\"', stdout);
		}
		break;

	case TYPE_TOKEN:
		fprintf(context->out," %s", term->u.token);
		break;

	case TYPE_PROSE:
    context->reached_unimplemented = true;
    return;

	case TYPE_GROUP:
		output_alt(context, term->u.group);
		break;
	}

	fprintf(context->out,"%s", e);
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

	alt = rule->alts;
	fprintf(context->out,"%s =", rule->name);
	output_alt(context, alt);

	for (alt = alt->next; alt != NULL; alt = alt->next) {
		fprintf(context->out,"\n\t|");
		output_alt(context, alt);
	}

	fprintf(context->out," .\n\n");
}

void
wsn_output(struct context* context, const struct ast_rule *grammar)
{
	const struct ast_rule *p;

	for (p = grammar; p != NULL; p = p->next) {
		output_rule(context, p);
	}
}

