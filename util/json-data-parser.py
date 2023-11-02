#!/usr/bin/env python3

import json
import sys
import os

def isFilePath(path, fileName):
  '''
  Scan the given path to check if the file exists
  '''
  currDir = os.scandir(path)

  # loop through entries in current path
  for dirEnt in currDir:
    if dirEnt.name == fileName \
       and dirEnt.is_file():
      return True

  return False

def getFilename():
  '''
  Handle CLI to get the file name and path
  '''
  if len(sys.argv) <= 1:
    sys.stderr.write('Too few args. Args must be 1.\n')
    sys.exit(-1)

  # get path components
  pathComponents = sys.argv[1].split('/')
  fileName = pathComponents[-1] # get the last item in the split
  path = '.'

  # recreate the path
  if len(pathComponents) > 1:
    path = '' # reset path to nothing to use given path verbatim
    for component in pathComponents[:-1]:
      path += component + '/'
  
  return (path, fileName)

def getFilePath():
  '''
  Get the split components of the file
  '''
  (path, fileName) = getFilename()
  # print("{} {} {}".format(path, fileName, path+fileName))

  isFound = isFilePath(path, fileName)

  if not isFound:
    sys.stderr.write('This file does not exist or is a directory.\n')
    sys.exit(-2)

  return (path, fileName)
  
def getJSON(path, fileName):
  '''
  Get the JSON file's contents
  '''

  jsonFile = open(path + fileName, 'r')
  jsonContents = json.loads(jsonFile.read())
  jsonFile.close()

  return jsonContents

def parseJSON(path, fileName):
  '''
  Parse the JSON file.
  '''

  # open the file and prepare to read line-by-line
  jsonFile = open((path + fileName), 'r')
  jsonLines = jsonFile.readlines()

  headers = []
  dheaders = 0
  currHeaders = []
  lineCount = 0

  for idx, line in enumerate(jsonLines):
    jsonContents = json.loads(line)

    if headers != currHeaders:
      print(headers)
      print(currHeaders)
      dheaders += 1

    # iterate through the JSON contents of the current line
    currHeaders.clear()
    print('DATA POINT {}'.format(idx + 1))
    for (key, val) in jsonContents.items():
      # skip lists and dicts
      if isinstance(val, list) or isinstance(val, dict):
        continue

      # compare read keys to check that data are all same values
      if idx == 0:
        headers.append(key)
        currHeaders.append(key)
      else:
        currHeaders.append(key)

      print("> {} {}".format(key, val))
    lineCount = idx + 1
  
  print('Processed {} data entries.'.format(lineCount))
  print('{} different headers found!'.format(dheaders))
  
  # for (key, val) in jsonContents.items():
  #   
    
  #   headers.append(key)
  #   print("{}: {}".format(key, val))

path, fileName = getFilePath()
# jsonContents = getJSON(path, fileName)
# print(json.dumps(jsonContents, indent=4))
parseJSON(path, fileName)

############
# TODO:
# [x] Get all data from JSON
# [x] Get JSON keys to use as headers
# [ ] Save as readable data format (csv)