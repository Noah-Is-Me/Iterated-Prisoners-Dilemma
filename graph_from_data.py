import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load CSV
filename = "Graphs/Collected Sliding Window Size IV Data/mut ON, 20sws 100t 0.01st/Raw_Data_mut ON, 20sws 100t 0.01st.csv"  # replace with your CSV file path
df = pd.read_csv(filename)

# Ensure numeric columns are read properly
numeric_cols = ["IV_Value", "Trial", "Prob_Cop_After_Cop", "Prob_Cop_After_Def"]
df[numeric_cols] = df[numeric_cols].apply(pd.to_numeric, errors='coerce')

# Compute averages and standard errors per IV_Value
grouped = df.groupby("IV_Value").agg(
    mean_cop_cop=("Prob_Cop_After_Cop", "mean"),
    sem_cop_cop=("Prob_Cop_After_Cop", lambda x: np.std(x, ddof=1)/np.sqrt(len(x))),
    mean_cop_def=("Prob_Cop_After_Def", "mean"),
    sem_cop_def=("Prob_Cop_After_Def", lambda x: np.std(x, ddof=1)/np.sqrt(len(x)))
).reset_index()

plt.figure(figsize=(10,8))

# Scatter with error bars
plt.errorbar(grouped["IV_Value"], grouped["mean_cop_cop"], yerr=grouped["sem_cop_cop"], 
             fmt='.', color="royalblue", capsize=4, label="Probability Cooperate After Cooperate (PCAC)")
plt.errorbar(grouped["IV_Value"], grouped["mean_cop_def"], yerr=grouped["sem_cop_def"], 
             fmt='.', color="crimson", capsize=4, label="Probability Cooperate After Cooperate (PCAD)")

plt.title("PCAC and PCAD Convergence vs. Mutation Stddev")
plt.xlabel("Mutation Stddev")
plt.ylabel("Probability of Cooperation")
plt.ylim(0, 1)

plt.legend()
plt.grid(True, linestyle="--", alpha=0.6)

plt.show()
plt.savefig("QUICK_PLOT.png")