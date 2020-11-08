/*
 * Copyright 2014-2017 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef KGT_DOT_IO_H
#define KGT_DOT_IO_H

struct ast_rule;
struct context;

void
dot_output(struct context* context, const struct ast_rule *grammar);

#endif

