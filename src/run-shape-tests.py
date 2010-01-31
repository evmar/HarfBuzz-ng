#!/usr/bin/python

# Copyright (C) 2010  Evan Martin <martine@danga.com>
#
#  This is part of HarfBuzz, an OpenType Layout engine library.
#
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in
# all copies of this software.
#
# IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
# DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
# IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
# DAMAGE.
#
# THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
# ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

import os
import simplejson as json
import subprocess
import sys

count = 0
for filename in os.listdir('shape-tests'):
    testcase = json.loads(open('shape-tests/' + filename).read())

    command = ['./shape']
    if 'rtl' in testcase:
        command.append('--rtl')
    # XXX generalize font paths/names/hashes.
    command.append('/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf')
    command.append(testcase['input'])

    output = subprocess.Popen(command, stdout=subprocess.PIPE).communicate()[0]
    result = json.loads(output)

    assert testcase == result
    count += 1

print "Ran %d test cases." % count
