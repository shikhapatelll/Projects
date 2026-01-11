#!/usr/bin/env python
# coding: utf-8

# using self to represent the instance of the class.
# defining the functions.
class Airport:
    def __init__(self, code, city, country):
        self.code = code
        self.city = city
        self.country = country

    def __repr__(self):
        rep = self.code + '(' + self.city + ',' + self.country + ')'
        return rep

    def getCode(self):
        return self.code

    def getCity(self):
        return self.city

    def getCountry(self):
        return self.country

    def setCity(self, city):
        self.city = city

    def setCountry(self, country):
        self.country = country
