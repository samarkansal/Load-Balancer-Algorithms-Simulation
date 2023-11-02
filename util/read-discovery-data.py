import pickle
import pandas as pd

path = "../data/feb2nov_deidf.pkl"

print("Opening pickled dataframe... this could take a while.")
temp_df = pd.read_pickle(path)

print(temp_df)

# print(temp_df.columns)
cols = [
  'AllocNodes',
  'Elapsed',
  'End',
  'NCPUS',
  'NNodes',
  'Start',
  'State',
  'Submit'
  ]

i = 0
allCols = temp_df.columns
# print(allCols)
for colName in allCols:
  if colName in cols:
    continue
  del temp_df[colName]
  i += 1

print("{} of 85 columns deleted.".format(i))
saveName = '../data/discovery-data.csv'
temp_df.to_csv(saveName)
print("Data saved to {}".format(saveName))