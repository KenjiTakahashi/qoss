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
