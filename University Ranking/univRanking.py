#!/usr/bin/env python
# coding: utf-8

# importing capitals and TopUni csv files
import csv


def getInformation(selectedCountry, TopUniFileName, capitalsFileName):
    writeFile = open("output.txt", "w")  # all the required information written in output.txt
    file1 = open(TopUniFileName)
    csvreaderfile1 = csv.reader(file1)
    TopUniheader = next(csvreaderfile1)
    TopUni = []
    for row in csvreaderfile1:
        TopUni.append(row)
    file2 = open(capitalsFileName)
    csvreaderfile2 = csv.reader(file2)
    capitalsheader = next(csvreaderfile2)
    capitals = []
    for row in csvreaderfile2:
        capitals.append(row)
    print(len(TopUni))
    print(capitalsheader)
    countries = set()  # creating set for countries
    continents = set()  # creating set for continents
    worldrank = 100000
    nationalRank = 100000
    nationalRankname = ''
    worldrankname = ''
    totalUniversity = 0
    selectedCountryScore = 0
    continentMaxScore = -1
    continent = ''
    countryList = []
    capitalCity = ''
    # print(capitals)
    writeFile.write("Total number of universities => " + str(len(TopUni)) + '\n')
    # print(TopUni)
    for i in TopUni:  # creating for loop to iterate through elements of TopUni list
        countries.add(i[2].upper())
    writeFile.write("Available countries => " + str(countries).replace('{', '').replace('}', '').replace('\'', ''))
    writeFile.write('\n')
    for i in capitals:  # creating for loop to iterate through elements of capitals list
        continents.add(i[5].upper())
    writeFile.write("Available continents => " + str(continents).replace('{', '').replace('}', '').replace('\'', ''))
    writeFile.write('\n')
    # using for loop function to iterate through elements of capitals list and putting if condition to print country list
    for i in capitals:
        if i[0].upper() == selectedCountry.upper():
            continent = i[5]
            capitalCity = i[1]
    for i in capitals:
        if i[5].upper() == continent.upper():
            countryList.append(i[0])

    # print(countryList)
    # using for loop function to iterate through elements of TopUni list and if condition is true execute following code
    for i in TopUni:
        if i[2].upper() == selectedCountry.upper():
            selectedCountryScore += float(i[8])
            totalUniversity += 1
            if int(i[3]) < nationalRank:
                nationalRank = int(i[3])
                nationalRankname = i[1]

            if int(i[0]) < worldrank:
                worldrank = int(i[0])
                worldrankname = i[1]

    for i in TopUni:
        if i[2] in countryList:
            if float(i[8]) > continentMaxScore:
                continentMaxScore = float(i[8])

    avgScore = selectedCountryScore / totalUniversity  # formula to calculate average score
    relativeScore = avgScore / continentMaxScore * 100  # formula to calculate relative score
    writeFile.write("At international rank => " + str(worldrank) + " the university name is => " + worldrankname)
    writeFile.write("At national rank => " + str(nationalRank) + " the university name is => " + nationalRankname)
    writeFile.write('\n')
    writeFile.write("The average score => " + str(avgScore) + "%\n")
    writeFile.write(
        "The relative score to the top university in " + continent.upper() + " is => (" + str(avgScore) + " / " + str(
            continentMaxScore) + ") x 100% = " + str(relativeScore) + "%\n")
    writeFile.write("The capital is => " + capitalCity)
    writeFile.write("The universities that contain the capital name => \n")
    # the output.txt file information
    c = 1
    for i in TopUni:
        # print(i)
        if capitalCity in i[1].split(" "):
            writeFile.write("  #" + str(c) + ' ' + i[1].upper() + '\n')
            c += 1

# getInformation('USA', "TopUni.csv", "capitals.csv")
