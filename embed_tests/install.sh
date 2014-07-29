#!/bin/bash -i

# Original toolkit install

# Create root directory
mkdir -p staging_orig/Ubuntu

# Install Layouts
mkdir -p staging_orig/Ubuntu/Layouts
cp ../modules/Ubuntu/Layouts/libUbuntuLayouts.so staging_orig/Ubuntu/Layouts/
cp ../modules/Ubuntu/Layouts/qmldir staging_orig/Ubuntu/Layouts/qmldir

# Install PerformanceMetrics
mkdir -p staging_orig/Ubuntu/PerformanceMetrics
cp ../modules/Ubuntu/PerformanceMetrics/libUbuntuPerformanceMetrics.so staging_orig/Ubuntu/PerformanceMetrics/
cp ../modules/Ubuntu/PerformanceMetrics/*.qml staging_orig/Ubuntu/PerformanceMetrics/
cp ../modules/Ubuntu/PerformanceMetrics/qmldir staging_orig/Ubuntu/PerformanceMetrics/

# Install Test
mkdir -p staging_orig/Ubuntu/Test
cp ../modules/Ubuntu/Test/libUbuntuTest.so staging_orig/Ubuntu/Test/
cp ../modules/Ubuntu/Test/*.qml staging_orig/Ubuntu/Test/
cp ../modules/Ubuntu/Test/qmldir staging_orig/Ubuntu/Test/

# Install Components
mkdir -p staging_orig/Ubuntu/Components
cp ../modules/Ubuntu/Components/libUbuntuComponents.so staging_orig/Ubuntu/Components/
cp ../modules/Ubuntu/Components/*.qml staging_orig/Ubuntu/Components/
cp ../modules/Ubuntu/Components/*.js staging_orig/Ubuntu/Components/
cp ../modules/Ubuntu/Components/qmldir staging_orig/Ubuntu/Components/
cp -r ../modules/Ubuntu/Components/artwork/ staging_orig/Ubuntu/Components/
mkdir -p staging_orig/Ubuntu/Components/Colors
cp ../modules/Ubuntu/Components/Colors/*.qml staging_orig/Ubuntu/Components/Colors/
mkdir -p staging_orig/Ubuntu/Components/ListItems
cp ../modules/Ubuntu/Components/ListItems/*.qml staging_orig/Ubuntu/Components/ListItems/
cp ../modules/Ubuntu/Components/ListItems/qmldir staging_orig/Ubuntu/Components/ListItems/
cp -r ../modules/Ubuntu/Components/ListItems/artwork/ staging_orig/Ubuntu/Components/ListItems/
mkdir -p staging_orig/Ubuntu/Components/Pickers
cp ../modules/Ubuntu/Components/Pickers/*.qml staging_orig/Ubuntu/Components/Pickers/
cp ../modules/Ubuntu/Components/Pickers/qmldir staging_orig/Ubuntu/Components/Pickers/
mkdir -p staging_orig/Ubuntu/Components/Popups
cp ../modules/Ubuntu/Components/Popups/*.qml staging_orig/Ubuntu/Components/Popups/
cp ../modules/Ubuntu/Components/Popups/*.js staging_orig/Ubuntu/Components/Popups/
cp ../modules/Ubuntu/Components/Popups/qmldir staging_orig/Ubuntu/Components/Popups/
mkdir -p staging_orig/Ubuntu/Components/Styles
cp ../modules/Ubuntu/Components/Styles/*.qml staging_orig/Ubuntu/Components/Styles/
cp ../modules/Ubuntu/Components/Styles/qmldir staging_orig/Ubuntu/Components/Styles/
mkdir -p staging_orig/Ubuntu/Components/Themes
cp ../modules/Ubuntu/Components/Themes/*.qml staging_orig/Ubuntu/Components/Themes/
cp ../modules/Ubuntu/Components/Themes/qmldir staging_orig/Ubuntu/Components/Themes/
mkdir -p staging_orig/Ubuntu/Components/Themes/Ambiance
cp ../modules/Ubuntu/Components/Themes/Ambiance/*.qml staging_orig/Ubuntu/Components/Themes/Ambiance/
cp ../modules/Ubuntu/Components/Themes/Ambiance/qmldir staging_orig/Ubuntu/Components/Themes/Ambiance/
cp -r ../modules/Ubuntu/Components/Themes/Ambiance/artwork/ staging_orig/Ubuntu/Components/Themes/Ambiance/
mkdir -p staging_orig/Ubuntu/Components/Themes/SuruDark
cp ../modules/Ubuntu/Components/Themes/SuruDark/*.qml staging_orig/Ubuntu/Components/Themes/SuruDark/
cp ../modules/Ubuntu/Components/Themes/SuruDark/qmldir staging_orig/Ubuntu/Components/Themes/SuruDark/
cp ../modules/Ubuntu/Components/Themes/SuruDark/parent_theme staging_orig/Ubuntu/Components/Themes/SuruDark/
cp -r ../modules/Ubuntu/Components/Themes/SuruDark/artwork/ staging_orig/Ubuntu/Components/Themes/SuruDark/
mkdir -p staging_orig/Ubuntu/Components/Themes/SuruGradient
cp ../modules/Ubuntu/Components/Themes/SuruGradient/*.qml staging_orig/Ubuntu/Components/Themes/SuruGradient/
cp ../modules/Ubuntu/Components/Themes/SuruGradient/qmldir staging_orig/Ubuntu/Components/Themes/SuruGradient/
cp ../modules/Ubuntu/Components/Themes/SuruGradient/parent_theme staging_orig/Ubuntu/Components/Themes/SuruGradient/
cp -r ../modules/Ubuntu/Components/Themes/SuruGradient/artwork/ staging_orig/Ubuntu/Components/Themes/SuruGradient/

echo "Installed original toolkit."

# Embedded toolkit install

# Create root directory
mkdir -p staging_embed/Ubuntu

# Install Layouts
mkdir -p staging_embed/Ubuntu/Layouts
cp ../modules/Ubuntu/Layouts/libUbuntuLayouts.so staging_embed/Ubuntu/Layouts/
cp ../modules/Ubuntu/Layouts/qmldir staging_embed/Ubuntu/Layouts/qmldir

# Install PerformanceMetrics
mkdir -p staging_embed/Ubuntu/PerformanceMetrics
cp ../modules/Ubuntu/PerformanceMetrics/libUbuntuPerformanceMetrics.so staging_embed/Ubuntu/PerformanceMetrics/
cp ../modules/Ubuntu/PerformanceMetrics/*.qml staging_embed/Ubuntu/PerformanceMetrics/
cp ../modules/Ubuntu/PerformanceMetrics/qmldir staging_embed/Ubuntu/PerformanceMetrics/

# Install Test
mkdir -p staging_embed/Ubuntu/Test
cp ../modules/Ubuntu/Test/libUbuntuTest.so staging_embed/Ubuntu/Test/
cp ../modules/Ubuntu/Test/*.qml staging_embed/Ubuntu/Test/
cp ../modules/Ubuntu/Test/qmldir staging_embed/Ubuntu/Test/

# Install Components
mkdir -p staging_embed/Ubuntu/Components
cp ../modules/Ubuntu/ComponentsEmbedded/libUbuntuComponents.so staging_embed/Ubuntu/Components/
#cp ../modules/Ubuntu/ComponentsEmbedded/Page10.qml staging_embed/Ubuntu/Components/ # FIXME(shouldn't be installed)
cp ../modules/Ubuntu/ComponentsEmbedded/qmldir staging_embed/Ubuntu/Components/
cp -r ../modules/Ubuntu/ComponentsEmbedded/artwork/ staging_embed/Ubuntu/Components/
mkdir -p staging_embed/Ubuntu/Components/Colors
cp ../modules/Ubuntu/ComponentsEmbedded/Colors/*.qml staging_embed/Ubuntu/Components/Colors/
mkdir -p staging_embed/Ubuntu/Components/ListItems
cp ../modules/Ubuntu/ComponentsEmbedded/ListItems/qmldir staging_embed/Ubuntu/Components/ListItems/
cp -r ../modules/Ubuntu/ComponentsEmbedded/ListItems/artwork/ staging_embed/Ubuntu/Components/ListItems/
mkdir -p staging_embed/Ubuntu/Components/Pickers
cp ../modules/Ubuntu/ComponentsEmbedded/Pickers/qmldir staging_embed/Ubuntu/Components/Pickers/
mkdir -p staging_embed/Ubuntu/Components/Popups
cp ../modules/Ubuntu/ComponentsEmbedded/Popups/qmldir staging_embed/Ubuntu/Components/Popups/
mkdir -p staging_embed/Ubuntu/Components/Styles
cp ../modules/Ubuntu/ComponentsEmbedded/Styles/*.qml staging_embed/Ubuntu/Components/Styles/
cp ../modules/Ubuntu/ComponentsEmbedded/Styles/qmldir staging_embed/Ubuntu/Components/Styles/
mkdir -p staging_embed/Ubuntu/Components/Themes
cp ../modules/Ubuntu/ComponentsEmbedded/Themes/*.qml staging_embed/Ubuntu/Components/Themes/
cp ../modules/Ubuntu/ComponentsEmbedded/Themes/qmldir staging_embed/Ubuntu/Components/Themes/
mkdir -p staging_embed/Ubuntu/Components/Themes/Ambiance
cp ../modules/Ubuntu/ComponentsEmbedded/Themes/Ambiance/*.qml staging_embed/Ubuntu/Components/Themes/Ambiance/
cp ../modules/Ubuntu/ComponentsEmbedded/Themes/Ambiance/qmldir staging_embed/Ubuntu/Components/Themes/Ambiance/
cp -r ../modules/Ubuntu/ComponentsEmbedded/Themes/Ambiance/artwork/ staging_embed/Ubuntu/Components/Themes/Ambiance/
mkdir -p staging_embed/Ubuntu/Components/Themes/SuruDark
cp ../modules/Ubuntu/ComponentsEmbedded/Themes/SuruDark/*.qml staging_embed/Ubuntu/Components/Themes/SuruDark/
cp ../modules/Ubuntu/ComponentsEmbedded/Themes/SuruDark/qmldir staging_embed/Ubuntu/Components/Themes/SuruDark/
cp ../modules/Ubuntu/ComponentsEmbedded/Themes/SuruDark/parent_theme staging_embed/Ubuntu/Components/Themes/SuruDark/
cp -r ../modules/Ubuntu/ComponentsEmbedded/Themes/SuruDark/artwork/ staging_embed/Ubuntu/Components/Themes/SuruDark/
mkdir -p staging_embed/Ubuntu/Components/Themes/SuruGradient
cp ../modules/Ubuntu/ComponentsEmbedded/Themes/SuruGradient/*.qml staging_embed/Ubuntu/Components/Themes/SuruGradient/
cp ../modules/Ubuntu/ComponentsEmbedded/Themes/SuruGradient/qmldir staging_embed/Ubuntu/Components/Themes/SuruGradient/
cp ../modules/Ubuntu/ComponentsEmbedded/Themes/SuruGradient/parent_theme staging_embed/Ubuntu/Components/Themes/SuruGradient/
cp -r ../modules/Ubuntu/ComponentsEmbedded/Themes/SuruGradient/artwork/ staging_embed/Ubuntu/Components/Themes/SuruGradient/

echo "Installed embedded toolkit."
