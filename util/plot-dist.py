import matplotlib.pyplot as plt
import numpy as np

# open the data
dist_data = np.genfromtxt('../model/lba-data.csv', delimiter=',')

# set up the plot's axes
fig, axs = plt.subplots(2, 2, figsize=(16, 9), sharex=True, sharey=False)
titles = [ # the titles for the subplots
  'Round robin',
  'Random',
  'Utilization based',
  'Least connections'
]

# plot in the subplots
for i, ax in enumerate(axs.flat):
  # calculate the value frequencies
  uniq, freq = np.unique(dist_data[i, :], return_counts=True)
  freq = freq / np.sum(freq)

  # plot the histogram
  markerline,stemline,_ = ax.stem(uniq, freq, linefmt='k-', markerfmt='k*', basefmt=' ', bottom=0)
  ax.set_title(titles[i])
  plt.setp(markerline, markersize = 3)
  plt.setp(stemline, linewidth = 1.25)

# set up the shared axes labels etc.
fig.add_subplot(111, frameon=False)
plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, \
  right=False)
plt.xlabel('Server ID')
plt.ylabel('Pick frequency')


# save the figure
plt.savefig("algsdist.png", dpi=300)
