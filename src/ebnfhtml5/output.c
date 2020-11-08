/*
 * Copyright 2014-2019 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

/*
 * Extended Backus-Naur Form Output, pretty-printed to HTML.
 *
 * This is my own made-up dialect. It's intended for ease
 * of human consumption (i.e. in documentation), rather than
 * to be parsed by machine.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "../txt.h"
#include "../ast.h"
#include "../context.h"
#include "../rrd/node.h"

#include "io.h"

extern const char *css_file;

void
cat(const char *in, const char *indent);

static void output_alt(struct context* context, const struct ast_alt *alt);

/* TODO: centralise */
static int
xml_escputc(FILE *f, char c)
{
	const char *name;

	assert(f != NULL);

	switch (c) {
	case '&': return fputs("&amp;", f);
	case '<': return fputs("&lt;", f);
	case '>': return fputs("&gt;", f);

	case '\a': name = "BEL"; break;
	case '\b': name = "BS";  break;
	case '\f': name = "FF";  break;
	case '\n': name = "LF";  break;
	case '\r': name = "CR";  break;
	case '\t': name = "TAB"; break;
	case '\v': name = "VT";  break;

	default:
		if (!isprint((unsigned char) c)) {
			return fprintf(f, "&#x3008;<tspan class='hex'>%02X</tspan>&#x3009;", (unsigned char) c);
		}

		return fprintf(f, "%c", c);
	}

	return fprintf(f, "&#x3008;<tspan class='esc'>%s</tspan>&#x3009;", name);
}

static int
atomic(const struct ast_term *term)
{
	assert(term != NULL);

	switch (term->type) {
	case TYPE_EMPTY:
	case TYPE_RULE:
	case TYPE_CI_LITERAL:
	case TYPE_CS_LITERAL:
	case TYPE_TOKEN:
	case TYPE_PROSE:
		return 1;

	case TYPE_GROUP:
		if (term->u.group->next != NULL) {
			return 0;
		}

		if (term->u.group->terms->next != NULL) {
			return 0;
		}

		return atomic(term->u.group->terms);
	}

	assert(!"unreached");
}

static const char *
rep(unsigned min, unsigned max)
{
	if (min == 1 && max == 1) {
		/* no operator */
		return "\0";
	}

	if (min == 1 && max == 1) {
		return "()";
	}

	if (min == 0 && max == 1) {
		return "[]";
	}

	if (min == 0 && max == 0) {
		return "{}";
	}

	return "()";
}

static void
output_literal(struct context* context, const char *prefix, const struct txt *t)
{
	size_t i;

	assert(t != NULL);
	assert(t->p != NULL);

	fprintf(context->out,"<tt class='literal %s'>&quot;", prefix);

	for (i = 0; i < t->n; i++) {
		xml_escputc(stdout, t->p[i]);
	}

	fprintf(context->out,"&quot;</tt>");
}

static void
output_term(struct context* context, const struct ast_term *term)
{
	const char *r;

	assert(term != NULL);
	assert(!term->invisible);

	r = rep(term->min, term->max);

	if (!r[0] && !atomic(term)) {
		r = "()";
	}

	if (r[0]) {
		fprintf(context->out,"<span class='rep'>%c</span> ", r[0]);
	}

	/* TODO: escaping */

	switch (term->type) {
	case TYPE_EMPTY:
		fprintf(context->out,"<span class='empty'>&epsilon;</span>");
		break;

	case TYPE_RULE:
		fprintf(context->out,"<a href='#%s' class='rule' data-min='%u' data-max='%u'>",
			term->u.rule->name, term->min, term->max);
		fprintf(context->out,"%s", term->u.rule->name);
		fprintf(context->out,"</a>");
		break;

	case TYPE_CI_LITERAL:
		output_literal(context, "ci", &term->u.literal);
		break;

	case TYPE_CS_LITERAL:
		output_literal(context, "cs", &term->u.literal);
		break;

	case TYPE_TOKEN:
		fprintf(context->out,"<span class='token'>");
		fprintf(context->out,"%s", term->u.token);
		fprintf(context->out,"</span>");
		break;

	case TYPE_PROSE:
		fprintf(context->out,"<span class='prose'>");
		fprintf(context->out,"%s", term->u.prose);
		fprintf(context->out,"</span>");
		break;

	case TYPE_GROUP: {
			const struct ast_alt *alt;

			for (alt = term->u.group; alt != NULL; alt = alt->next) {
				output_alt(context, alt);

				if (alt->next != NULL) {
					fprintf(context->out,"<span class='pipe'> | </span>");
				}
			}
		}

		break;
	}

	if (r[0]) {
		fprintf(context->out," <span class='rep'>%c</span>", r[1]);
	}

	if (term->max > 1) {
		fprintf(context->out,"<sub class='rep'>{%u, %u}</sub>", term->min, term->max);
	}
}

static void
output_alt(struct context* context, const struct ast_alt *alt)
{
	const struct ast_term *term;

	assert(alt != NULL);
	assert(!alt->invisible);

	for (term = alt->terms; term != NULL; term = term->next) {
		fprintf(context->out, "<span class='alt'>");
		output_term(context, term);
		fprintf(context->out,"</span>\n");

		if (term->next != NULL) {
			fprintf(context->out,"<span class='cat'> </span>");
		}
	}
}

static void
output_rule(struct context* context, const struct ast_rule *rule)
{
	const struct ast_alt *alt;

	fprintf(context->out,"  <dl class='bnf'>\n");

	fprintf(context->out,"    <dt>");
	fprintf(context->out,"<a name='%s'>", rule->name);
	fprintf(context->out,"%s", rule->name);
	fprintf(context->out,"</a>:");
	fprintf(context->out,"</dt>\n");

	fprintf(context->out,"    <dd>");

	for (alt = rule->alts; alt != NULL; alt = alt->next) {
		if (alt != rule->alts) {
			fprintf(context->out,"<span class='pipe'> | </span>");
		}

		fprintf(context->out,"\n");
		fprintf(context->out,"      ");
		output_alt(context, alt);

		if (alt->next != NULL) {
			fprintf(context->out,"<br/>\n");
			fprintf(context->out,"      ");
		}
	}

	fprintf(context->out,"    </dd>\n");

	fprintf(context->out,"  </dl>\n");
	fprintf(context->out,"\n");
}

static void
output(struct context* context, const struct ast_rule *grammar, int xml)
{
	const struct ast_rule *p;

	fprintf(context->out," <head>\n");
	if (xml) {
		fprintf(context->out,"  <meta charset='UTF-8'/>\n");
	}

	fprintf(context->out,"  <style>\n");

	fprintf(context->out,"    dl.bnf span.token {\n");
	fprintf(context->out,"    	text-transform: uppercase;\n");
	fprintf(context->out,"    }\n");
	fprintf(context->out,"    \n");
	fprintf(context->out,"    dl.bnf span.cat {\n");
	fprintf(context->out,"    	margin-right: 0.5ex;\n");
	fprintf(context->out,"    }\n");
	fprintf(context->out,"    \n");
	fprintf(context->out,"    dl.bnf dd > span.pipe {\n");
	fprintf(context->out,"    	float: left;\n");
	fprintf(context->out,"    	width: 1ex;\n");
	fprintf(context->out,"    	margin-left: -1.8ex;\n");
	fprintf(context->out,"    	text-align: right;\n");
	fprintf(context->out,"    	padding-right: .8ex; /* about the width of a space */\n");
	fprintf(context->out,"    }\n");
	fprintf(context->out,"    \n");
	fprintf(context->out,"    dl.bnf dt {\n");
	fprintf(context->out,"    	display: block;\n");
	fprintf(context->out,"    	min-width: 8em;\n");
	fprintf(context->out,"    	padding-right: 1em;\n");
	fprintf(context->out,"    }\n");
	fprintf(context->out,"    \n");
	fprintf(context->out,"    dl.bnf a.rule {\n");
	fprintf(context->out,"    	text-decoration: none;\n");
	fprintf(context->out,"    }\n");
	fprintf(context->out,"    \n");
	fprintf(context->out,"    dl.bnf a.rule:hover {\n");
	fprintf(context->out,"    	text-decoration: underline;\n");
	fprintf(context->out,"    }\n");
	fprintf(context->out,"    \n");
	fprintf(context->out,"    /* page stuff */\n");
	fprintf(context->out,"    dl.bnf { margin: 2em 4em; }\n");
	fprintf(context->out,"    dl.bnf dt { margin: 0.25em 0; }\n");
	fprintf(context->out,"    dl.bnf dd { margin-left: 2em; }\n");

	if (css_file != NULL) {
		cat(css_file, "    ");
	}

	fprintf(context->out,"  </style>\n");

	fprintf(context->out," </head>\n");

	fprintf(context->out," <body>\n");

	for (p = grammar; p != NULL; p = p->next) {
		output_rule(context, p);
	}

	fprintf(context->out," </body>\n");
}

void
ebnf_html5_output(struct context* context, const struct ast_rule *grammar)
{
	fprintf(context->out,"<!DOCTYPE html>\n");
	fprintf(context->out,"<html>\n");
	fprintf(context->out,"\n");

	output(context, grammar, 0);

	fprintf(context->out,"</html>\n");
}

void
ebnf_xhtml5_output(struct context* context, const struct ast_rule *grammar)
{
	fprintf(context->out,"<?xml version='1.0' encoding='utf-8'?>\n");
	fprintf(context->out,"<!DOCTYPE html>\n");
	fprintf(context->out,"<html xml:lang='en' lang='en'\n");
	fprintf(context->out,"  xmlns='http://www.w3.org/1999/xhtml'\n");
	fprintf(context->out,"  xmlns:xlink='http://www.w3.org/1999/xlink'>\n");
	fprintf(context->out,"\n");

	output(context, grammar, 1);

	fprintf(context->out,"</html>\n");
}

