/*
 * Copyright 2016 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Loïc Molinari <loic.molinari@canonical.com>
 */

// Creates the gaussian arrays to be pasted in utils.[h,cpp].

#include <QtCore/QtMath>
#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: ./creategaussianarrays count\n");
        return 1;
    }
    const int count = atoi(argv[1]);
    if (count < 1) {
        printf("count must be greater than 0.");
        return 1;
    }

    printf("static const int gaussianCount = %d;\n", count);

    // Offset to the kernel center in the kernels array.
    printf("static const int gaussianOffsets[%d] = {\n    ", count);
    int offset = 1;
    for (int i = 0; i < count; i++) {
        if (i < (count - 1)) {
            printf("%d, ", offset);
        } else {
            printf("%d\n};\n", offset);
        }
        offset += 2 * i + 4;
    }

    double* kernelSums = new double[count]();

    printf("static const float gaussianKernels[%d] = {\n    ", offset);
    for (int i = 0; i < count; i++) {
        const double kernelSize = 2 * i + 3;
        const double std = kernelSize * 0.23;  // Visually tweaked for various blur sizes.
        for (int j = 0; j < kernelSize; j++) {
            const double f = j - (i + 1);
            const double value = exp(-f * f / (2.0 * std * std));
            kernelSums[i] += value;
            if (j < (kernelSize - 1)) {
                printf("%10.8lff, ", value);
            } else if (i < (count - 1)) {
                printf("%10.8lff,\n    ", value);
            } else {
                printf("%10.8lff\n};\n", value);
            }
        }
    }

    printf("static const float gaussianSums[%d] = {\n    ", count);
    for (int i = 0; i < count; i++) {
        if (i < (count - 1)) {
            printf("%.8lff, ", kernelSums[i]);
        } else {
            printf("%.8lff\n};\n", kernelSums[i]);
        }
    }

    delete kernelSums;
    return 0;
}
