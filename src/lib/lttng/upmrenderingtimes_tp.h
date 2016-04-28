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
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER quick_plus

#if !defined(UPM_RENDERING_TIMES_TP_H_) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define UPM_RENDERING_TIMES_TP_H_

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(quick_plus, metrics,
	TP_ARGS(int, anint),
	TP_FIELDS(
		ctf_integer(int, frame_render_time_usec, anint)
	)
)

#endif

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./upmrenderingtimes_tp.h"

/* This part must be outside ifdef protection */
#include <lttng/tracepoint-event.h>
