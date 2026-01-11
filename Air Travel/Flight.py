#!/usr/bin/env python
# coding: utf-8
from Airport import *


# using self to represent the instance of the class.
# defining the functions.
class Flight:
    def __init__(self, flightNo, origin, destination):
        if not (isinstance(origin, Airport) and isinstance(destination,
                                                           Airport)):  # Checking if origin and destination are instance of Airport class
            raise TypeError("The origin and destination must be Airport objects")
        self.flightNo = flightNo
        self.origin = origin
        self.destination = destination

    def getFlightNumber(self):
        return self.flightNo

    def getOrigin(self):
        return self.origin

    def getDestination(self):
        return self.destination

    def isDomesticFlight(self):
        if self.origin.getCountry() == self.destination.getCountry():
            return True
        else:
            return False

    def setOrigin(self, origin):
        self.origin = origin

    def setDestination(self, destination):
        self.destination = destination

    def __repr__(self):
        rep = self.flightNo + " from " + self.origin.getCity() + " to " + self.destination.getCity()
        if self.isDomesticFlight():
            return "Flight: " + rep + " {domestic}"
        else:
            return "Flight: " + rep + " {international}"

    def __eq__(self, other):
        if self.origin == other.getOrigin() and self.destination == other.getDestination():
            return True
        else:
            return False
