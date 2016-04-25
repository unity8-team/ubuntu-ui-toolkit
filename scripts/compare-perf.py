#!/usr/bin/python

import sys, subprocess
import matplotlib.pyplot as plot
from matplotlib import rcParams

# Constants.
# MIN_BLUR_SIZE, MAX_BLUR_SIZE = 3, 13
PLOT_FONT_FAMILY = 'ubuntu'
PLOT_FONT_SIZE = 12
# TIME_LIMIT = 1000.0 / 30.0  # 30 FPS
# FRAME_COUNT = 100

# # Run a blur over a set of sizes returning the rendering times in an array.
# def measure(image, algorithm, sizes, axis):
#     args = [ image, '--fullscreen', '--algorithm', algorithm, '--timing',
#              str(FRAME_COUNT), '--size' ]
#     times = []
#     for i in sizes:
#         time = run(args + [str(i)])
#         times.append(time)
#         if (time > TIME_LIMIT):
#             # Don't measure blurs taking too much time. Since subsequent blurs
#             # would take longer, the times list is filled with the last
#             # measured value.
#             diff = len(sizes) - len(times)
#             for j in range(diff):
#                 times.append(time)
#             break
#     return times

def main(args):
    print 'Not ready yet ;)'
    sys.exit(1)

    if len(args) < 1:
        print 'Usage: ./compare-perf.py <filename1.qml> <filename2.qml>'
        exit(1)
    filename1 = args[0]
    filename2 = args[1]

    rcParams['font.family'] = PLOT_FONT_FAMILY
    rcParams['font.size'] = PLOT_FONT_SIZE
    plot.switch_backend('cairo')
    fig, axis = plot.subplots()

    commands = [
        ' --performance-logging --performance-log-file perf1.txt --continuous-update --quit-after-frame-count 200 ' + filename1,
        ' --performance-logging --performance-log-file perf2.txt --continuous-update --quit-after-frame-count 200 ' + filename2
    ]

    for i in commands:
        try:
            # sys.stdout.write('Running: ' + i + '\n')
            p = subprocess.Popen(['quick-plus-scene'] + commands[i], bufsize=4096,
                                 stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                 close_fds=True)
        except:
            print 'Error: quick-plus-scene command not found.'
            sys.exit(1)
        p.wait()

    # axis.grid()
    # axis.legend(loc=0)
    # axis.set_xticks(sizes)
    # axis.set_xlim(MIN_BLUR_SIZE - 1, MAX_BLUR_SIZE + 1)
    # axis.set_ylim(0.0, TIME_LIMIT)
    # plot.xlabel('kernel size')
    # plot.ylabel('rendering time (ms)')
    # plot.title(filename)
    # plot.savefig(dest_image, dpi=70)

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
