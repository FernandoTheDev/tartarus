import os 
import subprocess
import json 
import time 
import copy 
import multiprocessing 
from typing import cast 
import sys 
import shutil

with open("./build", "r") as File:
    Info = json.load(File)
Name: str = Info.get("Name", "out")
Flags = Info.get("Flags", [])

if "-debug" in sys.argv:
    Flags+=["-g"]

Ignore  = Info.get("Ignore", [])
Compiler = Info.get("Compiler")
print(f"[Info]: using compiler... {Compiler}")
if os.path.isdir("./.Temp") == False:
    os.mkdir("./.Temp")
    with open("./.Temp/pch.json", "w") as File:
        File.write("{}");

if "-clear" in sys.argv:
    shutil.rmtree("./.Temp")
    os.mkdir("./.Temp")
    with open("./.Temp/pch.json", "w") as File:
        File.write("{}");
    exit(0)

for indx, i in enumerate(Ignore): Ignore[indx] = os.path.abspath(i)

def Getcppfiles(Files: list, Path, ext: str = ".cpp"):
    f = []
    for i in Files:
        pth = Path + i
        if os.path.isfile(pth):
            if pth[-len(ext):] == ext:
                if os.path.abspath(pth) not in Ignore:
                    
                    f.append(pth)
        elif os.path.isdir(pth):
            if os.path.abspath(pth) not in Ignore:
                f+=Getcppfiles(os.listdir(pth), pth + "/")

    return f 

with open("./.Temp/pch.json", "r") as File:
    info = json.load(File)

NewInfo = copy.deepcopy(info)
Files = Getcppfiles(os.listdir('.'), "./", ext=Info.get("Type", ".cpp"))if Info['Files'] == "Grab" else Info['Files']

Comp = []
if "-recomp" not in sys.argv:
    for i in Files:
        if info.get(i) == None: 
            NewInfo[i] = os.path.getmtime(os.path.abspath(i))
            Comp.append(i)
        else: 
            if info[i] != os.path.getmtime(os.path.abspath(i)):
                Comp.append(i)
                NewInfo[i] = os.path.getmtime(os.path.abspath(i))
else: Comp = Files


def runcmdobj(i):
    cmd = [Compiler, "-c", i, "-o", f"./.Temp/{i.replace('/', '_')}.o"] 
    cmd+=Flags
    print(f"[Info]: Running Cmd: {cmd}")
    x = subprocess.run(cmd)
    if(x.returncode != 0): raise RuntimeError

if len(Comp) != 0:
    num_proc = min(len(Comp), multiprocessing.cpu_count())
    with multiprocessing.Pool(processes=num_proc) as pool:
        try:
            pool.map(runcmdobj, Comp)
        except RuntimeError:
            exit(1)
            

with open("./.Temp/pch.json", "w") as File:
    json.dump(NewInfo, File, indent=4)


if len(Comp) != 0 or "-anyway" in sys.argv:
    RunCmd = [Compiler] + Getcppfiles(os.listdir("./.Temp"), "./.Temp/", ext=".o") + ["-o", Name] + Flags
    print(f"[Info]: Running Cmd: {RunCmd}")
    x = subprocess.run(RunCmd)
    if(x.returncode != 0): exit(0)


if "-r" in sys.argv:
    RunCmd = ["./" + Name] + sys.argv[sys.argv.index("-r") + 1:]
    print(f"[Info]: Running Cmd: {RunCmd}\n")
    cmd = subprocess.run(RunCmd)
    print("\n[Info]: ErrCode:", cmd.returncode)
