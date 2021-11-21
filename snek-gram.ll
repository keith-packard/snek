#
#  Copyright © 2019 Keith Packard <keithp@keithp.com>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
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
				SNEK_CODE_HOOK_START
				snek_poly_t p = snek_exec(code);
				SNEK_CODE_HOOK_STOP
				if (snek_abort)
					return parse_return_error;
				if (snek_print_val && !snek_is_null(p)) {
					snek_poly_print(stdout, p, 'r');
					putchar('\n');
				}
			}@
		| DEF NAME
			@{
				snek_parse_nformal = 0;
				snek_parse_nnamed = 0;
				value_push_id(snek_token_val.id);
	 		}@
		  OP opt-formals CP COLON suite
			@{
				if (snek_compile[snek_code_prev_insn()] == snek_op_return)
					snek_code_delete_prev();
				else
					snek_code_add_op(snek_op_null);
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
				snek_parse_nnamed++;
				snek_code_add_op_id(snek_op_assign_named, snek_parse_formals[snek_parse_nformal-1]);
			}@
		|
			@{
				if (snek_parse_nnamed)
					return parse_return_syntax;
			}@
		;
opt-stats	: stat opt-stats
		|
		;
stat		: simple-stat
		| @{ snek_print_val = false; snek_code_add_line(); }@
		  compound-stat
		| NL
		;
simple-stat	: @{ snek_code_add_line(); }@ small-stat small-stats-p NL
		;
small-stats-p	: SEMI small-stat small-stats-p
		|
		;
small-stat	: assign-expr
		| RETURN ret-expr
			@{ snek_code_add_op(snek_op_return); }@
		| BREAK
			@{ snek_code_add_forward(snek_forward_break); }@
		| CONTINUE
			@{ snek_code_add_forward(snek_forward_continue); }@
		| PASS
		| GLOBAL globals
		| DEL del dels-p
		| ASSERT expr
			@{ snek_code_add_op(snek_op_assert); }@
		;

dels-p		: COMMA del
		  dels-p
		|
		;
del		: expr-array
			@{
				snek_token_val.op = snek_op_del;
				goto extract_lvalue;
			}@
			@ add_op_lvalue(); @
		;
ret-expr	: expr
		|
			@{ snek_code_add_op(snek_op_null); }@
		;
assign-expr	: expr assign-expr-p
		;
assign-expr-p	: ASSIGN
			@{
			extract_lvalue:
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
					if (snek_token_val.op != snek_op_del)
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
		  expr @ add_op_lvalue(); @
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
				snek_code_add_line();
			else_branch:
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
			@
				goto else_branch;
			@
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
				value_push_offset(snek_code_prev_insn());
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
				value_push_offset(snek_code_prev_insn());
			}@
		  suite
			@{
				/* push 2 - loop_end_off */
				snek_code_add_op_offset(snek_op_branch, 0);
				value_push_offset(snek_code_prev_insn());
				/* push 3 - while_else_stat_off */
				value_push_offset(snek_code_current());
			}@
		  while-else-stat
			@ patch_loop(); @
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
				value_push_offset(snek_code_prev_insn());
				/* push 3 - while_else_stat_off */
				value_push_offset(snek_code_current());
			}@ @{
				for_depth--;
			}@
		  while-else-stat
			@ patch_loop(); @
		;
for-params	: RANGE OP opt-actuals CP COLON
			@{
				snek_offset_t num = value_pop().offset;
				if (num >= 256)
					return parse_return_syntax;
				snek_id_t id = value_pop().id;
				snek_code_add_in_range(id, num, for_depth);
			for_push_prevs:
				/* push 0 - for_off */
				value_push_offset(snek_code_prev_insn());
				/* push 1 - top_off */
				value_push_offset(snek_code_prev_insn());
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
		| NL INDENT
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
			@ bool_branch(snek_op_branch_true); @
		  expr-and
			@ short_second(); @
		  expr-or-p
		|
		;
expr-and	: expr-not expr-and-p
		;
expr-and-p	: AND
			@ bool_branch(snek_op_branch_false); @
		  expr-not
			@ short_second(); @
		  expr-and-p
		|
		;
expr-not	: expr-cmp
		| NOT @ unop_first(); @ expr-not @ unop_second(); @
		;
expr-cmp	: expr-lor
			@{
				value_push_offset(0);
			}@
		  expr-cmp-p
			@{
				snek_offset_t cmp_off = value_pop().offset;
				if (cmp_off)
					snek_code_patch_forward(cmp_off, snek_code_current(), snek_forward_cmp, snek_code_current());
			}@
		;
expr-cmp-p	: cmpop
			@{
				/* A chain of comparison operators works as a conjunction,
				 * which requires special handling. We use a custom operator
				 * that does the compare and branches on false around the
				 * remaining comparison operations. Here, we replace
				 * the previous comparison operator with this new 'chain'
				 * comparison operator that will be patched with the
				 * correct offset at the end of the chain
				 */
				snek_offset_t cmp_offset = value_stack[value_stack_p - 1].offset;
				snek_offset_t prev_offset = snek_code_prev_insn();
				if (cmp_offset) {
					snek_op_t prev_cmp = snek_compile[prev_offset];
					snek_code_delete_prev();
					snek_code_add_forward_op(snek_forward_cmp,
								 prev_cmp + (snek_op_chain_eq - snek_op_eq));
				} else
					value_stack[value_stack_p - 1].offset = prev_offset;
				binop_first();
			}@
		  expr-lor @ binop_second(); @ expr-cmp-p
		|
		;
cmpop		: CMPOP
		| IN
		| IS
		;
expr-lor	: expr-lxor expr-lor-p
		;
expr-lor-p	: LOR @ binop_first(); @ expr-lxor @ binop_second(); @ expr-lor-p
		|
		;
expr-lxor	: expr-land expr-lxor-p
		;
expr-lxor-p	: LXOR @ binop_first(); @ expr-land @ binop_second(); @ expr-lxor-p
		|
		;
expr-land	: expr-shift expr-land-p
		;
expr-land-p	: LAND @ binop_first(); @ expr-shift @ binop_second(); @ expr-land-p
		|
		;
expr-shift	: expr-add expr-shift-p
		;
expr-shift-p	: SHIFT @ binop_first(); @ expr-add @ binop_second(); @ expr-shift-p
		|
		;
expr-add	: expr-mul expr-add-p
		;
expr-add-p	: PLUS @ binop_first(); @ expr-mul @ binop_second(); @ expr-add-p
		| MINUS @ binop_first(); @ expr-mul @ binop_second(); @ expr-add-p
		|
		;
expr-mul	: expr-unary expr-mul-p
		;
expr-mul-p	: MULOP @ binop_first(); @ expr-unary @ binop_second(); @ expr-mul-p
		|
		;
expr-unary	: LNOT @ unop_first(); @ expr-unary @ unop_second(); @
		| MINUS @ value_push_op(snek_op_uminus); @ expr-unary @ unop_second(); @
		| PLUS expr-unary
		| expr-pow
		;

# this needs to be right associative instead of left associative...

expr-pow	: expr-array expr-pow-p
		;

expr-pow-p	: POW @ binop_first(); @ expr-array expr-pow-p @ binop_second(); @
		|
		;
expr-array	: expr-prim expr-array-p
		;
expr-array-p	: OS
			@{
				snek_code_set_push(snek_code_prev_insn());
			}@
{SNEK_SLICE
		  array-index CS
			@{
				bool slice = !!value_pop().offset;
				if (slice) {
					snek_code_set_push(snek_code_prev_insn());
					uint8_t stride = value_pop().offset * SNEK_OP_SLICE_STRIDE;
					uint8_t end = value_pop().offset * SNEK_OP_SLICE_END;
					uint8_t start = value_pop().offset * SNEK_OP_SLICE_START;
					snek_code_add_slice(start | end | stride);
				} else {
					snek_code_add_op(snek_op_array);
				}
 			}@
}
{SNEK_NO_SLICE
		  expr CS
			@{
				snek_code_add_op(snek_op_array);
			}@
}
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
{SNEK_SLICE
array-index	: expr opt-slice
		|
			@{
				value_push_offset(0);
			}@
		  slice
		;
opt-slice	:
			@{
				value_push_offset(1);
			}@
		  slice
		|
			@{
				value_push_offset(0);
			}@
		;
slice		: COLON opt-expr slice-p
			@{
				value_push_offset(1);
			}@
		;
slice-p		: COLON opt-expr
		|
			@{
				value_push_offset(0);
			}@
		;
opt-expr	:	@{
				value_push_offset(1);
				snek_code_set_push(snek_code_prev_insn());
			}@
		  expr
		|
			@{
				value_push_offset(0);
			}@
		;
}
expr-prim	: OP opt-tuple CP
			@{
				snek_soffset_t num = value_pop().offset;
				if (num >= 0)
					snek_code_add_op_offset(snek_op_tuple, num);
			}@
		| OS opt-actuals CS
			@{
				snek_offset_t offset = value_pop().offset;
				if (offset >= 256)
					return parse_return_syntax;
				snek_code_add_op_offset(snek_op_list, offset);
			}@
{SNEK_DICT
		| OC
			@{
				/* Zero dict-ents so far */
				value_push_offset(0);
			}@
		  opt-dict-ents CC
			@{
				/* Fetch the number of entries compiled */
				snek_offset_t offset = value_pop().offset;
				snek_code_add_op_offset(snek_op_dict, offset);
			}@
}
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
				/* A tuple with zero elements */
				value_push_offset(0);
			}@
		;
opt-tuple-p	: COMMA
			@{
				snek_code_set_push(snek_code_prev_insn());
			}@
		  opt-actuals
			@{
				snek_offset_t num = value_pop().offset;
				if (num >= 256)
					return parse_return_syntax;
				/* A tuple with num + 1 elements (one for the first expr) */
				value_push_offset(num + 1);
			}@
		|
			@{
				/* Not a tuple */
				value_push_offset(-1);
			}@
		;
opt-actuals	: actuals
		|
			@{
				/* Zero actuals */
				value_push_offset(0);
			}@
		;
actuals		:
			@{
				/* Zero actuals so far */
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

				/* Stick the name ID on the stack */
				snek_code_add_number(id);
				snek_code_set_push(snek_code_prev_insn());
			}@
		  expr
			@{
				/* One more named parameter */
				value_push_offset(value_pop().offset + 256);
				snek_code_set_push(snek_code_prev_insn());
			}@
		|
			@{
				snek_offset_t offset = value_pop().offset;
				/* We're using offsets > 256 to flag named parameters */
				if (offset >= 256)
					return parse_return_syntax;
				/* One more positional parameter */
				value_push_offset(offset + 1);
				snek_code_set_push(snek_code_prev_insn());
			}@
		;
actuals-p	: COMMA actuals-end
		|
		;
actuals-end	: expr actual-p actuals-p
		|
		;
{SNEK_DICT
opt-dict-ents	: dict-ent dict-ents-p
		|
		;
dict-ents-p	: COMMA dict-ents-end
		|
		;
dict-ents-end	: dict-ent dict-ents-p
		|
		;
dict-ent	: expr
			@{
				value_push_offset(value_pop().offset + 1);
				snek_code_set_push(snek_code_prev_insn());
			}@
		  COLON expr
			@{
				value_push_offset(value_pop().offset + 1);
				snek_code_set_push(snek_code_prev_insn());
			}@
		;
}
