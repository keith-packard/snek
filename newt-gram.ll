#
#  Copyright © 2019 Keith Packard <keithp@keithp.com>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  General Public License for more details.
#
start		: command start
		|
		;
command		: stat
			@{
				newt_code_t *code = newt_code_finish();
				newt_poly_t p = newt_code_run(code);
				if (newt_abort)
					return parse_return_error;
				if (newt_print_val && newt_print_vals) {
					newt_poly_print(stdout, p);
					putchar('\n');
					newt_print_val = false;
				}
			}@
		| DEF
			@{
				nformal = 0;
				newt_code_add_op_offset(newt_op_line, newt_token_val.ints);
			}@
		  NAME
			@{
				value_push(newt_token_val);
	 		}@
		  OP opt-formals CP COLON suite
			@{
				newt_code_t	*code = newt_code_finish();
				if (!code)
					break;
				newt_func_t	*func = newt_func_alloc(code, nformal, formals);
				if (!func)
					break;
				newt_poly_t	poly = newt_func_to_poly(func);
				newt_id_t	id = value_pop().id;

				newt_poly_stash(poly);
				newt_poly_t *ref = newt_id_ref(id, true);
				poly = newt_poly_fetch();
				if (ref)
					*ref = poly;
			}@
		| DEL
			@{
				nformal = 0;
			}@
		  formals
			@{
				uint8_t i;
				for (i = 0; i < nformal; i++)
					if (!newt_id_del(formals[i])) {
						newt_error("%i: undefined", formals[i]);
						break;
					}
			}@
		| IMPORT NAME
		;
opt-formals	: formals
		|
		;
formals		: formal formals-p
		;
formals-p	: COMMA formal formals-p
		|
		;
formal		: NAME
			@{
				formals[nformal++] = newt_token_val.id;
			}@
		;
stats		: stat stats-p
		;
stats-p		: stat stats-p
		|
		;
stat		: simple-stat
			@{ newt_print_val = true; }@
		| compound-stat
		| NL
		;
simple-stat	: small-stat small-stats-p nl
		;
small-stats-p	: SEMI small-stat small-stats-p
		|
		;
small-stat	: assign-expr
		| RETURN expr
			@{ newt_code_add_forward(newt_forward_return); }@
		| BREAK
			@{ newt_code_add_forward(newt_forward_break); }@
		| CONTINUE
			@{ newt_code_add_forward(newt_forward_continue); }@
		| GLOBAL globals
		;
assign-expr	: expr assign-expr-p
		;
assign-expr-p	: ASSIGN
			@{
				newt_offset_t prev_offset = newt_code_prev_insn();
				uint8_t *prev = newt_code_at(prev_offset);
				newt_id_t id;

				/* look at the previous instruction to figure out what
				 * it was -- an id or an array. All others are illegal
				 */
				switch (*prev) {
				case newt_op_id:
					memcpy(&id, prev + 1, sizeof (newt_id_t));
					break;
				case newt_op_array:
					newt_code_set_push(newt_code_prev_prev_insn());
					id = NEWT_ID_NONE;
					break;
				default:
					return parse_return_syntax;
				}
				value_push_id(id);
				value_push_op(newt_token_val.op);

				/* eliminate the previous instruction having
				 * extracted the useful information from it
				 */
				newt_code_delete_prev();
			}@
		  expr
			@{
				newt_op_t op = value_pop().op;
				newt_id_t id = value_pop().id;
				
				/* add the assignment operator */
				newt_code_add_op_id(op, id);
			}@
		|
		;
globals		: NAME
			@{
				newt_code_add_op_id(newt_op_global, newt_token_val.id);
			}@
		  globals
		|
		;
compound-stat	: if-stat
		| while-stat
		| for-stat
		;
if-stat		: if suite elif-stats
			@{
				int elif_off = value_pop().ints;
				int if_off = value_pop().ints;
				newt_code_patch_branch(if_off, elif_off);
				newt_code_patch_forward(if_off, newt_forward_if, newt_code_current());
			}@
		;
if		: IF expr COLON
			@{
				newt_code_add_op_offset(newt_op_branch_false, 0);
				value_push_int(newt_compile_prev);
			}@
		;
elif-stats	: elif elif_expr suite elif-stats
			@{
				int elif_stats_off = value_pop().ints;
				int elif_expr_off = value_pop().ints;
				int elif_off = value_pop().ints;
				newt_code_patch_branch(elif_expr_off, elif_stats_off);
				value_push_int(elif_off);
			}@
		| else-stat
		;
elif		: ELIF
			@{
				newt_code_add_forward(newt_forward_if);
				value_push_int(newt_code_current());
			}@
		;
elif_expr	: expr COLON
			@{
				newt_code_add_op_offset(newt_op_branch_false, 0);
				value_push_int(newt_compile_prev);
			}@
		;
else-stat	: ELSE COLON
			@{
				newt_code_add_forward(newt_forward_if);
				value_push_int(newt_code_current());
			}@
		  suite
		|
			@{
				value_push_int(newt_code_current());
			}@
		;
while-stat	:
			@{
				/* push 0 - top_off */
				value_push_int(newt_code_current());
			}@
		  WHILE expr COLON
			@{
				/* push 1 - while_off */
				newt_code_add_op_offset(newt_op_branch_false, 0);
				value_push_int(newt_compile_prev);
			}@
		  suite
			@{
				/* push 2 - loop_end_off */
				newt_code_add_op_offset(newt_op_branch, 0);
				value_push_int(newt_compile_prev);
				/* push 3 - while_else_stat_off */
				value_push_int(newt_code_current());
			}@
		  while-else-stat
			@{
				int while_else_stat_off = value_pop().ints;
				int loop_end_off = value_pop().ints;
				int while_off = value_pop().ints;
				int top_off = value_pop().ints;

				newt_code_patch_branch(while_off, while_else_stat_off);
				newt_code_patch_branch(loop_end_off, top_off);
				newt_code_patch_forward(while_off, newt_forward_continue, top_off);
				newt_code_patch_forward(while_off, newt_forward_break, newt_code_current());
			}@
		;
while-else-stat	: ELSE COLON suite
		|
		;
for-stat	: for suite
			@{
				/* push 1 - loop_end_off */
				newt_code_add_op_offset(newt_op_branch, 0);
				value_push_int(newt_compile_prev);
				/* push 2 - while_else_stat_off */
				value_push_int(newt_code_current());
			}@
		  while-else-stat
			@{
				int while_else_stat_off = value_pop().ints;
				int loop_end_off = value_pop().ints;
				int for_off = value_pop().ints;

				newt_code_patch_branch(for_off, while_else_stat_off);
				newt_code_patch_branch(loop_end_off, for_off);
				newt_code_patch_forward(for_off, newt_forward_continue, for_off);
				newt_code_patch_forward(for_off, newt_forward_break, newt_code_current());
			}@
		;
for		: FOR NAME
			@{
				value_push(newt_token_val);
			}@
		  IN for-p
		;
for-p		: RANGE
			@{
			}@
		  OP actuals CP COLON
			@{
				int num = value_pop().ints;
				newt_id_t id = value_pop().id;
				newt_code_add_range_start(id, num);
				/* push 0 - for_off */
				newt_code_add_op_offset(newt_op_range_step, 0);
				value_push_int(newt_compile_prev);
			}@
		| expr COLON
			@{
				newt_id_t id = value_pop().id;
				newt_code_set_push(newt_code_prev_insn());
				newt_code_add_op_id(newt_op_in_start, id);
				/* push 0 - for_off */
				newt_code_add_op_offset(newt_op_in_step, 0);
				value_push_int(newt_compile_prev);
			}@
		;
suite		: simple-stat
		| nl INDENT
			@{
				value_push(newt_token_val);
			}@
		  stats EXDENT
			@{
				int indent = value_pop().ints;
				int exdent = newt_token_val.ints;

				newt_current_indent = indent;
				if (exdent > newt_current_indent) {
					newt_error("mismatching indentation indent %d exdent %d", indent, exdent);
					return parse_return_syntax;
				}
			}@
		;
expr		: expr-and expr-or-p
		;
expr-or-p	: OR
			@{
				newt_code_add_op_offset(newt_op_branch_false, 0);
				value_push_int(newt_compile_prev);
			}@
		  expr-and
			@{
				newt_code_patch_branch(value_pop().ints, newt_code_current());
			}@
		  expr-or-p
		|
		;
expr-and	: expr-not expr-and-p
		;
expr-and-p	: AND
			@{
				newt_code_add_op_offset(newt_op_branch_true, 0);
				value_push_int(newt_compile_prev);
			}@
		  expr-not
			@{
				newt_code_patch_branch(value_pop().ints, newt_code_current());
			}@
		  expr-and-p
		|
		;
expr-not	: expr-cmp
		| NOT
			@{
			unop_first:
				value_push(newt_token_val);
			}@
		  expr-not
			@{
			unop_second:
				newt_code_add_op(value_pop().op);
			}@
		;
expr-cmp	: expr-lor expr-cmp-p
		;
expr-cmp-p	: CMPOP
			@{
			binop_first:
				newt_code_set_push(newt_code_prev_insn());
				value_push(newt_token_val);
			}@
		  expr-lor
			@{
			binop_second:
				newt_code_add_op(value_pop().op);
			}@
		  expr-cmp-p
		| IS @ goto binop_first; @ expr-lor @ goto binop_second; @ expr-cmp-p
		|
		;
expr-lor	: expr-land expr-lor-p
		;
expr-lor-p	: LOR @ goto binop_first; @ expr-land @ goto binop_second; @ expr-lor-p
		|
		;
expr-land	: expr-lxor expr-land-p
		;
expr-land-p	: LAND @ goto binop_first; @ expr-lxor @ goto binop_second; @ expr-land-p
		|
		;
expr-lxor	: expr-shift expr-lxor-p
		;
expr-lxor-p	: LXOR @ goto binop_first; @ expr-shift @ goto binop_second; @ expr-lxor-p
		|
		;
expr-shift	: expr-add expr-shift-p
		;
expr-shift-p	: SHIFT @ goto binop_first; @ expr-add @ goto binop_second; @ expr-shift-p
		|
		;
expr-add	: expr-mul expr-add-p
		;
expr-add-p	: ADDOP @ goto binop_first; @ expr-mul @ goto binop_second; @ expr-add-p
		|
		;
expr-mul	: expr-unary expr-mul-p
		;
expr-mul-p	: MULOP @ goto binop_first; @ expr-unary @ goto binop_second; @ expr-mul-p
		|
		;
expr-unary	: LNOT @ goto unop_first; @ expr-unary @ goto unop_second; @
		| ADDOP @ goto unop_first; @ expr-unary @ goto unop_second; @
		| expr-pow
		;

# this needs to be right associative instead of left associative...

expr-pow	: expr-array expr-pow-p
		;

expr-pow-p	: POW @ goto binop_first; @ expr-array expr-pow-p @ goto binop_second; @
		|
		;
expr-array	: expr-prim expr-array-p
		;
expr-array-p	: OS
			@{
				newt_code_set_push(newt_code_prev_insn());
				++newt_ignore_nl;
			}@
		  array-index CS
			@{
				bool slice = value_pop().bools;
				if (slice) {
					newt_code_set_push(newt_code_prev_insn());
					bool stride = value_pop().bools;
					bool end = value_pop().bools;
					bool start = value_pop().bools;
					newt_code_add_slice(start, end, stride);
				} else {
					newt_code_add_op(newt_op_array);
				}
				--newt_ignore_nl;
 			}@
		  expr-array-p
		| OP
			@{
				++newt_ignore_nl;
				newt_code_set_push(newt_code_prev_insn());
			}@
		  opt-actuals CP
		        @{
				--newt_ignore_nl;
				newt_code_add_op_offset(newt_op_call, value_pop().ints);
			}@
		  expr-array-p
		|
		;
array-index	: expr opt-slice
		|
			@{ value_push_bool(false); }@
		  slice
		;
opt-slice	:
			@{ value_push_bool(true); }@
		  slice
		|
			@{ value_push_bool(false); }@
		;
slice		: COLON opt-expr slice-p
			@{ value_push_bool(true); }@
		;
slice-p		: COLON opt-expr
		|
			@{ value_push_bool(false); }@
		;
opt-expr	:	@{
				newt_code_set_push(newt_code_prev_insn());
				value_push_bool(true);
			}@
		  expr
		|
			@{
				value_push_bool(false);
			}@
		;
expr-prim	: OP @{ ++newt_ignore_nl; }@ opt-tuple CP @{ --newt_ignore_nl; }@
			@{
				bool tuple = value_pop().bools;

				if (tuple) {
					int num = value_pop().ints + 1;
					newt_code_add_op_offset(newt_op_tuple, num);
				}
			}@
		| OS @{ ++newt_ignore_nl; }@ opt-actuals CS @{ --newt_ignore_nl; }@
			@{
				int num = value_pop().ints;
				newt_code_add_op_offset(newt_op_list, num);
			}@
		| NAME
			@{
				newt_code_add_op_id(newt_op_id, newt_token_val.id);
			}@
		| NUMBER
			@{
				newt_code_add_number(newt_token_val.number);
			}@
		| STRING
			@{
				newt_code_add_string(newt_token_val.string);
			}@
		;
opt-tuple	: expr opt-tuple-p
		|
			@{
				value_push_int(-1);
				value_push_bool(true);
			}@
		;
opt-tuple-p	: COMMA
			@{
				newt_code_set_push(newt_code_prev_insn());
			}@
		  opt-actuals
			@{ value_push_bool(true); }@
		|
			@{ value_push_bool(false); }@
		;
opt-actuals	: actuals
		|
			@{
				value_push_int(0);
			}@
		;
actuals		: expr
			@{
				value_push_int(1);
				newt_code_set_push(newt_code_prev_insn());
			}@
		  actuals-p
		;
actuals-p	: COMMA expr
			@{
				value_push_int(value_pop().ints + 1);
				newt_code_set_push(newt_code_prev_insn());
			}@
		  actuals-p
		|
		;
nl		: NL
			@{
				newt_code_add_op_offset(newt_op_line, newt_token_val.ints);
			}@
		;
