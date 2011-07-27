# -*- coding: utf-8 -*-
from PyQt4 import QtGui
from PyQt4.QtCore import Qt, pyqtSignal

class QOSSBar(QtGui.QProgressBar):
    def __init__(self, mini, maxi, curr, parent = None):
        QtGui.QProgressBar.__init__(self, parent)
        self.setTextVisible(False)
        self.setFixedWidth(10)
        self.setOrientation(Qt.Vertical)
        self.setMinimum(mini)
        self.setMaximum(mini + maxi)
        self.setValue(curr)

class QOSSMute(QtGui.QPushButton):
    def __init__(self, value, parent = None):
        QtGui.QPushButton.__init__(self, 'M', parent)
        self.setFixedSize(21, 21)
        self.setCheckable(True)
        self.setChecked(value)

class QOSSSlider(QtGui.QSlider):
    def __init__(self, mini, maxi, curr, parent = None):
        QtGui.QSlider.__init__(self, parent)
        self.setMinimum(mini)
        self.setMaximum(mini + maxi)
        self.setValue(curr)

class QOSSSliderButton(QtGui.QPushButton):
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
    sliderChanged = pyqtSignal(int, dict, tuple)
    modesChanged = pyqtSignal(int, dict, int)
    muteChanged = pyqtSignal(int, dict, bool)
    onOffChanged = pyqtSignal(int, dict, bool)
    def __init__(self, name, parent = None):
        QtGui.QWidget.__init__(self, name, parent)
        self.layout = QtGui.QGridLayout()
        self.setLayout(self.layout)
        self.mute = None
        self.lPeak = None
        self.rPeak = None
        self.lSlider = None
        self.rSlider = None
        self.modes = None
        self.onoff = None
        self.eis = list()
        self.inteis = dict()
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
    def createPeaks(self, curr, mini, maxi):
        self.lPeak = QOSSBar(mini, maxi, maxi - curr[0])
        try:
            self.rPeak = QOSSBar(mini, maxi, maxi - curr[1])
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
        self.mute.toggled.connect(self.changeMute)
    def updateMute(self, value):
        self.mute.setChecked(value)
    def changeMute(self, value):
        self.muteChanged.emit(self.fd, self.inteis['mute'], value)
    def createSlider(self, curr, mini, maxi):
        self.lSlider = QOSSSlider(mini, maxi, curr[0])
        self.lSlider.valueChanged.connect(self.changelSlider)
        try:
            self.rSlider = QOSSSlider(mini, maxi, curr[1])
            self.rSlider.valueChanged.connect(self.changerSlider)
        except IndexError:
            pass
    def updateSlider(self, values):
        self.lSlider.setValue(values[0])
        try:
            self.rSlider.setValue(values[1])
        except AttributeError:
            pass
    def changelSlider(self, value):
        try:
            self.sliderChanged.emit(self.fd, self.inteis['slider'],
                    (value, self.rSlider.value()))
        except AttributeError:
            self.sliderChanged.emit(self.fd, self.inteis['slider'], (value,))
    def changerSlider(self, value):
        try:
            self.sliderChanged.emit(self.fd, self.inteis['slider'],
                    (self.lSlider.value(), value))
        except AttributeError:
            self.sliderChanged.emit(self.fd, self.inteis['slider'], (value,))
    def slidersLockage(self, value):
        if value:
            lValue = self.lSlider.value()
            rValue = self.rSlider.value()
            if lValue < rValue:
                self.lSlider.setValue(rValue)
            elif lValue > rValue:
                self.rSlider.setValue(lValue)
            self.lSlider.valueChanged.connect(self.rSlider.setValue)
            self.rSlider.valueChanged.connect(self.lSlider.setValue)
        else:
            self.lSlider.valueChanged.disconnect(self.rSlider.setValue)
            self.rSlider.valueChanged.disconnect(self.lSlider.setValue)
    def createModes(self, values, current):
        self.modes = QOSSModes(values, current)
        self.modes.currentIndexChanged[int].connect(self.changeModes)
    def updateModes(self, values, current):
        self.modes.update(values, current)
    def changeModes(self, current):
        self.modesChanged.emit(self.fd, self.inteis['modes'], current)
    def createOnOff(self, name, value):
        self.onoff = QtGui.QCheckBox(name)
        if value:
            self.onoff.setCheckState(2)
        self.onoff.stateChanged.connect(self.changeOnOff)
    def updateOnOff(self, value):
        if value:
            self.onoff.setCheckState(2)
        else:
            self.onoff.setCheckState(0)
    def changeOnOff(self, value):
        self.onOffChanged.emit(self.fd, self.inteis['onoff'], value)
    def do(self):
        if self.mute or self.lSlider or self.rSlider:
            span = 2
        else:
            span = 1
        hCount = 0
        if self.lPeak:
            self.layout.addWidget(self.lPeak, 0, 0, span, 1)
            hCount += 1
        if self.mute:
            self.layout.addWidget(self.mute, 0, hCount)
        if self.lSlider:
            self.layout.addWidget(self.lSlider, 1, hCount)
        if self.rSlider:
            hCount += 1
            self.layout.addWidget(self.rSlider, 1, hCount)
            sliderButton = QOSSSliderButton()
            sliderButton.toggled.connect(self.slidersLockage)
            self.layout.addWidget(sliderButton, 0, hCount)
        hCount += 1
        if self.rPeak:
            self.layout.addWidget(self.rPeak, 0, hCount, span, 1)
        if self.modes:
            span += 1
            self.layout.addWidget(self.modes, span, 0, 1, hCount)
        if self.onoff:
            self.layout.addWidget(self.onoff, span, 0, 1, hCount)
