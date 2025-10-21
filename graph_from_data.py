import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy import stats, odr
import statsmodels.api as sm


# Load CSV
filename = "Graphs/Collected Sliding Window Size IV Data/mut ON, 20sws 100t 0.01st/Raw_Data_mut ON, 20sws 100t 0.01st.csv"  # replace with your CSV file path
df = pd.read_csv(filename)

# Ensure numeric columns are read properly
numeric_cols = ["IV_Value", "Trial", "Prob_Cop_After_Cop", "Prob_Cop_After_Def"]
df[numeric_cols] = df[numeric_cols].apply(pd.to_numeric, errors='coerce')

# Compute averages and standard errors per IV_Value
grouped = df.groupby("IV_Value").agg(
    mean_cop_cop=("Prob_Cop_After_Cop", "mean"),
    # sem_cop_cop=("Prob_Cop_After_Cop", lambda x: np.std(x, ddof=1)),
    sem_cop_cop=("Prob_Cop_After_Cop", lambda x: np.std(x, ddof=1)/np.sqrt(len(x))),
    mean_cop_def=("Prob_Cop_After_Def", "mean"),
    # sem_cop_def=("Prob_Cop_After_Def", lambda x: np.std(x, ddof=1))
    sem_cop_def=("Prob_Cop_After_Def", lambda x: np.std(x, ddof=1)/np.sqrt(len(x)))
).reset_index()

# plt.figure(figsize=(12,8))
plt.figure(figsize=(20,16))


# Scatter with error bars
plt.errorbar(grouped["IV_Value"], grouped["mean_cop_cop"], yerr=grouped["sem_cop_cop"], 
             fmt='.', color="royalblue", capsize=4, label="Probability Cooperate After Cooperate (PCAC)")
plt.errorbar(grouped["IV_Value"], grouped["mean_cop_def"], yerr=grouped["sem_cop_def"], 
             fmt='.', color="crimson", capsize=4, label="Probability Cooperate After Cooperate (PCAD)")

resultsCC = stats.linregress(grouped["IV_Value"], grouped["mean_cop_cop"])
resultsCD = stats.linregress(grouped["IV_Value"], grouped["mean_cop_def"])

plt.plot(grouped["IV_Value"], resultsCC.intercept + resultsCC.slope*grouped["IV_Value"], label='PCAC Linear Fit', color='darkblue', lw=2, zorder=10, linestyle="--")
plt.plot(grouped["IV_Value"], resultsCD.intercept + resultsCD.slope*grouped["IV_Value"], label='PCAD Linear Fit', color='darkred', lw=2, zorder=10, linestyle="--")



polyDegreeCC = 3
poly_modelCC = odr.polynomial(polyDegreeCC)  # third degree polynomial
odr_objCC = odr.ODR(odr.Data(grouped["IV_Value"], grouped["mean_cop_cop"]), poly_modelCC)
outputCC = odr_objCC.run()  # running ODR fitting
polyCC = np.poly1d(outputCC.beta[::-1])
poly_yCC = polyCC(grouped["IV_Value"])

polyDegreeCD = 3
poly_modelCD = odr.polynomial(polyDegreeCD)  # third degree polynomial
odr_obj = odr.ODR(odr.Data(grouped["IV_Value"], grouped["mean_cop_def"]), poly_modelCD)
output = odr_obj.run()  # running ODR fitting
polyCD = np.poly1d(output.beta[::-1])
poly_yCD = polyCD(grouped["IV_Value"])


plt.plot(grouped["IV_Value"], poly_yCC, label=f"PCAC Polynomial Fit (degree {polyDegreeCC})", color="darkblue", lw=3, zorder=20)
plt.plot(grouped["IV_Value"], poly_yCD, label=f"PCAD Polynomial Fit (degree {polyDegreeCD})", color="darkred", lw=3, zorder=20)

def rsquared_poly(x,y,model):
    yhat = model(x)
    # ybar = np.sum(y)/len(y)
    ssres = np.sum((y-yhat)**2)
    sstot = np.sum((y - np.mean(y))**2)
    return 1- (ssres/sstot)


print("------- PCAC Linear -------")
print(f"PCAC linear fit: {resultsCC.slope:.6f}x + {resultsCC.intercept:.6f}")
print(f"PCAC linear R-squared: {resultsCC.rvalue**2:.6f}")
print(f"PCAC linear p-value: {resultsCC.pvalue}")
# print(f"PCAC poly fit: {polyCC}")
# print(f"PCAC poly R-squared: {rsquared_poly(grouped["IV_Value"], grouped["mean_cop_cop"], polyCC)}")
# print(f"PCAC poly p-value: {resultsCC.pvalue}")

print("------- PCAD Linear -------")
print(f"PCAD linear fit: {resultsCD.slope:.6f}x + {resultsCD.intercept:.6f}")
print(f"PCAD linear R-squared: {resultsCD.rvalue**2:.6f}")
print(f"PCAD linear p-value: {resultsCD.pvalue}")
# print(f"PCAD poly fit: {polyCD}")
# print(f"PCAD poly R-squared: {rsquared_poly(grouped["IV_Value"], grouped["mean_cop_def"], polyCD)}")
# print(f"PCAD poly p-value: {resultsCD.pvalue}")

print("\n------- PCAC Poly -------")
x_poly_cc = np.column_stack([grouped["IV_Value"]**i for i in range(polyDegreeCC+1)])
x_poly_cc = sm.add_constant(x_poly_cc)
model_cc = sm.OLS(grouped["mean_cop_cop"],x_poly_cc)
results_cc = model_cc.fit()
print(results_cc.summary())

print("\n------- PCAD Poly -------")
x_poly_cd = np.column_stack([grouped["IV_Value"]**i for i in range(polyDegreeCD+1)])
x_poly_cd = sm.add_constant(x_poly_cd)
model_cd = sm.OLS(grouped["mean_cop_def"],x_poly_cd)
results_cd = model_cd.fit()
print(results_cd.summary())


# plt.plot(grouped["IV_Value"], grouped["mean_cop_cop"], color="royalblue", label="Probability Cooperate After Cooperate (PCAC)")
# plt.fill_between(grouped["IV_Value"], grouped["mean_cop_cop"]-grouped["sem_cop_cop"], grouped["mean_cop_cop"]+grouped["sem_cop_cop"],
#     alpha=0.5, edgecolor="royalblue", facecolor='royalblue')

# plt.plot(grouped["IV_Value"], grouped["mean_cop_def"], color="crimson", label="Probability Cooperate After Defect (PCAD)")
# plt.fill_between(grouped["IV_Value"], grouped["mean_cop_def"]-grouped["sem_cop_def"], grouped["mean_cop_def"]+grouped["sem_cop_def"],
#     alpha=0.5, edgecolor="crimson", facecolor='crimson')


plt.title("Population Behavioural Convergence vs. Mutation Strength", fontsize=32, pad=32)
plt.xlabel("Mutation Stddev", fontsize=24, labelpad=24)
plt.ylabel("Probability of Cooperation", fontsize=24, labelpad=24)
# plt.ylim(0, 1)
plt.ylim(0, 0.4)
# plt.xscale('log')

plt.xticks(fontsize=18)
plt.yticks(fontsize=18)

plt.legend(fontsize=20)
plt.grid(True, linestyle="--", alpha=0.6)


plt.show()
plt.savefig("QUICK_PLOT.png")