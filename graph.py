import matplotlib.pyplot as plt
#from mpl_toolkits.mplot3d import Axes3D
import subprocess
import numpy as np
import os
from datetime import datetime
import signal
import matplotlib.transforms as mtransforms
from collections import Counter
from matplotlib.animation import FuncAnimation

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

# gppPath = r"/usr/bin/g++"
# buildCommand = [gppPath, "-fdiagnostics-color=always", "-std=c++2a", "-g", cppFile, "-pthread", *linkageFiles, "-o", exeFile]

gppPath = r"C:\msys64\ucrt64\bin\g++.exe"
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


workspaceDir = "Graphs"
os.makedirs(workspaceDir, exist_ok=True)

try:
    latestCommit = subprocess.check_output(["git", "log", "-1", "--pretty=%B"], text=True).strip()
except subprocess.CalledProcessError as e:
    print(f"Error fetching latest commit: {e}")
    latestCommit = "Unknown Commit"

commitFolderDir = os.path.join(workspaceDir, latestCommit)
os.makedirs(commitFolderDir, exist_ok=True)

def getOutputDir(folder:str=None, fileName:str=None):
    currentTime = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")

    if (folder):
        folderDir = os.path.join(commitFolderDir, folder)
        os.makedirs(folderDir, exist_ok=True)
        outputDir = os.path.join(folderDir, currentTime)
    else:
        outputDir = os.path.join(commitFolderDir, currentTime)

    os.makedirs(outputDir, exist_ok=True)
    return outputDir



figureWidth = 6.40 * 2
figureHeight = 4.80 * 1.5

# markers:  , . o x X none "" 

def normalizeData(data: list):
    if not data: return
    return (data - np.min(data)) / (np.max(data) - np.min(data))

def getStabalizationMoment(yValues: list):
    slidingWindowSize = min(10, len(yValues))
    stddevs = []

    #for i in range(slidingWindowSize-1, len(xValues)):
    for i in range(len(yValues)):
        if (i<slidingWindowSize): 
            stddevs.append(1)
            continue

        endingX = i
        startingX = max(endingX - slidingWindowSize, 0)
        slidingWindow = yValues[startingX:endingX+1]
        stddev = np.std(slidingWindow)
        stddevs.append(stddev)

    return stddevs

def getLastAverageValue(data: list, numberValues: int):
    values = data[-numberValues:]
    return np.mean(values)


def createAverageStrategyGraph(generationValues: list, probCopAfterCopValues: list, probCopAfterDefValues: list, stabilityValues: list, outputDir: str, exeArguments: dict, IVname:str):
    # stddevs = getStabalizationMoment(probCopAfterCopValues)
    fig, ax = plt.subplots()
    fig.set_size_inches(figureWidth, figureHeight)
    ax.set_title(f"Average Strategy v. Time")
    fig.text(0.01, 0.01, str(exeArguments))

    ax.set_xlabel("Generation")
    ax.set_xlim(0, max(generationValues))
    ax.set_ylabel("Probability")
    ax.set_ylim(0,1)
    ax.grid(True)

    ax.plot(generationValues, probCopAfterCopValues, marker=".", label="Probability Cop after Cop")
    ax.plot(generationValues, probCopAfterDefValues, marker=".", label="Probability Cop after Def")
    # stddevs = normalizeData(stddevs)
    # ax.plot(generationValues, stddevs, marker=".", label="Stabilization Stddev")
    ax.plot(generationValues, stabilityValues, marker=".", label="Stability Values")
    ax.axhline(exeArguments["stabilityThreshold"], color="red", linewidth=0.5)

    ax.legend(loc="upper right")
    #ax.legend(bbox_to_anchor=(1.05, 1.0), loc="upper left")
    #plt.tight_layout()
    plt.savefig(os.path.join(outputDir, f"Strategy vs Time at {IVname}={exeArguments[IVname]}.png"))
    plt.close(fig)


def createPopulationSnapshot(fig: plt.Figure, ax: plt.Axes, generation: int, probCopAfterCopValues: list, probCopAfterDefValues: list, exeArguments: dict):
    ax.set_title(f"Population at t=" + generation)
    fig.text(0.01, 0.01, str(exeArguments))

    ax.set_xlabel("Probability Cop after Cop")
    ax.set_ylabel("Probability Cop after Def")
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.grid(True)

    #ax.legend(loc="upper right")
    #ax.legend(bbox_to_anchor=(1.05, 1.0), loc="upper left")
    #plt.tight_layout()
    
    ax.plot(probCopAfterCopValues, probCopAfterDefValues, marker="o", label="Probability Cop after Cop", linestyle="none")



def createAnimation(data: list, exeArguments: dict, outputDir: str):
    fig, ax = plt.subplots()
    fig.set_size_inches(figureWidth, figureHeight)

    def update(frame):
        ax.clear()
        generation, probCopAfterCopValues, probCopAfterDefValues = data[frame]
        createPopulationSnapshot(fig, ax, generation, probCopAfterCopValues, probCopAfterDefValues, exeArguments)

    ani = FuncAnimation(fig, update, frames=len(data), interval=50)
    ani.save(os.path.join(outputDir, "Population Animation.gif"), writer="pillow")


def createFinalGraph(IV: tuple, convergenceValues: list, exeArguments: dict, outputDir: str):
    IVname = IV[0]
    IVvalues = IV[1]

    fig, ax = plt.subplots()
    fig.set_size_inches(figureWidth, figureHeight)
    ax.set_title(f"{IVname} vs Population Convergence")

    fig.text(0.01, 0.01, str(exeArguments))

    ax.set_xlabel("Miscommunication Rate")
    ax.set_xlim(min(IVvalues), max(IVvalues))
    ax.set_ylabel("Probability")
    ax.set_ylim(0,1)
    ax.grid(True)

    probCopAfterCopValues = convergenceValues[::2]
    probCopAfterDefValues = convergenceValues[1::2]

    ax.plot(IVvalues, probCopAfterCopValues, marker=".", label="Probability Cop after Cop")
    ax.plot(IVvalues, probCopAfterDefValues, marker=".", label="Probability Cop after Def")

    ax.legend(loc="upper right")
    #ax.legend(bbox_to_anchor=(1.05, 1.0), loc="upper left")
    #plt.tight_layout()
    plt.savefig(os.path.join(outputDir, f"Final Graph.png"))
    plt.close(fig)


def runIPD(exeArguments: dict, folder: str, IV: tuple, convergenceList: list):
    IVname = IV[0]
    IVvalue = exeArguments[IVname]

    outputDir = getOutputDir(folder)

    allData = []

    process = subprocess.Popen(
        [os.path.join(os.getcwd(), exeFile), *(str(argument) for argument in exeArguments.values())],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,  # Ensures the output is already decoded as text
        bufsize=1,  # Line-buffered output
    )

    for line in iter(process.stdout.readline, ""):
        line = line.strip()
        if not line:  # Skip empty lines
            continue

        if "[NOTICE]" in line:
            print(line)
            continue

        if "[GRAPH 1]" in line:
            line = line.replace("[GRAPH 1]", "")
            values = list(filter(lambda x: x.strip(), line.split(",")))
            generation = values[0]
            probCopAfterCopValues = [float(values[i].strip()) for i in range(1, len(values), 2)]
            probCopAfterDefValues = [float(values[i].strip()) for i in range(2, len(values), 2)]
            #createSingleGenerationGraph(generation, probCopAfterCopValues, probCopAfterDefValues)
            allData.append((generation, probCopAfterCopValues, probCopAfterDefValues))
            continue

        if "[GRAPH 2]" in line:
            line = line.replace("[GRAPH 2]", "")
            values = list(filter(lambda x: x.strip(), line.split(",")))
            probCopAfterCopValues = [float(values[i].strip()) for i in range(0, len(values), 3)]
            probCopAfterDefValues = [float(values[i].strip()) for i in range(1, len(values), 3)]
            stabilityValues = [float(values[i].strip()) for i in range(2, len(values), 3)]
            createAverageStrategyGraph(list(range(0,len(probCopAfterCopValues))), probCopAfterCopValues, probCopAfterDefValues, stabilityValues, outputDir, exeArguments)
            continue

        else:
            print("[MISC] " + line)
            continue

    convergenceList.append(getLastAverageValue(probCopAfterCopValues, exeArguments["slidingWindowSize"]))
    convergenceList.append(getLastAverageValue(probCopAfterDefValues, exeArguments["slidingWindowSize"]))
    process.stdout.close()
    process.wait()

    createAnimation(allData, exeArguments, outputDir)
    print(f"Graphs succesfully saved in: {outputDir}")


IV = ("miscommunicationRate", [0.01 * i for i in range(0,10)])
DVvalues = []

for i in IV[1]:
    exeArguments = {
        "miscommunicationRate": i,
        "misexecutionRate": 0.0,
        "mutationStddev": 0.005,
        "generations": 100,
        "matchupIterations": 100,
        "populationSize": 100, # doesn't work
        "stabilityThreshold": 0.01, # 0.005
        "slidingWindowSize": 20,
        "parallelProcess": False
    }
    exeArguments[IV[0]] = i

    folder = "Label test"

    print(f"Running IPD with", exeArguments)
    runIPD(exeArguments, folder, IV, DVvalues)

createFinalGraph(IV, DVvalues, exeArguments, getOutputDir(folder))