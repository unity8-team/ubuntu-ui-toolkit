TEMPLATE = subdirs

SUBDIRS += 1.2 \
           1.3 \
           SuruDarkModule.pro

#uri = Ubuntu.Components.Themes.SuruDark
#installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

#PARENT_THEME_FILE = parent_theme
#QMLDIR_FILE = qmldir
#QML_FILES = ./1.2/*.qml ./1.3/*.qml
#ARTWORK_FILES += artwork/*.png \
#                 artwork/*.svg \
#                 artwork/*.sci

#parent_theme_file.installPath = $$installPath
#parent_theme_file.files = $$PARENT_THEME_FILE

#qmldir_file.installPath = $$installPath
#qmldir_file.files = $$QMLDIR_FILE

#qml_files.installPath = $$installPath
#qml_files.files = $$QML_FILES

#artwork_files.installPath = $$installPath/artwork
#artwork_files.files = $$ARTWORK_FILES


#UBUNTU_QML_MODULE_FILES += parent_theme_file qmldir_file qml_files artwork_files