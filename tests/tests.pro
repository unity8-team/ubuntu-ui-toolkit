TEMPLATE = subdirs
SUBDIRS += unit unit_x11

autopilot_module.path = /usr/lib/python2.7/dist-packages/ubuntuuitoolkit
python3_autopilot_module.path = /usr/lib/python3/dist-packages/ubuntuuitoolkit
autopilot_module.files = autopilot/ubuntuuitoolkit/*
python3_autopilot_module.files = autopilot/ubuntuuitoolkit/*

INSTALLS += python3_autopilot_module
INSTALLS += autopilot_module
