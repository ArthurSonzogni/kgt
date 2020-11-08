/*
 * Copyright 2020 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

#include "context.h"

struct context default_context() {
  struct context ret;
  ret.reached_unimplemented = false;
  ret.out = stdout;
  ret.error = stderr;
  return ret;
}
