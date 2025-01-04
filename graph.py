import matplotlib.pyplot as plt
from itertools import cycle
import subprocess
import numpy as np
import os
from datetime import datetime
import signal
import matplotlib.transforms as mtransforms

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
# gppPath = r"/usr/bin/g++"
buildCommand = [gppPath, "-fdiagnostics-color=always", "-g", cppFile, *linkageFiles, "-o", exeFile]

if not os.path.exists(gppPath):
    print(f"Error: g++ compiler not found at {gppPath}")
    exit(1)

# Function to check if rebuild is necessary
def needsRebuild(cppFile, exeFile):
    if not os.path.exists(exeFile):
        return True

    cppModTime = os.path.getmtime(cppFile)
    exeModTime = os.path.getmtime(exeFile)

    if cppModTime > exeModTime:
        return True

    for linkageFile in linkageFiles:
        if os.path.exists(linkageFile):
            linkageModTime = os.path.getmtime(linkageFile)
            if linkageModTime > exeModTime:
                return True
            
            headerFile = os.path.splitext(linkageFile)[0] + ".h"
            if os.path.exists(headerFile):
                headerModTime = os.path.getmtime(headerFile)
                if headerModTime > exeModTime:
                    return True

    return False

if needsRebuild(cppFile, exeFile):
    try:
        print("Building the C++ program...")
        subprocess.run(buildCommand, check=True)
        print("Build successful!")

    except subprocess.CalledProcessError as e:
        print("Compilation or execution failed!")
        print(f"Error message:\n{e.stderr}")
        exit(1)
else:
    print("No changes detected, skipping build.")


process = subprocess.Popen(
    [os.path.join(os.getcwd(), exeFile)],
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True,  # Ensures the output is already decoded as text
    bufsize=1,  # Line-buffered output
)

allData = []

for line in iter(process.stdout.readline, ""):
    line = line.strip()
    if not line:  # Skip empty lines
        continue

    if "[NOTICE]" in line:
        print(line)
        continue

    # print(line)
    values = list(filter(lambda x: x.strip(), line.split(",")))
    name = values[0].strip()

    miscomValues = [float(values[i].strip()) for i in range(1, len(values), 3)]
    misexValues = [float(values[i].strip()) for i in range(2, len(values), 3)]
    yValues = [float(values[i].strip()) for i in range(3, len(values), 3)]

    allData.append((name, miscomValues, misexValues, yValues))



process.stdout.close()
process.wait()


workspaceDir = "Graphs"
os.makedirs(workspaceDir, exist_ok=True)

try:
    latestCommit = subprocess.check_output(["git", "log", "-1", "--pretty=%B"], text=True).strip()
except subprocess.CalledProcessError as e:
    print(f"Error fetching latest commit: {e}")
    latestCommit = "Unknown Commit"

commitFolderDir = os.path.join(workspaceDir, latestCommit)
os.makedirs(commitFolderDir, exist_ok=True)

currentTime = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
outputDir = os.path.join(commitFolderDir, currentTime)
os.makedirs(outputDir, exist_ok=True)


figureWidth = 6.40 * 2
figureHeight = 4.80 * 1.5

aggregateGraphMarker = "."
singleGraphMarker = "o"
# markers:  , . o x X none "" 


def createSingleGraph(data, miscommunicationRange, misexecutionRange, constMiscom, constMisex):
    # slope, intercept = np.polyfit(xValues, yValues, 1)
    # bestFitY = [slope * x + intercept for x in xValues]
    # plt.plot(xValues, bestFitY, "-", label="Line of best fit", color="red")

    (name, miscomValues, misexValues, yValues) = data

    fig, ax1 = plt.subplots()
    fig.set_size_inches(figureWidth, figureHeight)
    ax1.set_ylabel("Average Points")
    ax1.grid(True)

    if (not constMiscom):
        ax1.plot(miscomValues, yValues, marker=singleGraphMarker, label="Data points")
        ax1.set_xlabel("Miscommunication Rate")
        ax1.set_xlim(miscommunicationRange[0], miscommunicationRange[1])

        if (not constMisex):
            ax2 = ax1.twiny()
            ax2.set_xlim(misexecutionRange[0], misexecutionRange[1])
            ax2.set_xlabel("Misexecution Rate")
            ax2.grid(True)
            ax1.set_title(name + " Average Points vs Fail Rates", pad=20)
        
        else:
            ax1.set_title(name + " Average Points vs Miscommunication Rate at Misexecution Rate = " + str(misexecutionRange[0]), pad=20)
        
    else:
        ax1.plot(misexValues, yValues, marker=singleGraphMarker, label="Data points")
        ax1.set_xlabel("Misexecution Rate")
        ax1.set_xlim(misexecutionRange[0], misexecutionRange[1])
        ax1.set_title(name + " Average Points vs Misexecution Rate at Miscommunication Rate = " + str(miscommunicationRange[0]), pad=20)


    #ax1.legend()
    plt.savefig(os.path.join(outputDir, f"{name}.png"))



def createAggregateGraph(data, miscommunicationRange, misexecutionRange, constMiscom, constMisex):
    fig, ax1 = plt.subplots()
    fig.set_size_inches(figureWidth, figureHeight)
    ax1.set_ylabel("Average Points")
    ax1.grid(True)

    defaultColors = plt.rcParams["axes.prop_cycle"].by_key()["color"]
    numColors = len(defaultColors)

    lineStyles = ["-", "--", "-.", ":"]
    lineStyleCycle = cycle(lineStyles)

    if (not constMiscom):
        for i, (name, miscomValues, misexValues, yValues) in enumerate(data):
            if i%numColors == 0:
                lineStyle=next(lineStyleCycle)
            ax1.plot(miscomValues, yValues, marker=aggregateGraphMarker, label=name, linestyle=lineStyle)
        ax1.set_xlabel("Miscommunication Rate")
        ax1.set_xlim(miscommunicationRange[0], miscommunicationRange[1])

        if (not constMisex):
            ax2 = ax1.twiny()
            ax2.set_xlim(misexecutionRange[0], misexecutionRange[1])
            ax2.set_xlabel("Misexecution Rate")
            ax2.grid(True)
            ax1.set_title("All Strategies Average Points vs Fail Rates", pad=20)
        
        else:
            ax1.set_title("All Strategies Average Points vs Miscommunication Rate at Misexecution Rate = " + str(misexecutionRange[0]), pad=20)
        
    else:
        for i, (name, miscomValues, misexValues, yValues) in enumerate(data):
            if i%numColors == 0:
                lineStyle=next(lineStyleCycle)
            ax1.plot(misexValues, yValues, marker=aggregateGraphMarker, label=name, linestyle=lineStyle)
        ax1.set_xlabel("Misexecution Rate")
        ax1.set_xlim(misexecutionRange[0], misexecutionRange[1])
        ax1.set_title("All Strategies Average Points vs Misexecution Rate at Miscommunication Rate = " + str(miscommunicationRange[0]), pad=20)


    ax1.legend(bbox_to_anchor=(1.05, 1.0), loc="upper left")
    plt.tight_layout()
    plt.savefig(os.path.join(outputDir, f"All Strategies.png"))


miscommunicationRange = (
    min(allData[0][1]),
    max(allData[0][1])
)
misexecutionRange = (
    min(allData[0][2]),
    max(allData[0][2])
)

constMiscom = (miscommunicationRange[0] == miscommunicationRange[1])
constMisex = (misexecutionRange[0] == misexecutionRange[1])

if (constMiscom and constMisex):
    constMiscom, constMisex = False, False
    print("[NOTICE] Both miscommunication and misinformation are constant! Automatically expanding graph.")
    miscommunicationRange = (miscommunicationRange[0]-0.01, miscommunicationRange[1]+0.01)
    misexecutionRange = (misexecutionRange[0]-0.01, misexecutionRange[1]+0.01)

# scalingFactor = (misexecutionRange[1] - misexecutionRange[0]) / (miscommunicationRange[1] - miscommunicationRange[0])

# def miscomToMisex(x):
#     return [misexecutionRange[0] + (val - miscommunicationRange[0]) * scalingFactor for val in x]

# def misexToMiscom(x):
#     return [miscommunicationRange[0] + (val - misexecutionRange[0]) / scalingFactor for val in x]


createAggregateGraph(allData, miscommunicationRange, misexecutionRange, constMiscom, constMisex)

for data in allData:
    createSingleGraph(data, miscommunicationRange, misexecutionRange, constMiscom, constMisex)



print(f"Graphs succesfully saved in: {outputDir}")




