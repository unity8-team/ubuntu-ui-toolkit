#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright © 2016 Canonical Ltd.
# Author: Loïc Molinari <loic.molinari@canonical.com>
#
# This file is part of Quick+.
#
# Quick+ is free software: you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; version 3.
#
# Quick+ is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Quick+. If not, see <http://www.gnu.org/licenses/>.

import sys, subprocess, os, tempfile, math
import matplotlib.pyplot as plot
from matplotlib import rcParams

COUNTERS = [
    { 'name':'frameCount',    'type':int,  'factor':1.0,      'label':'Frame count' },
    { 'name':'syncTime',      'type':long, 'factor':0.000001, 'label':'Sync time (ms)' },
    { 'name':'renderTime',    'type':long, 'factor':0.000001, 'label':'Render time (ms)' },
    { 'name':'gpuRenderTime', 'type':long, 'factor':0.000001, 'label':'GPU render time (ms)' },
    { 'name':'cpuUsage',      'type':int,  'factor':1.0,      'label':'CPU usage (%)' },
    { 'name':'vszMemory',     'type':int,  'factor':1.0,      'label':'Virtual size memory (kB)' },
    { 'name':'rssMemory',     'type':int,  'factor':1.0,      'label':'RSS memory (kB)' }
]
PLOT_FONT_NAME = 'Ubuntu'
PLOT_FONT_SIZE = 12
FRAME_COUNT = 100

def show_usage_quit():
    print 'Usage: ./compare-perf.py <counter> <filename1.qml> [filename2.qml, ...]'
    print ''
    print '  Plot and compare per-frame counter values of different QML files.'
    print ''
    print '  counters: \'frameCount\', \'syncTime\', \'renderTime\', \'gpuRenderTime\','
    print '            \'cpuUsage\', \'vszMemory\', \'rssMemory\''
    sys.exit(1)

# Gets average and standard deviation
def getStats(values):
    sum = 0.0
    for i in values:
        sum += i
    avg = sum / len(values)
    var = 0.0
    for i in values:
        var += (i - avg) * (i - avg)
    var /= len(values)
    return (avg, math.sqrt(var))

def main(args):
    # Command line arguments
    if len(args) < 2:
        show_usage_quit()
    counter_index = 0
    for i in range(0, len(COUNTERS)):
        if COUNTERS[i]['name'] == args[0]:
            counter_index = i
            break
    else:
        show_usage_quit()
    qml_files = args[1:]

    # Use OpenGL renderer string as title
    # FIXME(loicm) Add EGL support.
    title = ''
    try:
        p = subprocess.Popen('glxinfo', bufsize=4096*4, stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE, close_fds=True)
        p.wait()
        for line in p.stdout.readlines():
            if line.startswith('OpenGL renderer string:'):
                title = line.split('OpenGL renderer string:')[1]
                break
    except:
        print 'Warning: Can\'t set OpenGL renderer as plot title (glxinfo not installed).'

    rcParams['font.family'] = 'sans-serif'
    rcParams['font.sans-serif'] = [PLOT_FONT_NAME]
    rcParams['font.size'] = PLOT_FONT_SIZE
    fig, axis = plot.subplots()

    counters_type = []
    for i in COUNTERS:
        counters_type.append(i['type']);

    for i in range(0, len(qml_files)):
        (temp_fd, temp_name) = tempfile.mkstemp()

        # Spawn quick-plus-scene
        try:
            command = [
                'quick-plus-scene', '--performance-logging', '--performance-log-file', temp_name,
                '--continuous-update', '--quit-after-frame-count', str(FRAME_COUNT), qml_files[i]
            ]
            p = subprocess.Popen(command)
            p.wait()
        except:
            print 'Error: can\'t spawn quick-plus-scene'
            os.remove(temp_name)
            sys.exit(1)

        # Plot values
        temp_file = os.fdopen(temp_fd, 'r')
        values = []
        for j in range(0, FRAME_COUNT):
            counters_line = temp_file.readline()
            counters_value = [ t(s) for t, s in zip(counters_type, counters_line.split()) ]
            if len(counters_value) == len(COUNTERS):  # Prevents quick-scene-plus early exit issues
                values.append(counters_value[counter_index] * COUNTERS[counter_index]['factor'])
        if len(values) == FRAME_COUNT:  # Prevents quick-scene-plus early exit issues
            (avg, stdev) = getStats(values)
            label = qml_files[i].split('/')[-1] + ' (avg=' + ('%.2f' % avg) + \
                ', stdev=' + ('%.2f' % stdev) + ')'
            axis.plot(range(1, FRAME_COUNT + 1), values, '-', label=label)
            axis.plot((1, FRAME_COUNT), (avg, avg), '--', color='red', alpha=0.5)
        os.remove(temp_name)

    # Set plot infos and render.
    axis.grid()
    axis.legend(loc=0)
    axis.set_xlim(0, FRAME_COUNT + 1)
    axis.set_ylim(0)
    plot.title(title)
    plot.xlabel('Frame')
    plot.ylabel(COUNTERS[counter_index]['label'])
    plot.show()

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
