#!/usr/bin/env python2

import sys

from DebutForm import DebutForm as MainDialog
from PyQt4 import QtCore, QtGui

class main():
    app = QtGui.QApplication(sys.argv)
    window = MainDialog()

    window.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
