#!/usr/bin/python

import os, sys, subprocess
import matplotlib.pyplot as plot
from matplotlib import rcParams

# Constants.
MIN_BLUR_SIZE, MAX_BLUR_SIZE = 3, 13
PLOT_FONT_FAMILY = 'ubuntu'
PLOT_FONT_SIZE = 12
TIME_LIMIT = 1000.0 / 30.0  # 30 FPS
FRAME_COUNT = 100

# Run the blur command returning the rendering time in ms.
def run(args):
    try:
        sys.stdout.write('Running: ./blur ')
        for i in args:
            sys.stdout.write(i)
            sys.stdout.write(' ')
        sys.stdout.write('\n')
        p = subprocess.Popen(['./blur'] + args, bufsize=4096,
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             close_fds=True)
    except:
        print 'Error: blur command not found.'
        sys.exit(1)
    p.wait()
    time = p.stdout.readlines()[0]
    return float(time) * 0.000001

# Run a blur over a set of sizes returning the rendering times in an array.
def measure(image, algorithm, sizes, axis):
    args = [ image, '--fullscreen', '--algorithm', algorithm, '--timing',
             str(FRAME_COUNT), '--size' ]
    times = []
    for i in sizes:
        time = run(args + [str(i)])
        times.append(time)
        if (time > TIME_LIMIT):
            # Don't measure blurs taking too much time. Since subsequent blurs
            # would take longer, the times list is filled with the last
            # measured value.
            diff = len(sizes) - len(times)
            for j in range(diff):
                times.append(time)
            break
    return times

def main(args):
    rcParams['font.family'] = PLOT_FONT_FAMILY
    rcParams['font.size'] = PLOT_FONT_SIZE
    plot.switch_backend('cairo')
    fig, axis = plot.subplots()

    environ = dict(os.environ)

    environ['QML2_IMPORT_PATH'] = os.getcwd() + '/staging_orig:' + environ['QML2_IMPORT_PATH']
    environ['UBUNTU_UI_TOOLKIT_THEMES_PATH'] = os.getcwd() + '/staging_orig'
    try:
        p = subprocess.Popen(['./test'] + args, bufsize=4096,
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             close_fds=True, env=environ)
    except:
        print 'Error: test ELF binary not found.'
        sys.exit(1)
    p.wait()

    # environ['QML2_IMPORT_PATH'] = os.getcwd() + '/staging_embed:' + environ['QML2_IMPORT_PATH']
    # environ['UBUNTU_UI_TOOLKIT_THEMES_PATH'] = os.getcwd() + '/staging_embed'
    # try:
    #     p = subprocess.Popen(['./test'] + args, bufsize=4096,
    #                          stdout=subprocess.PIPE, stderr=subprocess.PIPE,
    #                          close_fds=True, env=environ)
    # except:
    #     print 'Error: test ELF binary not found.'
    #     sys.exit(1)
    # p.wait()

    # axis.plot(sizes, measure(source_image, i, sizes, axis), '-', label=i)

    # axis.grid()
    # axis.legend(loc=0)
    # axis.set_xticks(sizes)
    # axis.set_xlim(MIN_BLUR_SIZE - 1, MAX_BLUR_SIZE + 1)
    # axis.set_ylim(0.0, TIME_LIMIT)
    # plot.xlabel('kernel size')
    # plot.ylabel('rendering time (ms)')
    # plot.title(title)
    # plot.savefig(dest_image, dpi=70)

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
