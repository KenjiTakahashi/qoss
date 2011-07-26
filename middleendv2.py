from PyQt4 import QtGui
from PyQt4.QtCore import Qt

class QOSSBar(QtGui.QProgressBar):
    def __init__(self, maxi, curr, parent = None):
        QtGui.QProgressBar.__init__(self, parent)
        self.setTextVisible(False)
        self.setFixedWidth(10)
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
    def update(self, modes, current):
        self.clear()
        self.addItems(modes)
        self.setCurrentIndex(current)

class QOSSWidget(QtGui.QGroupBox, object):
    def __init__(self, name, parent = None):
        QtGui.QWidget.__init__(self, name, parent)
        self.layout = QtGui.QGridLayout()
        self.setLayout(self.layout)
        self.mute = None
        self.lPeak = None
        self.rPeak = None
        self.lControl = None
        self.rControl = None
        self.modes = None
        self.onoff = None
        self.eis = list()
    @property
    def fd(self):
        return self.__fd
    @fd.setter
    def fd(self, fd):
        self.__fd = fd
    @property
    def device(self):
        return self.__device
    @device.setter
    def device(self, device):
        self.__device = device
    def createPeaks(self, curr, maxi):
        self.lPeak = QOSSBar(maxi, maxi - curr[0])
        try:
            self.rPeak = QOSSBar(maxi, maxi - curr[1])
        except IndexError:
            pass
    def updatePeaks(self, values):
        self.lPeak.setValue(self.lPeak.maximum() - values[0])
        try:
            self.rPeak.setValue(self.rPeak.maximum() - values[1])
        except AttributeError:
            pass
    def createMute(self, value):
        self.mute = QOSSMute(value)
    def updateMute(self, value):
        self.mute.setChecked(value)
    def createControls(self, curr, maxi):
        self.lControl = QOSSControl(maxi, curr[0])
        try:
            self.rControl = QOSSControl(maxi, curr[1])
        except IndexError:
            pass
    def updateControls(self, values):
        self.lControl.setValue(values[0])
        try:
            self.rControl.setValue(values[1])
        except AttributeError:
            pass
    def createModes(self, values, current):
        self.modes = QOSSModes(values, current)
    def updateModes(self, values, current):
        self.modes.update(values, current)
    def createOnOff(self, name, value):
        self.onoff = QtGui.QCheckBox(name)
        if value:
            self.onoff.setCheckState(2)
    def updateOnOff(self, value):
        if value:
            self.onoff.setCheckState(2)
        else:
            self.onoff.setCheckState(0)
    def do(self):
        if self.mute or self.lControl or self.rControl:
            span = 2
        else:
            span = 1
        hCount = 0
        if self.lPeak:
            self.layout.addWidget(self.lPeak, 0, 0, span, 1)
            hCount += 1
        if self.mute:
            self.layout.addWidget(self.mute, 0, hCount)
        if self.lControl:
            self.layout.addWidget(self.lControl, 1, hCount)
        if self.rControl:
            hCount += 1
            self.layout.addWidget(self.rControl, 1, hCount)
            self.layout.addWidget(QOSSControlButton(), 0, hCount)
        hCount += 1
        if self.rPeak:
            self.layout.addWidget(self.rPeak, 0, hCount, span, 1)
        if self.modes:
            span += 1
            self.layout.addWidget(self.modes, span, 0, 1, hCount)
        if self.onoff:
            self.layout.addWidget(self.onoff, span, 0, 1, hCount)
