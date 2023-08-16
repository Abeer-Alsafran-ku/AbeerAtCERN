
import seaborn as sns
import matplotlib.pyplot as plt
# seaborn graph
sns.set()
dataset = sns.load_dataset("flights")

# plotting strip plot with seaborn
ax = sns.relplot(data=dataset, x="size", y="performance", hue="backend", marker="o", kind="line");

# giving title to the plot
plt.title('MyGraph');

# function to show plot
plt.show()