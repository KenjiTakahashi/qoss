# -*- coding: utf-8 -*-
"""
File:    signal.py
Author:  Thiago Marcos P. Santos
Created: August 28, 2008

Purpose: A signal/slot implementation
"""

from weakref import WeakValueDictionary

class Signal(object):
    def __init__(self):
        self.__slots = WeakValueDictionary()
    def __call__(self, *args, **kargs):
        for key in self.__slots:
            func, _ = key
            func(self.__slots[key], *args, **kargs)
    def connect(self, slot):
        key = (slot.__func__, id(slot.__self__))
        self.__slots[key] = slot.__self__
    def disconnect(self, slot):
        key = (slot.__func__, id(slot.__self__))
        if key in self.__slots:
            self.__slots.pop(key)
    def clear(self):
        self.__slots.clear()
