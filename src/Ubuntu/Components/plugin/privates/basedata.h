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

// FIXME(loicm) Clean up code generators in the tools folder.

// Squircle SVG string.
extern const char squircleSvg[];

// Squircle signed distance field.
const float squircleOffset = 1.0f;
const int squircleSdfWidth = 32;
extern const float squircleSdf[][squircleSdfWidth];

// Gaussian kernels. Changing one field requires an update of the others, use
// the appropriate tool in the tools folder.
const int gaussianCount = 128;
extern const int gaussianOffsets[];
extern const float gaussianKernels[];
extern const float gaussianSums[];
