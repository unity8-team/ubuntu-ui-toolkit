// Copyright © 2015 Canonical Ltd.
// Author: Loïc Molinari <loic.molinari@canonical.com>
//
// This file is part of Quick+.
//
// Quick+ is free software: you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; version 3.
//
// Quick+ is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Quick+. If not, see <http://www.gnu.org/licenses/>.

// In order to generate a new file, the following commands must be used:
// $ cd tools
// $ qmake && make
// $ ./createdropshadowtexture ../src/dropshadowtexture.h

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QImage>

const int baseLevelSize = 256;  // Base level 0 has the highest texture size.
const int maxLevel = 4;         // Defines the number of levels and the lowest texture size.
const int mediumQualityLevel = 2;
const int lowQualityLevel = 3;

static quint8 data[baseLevelSize * baseLevelSize];
static quint8 temp[baseLevelSize * baseLevelSize];
static quint8 kernel[baseLevelSize];

Q_STATIC_ASSERT((baseLevelSize >= 16) && !((baseLevelSize - 1) & baseLevelSize));
Q_STATIC_ASSERT(mediumQualityLevel < maxLevel);
Q_STATIC_ASSERT(lowQualityLevel < maxLevel);

static void render(quint8* data, int size)
{
    const int width = size;
    const int height = size;
    const int dataSize = width * height;
    const int kernelSize = size - 1;
    const int halfKernelSize = kernelSize / 2;
    int kernelSum = 0;

    // Clear buffers.
    memset(data, 0, baseLevelSize * baseLevelSize);
    memset(temp, 0, baseLevelSize * baseLevelSize);
    memset(kernel, 0, baseLevelSize);

    // Generate the gaussian kernel.
    const float std = 20.0 * (width / 128.0);
    for (int i = 0; i < kernelSize; i++) {
        int f = i - halfKernelSize;
        kernel[i] = static_cast<int>(255.0 * exp(-f * f / (2.0 * std * std)));
        kernelSum += kernel[i];
    }

    // Fill source buffer with a white rectangle at (width/2, height/2).
    for (int i = height / 2; i < height; i++) {
        for (int j = width / 2; j < width; j++) {
            data[width * i + j] = 255;
        }
    }

    // Compute the gaussian blur using separate horizontal and vertical passes.
    for (int i = 0; i < height; i++) {
        quint8* src = &data[width * i];
        quint8* dst = &temp[width * i];
        for (int j = 0; j < width; j++) {
            int x = 0;
            for (int k = 0; k < kernelSize; k++) {
                x += src[qBound(0, j - halfKernelSize + k, width - 1)] * kernel[k];
            }
            dst[j] = x / kernelSum;
        }
    }
    for (int i = 0; i < height; i++) {
        quint8* src = &temp[width * i];
        quint8* dst = &data[width * i];
        for (int j = 0; j < width; j++) {
            int x = 0;
            for (int k = 0; k < kernelSize; k++) {
                src = &temp[width * qBound(0, (i - halfKernelSize + k), height - 1)];
                x += src[j] * kernel[k];
            }
            dst[j] = x / kernelSum;
        }
    }

    // Scale and clamp all the values so that once mapped on the scene graph
    // geometry node, the highest mapped value is 255 (1.0) making shadow
    // opacity changes a single multiplication.
    const float scale = 255.0f / data[(width * (height / 2)) + (width - 1)];
    for (int i = 0; i < dataSize; i++) {
        data[i] = static_cast<quint8>(qMin(255.0f, data[i] * scale));
    }
}

static void dump(QTextStream& cppOut, const quint8* data, int size)
{
    cppOut.setIntegerBase(16);
    cppOut.setFieldWidth(2);
    cppOut.setPadChar('0');
    const int squaredSize = size * size;
    for (int i = 0; i < squaredSize; i += 16) {
        cppOut << "    "
               << "0x"  << data[i]    << ",0x" << data[i+1]
               << ",0x" << data[i+2]  << ",0x" << data[i+3]
               << ",0x" << data[i+4]  << ",0x" << data[i+5]
               << ",0x" << data[i+6]  << ",0x" << data[i+7]
               << ",0x" << data[i+8]  << ",0x" << data[i+9]
               << ",0x" << data[i+10] << ",0x" << data[i+11]
               << ",0x" << data[i+12] << ",0x" << data[i+13]
               << ",0x" << data[i+14] << ",0x" << data[i+15];
        if (i != (squaredSize - 16)) {
            cppOut << ",\n";
        }
    }
    cppOut.setIntegerBase(10);
    cppOut.setFieldWidth(0);
    cppOut.setPadChar(' ');
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        qWarning("Usage: createdropshadowtexture filename");
        return 1;
    }

    QFile cppFile(argv[1]);
    if (!cppFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Can't create C++ header file \'%s\'", argv[1]);
        return 1;
    }

    QTextStream cppOut(&cppFile);
    cppOut << "// Copyright 2015 Canonical Ltd.\n"
           << "// Automatically generated by the createdropshadowtexture tool.\n"
           << "\n"
           << "const int dropShadowBaseLevelSize = " << baseLevelSize << ";\n"
           << "const int dropShadowMaxLevel = " << maxLevel << ";\n"
           << "const int dropShadowLowQualityLevel = " << lowQualityLevel << ";\n"
           << "const int dropShadowMediumQualityLevel = " << mediumQualityLevel << ";\n"
           << "const int dropShadowOffsets[" << maxLevel + 1 << "] = { 0";
    int offset = 0;
    for (int i = 0; i < maxLevel; i++) {
        offset += (baseLevelSize >> i) * (baseLevelSize >> i);
        cppOut << ", " << offset;
    }
    offset += (baseLevelSize >> maxLevel) * (baseLevelSize >> maxLevel);
    cppOut << " };\n\n"
           << "static const unsigned char dropShadowData[" << offset << "] = {\n";
    for (int i = 0; i <= maxLevel; i++) {
        render(data, baseLevelSize >> i);
        cppOut << (i ? ",\n\n" : "") << "    // Mipmap level " << i << ".\n";
        dump(cppOut, data, baseLevelSize >> i);
    }
    cppOut << "\n};\n";

    // Save the texture data as a grayscale image for debugging purpose
    // (requires Qt 5.5).
    // render(data, baseLevelSize);
    // QImage image(data, baseLevelSize, baseLevelSize, baseLevelSize, QImage::Format_Grayscale8);
    // image.save("dropshadowtexture.png");

    return 0;
}
