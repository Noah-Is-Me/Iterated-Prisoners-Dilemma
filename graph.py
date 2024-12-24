import matplotlib.pyplot as plt
import subprocess
import numpy as np
import os
from datetime import datetime
import signal

print("Hello world!")


# Handle clean program termination
process = None

def cleanup(signal_received, frame):
    global process
    if process:
        print("Terminating child process...")
        process.terminate()  # Gracefully terminate the child process
        process.wait()
    print("Exiting Python program.")
    exit(0)

signal.signal(signal.SIGINT, cleanup)  # Handle Ctrl+C
signal.signal(signal.SIGTERM, cleanup)  # Handle kill signals



workspaceDir = "Graphs"
timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
outputDir = os.path.join(workspaceDir, timestamp)

os.makedirs(outputDir, exist_ok=True)

process = subprocess.Popen(
    ["./iterated-prisoners-dilemma"],
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True,  # Ensures the output is already decoded as text
    bufsize=1,  # Line-buffered output
)

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

    xValues = []
    yValues = []

    for i in range(1, len(values)):
        if i%2 != 0:
            xValues.append(float(values[i].strip()))
        else:
            yValues.append(float(values[i].strip()))

    # print(xValues)
    # print(yValues)

    slope, intercept = np.polyfit(xValues, yValues, 1)
    bestFitY = [slope * x + intercept for x in xValues]

    plt.figure()
    plt.plot(xValues, yValues, marker="o", label="Data points")
    plt.plot(xValues, bestFitY, '-', label="Line of best fit", color="red")
    plt.title(name + " Average Points v. Fail Chance")
    plt.xlabel("Fail Chance")
    plt.ylabel("Average Points")
    plt.grid(True)
    plt.legend()

    plt.savefig(os.path.join(outputDir, f"{name}.png"))
    # plt.show()

process.stdout.close()
process.wait()

print(f"Graphs succesfully saved in: {outputDir}")