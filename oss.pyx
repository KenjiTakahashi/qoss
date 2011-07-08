# -*- coding: utf-8 -*-
cimport coss
from posix cimport fcntl
from posix cimport unistd
from os import getenv

class OSSError(Exception):
    def __init__(self, msg = ''):
        self.msg = msg
    def __str__(self):
        return repr(self.msg)

cdef class OSS:
    cdef int __numberOfDevices
    cdef list __mixerinfo
    def __cinit__(self):
        cdef bytes devmixer = getenv('OSS_MIXERDEV') or '/dev/mixer'
        cdef int gfd = fcntl.open(devmixer, fcntl.O_RDWR, 0)
        if gfd == -1:
            raise OSSError()
        if coss.ioctl(gfd, coss.SNDCTL_MIX_NRMIX,
                &self.__numberOfDevices) == -1:
            raise OSSError()
        self.__mixerinfo = list()
        cdef coss.oss_mixerinfo mi
        for i in range(self.__numberOfDevices):
            mi.dev = i
            if coss.ioctl(gfd, coss.SNDCTL_MIXERINFO, &mi) == -1:
                raise OSSError()
            self.__mixerinfo.append(self.__convertMixerinfo(mi))
        cdef int err = unistd.close(gfd)
        if err == -1:
            raise OSSError()
    cpdef int numberOfDevices(self):
        return self.__numberOfDevices
    cpdef dict mixerinfo(self, int i):
        return self.__mixerinfo[i]
    cpdef list mixerinfoList(self):
        return self.__mixerinfo
    cpdef int openDevice(self, bytes device):
        cdef int fd = fcntl.open(device, fcntl.O_RDWR, 0)
        if fd == -1:
            raise OSSError()
        return fd
    cpdef list extinfo(self, int i, int fd):
        cdef list tmp = list()
        cdef coss.oss_mixext ei
        cdef dict mi = self.__mixerinfo[i]
        for k in range(mi['nrext']):
            ei.dev = i
            ei.ctrl = k
            if coss.ioctl(fd, coss.SNDCTL_MIX_EXTINFO, &ei) == -1:
                raise OSSError()
            tmp.append(self.__convertExtinfo(ei))
        return tmp
    cpdef closeDevice(self, int fd):
        cdef int err = unistd.close(fd)
        if err == -1:
            raise OSSError()
    cdef dict __convertMixerinfo(self, coss.oss_mixerinfo mi):
        cdef dict tmp = dict()
        tmp['name'] = mi.name
        tmp['id'] = mi.id
        tmp['modify_counter'] = mi.modify_counter
        tmp['card_number'] = mi.card_number
        tmp['port_number'] = mi.port_number
        tmp['handle'] = mi.handle
        tmp['enabled'] = mi.enabled
        tmp['caps'] = mi.caps
        tmp['nrext'] = mi.nrext
        tmp['priority'] = mi.priority
        tmp['devnode'] = mi.devnode
        tmp['legacy_device'] = mi.legacy_device
        return tmp
    cdef dict __convertExtinfo(self, coss.oss_mixext ei):
        cdef dict tmp = dict()
        tmp['dev'] = ei.dev
        tmp['ctrl'] = ei.ctrl
        tmp['type'] = ei.type
        tmp['maxvalue'] = ei.maxvalue
        tmp['minvalue'] = ei.minvalue
        tmp['flags'] = ei.flags
        tmp['id'] = ei.id
        tmp['parent'] = ei.parent
        tmp['timestamp'] = ei.timestamp
        tmp['data'] = ei.data
        tmp['enum_present'] = <char*>ei.enum_present
        tmp['control_no'] = ei.control_no
        tmp['desc'] = ei.desc
        tmp['extname'] = ei.extname
        tmp['update_counter'] = ei.update_counter
        tmp['rgbcolor'] = ei.rgbcolor
        return tmp
