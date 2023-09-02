import sys
import platform
from enum import Enum
import os

class ProjectType(Enum):
    Library = 1
    Executable = 2
    def __str__(self):
        return self.name

class LibType(Enum):
    Shared = 1
    Static = 2
    def __str__(self):
        return self.name

class ReleaseType(Enum):
    Debug = 1
    Release = 2
    def __str__(self):
        return self.name

class AppOperations(Enum):
    Configure = 1
    Build = 2
    def __str__(self):
        return self.name

def GetScriptRootFolder() -> os.path:
    return os.path.abspath(os.path.dirname(os.path.abspath(__file__)))

def GetScriptBuildPath(buildFolderName : str) -> os.path:
    return os.path.join(os.path.abspath(os.path.dirname(os.path.abspath(__file__))), buildFolderName)

def AskForInput():
    inp = input(": ")
    print("----------")
    return inp

class Configurator:
    def __init__(self): 
        self._cmd = ["cmake"]
    def AddLibType(self, libType : LibType) -> None:
        self._cmd.append(["-DARX_LIB_STATIC=OFF", "-DARX_LIB_STATIC=ON"][libType.value - 1])
    def AddReleaseType(self, relType : ReleaseType) -> None:
        self._cmd.append(["-DCMAKE_BUILD_TYPE=Debug", "-DCMAKE_BUILD_TYPE=Release"][relType.value - 1])
    def AddBuildTest(self, buildTest : bool) -> None:
        self._cmd.append(["-DARX_BUILD_TEST=OFF", "-DARX_BUILD_TEST=ON"][int(buildTest)])
    def AddSourceFolder(self, sourcePath : os.path) -> None:
        self._cmd.extend(["-S", str(sourcePath)])
    def AddLibBuildFolder(self, sourcePath : os.path, buildFolderName : str, libType : LibType, relType : ReleaseType) -> None:
        self._cmd.extend(["-B", f'{str(sourcePath)}/{buildFolderName}/{str(libType)}_{str(relType)}'])
    def AddExecBuildFolder(self, sourcePath : os.path, buildFolderName : str, relType : ReleaseType) -> None:
        self._cmd.extend(["-B", f'{str(sourcePath)}/{buildFolderName}/Exec_{relType}'])
    def AddBuildProjectCommand(self, buildFolder : os.path, projName : str):
        self._cmd.extend(["--build", f'{buildFolder}/{projName}'])
        
    def GetCmd(self) -> str:
        return " ".join(self._cmd)

def GetLibTypeFromUser() -> LibType:
    cntr = 0
    print("Library type:")
    for libType in LibType:
        print(f'[{cntr + 1}] {libType}')
        cntr += 1
    libType = 0
    while True:
        try:
            libType = LibType(int(AskForInput()))
            break
        except ValueError:
            print("Invalid input. Try again.")
    return libType 

def GetReleaseTypeFromUser() -> ReleaseType:
    cntr = 0
    print("Release type:")
    for relType in ReleaseType:
        print(f'[{cntr + 1}] {relType}')
        cntr += 1
    relType = 0
    while True:
        try:
            relType = ReleaseType(int(AskForInput()))
            break
        except ValueError:
            print("Invalid input. Try again.")
    return relType

def GetDoBuildTestFromUser() -> bool:
    print("Build test?")
    print("[1] Yes")
    print("[2] No")
    i = 0
    while True:
        try:
            i = int(AskForInput()) - 1
            break
        except ValueError:
            print("Invalid input. Try again.")
    return bool([True, False][i])

def Configure(projType : ProjectType) -> None:
    configurator = Configurator()
    libType = None
    if (projType == ProjectType.Library):
        libType = GetLibTypeFromUser()
        configurator.AddLibType(libType)
    
    relType = GetReleaseTypeFromUser()
    configurator.AddReleaseType(relType)
    if (projType == ProjectType.Library):
        configurator.AddLibBuildFolder(GetScriptRootFolder(), "build", libType, relType)
    else:
        configurator.AddExecBuildFolder(GetScriptRootFolder(), "build", relType)
    configurator.AddBuildTest(GetDoBuildTestFromUser())
    configurator.AddSourceFolder(GetScriptRootFolder())
    os.system(configurator.GetCmd())

def Build(projType : ProjectType):
    buildFolder =  GetScriptBuildPath("build")     
    valid_folder_names = []
    if (projType == ProjectType.Library):
        for libType in LibType:
            for relType in ReleaseType:
                valid_folder_names.append(f'{str(libType)}_{str(relType)}')
    else:
        for relType in ReleaseType:
            valid_folder_names.append(f'Exec_{str(relType)}')

    folders = [name for name in os.listdir(buildFolder) if os.path.isdir(os.path.join(buildFolder, name)) and name in valid_folder_names]
    if (len(folders) == 0):
        print("Configured folder not found")
        return
    elif(len(folders) == 1):
        os.system(f'cmake --build {buildFolder}/{folders[0]}')
        return
    
    print("Projects")
    for i in range(0, len(folders)):
        print(f'[{i + 1}] {folders[i]}')
    option = 0
    while True:
        try:
            option = int(AskForInput()) - 1
            break
        except ValueError:
            print("Invalid input. Try again.")
    os.system(f'cmake --build {buildFolder}/{folders[option]}')    

def SelectAction(projType : ProjectType):
    cntr = 0
    print("Commands: ")
    for cmd in AppOperations:
        print(f'[{cntr + 1}] {cmd}')
        cntr += 1
    i = 0
    while True:
        try:
            i = int(AskForInput()) - 1
            break
        except ValueError:
            print("Invalid input. Try again.")
    
    [Configure, Build][i](projType)
           
def main() -> int:
    SelectAction(ProjectType.Library)
    return 0

if __name__ == '__main__':
    main()
