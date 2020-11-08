/*
 * Copyright 2020 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#ifndef KGT_CONTEXT_H
#define KGT_CONTEXT_H

#include <stdbool.h>
#include <stdio.h>

/*
 * A context is passed to the output function.
 * It holds states to configure the output. It also hold flags set by thoses
 * function about the success of the operations.
 */
struct context {
  bool reached_unimplemented;

  // TODO(arthursonzogni): Implement correctly file_{output, error} to give the
  // ability to select the non default ones.
  FILE *out;
  FILE *error;

  // TODO(arthursonzogni): Consider adding something about the allocations.
};

struct context default_context();

#endif // KGT_CONTEXT_H
