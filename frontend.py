# -*- coding: utf-8 -*-
# This is a part of qoss @ http://github.com/KenjiTakahashi/qoss/
# Karol "Kenji Takahashi" Wozniak (C) 2011
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import sys
import oss
from PyQt4 import QtGui
from PyQt4.QtCore import Qt, pyqtSignal
from middleendv2 import QOSSConfigWidget
from signal2 import Signal
from threading import Thread
from time import sleep

class QOSSWatcher(Thread):
    def __init__(self, oss_):
        Thread.__init__(self)
        self.updated = Signal()
        self.running = True
        self.items = list()
        self.oss = oss_
    def run(self):
        while self.running:
            for (device, name, fd, eis) in self.items:
                for (ei, values) in eis:
                    try:
                        cur = self.oss.getCurrentMode(fd, ei)
                    except oss.OSSNoValueError:
                        new_values = self.oss.getValues(fd, ei)
                        if new_values != values:
                            self.updated(device, name, ei, new_values)
                    else:
                        new_values = self.oss.modeValues(fd, ei)
                        if cur != values[1] or new_values != values[0]:
                            self.updated(device, name, ei, (new_values, cur))
                    sleep(.01)
    def register(self, device, name, fd, ei):
        self.items.append((device, name, fd, ei))
    def unregister(self, name):
        for item in self.items:
            if item[1] == name:
                self.items.remove(item)
                break
    def stop(self):
        self.running = False

class QOSSConfig(QtGui.QDialog):
    sliderUpdated = pyqtSignal(QOSSConfigWidget, tuple)
    peaksUpdated = pyqtSignal(QOSSConfigWidget, tuple)
    muteUpdated = pyqtSignal(QOSSConfigWidget, bool)
    modesUpdated = pyqtSignal(QOSSConfigWidget, list, int)
    onOffUpdated = pyqtSignal(QOSSConfigWidget, bool)
    def __init__(self, oss_, parent = None):
        QtGui.QDialog.__init__(self, parent)
        self.oss = oss_
        self.watcher = QOSSWatcher(self.oss)
        self.watcher.updated.connect(self.chainer)
        self.sliderUpdated.connect(self.updateSlider)
        self.peaksUpdated.connect(self.updatePeak)
        self.muteUpdated.connect(self.updateMute)
        self.modesUpdated.connect(self.updateModes)
        self.onOffUpdated.connect(self.updateOnOff)
        widget = QtGui.QWidget()
        wlayout = QtGui.QHBoxLayout()
        tree = QtGui.QTreeWidget()
        def selection():
            try:
                item = widget.children()[2]
            except IndexError:
                pass
            else:
                self.watcher.unregister(str(item.title()))
                wlayout.removeWidget(item)
                item.setParent(None)
            sitem = tree.selectedItems()[0]
            s = str(sitem.text(0))
            try:
                item = self.widgets[str(sitem.parent().text(0))][s]
            except AttributeError:
                pass
            else:
                self.watcher.register(item.device, s, item.fd, item.eis)
                wlayout.addWidget(item)
        tree.headerItem().setHidden(True)
        tree.itemSelectionChanged.connect(selection)
        wlayout.addWidget(tree)
        widget.setLayout(wlayout)
        self.widgets = dict()
        self.fds = list()
        for i, device in enumerate(self.oss.mixerinfoList()):
            fd = self.oss.openDevice(device['devnode'])
            self.fds.append(fd)
            item = QtGui.QTreeWidgetItem([device['name']])
            tree.addTopLevelItem(item)
            widgets = dict()
            children = set()
            for ei in self.oss.extinfo(fd, i):
                if ei['type'] != 1:
                    partial = ei['extname'][-4:]
                    if partial == 'mute' or partial == 'mode':
                        name = ei['extname'][:-5]
                    else:
                        name = ei['extname']
                    qosswidget = widgets.setdefault(
                        name,
                        QOSSConfigWidget(name)
                    )
                    qosswidget.fd = fd
                    qosswidget.device = device['name']
                    try:
                        values = self.oss.peakValues(fd, ei)
                        qosswidget.createPeaks(values,
                                ei['minvalue'], ei['maxvalue'])
                    except oss.OSSError:
                        pass
                    try:
                        values = self.oss.getMute(fd, ei)
                    except oss.OSSError:
                        pass
                    else:
                        qosswidget.createMute(values)
                        qosswidget.inteis['mute'] = ei
                        qosswidget.muteChanged.connect(self.oss.setMute)
                    try:
                        values = self.oss.getSliders(fd, ei)
                    except oss.OSSError:
                        pass
                    else:
                        qosswidget.createSlider(values,
                                ei['minvalue'], ei['maxvalue'])
                        qosswidget.inteis['slider'] = ei
                        qosswidget.sliderChanged.connect(self.oss.setSliders)
                    try:
                        values = self.oss.getOnOff(fd, ei)
                    except oss.OSSError:
                        pass
                    else:
                        qosswidget.createOnOff(name, values)
                        qosswidget.inteis['onoff'] = ei
                        qosswidget.onOffChanged.connect(self.oss.setOnOff)
                    try:
                        cur = self.oss.getCurrentMode(fd, ei)
                    except oss.OSSError:
                        pass
                    else:
                        values = (self.oss.modeValues(fd, ei), cur)
                        qosswidget.createModes(values[0], cur)
                        qosswidget.inteis['modes'] = ei
                        qosswidget.modesChanged.connect(self.oss.setCurrentMode)
                    try:
                        qosswidget.eis.append((ei, values))
                    except UnboundLocalError:
                        pass
                    qosswidget.do()
                    widgets[name] = qosswidget
                    children.add(name)
            item.addChildren([QtGui.QTreeWidgetItem([name]) for name in children])
            item.sortChildren(0, Qt.AscendingOrder)
            self.widgets[device['name']] = widgets
        layout = QtGui.QVBoxLayout()
        layout.addWidget(widget)
        dropper = QtGui.QGroupBox('Drop into me!')
        dropper.setMinimumHeight(160)
        layout.addWidget(dropper)
        buttons = QtGui.QDialogButtonBox()
        save = QtGui.QPushButton(self.trUtf8('&Save'))
        cancel = QtGui.QPushButton(self.trUtf8('&Cancel'))
        cancel.clicked.connect(self.close)
        buttons.addButton(save, QtGui.QDialogButtonBox.AcceptRole)
        buttons.addButton(cancel, QtGui.QDialogButtonBox.RejectRole)
        layout.addWidget(buttons)
        self.setLayout(layout)
        self.watcher.start()
    def chainer(self, device, name, ei, values):
        widget = self.widgets[device][name]
        for i, (ei_, values_) in enumerate(widget.eis):
            if ei_ == ei:
                del widget.eis[i]
                widget.eis.append((ei, values))
        if self.oss.isSlider(ei):
            self.sliderUpdated.emit(widget, values)
        elif self.oss.isPeak(ei):
            self.peaksUpdated.emit(widget, values)
        elif self.oss.isMute(ei):
            self.muteUpdated.emit(widget, values)
        elif self.oss.isEnum(ei):
            self.modesUpdated.emit(widget, values[0], values[1])
        elif self.oss.isOnOff(ei):
            self.onOffUpdated.emit(widget, values)
        else:
            print(name)
    def updateSlider(self, widget, values):
        widget.updateSlider(values)
    def updatePeak(self, widget, values):
        widget.updatePeaks(values)
    def updateMute(self, widget, values):
        widget.updateMute(values)
    def updateModes(self, widget, values, current):
        widget.updateModes(values, current)
    def updateOnOff(self, widget, values):
        widget.updateOnOff(values)
    def closeEvent(self, event):
        self.watcher.stop()
        self.watcher.join()
        for fd in self.fds:
            self.oss.closeDevice(fd)

def run():
    app = QtGui.QApplication(sys.argv)
    app.setApplicationName('qoss')
    main = QOSSConfig(oss.OSS())
    main.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    run()
