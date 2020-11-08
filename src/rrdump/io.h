/*
 * Copyright 2014-2017 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef KGT_RRDUMP_IO_H
#define KGT_RRDUMP_IO_H

struct ast_rule;
struct context;

extern int prettify;

void
rrdump_output(struct context* context, const struct ast_rule*);

#endif
