# -*- coding: utf-8 -*-
import sys
from oss import OSS, OSSError
from PyQt4 import QtGui
from middleendv2 import QOSSWidget

class QOSSConfig(QtGui.QWidget):
    def __init__(self, oss, parent = None):
        QtGui.QWidget.__init__(self, parent)
        self.oss = oss
        widget = QtGui.QWidget()
        wlayout = QtGui.QHBoxLayout()
        tree = QtGui.QTreeWidget()
        self.widgets = dict()
        def selection():
            try:
                item = widget.children()[2]
            except IndexError:
                pass
            else:
                wlayout.removeWidget(item)
                item.setParent(None)
            sitem = tree.selectedItems()[0]
            s = str(sitem.text(0))
            try:
                ss = str(sitem.parent().text(0))
            except AttributeError:
                pass
            else:
                wlayout.addWidget(self.widgets[ss][s])
        tree.headerItem().setHidden(True)
        tree.itemSelectionChanged.connect(selection)
        wlayout.addWidget(tree)
        widget.setLayout(wlayout)
        for i, device in enumerate(self.oss.mixerinfoList()):
            fd = self.oss.openDevice(device['devnode'])
            item = QtGui.QTreeWidgetItem([device['name']])
            tree.addTopLevelItem(item)
            widgets = dict()
            for ei in self.oss.extinfo(fd, i):
                if ei['type'] != 1 and ei['extname'] not in widgets:
                    qosswidget = QOSSWidget(ei['extname'])
                    qosswidget.fd = fd
                    qosswidget.ei = ei
                    try:
                        qosswidget.createPeaks(
                                self.oss.peakValues(fd, ei), ei['maxvalue'])
                    except OSSError:
                        pass
                    try:
                        qosswidget.createMute(self.oss.getMuteValue(fd, ei))
                    except OSSError:
                        pass
                    try:
                        qosswidget.createControls(
                                self.oss.getControlValues(fd, ei),
                                ei['maxvalue'])
                    except OSSError:
                        pass
                    try:
                        cur = self.oss.getCurrentMode(fd, ei)
                    except OSSError:
                        pass
                    else:
                        qosswidget.createModes(self.oss.modeValues(fd, ei), cur)
                    try:
                        qosswidget.createOnOff(ei['extname'],
                                self.oss.getOnOff(fd, ei))
                    except OSSError:
                        pass
                    qosswidget.do()
                    widgets[ei['extname']] = qosswidget
                    item.addChild(QtGui.QTreeWidgetItem([ei['extname']]))
            self.widgets[device['name']] = widgets
            self.oss.closeDevice(fd)
        layout = QtGui.QVBoxLayout()
        layout.addWidget(widget)
        self.setLayout(layout)

def run():
    app = QtGui.QApplication(sys.argv)
    app.setApplicationName(u'qoss')
    main = QOSSConfig(OSS())
    main.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    run()
