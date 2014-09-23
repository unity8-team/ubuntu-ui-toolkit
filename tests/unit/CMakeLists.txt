add_subdirectory(testparser)

#######################################
# these tests are failing in armhf/qemu
# can not create QQuickView
# tested with Qt 5 beta 1 and beta 2
#
# https://bugs.launchpad.net/qemu-linaro/+bug/1084148
#######################################
#!contains(QMAKE_HOST.arch,armv7l) {
add_subdirectory(tst_components)
add_subdirectory(tst_components_benchmark)
#}

add_subdirectory(tst_units)
add_subdirectory(tst_scaling_image_provider)
add_subdirectory(tst_qquick_image_extension)
add_subdirectory(tst_performance)
add_subdirectory(tst_mainview)
add_subdirectory(tst_i18n)
add_subdirectory(tst_arguments)
add_subdirectory(tst_argument)
add_subdirectory(tst_alarms)
add_subdirectory(tst_theme)
