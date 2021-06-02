from subprocess import Popen, PIPE
import sys
import os
from os import walk
from prettytable import PrettyTable

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


#where the roms to test live
TestRomLocation = "./.roms/testRoms/"

def main():
    #no need to call this from the root, since the makefile is already called from the root
    #run the tests
    results = runTests()
    #clean up the results
    cleanedResults = cleanData(results)
    #display the results
    printResults(cleanedResults)

    

def runTests():
    #get a list of the test roms to run
    testRoms = []
    for file in  os.listdir(TestRomLocation):
        testRoms.append(TestRomLocation + file)

    results = []

    for rom in testRoms:
        print("Testing rom file: " + rom)
        process = Popen(["./gameBoyTest", rom], stdout=PIPE)
        (output, err) = process.communicate()
        results.append(output.decode('utf-8'))
        exit_code = process.wait()
    
    return results

def cleanData(data):
    #move the data to a new array
    cleanedData = []

    for i in range(0, len(data)):
        cleanedData.append(data[i].split("\n"))

    #we now have a 2d array, with runs, and dat afor each run
    #remove the strings we don't want
    for i in range(0, len(data)):
        cleanedData[i] = list(filter(None, cleanedData[i]))
        myfilter = lambda x: ("Error:"not in x)
        cleanedData[i] = list(filter(myfilter, cleanedData[i]))
        
    return cleanedData
    
def printResults(results):
    print("\nResults:")
    numTests = len(results)
    numPassed = 0
    table = PrettyTable()

    for run in results:
        passed = "Passed" in run
        columnData = []

        if passed:
            numPassed = numPassed + 1
            columnData.append(bcolors.OKGREEN + "Passed :)" + bcolors.ENDC)
            columnData.append("")
        else:
            columnData.append(bcolors.FAIL + "Failed :(" + bcolors.ENDC)
            columnData.append(run[len(run) -2][-8:])

        table.add_column(run[0], columnData)

    print(table)

    print("%s/%s passed (%d%%)"%(numPassed, numTests, (numPassed / numTests * 100)))





main()