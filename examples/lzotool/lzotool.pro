QT             = core
QT            -= gui
QT            += QtLzo

CONFIG(debug, debug|release) {
TARGET         = lzotoold
} else {
TARGET         = lzotool
}

CONFIG        += console

TEMPLATE       = app

SOURCES       += $${PWD}/lzotool.cpp

win32 {
RC_FILE        = $${PWD}/lzotool.rc
OTHER_FILES   += $${PWD}/lzotool.rc
}
