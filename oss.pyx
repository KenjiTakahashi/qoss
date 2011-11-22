# -*- coding: utf-8 -*-
cimport coss
from posix cimport fcntl
from posix cimport unistd
from libc.errno cimport errno
from os import getenv, strerror
from cpython cimport bool

class OSSError(Exception):
    def __init__(self, msg = ''):
        self.msg = msg
    def __str__(self):
        return repr(self.msg)

class OSSNoValueError(OSSError):
    def __init__(self):
        self.msg = "no such value for this control"

cdef class OSS:
    cdef int __numberOfDevices
    cdef list __mixerinfo
    def __cinit__(self):
        cdef bytes devmixer = getenv('OSS_MIXERDEV') or b'/dev/mixer'
        cdef int gfd = fcntl.open(devmixer, fcntl.O_RDWR, 0)
        if gfd == -1:
            raise OSSError(strerror(errno))
        if coss.ioctl(gfd, coss.SNDCTL_MIX_NRMIX,
                &self.__numberOfDevices) == -1:
            raise OSSError(strerror(errno))
        self.__mixerinfo = list()
        cdef coss.oss_mixerinfo mi
        for i in range(self.__numberOfDevices):
            mi.dev = i
            if coss.ioctl(gfd, coss.SNDCTL_MIXERINFO, &mi) == -1:
                raise OSSError(strerror(errno))
            self.__mixerinfo.append(self.__convertMixerinfo(mi))
        cdef int err = unistd.close(gfd)
        if err == -1:
            raise OSSError(strerror(errno))
    cpdef int numberOfDevices(self):
        return self.__numberOfDevices
    cpdef dict mixerinfo(self, int i):
        return self.__mixerinfo[i]
    cpdef list mixerinfoList(self):
        return self.__mixerinfo
    cpdef int openDevice(self, bytes device):
        cdef int fd = fcntl.open(device, fcntl.O_RDWR, 0)
        if fd == -1:
            raise OSSError(strerror(errno))
        return fd
    cpdef list extinfo(self, int fd, int i):
        cdef list tmp = list()
        cdef coss.oss_mixext ei
        cdef dict mi = self.__mixerinfo[i]
        for k in range(mi['nrext']):
            ei.dev = i
            ei.ctrl = k
            if coss.ioctl(fd, coss.SNDCTL_MIX_EXTINFO, &ei) == -1:
                raise OSSError(strerror(errno))
            tmp.append(self.__convertExtinfo(ei))
        return tmp
    cpdef dict sextinfo(self, int fd, int i, int ctrl):
        cdef coss.oss_mixext ei
        ei.dev = i
        ei.ctrl = ctrl
        if coss.ioctl(fd, coss.SNDCTL_MIX_EXTINFO, &ei) == -1:
            raise OSSError(strerror(errno))
        return self.__convertExtinfo(ei)
    cpdef tuple peakValues(self, int fd, dict ei):
        if ei['type'] == coss.MIXT_MONOPEAK:
            return (self.__getValue(fd, ei) & 0xff,)
        elif ei['type'] == coss.MIXT_STEREOPEAK:
            value = self.__getValue(fd, ei)
            return (ei['maxvalue'] - (value & 0xff),
                    ei['maxvalue'] - ((value >> 8) & 0xff))
        else:
            raise OSSNoValueError()
    cpdef bint getMute(self, int fd, dict ei) except -1:
        if ei['type'] == coss.MIXT_MUTE:
            return self.__getValue(fd, ei)
        else:
            raise OSSNoValueError()
    cpdef setMute(self, int fd, dict ei, bint value):
        if ei['type'] == coss.MIXT_MUTE:
            self.__setValue(fd, ei, value)
        else:
            raise OSSNoValueError()
    cpdef list modeValues(self, int fd, dict ei):
        cdef coss.oss_mixer_enuminfo mei
        mei.dev = ei['dev']
        mei.ctrl = ei['ctrl']
        if coss.ioctl(fd, coss.SNDCTL_MIX_ENUMINFO, &mei) == -1:
            raise OSSError(strerror(errno))
        cdef list tmp = list()
        cdef int j
        cdef int k = 0
        for i in range(mei.nvalues):
            j = mei.strindex[i]
            if j != 0:
                tmp.append(str(mei.strings[k:j - 1], 'utf-8'))
            k = j
        tmp.append(str(mei.strings[k:], 'utf-8'))
        return tmp
    cpdef int getCurrentMode(self, int fd, dict ei) except -1:
        if ei['type'] == coss.MIXT_ENUM:
            return self.__getValue(fd, ei)
        else:
            raise OSSNoValueError()
    cpdef setCurrentMode(self, int fd, dict ei, int value):
        if ei['type'] == coss.MIXT_ENUM:
            self.__setValue(fd, ei, value)
        else:
            raise OSSNoValueError()
    cpdef tuple getSliders(self, int fd, dict ei):
        if ei['type'] == coss.MIXT_STEREOSLIDER16:
            value = self.__getValue(fd, ei)
            return (value & 0xffff, (value >> 16) & 0xffff)
        elif ei['type'] == coss.MIXT_STEREOSLIDER:
            value = self.__getValue(fd, ei)
            return (value & 0xff, (value >> 8) & 0xff)
        elif ei['type'] == coss.MIXT_MONOSLIDER16:
            return (self.__getValue(fd, ei) & 0xffff,)
        elif ei['type'] == coss.MIXT_MONOSLIDER:
            return (self.__getValue(fd, ei) & 0xff,)
        elif ei['type'] == coss.MIXT_SLIDER:
            return (self.__getValue(fd, ei),)
        else:
            raise OSSNoValueError()
    cpdef setSliders(self, int fd, dict ei, tuple values):
        if ei['type'] == coss.MIXT_STEREOSLIDER16:
            self.__setValue(fd, ei,
                    (values[0] | 0x0000) + ((values[1] << 16) | 0x0000))
        elif ei['type'] == coss.MIXT_STEREOSLIDER:
            self.__setValue(fd, ei,
                    (values[0] | 0x00) + ((values[1] << 8) | 0x00))
        elif ei['type'] == coss.MIXT_MONOSLIDER16:
            self.__setValue(fd, ei, values[0] | 0x0000)
        elif ei['type'] == coss.MIXT_MONOSLIDER:
            self.__setValue(fd, ei, values[0] | 0x00)
        else:
            raise OSSNoValueError()
    cpdef bint getOnOff(self, int fd, dict ei) except -1:
        if ei['type'] == coss.MIXT_ONOFF:
            return self.__getValue(fd, ei)
        else:
            raise OSSNoValueError()
    cpdef setOnOff(self, int fd, dict ei, bint value):
        if ei['type'] == coss.MIXT_ONOFF:
            self.__setValue(fd, ei, value)
        else:
            raise OSSNoValueError()
    cpdef getValues(self, int fd, dict ei):
        if ei['type'] == coss.MIXT_MONOPEAK:
            return (self.__getValue(fd, ei) & 0xff,)
        elif ei['type'] == coss.MIXT_STEREOPEAK:
            value = self.__getValue(fd, ei)
            return (ei['maxvalue'] - (value & 0xff),
                    ei['maxvalue'] - ((value >> 8) & 0xff))
        elif ei['type'] == coss.MIXT_MUTE:
            return <bint>self.__getValue(fd, ei)
        elif ei['type'] == coss.MIXT_STEREOSLIDER16:
            value = self.__getValue(fd, ei)
            return (value & 0xffff, (value >> 16) & 0xffff)
        elif ei['type'] == coss.MIXT_STEREOSLIDER:
            value = self.__getValue(fd, ei)
            return (value & 0xff, (value >> 8) & 0xff)
        elif ei['type'] == coss.MIXT_MONOSLIDER16:
            return (self.__getValue(fd, ei) & 0xffff,)
        elif ei['type'] == coss.MIXT_MONOSLIDER:
            return (self.__getValue(fd, ei) & 0xff,)
        elif ei['type'] == coss.MIXT_SLIDER:
            return (self.__getValue(fd, ei),)
        elif ei['type'] == coss.MIXT_ONOFF:
            return <bint>self.__getValue(fd, ei)
        elif ei['type'] == coss.MIXT_ENUM:
            return self.__getValue(fd, ei)
        elif ei['type'] == coss.MIXT_VALUE:
            return self.__getValue(fd, ei)
        else:
            raise OSSNoValueError()
    cpdef bool isSlider(self, dict ei):
        return ei['type'] == coss.MIXT_STEREOSLIDER16 or \
                ei['type'] == coss.MIXT_STEREOSLIDER or \
                ei['type'] == coss.MIXT_MONOSLIDER16 or \
                ei['type'] == coss.MIXT_MONOSLIDER or \
                ei['type'] == coss.MIXT_SLIDER
    cpdef bool isPeak(self, dict ei):
        return ei['type'] == coss.MIXT_STEREOPEAK or ei['type'] == coss.MIXT_MONOPEAK
    cpdef bool isMute(self, dict ei):
        return ei['type'] == coss.MIXT_MUTE
    cpdef bool isOnOff(self, dict ei):
        return ei['type'] == coss.MIXT_ONOFF
    cpdef bool isEnum(self, dict ei):
        return ei['type'] == coss.MIXT_ENUM
    cpdef closeDevice(self, int fd):
        cdef int err = unistd.close(fd)
        if err == -1:
            raise OSSError(strerror(errno))
    cdef int __getValue(self, int fd, dict ei) except? -1:
        cdef coss.oss_mixer_value v
        v.dev = ei['dev']
        v.ctrl = ei['ctrl']
        v.timestamp = ei['timestamp']
        if coss.ioctl(fd, coss.SNDCTL_MIX_READ, &v) == -1:
            raise OSSError(strerror(errno))
        return v.value
    cdef void __setValue(self, int fd, dict ei, int value) except *:
        cdef coss.oss_mixer_value v
        v.dev = ei['dev']
        v.ctrl = ei['ctrl']
        v.timestamp = ei['timestamp']
        v.value = value
        if coss.ioctl(fd, coss.SNDCTL_MIX_WRITE, &v) == -1:
            raise OSSError(strerror(errno))
    cdef dict __convertMixerinfo(self, coss.oss_mixerinfo mi):
        cdef dict tmp = dict()
        tmp['name'] = str(mi.name, 'utf-8')
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
        tmp['extname'] = str(ei.extname, 'utf-8')
        tmp['update_counter'] = ei.update_counter
        tmp['rgbcolor'] = ei.rgbcolor
        return tmp
