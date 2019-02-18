#
# Copyright © 2019 Keith Packard <keithp@keithp.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#

def slice_model(start, end, step, len):
    indices = []

    if step == ():
        step = 1
    
    ret = []

    if step > 0:

        # default start is 0
        if start == ():
            start = 0

        # start < 0 is from end

        elif start < 0:
            start = len + start

        # default end is len
        if end == ():
            end = len

        # end < 0 is from end
        elif end < 0:
            end = len + end


        # bounds check start
        if start < 0:
            start = 0

        val = start

        while val < end:
            if 0 <= val and val < len:
                ret += [val]
            val += step

    else:

        # default start is len - 1
        if start == ():
            start = len - 1
        elif start < 0:
            start = len + start
            
        # default end is -1
        if end == ():
            end = -1
        elif end < 0:
            end = len + end

        if start >= len:
            start = len - 1

        val = start
        while val > end:
            if 0 <= val and val < len:
                ret += [val]
            val += step

    return ret

def slice_string(value, start, end, step):

    indices = slice_model(start, end, step, len(value))

    ret = ""
    for i in indices:
        ret += value[i]
    return ret

def slice_list(value, start, end, step):

    indices = slice_model(start, end, step, len(value))

    ret = []
    for i in indices:
        ret += [value[i]]
    return ret


def slice_tuple(value, start, end, step):

    indices = slice_model(start, end, step, len(value))

    ret = ()
    for i in indices:
        ret += (value[i],)
    return ret

s = "hello"
l = [1,2,3,4,5]
t = (1,2,3,4,5)

def check(model, impl, mess):
    if model != impl:
        print("fail: %s (model %r impl %r)" % (mess, model, impl))
        exit(1)

# Test lots of combinations of ranges

for start in range(-10,10):
    for end in range(-10, 10):
        for step in range(-10,10):
            if step == 0:
                continue

            check (slice_string(s, (), (), ()), s[::], "[::]")

            check (slice_string(s, start, (), ()), s[start::], "[%d::]" % (start))

            check (slice_string(s, (), end, ()), s[:end:], "[:%d:]" % (end))

            check (slice_string(s, start, end, ()), s[start:end:], "[%d:%d:]" % (start, end))

            check (slice_string(s, (), (), step), s[::step], "[::%d]" % (step))

            check (slice_string(s, start, (), step), s[start::step], "[%d::%d]" % (start, step))

            check (slice_string(s, (), end, step), s[:end:step], "[:%d:%d]" % (end, step))

            check (slice_string(s, start, end, step), s[start:end:step], "[%d:%d:%d]" % (start, end, step))


            check (slice_list(l, (), (), ()), l[::], "[::]")

            check (slice_list(l, start, (), ()), l[start::], "[%d::]" % (start))

            check (slice_list(l, (), end, ()), l[:end:], "[:%d:]" % (end))

            check (slice_list(l, start, end, ()), l[start:end:], "[%d:%d:]" % (start, end))

            check (slice_list(l, (), (), step), l[::step], "[::%d]" % (step))

            check (slice_list(l, start, (), step), l[start::step], "[%d::%d]" % (start, step))

            check (slice_list(l, (), end, step), l[:end:step], "[:%d:%d]" % (end, step))

            check (slice_list(l, start, end, step), l[start:end:step], "[%d:%d:%d]" % (start, end, step))


            check (slice_tuple(t, (), (), ()), t[::], "[::]")

            check (slice_tuple(t, start, (), ()), t[start::], "[%d::]" % (start))

            check (slice_tuple(t, (), end, ()), t[:end:], "[:%d:]" % (end))

            check (slice_tuple(t, start, end, ()), t[start:end:], "[%d:%d:]" % (start, end))

            check (slice_tuple(t, (), (), step), t[::step], "[::%d]" % (step))

            check (slice_tuple(t, start, (), step), t[start::step], "[%d::%d]" % (start, step))

            check (slice_tuple(t, (), end, step), t[:end:step], "[:%d:%d]" % (end, step))

            check (slice_tuple(t, start, end, step), t[start:end:step], "[%d:%d:%d]" % (start, end, step))



