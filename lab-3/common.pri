CONFIG -= lex yacc testcase_targets import_plugins import_qpa_plugin shared qpa no_mocdepend qt_no_framework qt app_bundle

TEMPLATE = app
CONFIG += console

DRIVER = g++-7
QMAKE_CXX  = $$DRIVER
QMAKE_LINK = $$DRIVER

OBJECTS_DIR = output
DESTDIR = $$OBJECTS_DIR

FLAGS += -std=c++17 -fconcepts
QMAKE_CXXFLAGS += $$FLAGS
QMAKE_LFLAGS   += $$FLAGS

QMAKE_DEBUG   += -g3 -fvar-tracking -fvar-tracking-assignments -O0 -gdwarf-4 -ggdb -gcolumn-info -ftrack-macro-expansion=0
QMAKE_RELEASE += -O3 -DNDEBUG

QMAKE_CXXFLAGS_DEBUG   += $$QMAKE_DEBUG
QMAKE_LFLAGS_DEBUG     += $$QMAKE_DEBUG
QMAKE_CXXFLAGS_RELEASE += $$QMAKE_RELEASE
QMAKE_LFLAGS_RELEASE   += $$QMAKE_RELEASE

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-label

INCLUDEPATH += \
    "/media/Data/downloads/cmcstl2-master/include" \
    "$$PWD" \
