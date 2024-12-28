import matplotlib.pyplot as plt
from itertools import cycle
import subprocess
import numpy as np
import os
from datetime import datetime
import signal

print("Hello world!")


process = None
def cleanup(signal_received, frame):
    global process
    if process and process.poll() is None:
        print("Terminating child process...")
        process.terminate()
        process.wait()
    print("Exiting Python program.")
    exit(0)

signal.signal(signal.SIGINT, cleanup)  # Handle Ctrl+C
signal.signal(signal.SIGTERM, cleanup)  # Handle kill signals


cppFile = "iterated-prisoners-dilemma.cpp"
linkageFiles = ["helper.cpp", "strategy.cpp"]
exeFile = "iterated-prisoners-dilemma.exe"

gppPath = r"C:\msys64\ucrt64\bin\g++.exe"
buildCommand = [gppPath, "-fdiagnostics-color=always", "-g", cppFile, *linkageFiles, "-o", exeFile]

if not os.path.exists(gppPath):
    print(f"Error: g++ compiler not found at {gppPath}")
    exit(1)

try:
    print("Building the C++ program...")
    subprocess.run(buildCommand, check=True)
    print("Build successful!")

except subprocess.CalledProcessError as e:
    print("Compilation or execution failed!")
    print(f"Error message:\n{e.stderr}")
    exit(1)



workspaceDir = "Graphs"
timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
outputDir = os.path.join(workspaceDir, timestamp)
os.makedirs(outputDir, exist_ok=True)


figureWidth = 6.40 * 2
figureHeight = 4.80 * 1.5


process = subprocess.Popen(
    [exeFile],
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True,  # Ensures the output is already decoded as text
    bufsize=1,  # Line-buffered output
)

allData = []

for line in iter(process.stdout.readline, ''):
    line = line.strip()
    if not line:  # Skip empty lines
        continue

    if "[NOTICE]" in line:
        print(line)
        continue

    # print("BREAK")
    print(line)
    values = list(filter(lambda x: x.strip(), line.split(",")))
    name = values[0].strip()

    xValues = [float(values[i].strip()) for i in range(1, len(values), 2)]
    yValues = [float(values[i].strip()) for i in range(2, len(values), 2)]


    allData.append((name, xValues, yValues))

    # print(xValues)
    # print(yValues)

    # slope, intercept = np.polyfit(xValues, yValues, 1)
    # bestFitY = [slope * x + intercept for x in xValues]

    fig = plt.figure()
    fig.set_size_inches(figureWidth, figureHeight)
    plt.plot(xValues, yValues, marker="o", label="Data points")
    # plt.plot(xValues, bestFitY, '-', label="Line of best fit", color="red")
    plt.title(name + " Average Points vs Fail Chance")
    plt.xlabel("Fail Chance")
    plt.ylabel("Average Points")
    plt.grid(True)
    plt.legend()

    plt.savefig(os.path.join(outputDir, f"{name}.png"))
    # plt.show()

process.stdout.close()
process.wait()

fig = plt.figure()
fig.set_size_inches(figureWidth, figureHeight)

plt.title("All Strategies vs Fail Chance")
plt.xlabel("Fail Chance")
plt.ylabel("Average Points")
plt.grid(True)

defaultColors = plt.rcParams['axes.prop_cycle'].by_key()['color']
numColors = len(defaultColors)

lineStyles = ['-', '--', '-.', ':']
lineStyleCycle = cycle(lineStyles)

for i, (name, xValues, yValues) in enumerate(allData):
    if i%numColors == 0:
        lineStyle=next(lineStyleCycle)
    plt.plot(xValues, yValues, marker=".", label=name, linestyle=lineStyle)

plt.legend(bbox_to_anchor=(1.05, 1.0), loc='upper left')
plt.tight_layout()

plt.savefig(os.path.join(outputDir, f"All Strategies.png"))


print(f"Graphs succesfully saved in: {outputDir}")