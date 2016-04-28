TEMPLATE = lib
TARGET  = QuickPlusMetricsLTTNG
SOURCES = tracepoints.c
LIBS += -llttng-ust

target.path = /usr/lib
INSTALLS += target
