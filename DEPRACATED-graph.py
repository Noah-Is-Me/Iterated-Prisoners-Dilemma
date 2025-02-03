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

# markers:  , . o x X none "" 


def createAverageStrategyGraph(generationValues, probCopAfterCopValues, probCopAfterDefValues):
    fig, ax = plt.subplots()
    fig.set_size_inches(figureWidth, figureHeight)
    ax.set_title("Average Strategy v. Time")

    ax.set_xlabel("Generation")
    ax.set_xlim(0, len(generationValues))
    ax.set_ylabel("Probability")
    ax.set_ylim(0,1)
    ax.grid(True)

    ax.plot(generationValues, probCopAfterCopValues, marker=".", label="Probability Cop after Cop")
    ax.plot(generationValues, probCopAfterDefValues, marker=".", label="Probability Cop after Def")

    ax.legend(loc="upper right")
    #ax.legend(bbox_to_anchor=(1.05, 1.0), loc="upper left")
    #plt.tight_layout()
    plt.savefig(os.path.join(outputDir, f"Strategy vs Time.png"))


def createPopulationGraph(generation, probCopAfterCopValues, probCopAfterDefValues):
    fig, ax = plt.subplots()
    fig.set_size_inches(figureWidth, figureHeight)
    ax.set_title("Population at t=" + generation)

    ax.set_xlabel("Probability Cop after Cop")
    ax.set_xlim(0,1)
    ax.set_ylabel("Probability Cop after Def")
    ax.set_ylim(0,1)
    ax.grid(True)

    ax.plot(probCopAfterCopValues, probCopAfterDefValues, marker="o", linestyle="None")
    #plt.tight_layout()
    plt.savefig(os.path.join(outputDir, f"Population at t={generation} Graph"))



process = subprocess.Popen(
    [os.path.join(os.getcwd(), exeFile)],
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
        createPopulationGraph(generation, probCopAfterCopValues, probCopAfterDefValues)
        continue

    if "[GRAPH 2]" in line:
        line = line.replace("[GRAPH 2]", "")
        values = list(filter(lambda x: x.strip(), line.split(",")))
        probCopAfterCopValues = [float(values[i].strip()) for i in range(0, len(values), 2)]
        probCopAfterDefValues = [float(values[i].strip()) for i in range(1, len(values), 2)]
        createAverageStrategyGraph(list(range(0,len(probCopAfterCopValues))), probCopAfterCopValues, probCopAfterDefValues)
        continue


process.stdout.close()
process.wait()

print(f"Graphs succesfully saved in: {outputDir}")




