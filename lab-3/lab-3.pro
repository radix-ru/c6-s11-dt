TEMPLATE = subdirs
SUBDIRS = nim ttt ab

HEADERS += \
    utils.hxx \
    general.hxx \
    symmetries.hxx \

DISTFILES += \
    common.pri \
    nim.sh \
    ttt.sh \

QMAKE_EXTRA_TARGETS += \
    nim.dot \
    nim.image \
    ttt.dot \
    ttt.image \
    first \

NIM_PILES = 1 2 1

nim.dot.target = "output/nim.dot"
nim.dot.depends = "nim/output/nim"
nim.dot.commands = "$$nim.dot.depends" $$NIM_PILES > "$$nim.dot.target"

nim.image.target = "output/nim.svg"
nim.image.commands = ./nim.sh "$$nim.dot.target" > "$$nim.image.target"
nim.image.depends = ./nim.sh nim.dot

ttt.dot.target = "output/ttt.dot"
ttt.dot.depends = "ttt/3x3-tree/output/3x3-tree"
ttt.dot.commands = "$$ttt.dot.depends" > "$$ttt.dot.target"

ttt.image.target = "output/ttt.svg"
ttt.image.commands = ./ttt.sh "$$ttt.dot.target" > "$$ttt.image.target"
ttt.image.depends = ./ttt.sh ttt.dot

first.depends += nim.image ttt.image
