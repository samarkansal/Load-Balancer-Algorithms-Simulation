import matplotlib.pyplot as plt
import matplotlib.figure as fig
import numpy as np

# open the data
dist_data = np.genfromtxt('../model/mqms_random.csv', delimiter=',')
print(dist_data.shape)

# set up the plot's axes
w, h = fig.figaspect(1/3)
fig, axs = plt.subplots(1, 3, figsize=(w, h))
titles = [ # the titles for the subplots
  'Utilization mean={:.2f}, sd={:.2f}'.format(np.mean(dist_data[1:, 1]), np.std(dist_data[1:, 1])),
  'Service Time (s) mean={:.2f}, sd={:.2f}'.format(np.mean(dist_data[1:, 2]), np.std(dist_data[1:, 2])),
  'Number of Jobs mean={:.2f}, sd={:.2f}'.format(np.mean(dist_data[1:, 3]), np.std(dist_data[1:, 3]))
]
ylabels = [
  'utilization (%)',
  'time (s)',
  '# jobs'
]

# plot in the subplots
for i, ax in enumerate(axs.flat):
  # calculate the value frequencies
  # uniq, freq = np.unique(dist_data[i, :], return_counts=True)
  # freq = freq / np.sum(freq)

  # plot the histogram
  # ax.scatter(dist_data[1:,0], dist_data[1:,i+1], c='k', marker='o', s=1)
  ax.stem(dist_data[1:,0], dist_data[1:,i+1], linefmt='k-', markerfmt='k.', \
    basefmt=' ', bottom=0)
  ax.set_title(titles[i])
  ax.set_ylabel(ylabels[i])
  ax.set_xlabel('Server ID')
  ax.set_xticks(dist_data[1:,0])
  maxVal = np.max(dist_data[1:,i+1])
  maxVal += maxVal * 0.05
  ax.set_ylim([0, maxVal])
  # plt.setp(markerline, markersize = 3)
  # plt.setp(stemline, linewidth = 1.25)
plt.tight_layout()


# save the figure
plt.savefig("test-dist.png", dpi=300)
