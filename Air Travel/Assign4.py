#!/usr/bin/env python
# coding: utf-8

from Flight import *

allAirports = dict()
allFlights = dict()


def loadData(airportFile, flightFile):
    try:
        airports = open(airportFile, 'r')
        airportlines = airports.readlines()  # Reading lines in file
        for line in airportlines:
            temp = line.replace('\t', '').strip().split(',')
        # Here we are removing the space, splitting on ',' and it is converted into list
            allAirports[temp[0].strip()] = Airport(temp[0].strip(), temp[2].strip(), temp[1].strip())
        flights = open(flightFile, 'r')
        flightlines = flights.readlines()  # Reading lines in file
        for line in flightlines:
            temp = line.replace('\t', '').strip().split(',')
        # Here we are removing the space, splitting on ',' and it is converted into list
            flight = Flight(temp[0].strip(), allAirports[temp[1].strip()], allAirports[temp[2].strip()])
            allFlights.setdefault(temp[1].strip(), []).append(flight)
        return True
    except:
        return False


def getAirportByCode(code):
    if allAirports.get(code) is None:
        return -1
    return allAirports[code]


def findAllCityFlights(city):
    l = []
    for flight in allFlights.values():
        for each in flight:
            if each.getOrigin().getCity() == city or each.getDestination().getCity() == city:
                l.append(each)
    return l


def findAllCountryFlights(country):
    l = []
    for flight in allFlights.values():
        for each in flight:
            # print(each.getOrigin())
            if each.getOrigin().getCountry() == country or each.getDestination().getCountry() == country:
                l.append(each)
    return l


def findFlightBetween(origAirport, destAirport):
    s = set()
    for each in allFlights[origAirport.getCode()]:
        if each.getOrigin().getCode() == origAirport.getCode() and each.getDestination().getCode() == destAirport.getCode():
            return "Direct Flight: " + origAirport.getCode() + " to " + destAirport.getCode()
    middleHopList = []
    for each in allFlights[origAirport.getCode()]:
        middleHopList.append(each.getDestination().getCode())  # Creating middle hop list
    for middleHop in middleHopList:
        for each in allFlights[middleHop]:
            if each.getDestination().getCode() == destAirport.getCode():  # finding flight between middle hop and destination
                s.add(middleHop)
    if len(s):
        return s
    else:
        return -1


def findReturnFlight(firstFlight):
    for each in allFlights[firstFlight.getDestination().getCode()]:  # Searching all the flights from destination
        if each.getDestination().getCode() == firstFlight.getOrigin().getCode():
            return each
    return -1
