.pragma library

// Returns a formatted string from a date, using Qt Date.prototype.toLocaleString()
function format(date, format, localeStr) {
    var locale = Qt.locale(localeStr || 'en_US')
    return date.toLocaleString(locale, format)
}

// Clone a Date object, and optionally invokes methods on it
function clone(date, calls) {
    var d = date? new Date(date.getTime()) : new Date()
    if (!calls) return d
    if (!Array.isArray(calls)) {
        calls = [[calls].concat([].slice.call(arguments, 2))]
    }
    for (var i = 0; i < calls.length; i++) {
        if (!calls[i]) continue
        d[calls[i][0]].apply(d, calls[i].slice(1))
    }
    return d
}

// Return true if date1 and date2 are the same day
function sameDay(date1, date2) {
    return date1.getDate() === date2.getDate() &&
           date1.getMonth() === date2.getMonth() &&
           date1.getFullYear() === date2.getFullYear()
}

// Compare two dates
function compareDates(date1, date2) {
    var time1 = date1.getTime()
    var time2 = date2.getTime()
    if (time1 > time2) return 1
    if (time1 < time2) return -1
    return 0
}

var shortDayNames = 'Mon Tue Wed Thu Fri Sat Sun'.split(' ')

// Return a short day name based on a date object or a day number
function shortDayName(date) {
    if (typeof date === 'number') {
        return shortDayNames[date]
    }
    return shortDayNames[date.getDay()]
}

// Return the number of months between date1 and date2 (including both)
function getMonthsRange(date1, date2) {
    return (date2.getFullYear() - date1.getFullYear()) * 12 +
           (date2.getMonth() - date1.getMonth())
}
