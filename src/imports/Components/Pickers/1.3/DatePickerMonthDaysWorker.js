function formatDay(d, month) {
  var dateNum = d.getDate()
  return {
    dateNum: dateNum,
    text: dateNum + '',
    surrounding: d.getMonth() !== month,
    date: d
  }
}

function getMonthDays(Calendar, displayedMonth, firstDayOfWeek) {

  var cal = new Calendar(firstDayOfWeek)
  var month = displayedMonth.getMonth()
  var year = displayedMonth.getFullYear()

  function setTime(date) {
    var d = new Date(displayedMonth.getTime())
    d.setFullYear(date.getFullYear())
    d.setMonth(date.getMonth())
    d.setDate(date.getDate())
    return d
  }

  var monthDates = []
  cal.monthDates(year, month, function(d) {
    monthDates.push(formatDay(setTime(d), month))
  })

  if (monthDates.length / 7 > 5) {
    return monthDates
  }

  // var lastDate = new Date(year, month, 1)
  var lastDate = new Date(displayedMonth.getTime())
  lastDate.setFullYear(year)
  lastDate.setMonth(month)
  lastDate.setDate(1)

  var lastDay = monthDates[monthDates.length - 1]
  if (lastDay.surrounding) lastDate.setMonth(month + 1)
    lastDate.setDate(lastDay.dateNum)

  for (var i = monthDates.length; i < 7 * 6; i++) {
    lastDate.setDate(lastDate.getDate() + 1)
    monthDates.push(formatDay(lastDate, month))
  }

  return monthDates
}

WorkerScript.onMessage = function(msg) {
  if (msg.name === 'requestMonthDays') {
    Qt.include('calendar.js', function(status) {
      WorkerScript.sendMessage({
        name: 'monthDays',
        monthDays: getMonthDays(Calendar, msg.displayedMonth, msg.firstDayOfWeek),
      })
    })
  }
}
