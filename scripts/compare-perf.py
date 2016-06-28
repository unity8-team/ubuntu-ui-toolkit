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

FRAME_METRICS = [
    { 'name':'timeStamp',  'type':int,  'factor':1.0,      'label':'time stamp' },
    { 'name':'window',     'type':int,  'factor':1.0,      'label':'Window id' },
    { 'name':'number',     'type':int,  'factor':1.0,      'label':'Frame number' },
    { 'name':'deltaTime',  'type':long, 'factor':0.000001, 'label':'Delta time (ms)' },
    { 'name':'syncTime',   'type':long, 'factor':0.000001, 'label':'Sync time (ms)' },
    { 'name':'renderTime', 'type':long, 'factor':0.000001, 'label':'Render time (ms)' },
    { 'name':'gpuTime',    'type':long, 'factor':0.000001, 'label':'GPU time (ms)' },
    { 'name':'swapTime',   'type':long, 'factor':0.000001, 'label':'Swap time (ms)' },
]
PLOT_FONT_NAME = 'Ubuntu'
PLOT_FONT_SIZE = 12
FRAME_COUNT = 100
LIMIT_60HZ = 1000.0 / 60.0
LIMIT_30HZ = 1000.0 / 30.0

def show_usage_quit():
    print 'Usage: ./compare-perf.py <metric> <filename1.qml> [filename2.qml, ...]'
    print ''
    print '  Plot and compare per-frame metrics of different QML files.'
    print ''
    print '  metrics: \'window\', \'number\', \'deltaTime\', \'syncTime\', \'renderTime\','
    print '           \'gpuTime\', \'swapTime\''
    sys.exit(1)

# Gets average, standard deviation, min, max.
def getStats(values):
    sum = 0.0
    min = sys.maxsize
    max = 0
    for i in values:
        if i < min:
            min = i
        if i > max:
            max = i
        sum += i
    avg = sum / len(values)
    var = 0.0
    for i in values:
        var += (i - avg) * (i - avg)
    var /= len(values)
    return (avg, math.sqrt(var), min, max)

def main(args):
    # Command line arguments.
    if len(args) < 2:
        show_usage_quit()
    metric_index = 0
    for i in range(0, len(FRAME_METRICS)):
        if FRAME_METRICS[i]['name'] == args[0]:
            metric_index = i
            break
    else:
        show_usage_quit()
    qml_files = args[1:]

    # Use OpenGL renderer string as title.
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

    metrics_type = []
    for i in FRAME_METRICS:
        metrics_type.append(i['type']);

    min_value = sys.maxsize
    max_value = 0
    for i in range(0, len(qml_files)):
        (temp_fd, temp_name) = tempfile.mkstemp()

        # Spawn quick-plus-scene.
        try:
            command = [
                'quick-plus-scene', '--performance-logging-minimal', '--performance-logging',
                temp_name, '--continuous-updates', '--quit-after-frame-count', str(FRAME_COUNT),
                qml_files[i]
            ]
            p = subprocess.Popen(command)
            p.wait()
        except:
            print 'Error: Can\'t spawn quick-plus-scene.'
            os.remove(temp_name)
            sys.exit(1)

        # Plot values.
        temp_file = os.fdopen(temp_fd, 'r')
        values = []
        count = 0
        while count < FRAME_COUNT:
            metrics_line = temp_file.readline()
            if len(metrics_line) > 0 and metrics_line[0] == 'F':
                metrics_line = metrics_line[2:]
                metrics_value = [ t(s) for t, s in zip(metrics_type, metrics_line.split()) ]
                # Prevents quick-scene-plus early exit issues.
                if len(metrics_value) == len(FRAME_METRICS):
                    values.append(metrics_value[metric_index] *
                                  FRAME_METRICS[metric_index]['factor'])
                count += 1
        # Prevents quick-scene-plus early exit issues.
        if len(values) == FRAME_COUNT:
            (avg, stdev, min, max) = getStats(values)
            label = qml_files[i].split('/')[-1] + ' (avg=' + ('%.2f' % avg) + \
                ', stdev=' + ('%.2f' % stdev) + ')'
            base_line, = axis.plot(range(1, FRAME_COUNT + 1), values, '-', label=label)
            axis.plot((1, FRAME_COUNT), (avg, avg), '--', color=base_line.get_color(), alpha=0.5)
            if min < min_value:
                min_value = min
            if max > max_value:
                max_value = max
        os.remove(temp_name)

    # Draw 60 and 30 Hz limits.
    # FIXME(loicm) Doesn't make sense for metrics other than timers.
    if max_value > LIMIT_60HZ:
        axis.plot((1, FRAME_COUNT), (LIMIT_60HZ, LIMIT_60HZ), '-', color='orange')
    if max_value > LIMIT_30HZ:
        axis.plot((1, FRAME_COUNT), (LIMIT_30HZ, LIMIT_30HZ), '-', color='red')

    # Set plot infos and render.
    axis.grid()
    axis.legend(loc=0)
    axis.set_xlim(0, FRAME_COUNT + 1)
    if min_value > LIMIT_60HZ - 1:
        # FIXME(loicm) Doesn't make sense for metrics other than timers.
        axis.set_ylim(LIMIT_60HZ - 1)
    plot.title(title)
    plot.xlabel('Frame')
    plot.ylabel(FRAME_METRICS[metric_index]['label'])
    plot.show()

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
