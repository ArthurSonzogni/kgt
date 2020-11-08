/*
 * Copyright 2014-2017 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

/*
 * Abstract Railroad Diagram tree dump to Graphivz
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../txt.h"
#include "../ast.h"
#include "../context.h"

#include "../rrd/rrd.h"
#include "../rrd/pretty.h"
#include "../rrd/node.h"
#include "../rrd/list.h"

#include "io.h"

static int
escputc(int c, FILE *f)
{
	size_t i;

	const struct {
		int c;
		const char *s;
	} a[] = {
		{ '\\', "\\\\"   },
		{ '&',  "&amp;"  },
		{ '\"', "&quot;" },
		{ '<',  "&#x3C;" },
		{ '>',  "&#x3E;" }
	};

	assert(f != NULL);

	for (i = 0; i < sizeof a / sizeof *a; i++) {
		if (a[i].c == c) {
			return fputs(a[i].s, f);
		}
	}

	if (!isprint((unsigned char) c)) {
		return fprintf(f, "&#x%X;", (unsigned char) c);
	}

	return putc(c, f);
}

static int
escputs(const char *s, FILE *f)
{
	const char *p;
	int r;

	assert(f != NULL);
	assert(s != NULL);

	for (p = s; *p != '\0'; p++) {
		r = escputc(*p, f);
		if (r < 0) {
			return -1;
		}
	}

	return 0;
}

static int
escputt(const struct txt *t, FILE *f)
{
	size_t i;
	int r;

	assert(t != NULL);
	assert(t->p != NULL);

	for (i = 0; i < t->n; i++) {
		r = escputc(t->p[i], f);
		if (r < 0) {
			return -1;
		}
	}

	return 0;
}

static void
rrd_print_dot(struct context* context, const char *prefix, const void *parent, const char *port,
	const struct node *node)
{
	if (node == NULL) {
		return;
	}

	switch (node->type) {
		const struct list *p;

	case NODE_ALT:
	case NODE_ALT_SKIPPABLE:
		fprintf(context->out,"\t{ rank = same;\n");
		for (p = node->u.alt; p != NULL; p = p->next) {
			fprintf(context->out,"\t\t\"%s/%p\";\n", prefix, (void *) p->node);
		}
		fprintf(context->out,"\t};\n");
		break;

	case NODE_SEQ:
		fprintf(context->out,"\t{ rank = same;\n");
		for (p = node->u.seq; p != NULL; p = p->next) {
			fprintf(context->out,"\t\t\"%s/%p\";\n", prefix, (void *) p->node);
		}
		fprintf(context->out,"\t};\n");
		break;

	default:
		break;
	}

	fprintf(context->out,"\t\"%s/%p\"%s -> \"%s/%p\"",
		prefix, parent, port,
		prefix, (void *) node);
	if (node->invisible) {
		fprintf(context->out," [ color = blue, style = dashed ]");
	}
	fprintf(context->out,";\n");

	fprintf(context->out,"\t\"%s/%p\" [ ",
		prefix, (void *) node);
	if (node->invisible) {
		fprintf(context->out,"color = blue, fontcolor = blue, fillcolor = aliceblue, style = \"rounded,dashed\", ");
	}

	switch (node->type) {
	case NODE_CI_LITERAL:
		fprintf(context->out,"style = \"%s\", shape = box, label = \"\\\"",
			node->invisible ? "filled,dashed" : "filled");
		escputt(&node->u.literal, stdout);
		fprintf(context->out,"\\\"\"/i");
		break;

	case NODE_CS_LITERAL:
		fprintf(context->out,"style = \"%s\", shape = box, label = \"\\\"",
			node->invisible ? "filled,dashed" : "filled");
		escputt(&node->u.literal, stdout);
		fprintf(context->out,"\\\"\"");
		break;

	case NODE_RULE:
		fprintf(context->out,"label = \"\\<");
		escputs(node->u.name, stdout);
		fprintf(context->out,"\\>\"");
		break;

	case NODE_PROSE:
		/* TODO: escaping to somehow avoid ? */
		fprintf(context->out,"label = \"?");
		escputs(node->u.prose, stdout);
		fprintf(context->out,"?\"");
		break;

	case NODE_ALT:
		fprintf(context->out,"label = \"ALT\"");
		break;

	case NODE_ALT_SKIPPABLE:
		fprintf(context->out,"label = \"ALT|&epsilon;\"");
		break;

	case NODE_SEQ:
		fprintf(context->out,"label = \"SEQ\"");
		break;

	case NODE_LOOP:
		fprintf(context->out,"label = \"<b> &larr;|LOOP "); /* TODO: utf8 */

		if (node->u.loop.min == 1 && node->u.loop.max == 1) {
			/* nothing */
		} else if (!node->u.loop.max) {
			fprintf(context->out,"\\{%u,""\\}&times;", node->u.loop.min);
		} else if (node->u.loop.min == node->u.loop.max) {
			fprintf(context->out,"%u&times;", node->u.loop.min);
		} else {
			fprintf(context->out,"\\{%u,%u\\}&times;", node->u.loop.min, node->u.loop.max);
		}

		fprintf(context->out,"|<f> &rarr;\"");
		break;

	default:
		fprintf(context->out,"label = \"?\", color = red");
	}

	fprintf(context->out," ];\n");

	switch (node->type) {
		const struct list *p;

	case NODE_ALT:
	case NODE_ALT_SKIPPABLE:
		for (p = node->u.alt; p != NULL; p = p->next) {
			rrd_print_dot(context, prefix, node, "", p->node);
		}
		break;

	case NODE_SEQ:
		for (p = node->u.seq; p != NULL; p = p->next) {
			rrd_print_dot(context, prefix, node, "", p->node);
		}
		break;

	case NODE_LOOP:
		rrd_print_dot(context, prefix, node, ":f", node->u.loop.forward);
		rrd_print_dot(context, prefix, node, ":b", node->u.loop.backward);
		break;

	default:
		break;
	}
}

void
rrdot_output(struct context* context, const struct ast_rule *grammar)
{
	const struct ast_rule *p;

	fprintf(context->out,"digraph G {\n");
	fprintf(context->out,"\tnode [ shape = record, style = rounded ];\n");
	fprintf(context->out,"\tedge [ dir = none ];\n");

	for (p = grammar; p != NULL; p = p->next) {
		struct node *rrd;

		if (!ast_to_rrd(p, &rrd)) {
			perror("ast_to_rrd");
			return;
		}

		if (prettify) {
			rrd_pretty(&rrd);
		}

        fprintf(context->out,"\t\"%s/%p\" [ shape = plaintext, label = \"%s\" ];\n",
			p->name, (void *) p,
			p->name);

		rrd_print_dot(context, p->name, p, "", rrd);

		node_free(rrd);
	}

	fprintf(context->out,"}\n");
	fprintf(context->out,"\n");
}

