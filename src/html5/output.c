/*
 * Copyright 2014-2019 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

/*
 * Railroad Diagram HTML5 Output
 *
 * Output a HTML page of inlined SVG diagrams
 */

#define _BSD_SOURCE
#define _DEFAULT_SOURCE

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "../txt.h"
#include "../ast.h"
#include "../xalloc.h"
#include "../context.h"

#include "../rrd/rrd.h"
#include "../rrd/pretty.h"
#include "../rrd/node.h"
#include "../rrd/rrd.h"
#include "../rrd/list.h"
#include "../rrd/tnode.h"

#include "io.h"

/* XXX */
extern struct dim svg_dim;
void svg_render_rule(struct context* context, const struct tnode *node,
    const char *base, const struct ast_rule *grammar);

extern const char *css_file;

void
cat(const char *in, const char *indent)
{
	FILE *f;

	char buf[BUFSIZ];

	f = fopen(in, "r");
	if (f == NULL) {
		perror(in);
		exit(EXIT_FAILURE);
	}

	fputs(indent, stdout);

	for (;;) {
		buf[sizeof buf - 1] = 'x';

		if (!fgets(buf, sizeof buf, f)) {
			break;
		}

		fputs(buf, stdout);

		if (buf[sizeof buf - 1] != '\0' || buf[sizeof buf - 2] == '\n') {
			fputs(indent, stdout);
		}
	}

	(void) fclose(f);
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

	fprintf(context->out,"      rect, line, path { stroke-width: 1.5px; stroke: black; fill: transparent; }\n");
	fprintf(context->out,"      rect, line, path { stroke-linecap: square; stroke-linejoin: rounded; }\n");
	fprintf(context->out,"      path { fill: transparent; }\n");
	fprintf(context->out,"      text.literal { font-family: monospace; }\n");
	fprintf(context->out,"      a { fill: blue; }\n");
	fprintf(context->out,"      a:hover rect { fill: aliceblue; }\n");
	fprintf(context->out,"      h2 { font-size: inherit; font-weight: inherit; }\n");
	fprintf(context->out,"      line.ellipsis { stroke-dasharray: 1 3.5; }\n");
	fprintf(context->out,"      tspan.hex { font-family: monospace; font-size: 90%%; }\n");
	fprintf(context->out,"      path.arrow { fill: black; }\n");
	fprintf(context->out,"      svg { margin-left: 30px; }\n");

	if (css_file != NULL) {
		cat(css_file, "      ");
	}

	fprintf(context->out,"  </style>\n");

	fprintf(context->out," </head>\n");
	fprintf(context->out,"\n");

	fprintf(context->out," <body>\n");

	for (p = grammar; p; p = p->next) {
		struct tnode *tnode;
		struct node *rrd;
		unsigned h, w;

		if (!ast_to_rrd(p, &rrd)) {
			perror("ast_to_rrd");
			return;
		}

		if (prettify) {
			rrd_pretty(&rrd);
		}

		tnode = rrd_to_tnode(rrd, &svg_dim);

		node_free(rrd);

		fprintf(context->out," <section>\n");
		fprintf(context->out,"  <h2><a name='%s'>%s:</a></h2>\n",
			p->name, p->name);

		h = (tnode->a + tnode->d + 1) * 10 + 5;
		w = (tnode->w + 6) * 10;

		fprintf(context->out,"  <svg");
		if (xml) {
			fprintf(context->out," xmlns='http://www.w3.org/2000/svg'");
		}
		fprintf(context->out," height='%u' width='%u'>\n", h, w);
		svg_render_rule(context, tnode, "", grammar);
		fprintf(context->out,"  </svg>\n");

		fprintf(context->out," </section>\n");
		fprintf(context->out,"\n");

		tnode_free(tnode);
	}

	fprintf(context->out," </body>\n");
}

void
html5_output(struct context* context, const struct ast_rule *grammar)
{
	fprintf(context->out,"<!DOCTYPE html>\n");
	fprintf(context->out,"<html>\n");
	fprintf(context->out,"\n");

	output(context, grammar, 0);

	fprintf(context->out,"</html>\n");
}

void
xhtml5_output(struct context* context, const struct ast_rule *grammar)
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

