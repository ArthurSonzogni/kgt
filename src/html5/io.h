/*
 * Copyright 2019 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef KGT_HTML5_IO_H
#define KGT_HTML5_IO_H

struct ast_rule;
struct context;

extern int prettify;

void
html5_output(struct context* context, const struct ast_rule*);

void
xhtml5_output(struct context* context, const struct ast_rule*);

#endif
