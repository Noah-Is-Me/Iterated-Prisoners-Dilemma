import matplotlib.pyplot as plt
#from mpl_toolkits.mplot3d import Axes3D
import subprocess
import numpy as np
import os
import sys
from datetime import datetime
import signal
import matplotlib.transforms as mtransforms
from collections import Counter
from matplotlib.animation import FuncAnimation
import csv
from concurrent.futures import ProcessPoolExecutor, as_completed
import time
from datetime import datetime, timedelta


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


if sys.platform == "win32": # Windows
    gppPath = r"C:\msys64\ucrt64\bin\g++.exe"
    buildCommand = [gppPath, "-fdiagnostics-color=always", "-std=c++2a", "-g", "-O3", cppFile, *linkageFiles, "-o", exeFile]

else: # Linux/Chromebook
    gppPath = r"/usr/bin/g++"
    buildCommand = [gppPath, "-fdiagnostics-color=always", "-std=c++2a", "-g", cppFile, "-pthread", "-o", exeFile, *linkageFiles]


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

def getOutputDir(*folders):

    if folders:
        outputDir = os.path.join(commitFolderDir, *folders)
    else:
        currentTime = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
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
    plt.savefig(os.path.join(outputDir, f"Strategy vs Time at {IVname}={exeArguments[IVname]}, {probCopAfterCopValues[4]}.png"))
    plt.close(fig)


def createPopulationSnapshot(fig: plt.Figure, ax: plt.Axes, generation: int, probCopAfterCopValues: list, probCopAfterDefValues: list, exeArguments: dict):
    ax.set_title(f"Population at t=" + generation)
    # fig.text(0.01, 0.01, str(exeArguments))

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
    ani.save(os.path.join(outputDir, f"Population Animation, {data[5][1][5]}.gif"), writer="pillow")
    plt.close(fig)


def createFinalGraph(IV: tuple, convergenceValues: list, exeArguments: dict, IVtrialCount: int, outputDir: str):
    IVname = IV[0]

    fig, ax = plt.subplots()
    fig.set_size_inches(figureWidth, figureHeight)
    ax.set_title(f"{IVname} vs Population Convergence")

    fig.text(0.01, 0.01, str(exeArguments))

    IVvalues, probCopAfterCopValues, CACstddev, probCopAfterDefValues, CADstddev = zip(*convergenceValues)

    CACstderr = [stddev/np.sqrt(IVtrialCount) for stddev in CACstddev]
    CADstderr = [stddev/np.sqrt(IVtrialCount) for stddev in CADstddev]

    ax.set_xlabel("Miscommunication Rate")
    ax.set_xlim(min(IVvalues), max(IVvalues))
    ax.set_ylabel("Probability")
    ax.set_ylim(0,1)
    ax.grid(True)


    ax.errorbar(IVvalues, probCopAfterCopValues, yerr=CACstderr, capsize=4, marker=".", label="Probability Cop after Cop", linestyle="None")
    ax.errorbar(IVvalues, probCopAfterDefValues, yerr=CADstderr, capsize=4, marker=".", label="Probability Cop after Def", linestyle="None")

    ax.legend(loc="upper right")
    #ax.legend(bbox_to_anchor=(1.05, 1.0), loc="upper left")
    #plt.tight_layout()
    plt.savefig(os.path.join(outputDir, f"Final Graph.png"))
    plt.close(fig)


def runIPD(exeArguments: dict, folder: str, IV: tuple):
    global process
    
    IVname = IV[0]
    IVvalue = exeArguments[IVname]

    # currentIV = f"{IVname}={IVvalue}"
    # outputDir = getOutputDir(folder, currentIV)

    allData = []

    # print([os.path.join(os.getcwd(), exeFile), *(str(argument) for argument in exeArguments.values())])
    if sys.platform == "win32": # Windows
        runPath = exeFile
    else:
        runPath = "iterated-prisoners-dilemma"
    
    process = subprocess.Popen(
        [os.path.join(os.getcwd(), runPath), *(str(argument) for argument in exeArguments.values())],
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
            # createAverageStrategyGraph(list(range(0,len(probCopAfterCopValues))), probCopAfterCopValues, probCopAfterDefValues, stabilityValues, outputDir, exeArguments, IVname)
            continue

        else:
            print("[MISC] " + line)
            continue


    process.stdout.close()
    process.wait()

    # createAnimation(allData, exeArguments, outputDir)
    # print(f"Graphs succesfully saved in: {outputDir}")

    DVs = (getLastAverageValue(probCopAfterCopValues, exeArguments["slidingWindowSize"]), getLastAverageValue(probCopAfterDefValues, exeArguments["slidingWindowSize"]))
    return DVs


exeArguments = {
        "miscommunicationRate": 0.1,
        "misexecutionRate": 0.05,
        "mutationStddev": 0, # 0.005
        "generations": 100,
        "matchupIterations": 50,
        "populationSize": 30, 
        "stabilityThreshold": 0.01, # 0.005
        "slidingWindowSize": 20,
        "parallelProcess": True
    }


IV = ("mutationStddev", [0.0001 * i for i in range(0,1001)])
IVtrialCount = 100
folder = "DEMO RUN"
maxCores = 13



csvOutputDir = getOutputDir(folder)
csvFile = os.path.join(csvOutputDir, f"Raw_Data_{folder}.csv")
with open(csvFile, mode="w", newline="") as file:
    writer = csv.writer(file)
    writer.writerow(["IV_Name", "IV_Value", "Trial", "Prob_Cop_After_Cop", "Prob_Cop_After_Def"])


IVlevelArgs = [{**exeArguments, IV[0]: IVlevel} for IVlevel in IV[1]]


def runIVlevel(IVname, IVlevel):
    print(f"Running {IVname}={IVlevel}")
    args = exeArguments.copy()
    args[IVname] = IVlevel

    CACconvergenceValues = []
    CADconvergenceValues = []

    csvRows = []

    for trial in range(IVtrialCount):
        # print(f"[{IV[0]}={IVlevel}] Trial {trial+1}/{IVtrialCount}")
        (CACconvergence, CADconvergence) = runIPD(args, folder, IV)
        CACconvergenceValues.append(CACconvergence)
        CADconvergenceValues.append(CADconvergence)
        csvRows.append([IV[0], IVlevel, trial, CACconvergence, CADconvergence])

    CACconvergenceAverage = np.average(CACconvergenceValues)
    CACstddev = np.std(CACconvergenceValues)
    CADconvergenceAverage = np.average(CADconvergenceValues)
    CADstddev = np.std(CADconvergenceValues)

    return (IVlevel, CACconvergenceAverage, CACstddev, CADconvergenceAverage, CADstddev, csvRows)


DVvalues = []

startTime = time.time()


if __name__ == "__main__":
    with ProcessPoolExecutor(max_workers=maxCores) as executor:
        futures = [executor.submit(runIVlevel, IV[0], IVlevel) for IVlevel in IV[1]]

        for future in as_completed(futures):
            (IVlevel, CACconvergenceAverage, CACstddev, CADconvergenceAverage, CADstddev, csvRows) = future.result()
            DVvalues.append((IVlevel, CACconvergenceAverage, CACstddev, CADconvergenceAverage, CADstddev))

            with open(csvFile, mode="a", newline="") as file:
                writer = csv.writer(file)
                writer.writerows(csvRows)
            
            elapsedTime = time.time() - startTime
            remainingIVlevels = len(futures) - len(DVvalues)
            avgTimePerIVlevel = elapsedTime / len(DVvalues)
            estimatedRemainingTime = avgTimePerIVlevel * remainingIVlevels

            print(f"Completed IV level {IVlevel}  ({len(DVvalues)}/{len(futures)}). Total elapsed time: {timedelta(seconds=int(elapsedTime))}, "
                  f"Estimated remaining time: {timedelta(seconds=int(estimatedRemainingTime))}")

    createFinalGraph(IV, DVvalues, exeArguments, IVtrialCount, getOutputDir(folder))