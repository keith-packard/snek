# Contributing to Snek

We welcome contributions of all forms, including patches to code and
documentation, issue reports, or just kind words.

## Getting Current Source Code

Snek sources are maintained in git and hosted in two places.

 1. [keithp.com](https://keithp.com/cgit/snek.git/). This is the
    'canonical' source location.

 2. [github](https://github.com/keith-packard/snek). This should
    exactly mirror the code at keithp.com. I've placed it here to
    make contributing to Snek easier for people familiar with github.

You can create a local copy of the repository with git clone:

	$ git clone git://keithp.com/git/snek.git

or

	$ git clone https://github.com/keith-packard/snek.git

## Lola

The only non-standard tool used to build Snek is “lola”, which is a
custom LL parser generator designed to construct compact parse tables
for simple languages, like Snek. That's also available on both
keithp.com and github:

 1. [keithp.com](https://keithp.com/git/lola.git/).
 2. [github](https://github.com/keith-packard/lola).

Just like Snek, you can get it from either place using:

	$ git clone git://keithp.com/git/lola.git

or

	$ git clone https://github.com/keith-packard/lola.git

Lola is a Python program; installing it is a simple matter of copying
the script someplace in your path. You can use the Makefile for that
if you like:

	$ cd lola
	$ make install

The default location is /usr/local/bin

## Building Snek from Source

Snek uses simple Makefiles. Because so much of snek targets embedded
systems, using a more sophisticated system like meson is actually
pretty hard.

	$ cd snek
	$ make

## Patch Submission

You can submit patches in a couple of ways:

 1. Mail to the snek list (see below). This can be tricky as it
    requires a friendly email system, and you'll have to subscribe to
    the mailing list before it will let you post anything
    (sigh). Here's a simple example sending the latest patch in your
    local repository to the list:

	$ git send-email --to snek@keithp.com HEAD^

 2. Generate a pull-request in github.

    1. Fork the snek project into your own github account
    2. Push patches to that repository
    3. While viewing your repository, click on the 'New pull request'
       button and follow the instructions there.

## Issue Tracking

We're using the issue tracker on Github for now; if you have issues,
please submit them to the
[Snek Issue Tracker](https://github.com/keith-packard/snek/issues)

## Mailing List

Snek has a mailing list, hosted at keithp.com. You can
[subscribe here](https://keithp.com/mailman/listinfo/snek).
This is a public list, with public archives. Participants are expected
to abide by the Snek Code of Conduct.

## Code of Conduct

Snek uses the
[Contributor Covenant](https://www.contributor-covenant.org/), which
you'll find in the source tree. Please help make Snek a kind and
welcoming environment by following those rules.
