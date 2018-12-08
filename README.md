# Newt — a tiny python-inspired language for embedded computing

Newt is a tiny embeddable language targeting processors with only a
few kB of flash and ram. Think of something that would have been
running BASIC years ago and you'll have the idea. These processors are
too small to run [MicroPython](https://micropython.org/).

## Newt Capabilities

Newt borrows semantics and syntax from python, but only provides a
tiny subset of that large language. The goal is to have Newt programs
able to run in a full Python (version 3) implementation so that any
knowledge gained in learning Newt will transfer directly to learning
Python.

### Datatypes

To keep things simple, Newt does not support Python objects at
all. Inspired by BASIC, Newt provides only a single numeric data
type, 32-bit floats. In addition, there are lists, tuples, strings and
functions. True and False are just syntax for 1 and 0. None doesn't
exist at all.

### Programming constructs

For expressions, newt includes the usual arithmetic (+, -, *, /, %,
//), relational (<=, <, ==, >, >=, !=) and logical (or, and, not)
operators.

Newt implements 'if', 'while' and 'for' control flow, along with
'break', 'continue' and 'return'.

### Functions

You can define new functions in Newt; parameters and local variables
have lexical scope, and you can refer to global variables with the
'global' statement (just like python).

## To Do list

Newt doesn't even do everything in this document yet, but its getting
close. Still to do are:

 * For statement (which requires generators)
 * Built-in functions (like print).

## A complete Newt system

I'd like to allow Newt systems to support local development, instead
of relying on a larger remote system for editing and storing
programs. That means creating a tiny IDE of some kind. I think what I
want is a text-mode system with two panes — an editor pane containing
the active Newt code and an interactive pane where you can type
interactive Newt commands to run. With that and some builtins to
save/load projects the system should be usable through a terminal
emulator running on another host, or through a local text interface.
