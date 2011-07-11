from PyQt4 import QtGui
from PyQt4.QtCore import Qt

class QOSSBar(QtGui.QProgressBar):
    def __init__(self, maxi, curr, parent = None):
        QtGui.QProgressBar.__init__(self, parent)
        self.setTextVisible(False)
        self.setFixedWidth(16)
        self.setOrientation(Qt.Vertical)
        self.setMaximum(maxi)
        self.setValue(curr)

class QOSSMute(QtGui.QPushButton):
    def __init__(self, value, parent = None):
        QtGui.QPushButton.__init__(self, 'M', parent)
        self.setFixedSize(21, 21)
        self.setCheckable(True)
        self.setChecked(value)

class QOSSControl(QtGui.QSlider):
    def __init__(self, maxi, curr, parent = None):
        QtGui.QSlider.__init__(self, parent)
        self.setMaximum(maxi)
        self.setValue(curr)

class QOSSControlButton(QtGui.QPushButton):
    def __init__(self, parent = None):
        QtGui.QPushButton.__init__(self, parent)
        self.setFixedSize(21, 21)
        self.setCheckable(True)

class QOSSModes(QtGui.QComboBox):
    def __init__(self, modes, current, parent = None):
        QtGui.QComboBox.__init__(self, parent)
        self.addItems(modes)
        self.setCurrentIndex(current)

class QOSSWidget(QtGui.QWidget):
    def __init__(self, fd, values, parent = None):
        QtGui.QWidget.__init__(self, parent)
        self.fd = fd
        layout = QtGui.QGridLayout()
        self.setLayout(layout)
        count = 0
        pcurr = None
        pmaxi = None
        try:
            (pcurr, pmaxi) = values['peaks']
        except KeyError:
            pass
        else:
            count += 1
            layout.addWidget(QOSSBar(pmaxi, pcurr[0]), 0, 0, 2, 1)
        try:
            layout.addWidget(QOSSMute(values['mute']), 0, count)
        except KeyError:
            pass
        try:
            (curr, maxi) = values['controls']
        except KeyError:
            pass
        else:
            layout.addWidget(QOSSControl(maxi, curr[0]), 1, count)
            try:
                count += 1
                layout.addWidget(QOSSControl(maxi, curr[1]), 1, count)
                layout.addWidget(QOSSControlButton(), 0, count)
            except IndexError:
                count -= 1
        try:
            count += 1
            layout.addWidget(QOSSBar(pmaxi, pcurr[1]), 0, count, 2, 1)
        except (IndexError, TypeError):
            pass
        try:
            layout.addWidget(QOSSModes(values['modes'],
                values['currentMode']), 2, 0, 1, count)
        except KeyError:
            pass
