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
start		: command @{ snek_parse_middle = false; }@ start
		|
		;
command		: @{ snek_print_val = snek_interactive; }@ stat
			@{
				snek_code_t *code = snek_code_finish();
				snek_poly_t p = snek_code_run(code);
				if (snek_abort)
					return parse_return_error;
				if (snek_print_val) {
					snek_poly_print(stdout, p, 'r');
					putchar('\n');
				}
			}@
		| DEF
			@{
				snek_parse_nformal = 0;
				snek_code_add_op_offset(snek_op_line, snek_line);
			}@
		  NAME
			@{
				value_push_id(snek_token_val.id);
	 		}@
		  OP opt-formals CP COLON suite
			@{
				snek_code_t	*code = snek_code_finish();
				if (!code)
					break;
				snek_func_t	*func = snek_func_alloc(code);
				if (!func)
					break;
				snek_poly_t	poly = snek_func_to_poly(func);
				snek_id_t	id = value_pop().id;

				snek_stack_push(poly);
				snek_poly_t *ref = snek_id_ref(id, true);
				poly = snek_stack_pop();
				if (ref)
					*ref = poly;
			}@
		| DEL
			@{
				snek_parse_nformal = 0;
			}@
		  formals
			@{
				uint8_t i;
				for (i = 0; i < snek_parse_nformal; i++)
					if (!snek_id_del(snek_parse_formals[i])) {
						snek_undefined(snek_parse_formals[i]);
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
				snek_parse_formals[snek_parse_nformal++] = snek_token_val.id;
			}@
		  opt-named-p
		;
opt-named-p	: ASSIGN expr
			@{
				snek_code_add_op_id(snek_op_assign_named, snek_parse_formals[--snek_parse_nformal]);
			}@
		|
		;
opt-stats	: stat opt-stats
		|
		;
stat		: simple-stat
		| @{ snek_print_val = false; }@ compound-stat
		| @{ snek_print_val = false; }@ NL
		;
simple-stat	: small-stat small-stats-p nl
		;
small-stats-p	: SEMI small-stat small-stats-p
		|
		;
small-stat	: assign-expr
		| RETURN expr
			@{ snek_code_add_forward(snek_forward_return); }@
		| BREAK
			@{ snek_code_add_forward(snek_forward_break); }@
		| CONTINUE
			@{ snek_code_add_forward(snek_forward_continue); }@
		| GLOBAL globals
		;
assign-expr	: expr assign-expr-p
		;
assign-expr-p	: ASSIGN
			@{
				snek_print_val = false;
				snek_offset_t prev_offset = snek_code_prev_insn();
				uint8_t *prev = snek_code_at(prev_offset);
				snek_id_t id;

				/* look at the previous instruction to figure out what
				 * it was -- an id or an array. All others are illegal
				 */
				switch (*prev) {
				case snek_op_id:
					memcpy(&id, prev + 1, sizeof (snek_id_t));
					break;
				case snek_op_array:
					snek_code_set_push(snek_code_prev_prev_insn());
					id = SNEK_ID_NONE;
					break;
				default:
					return parse_return_syntax;
				}
				value_push_id(id);
				value_push_offset(snek_token_val.op);

				/* eliminate the previous instruction having
				 * extracted the useful information from it
				 */
				snek_code_delete_prev();
			}@
		  expr
			@{
				snek_op_t op = value_pop().offset;
				snek_id_t id = value_pop().id;
				
				/* add the assignment operator */
				snek_code_add_op_id(op, id);
			}@
		|
		;
globals		: global globals-p
		|
		;
globals-p	: COMMA global globals-p
		|
		;
global		: NAME
			@{
				snek_code_add_op_id(snek_op_global, snek_token_val.id);
			}@
		;
compound-stat	: if-stat
		| while-stat
		| for-stat
		;
if-stat		: IF if-expr suite elif-stats
			@{
				snek_offset_t elif_stats_off = value_pop().offset;
				snek_offset_t if_expr_off = value_pop().offset;
				snek_code_patch_branch(if_expr_off, elif_stats_off);
				snek_code_patch_forward(if_expr_off, snek_compile_size,
							snek_forward_if, snek_code_current());
			}@
		;
elif-stats	: ELIF
			@{
				snek_code_add_forward(snek_forward_if);
				value_push_offset(snek_code_current());
			}@
		  if-expr suite elif-stats
			@{
				snek_offset_t elif_stats_off = value_pop().offset;
				snek_offset_t if_expr_off = value_pop().offset;
				snek_code_patch_branch(if_expr_off, elif_stats_off);
			}@
		| ELSE COLON
			@{
				snek_code_add_forward(snek_forward_if);
				value_push_offset(snek_code_current());
			}@
		  suite
		|
			@{
				/* push 1 - elif_stats_off */
				value_push_offset(snek_code_current());
			}@
		;
if-expr		: expr COLON
			@{
				snek_code_add_op_offset(snek_op_branch_false, 0);

				/* push 0 - if_expr_off */
				value_push_offset(snek_compile_prev);
			}@
		;
while-stat	:
			@{
				/* push 0 - top_off */
				value_push_offset(snek_code_current());
			}@
		  WHILE expr COLON
			@{
				snek_code_add_op_offset(snek_op_branch_false, 0);
				/* push 1 - while_off */
				value_push_offset(snek_compile_prev);
			}@
		  suite
			@{
				/* push 2 - loop_end_off */
				snek_code_add_op_offset(snek_op_branch, 0);
				value_push_offset(snek_compile_prev);
				/* push 3 - while_else_stat_off */
				value_push_offset(snek_code_current());
			}@
		  while-else-stat
			@{
			patch_loop:;
				snek_offset_t while_else_stat_off = value_pop().offset;
				snek_offset_t loop_end_off = value_pop().offset;
				snek_offset_t while_off = value_pop().offset;
				snek_offset_t top_off = value_pop().offset;

				snek_code_patch_branch(while_off, while_else_stat_off);
				snek_code_patch_branch(loop_end_off, top_off);
				snek_code_patch_forward(while_off, loop_end_off, snek_forward_continue, top_off);
				snek_code_patch_forward(while_off, loop_end_off, snek_forward_break, snek_code_current());
			}@
		;
while-else-stat	: ELSE COLON suite
		|
		;
for-stat	: FOR NAME
			@{
				value_push_id(snek_token_val.id);
			}@
		  IN for-params suite
			@{
				snek_code_add_op_offset(snek_op_branch, 0);
				/* push 2 - loop_end_off */
				value_push_offset(snek_compile_prev);
				/* push 3 - while_else_stat_off */
				value_push_offset(snek_code_current());
				for_depth--;
			}@
		  while-else-stat
			@{
				goto patch_loop;
			}@
		;
for-params	: RANGE
			@{
			}@
		  OP opt-actuals CP COLON
			@{
				snek_offset_t num = value_pop().offset;
				snek_id_t id = value_pop().id;
				snek_code_add_in_range(id, num, for_depth);
			for_push_prevs:
				/* push 0 - for_off */
				value_push_offset(snek_compile_prev);
				/* push 1 - top_off */
				value_push_offset(snek_compile_prev);
				for_depth++;
			}@
		| expr COLON
			@{
				snek_id_t id = value_pop().id;
				snek_code_add_in_enum(id, for_depth);
				goto for_push_prevs;
			}@
		;
suite		: simple-stat
		| nl INDENT
			@{
				value_push_indent(snek_token_val.indent);
			}@
		  stat opt-stats EXDENT
			@{
				uint8_t indent = value_pop().indent;
				uint8_t exdent = snek_token_val.indent;

				snek_current_indent = indent;
				if (exdent > snek_current_indent) {
					snek_error("mismatching indentation indent %d exdent %d", indent, exdent);
					return parse_return_syntax;
				}
			}@
		;
expr		: expr-and expr-or-p
		;
expr-or-p	: OR
			@{
				snek_code_add_op_offset(snek_op_branch_true, 0);
				value_push_offset(snek_compile_prev);
			}@
		  expr-and
			@{
			short_second:
				snek_code_patch_branch(value_pop().offset, snek_code_current());
				snek_code_add_op(snek_op_nop);
			}@
		  expr-or-p
		|
		;
expr-and	: expr-not expr-and-p
		;
expr-and-p	: AND
			@{
				snek_code_add_op_offset(snek_op_branch_false, 0);
				value_push_offset(snek_compile_prev);
			}@
		  expr-not
			@{
				goto short_second;
			}@
		  expr-and-p
		|
		;
expr-not	: expr-cmp
		| NOT
			@{
			unop_first:
				value_push_op(snek_token_val.op);
			}@
		  expr-not
			@{
			unop_second:
				snek_code_add_op(value_pop().op);
			}@
		;
expr-cmp	: expr-lor expr-cmp-p
		;
expr-cmp-p	: cmpop
			@{
			binop_first:
				snek_code_set_push(snek_code_prev_insn());
				value_push_op(snek_token_val.op);
			}@
		  expr-lor
			@{
			binop_second:
				snek_code_add_op(value_pop().op);
			}@
		  expr-cmp-p
		| IS @ goto binop_first; @ expr-lor @ goto binop_second; @ expr-cmp-p
		|
		;
cmpop		: CMPOP
		| IN
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
expr-add-p	: PLUS @ goto binop_first; @ expr-mul @ goto binop_second; @ expr-add-p
		| MINUS @ goto binop_first; @ expr-mul @ goto binop_second; @ expr-add-p
		|
		;
expr-mul	: expr-unary expr-mul-p
		;
expr-mul-p	: MULOP @ goto binop_first; @ expr-unary @ goto binop_second; @ expr-mul-p
		|
		;
expr-unary	: LNOT @ goto unop_first; @ expr-unary @ goto unop_second; @
		| MINUS
			@{
				value_push_op(snek_op_uminus);
			}@
		  expr-unary @ goto unop_second; @
		| PLUS expr-unary
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
				snek_code_set_push(snek_code_prev_insn());
			}@
		  array-index CS
			@{
				bool slice = value_pop().bools;
				if (slice) {
					snek_code_set_push(snek_code_prev_insn());
					bool stride = value_pop().bools;
					bool end = value_pop().bools;
					bool start = value_pop().bools;
					snek_code_add_slice(start, end, stride);
				} else {
					snek_code_add_op(snek_op_array);
				}
 			}@
		  expr-array-p
		| OP
			@{
				snek_code_set_push(snek_code_prev_insn());
			}@
		  opt-actuals CP
		        @{
				snek_code_add_op_offset(snek_op_call, value_pop().offset);
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
				snek_code_set_push(snek_code_prev_insn());
				value_push_bool(true);
			}@
		  expr
		|
			@{
				value_push_bool(false);
			}@
		;
expr-prim	: OP opt-tuple CP
			@{
				bool tuple = value_pop().bools;

				if (tuple) {
					snek_offset_t num = value_pop().offset;
					snek_code_add_op_offset(snek_op_tuple, num);
				}
			}@
		| OS opt-actuals CS
			@{
				snek_offset_t num = value_pop().offset;
				snek_code_add_op_offset(snek_op_list, num);
			}@
		| NAME
			@{
				snek_code_add_op_id(snek_op_id, snek_token_val.id);
			}@
		| NUMBER
			@{
				snek_code_add_number(snek_token_val.number);
			}@
		| STRING
			@{
				snek_code_add_string(snek_token_val.string);
			}@
		  strings-p
		;
strings-p	: STRING
			@{
				snek_code_set_push(snek_code_prev_insn());
				snek_code_add_string(snek_token_val.string);
				snek_code_add_op(snek_op_plus);
			}@
		  strings-p
		|
		;

opt-tuple	: expr opt-tuple-p
		|
			@{
				value_push_offset(0);
				value_push_bool(true);
			}@
		;
opt-tuple-p	: COMMA
			@{
				snek_code_set_push(snek_code_prev_insn());
			}@
		  opt-actuals
			@{
				value_push_offset(value_pop().offset + 1);
				value_push_bool(true);
			}@
		|
			@{
				value_push_bool(false);
			}@
		;
opt-actuals	: actuals
		|
			@{
				value_push_offset(0);
			}@
		;
actuals		:
			@{
				value_push_offset(0);
			}@
		  expr actual-p actuals-p
		;
actual-p	: ASSIGN
			@{
				snek_offset_t prev_offset = snek_code_prev_insn();
				uint8_t *prev = snek_code_at(prev_offset);
				snek_id_t id;

				/* look at the previous instruction to figure out
				 * if it's an id.
				 */
				if (*prev != snek_op_id)
					return parse_return_syntax;
				memcpy(&id, prev + 1, sizeof (snek_id_t));
				snek_code_delete_prev();
				snek_code_add_number(id);
				snek_code_set_push(snek_code_prev_insn());
			}@
		  expr
			@{
				snek_code_set_push(snek_code_prev_insn());
				value_push_offset(value_pop().offset + 256);
			}@
		|
			@{
				snek_offset_t offset = value_pop().offset;
				if (offset >= 256)
					return parse_return_syntax;
				value_push_offset(offset + 1);
				snek_code_set_push(snek_code_prev_insn());
			}@
		;
actuals-p	: COMMA expr actual-p actuals-p
		|
		;
nl		: NL
			@{
				if (snek_compile_size)
					snek_code_add_op_offset(snek_op_line, snek_line);
			}@
		;
