# -*- coding: utf-8 -*-
import sys
import oss
from PyQt4 import QtGui
from PyQt4.QtCore import Qt, QThread, pyqtSignal
from middleendv2 import QOSSWidget
#from signal2 import Signal
#from threading import Thread
from time import sleep

class QOSSWatcher(QThread):
    updated = pyqtSignal(str, str, dict, tuple)
    def __init__(self, oss_):
        QThread.__init__(self)
        #self.updated = Signal()
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
                            self.updated.emit(device, name, ei, new_values)
                    else:
                        new_values = self.oss.modeValues(fd, ei)
                        if cur != values[1] or new_values != values[0]:
                            self.updated.emit(device, name, ei, (new_values, cur))
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

class QOSSConfig(QtGui.QWidget):
    def __init__(self, oss_, parent = None):
        QtGui.QWidget.__init__(self, parent)
        self.oss = oss_
        self.watcher = QOSSWatcher(self.oss)
        self.watcher.updated.connect(self.update)
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
        for i, device in enumerate(self.oss.mixerinfoList()):
            fd = self.oss.openDevice(device['devnode'])
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
                    qosswidget = widgets.setdefault(name, QOSSWidget(name))
                    qosswidget.fd = fd
                    qosswidget.device = device['name']
                    try:
                        values = self.oss.peakValues(fd, ei)
                        qosswidget.createPeaks(values, ei['maxvalue'])
                    except oss.OSSError:
                        pass
                    try:
                        values = self.oss.getMuteValue(fd, ei)
                        qosswidget.createMute(values)
                    except oss.OSSError:
                        pass
                    try:
                        values = self.oss.getControlValues(fd, ei)
                        qosswidget.createControls(values, ei['maxvalue'])
                    except oss.OSSError:
                        pass
                    try:
                        values = self.oss.getOnOff(fd, ei)
                        qosswidget.createOnOff(name, values)
                    except oss.OSSError:
                        pass
                    try:
                        cur = self.oss.getCurrentMode(fd, ei)
                    except oss.OSSError:
                        pass
                    else:
                        values = (self.oss.modeValues(fd, ei), cur)
                        qosswidget.createModes(values[0], cur)
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
            #self.oss.closeDevice(fd) # add it somewhere
        layout = QtGui.QVBoxLayout()
        layout.addWidget(widget)
        self.setLayout(layout)
        self.watcher.start()
    def update(self, device, name, ei, values):
        widget = self.widgets[str(device)][str(name)]
        for i, (ei_, values_) in enumerate(widget.eis):
            if ei_ == ei:
                del widget.eis[i]
                widget.eis.append((ei, values))
        if self.oss.isSlider(ei['type']):
            widget.updateControls(values)
        elif self.oss.isPeak(ei['type']):
            widget.updatePeaks(values)
        else:
            print name
    def closeEvent(self, event):
        self.watcher.stop()
        self.watcher.wait()

def run():
    app = QtGui.QApplication(sys.argv)
    app.setApplicationName(u'qoss')
    main = QOSSConfig(oss.OSS())
    main.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    run()
