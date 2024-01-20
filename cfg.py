import sys
import platform
from enum import Enum
import os

class ProjectType(Enum):
    Library = 0
    Executable = 1
    def __str__(self):
        return self.name

class LibType(Enum):
    Shared = 0
    Static = 1
    def __str__(self):
        return self.name

class ReleaseType(Enum):
    Debug = 0
    Release = 1
    def __str__(self):
        return self.name
    
    @staticmethod
    def FromStr(val: str):
        if(val == "Release"):
            return ReleaseType.Release
        elif(val == "Debug"):
            return ReleaseType.Debug

        raise ValueError

class AppOperations(Enum):
    Configure = 0
    Build = 1
    Test = 2
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

def SelectVariant(name, variants) -> int:
    if (len(variants) <= 1):
        return 0
    
    print(f'{name}: ')
    cntr = 0
    for variant in variants:
        print(f'[{cntr + 1}] {variant}')
        cntr += 1
    variantIndex = 0
    while True:
        try:
            variantIndex = int(AskForInput()) - 1
            if (variantIndex > len(variants) or variantIndex < 0):
                raise ValueError
            
            break
        except ValueError:
            print("Invalid input. Try again.")
    
    return variantIndex

class Configurator:
    def __init__(self): 
        self._cmd = ["cmake"]
    def AddLibType(self, libType : LibType) -> None:
        self._cmd.append(["-DARX_LIB_STATIC=OFF", "-DARX_LIB_STATIC=ON"][libType.value])
    def AddReleaseType(self, relType : ReleaseType) -> None:
        self._cmd.append(["-DCMAKE_BUILD_TYPE=Debug", "-DCMAKE_BUILD_TYPE=Release"][relType.value])
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
    def AddBuildReleaseType(self, relType : ReleaseType):
        self._cmd.extend(["--config",  ["Debug", "Release"][relType.value]])
    def GetCmd(self) -> str:
        return " ".join(self._cmd)

def GetLibTypeFromUser() -> LibType:
    return LibType(SelectVariant("Library type", LibType))

def GetReleaseTypeFromUser() -> ReleaseType:
    return ReleaseType(SelectVariant("Release type", ReleaseType))

def GetDoBuildTestFromUser() -> bool:
    return bool([True, False][SelectVariant("Build test", ["Yes", "No"])])

def GetBuildFolders(projType : ProjectType, buildFolder : os.path) -> list:
    valid_folder_names = []
    if (projType == ProjectType.Library):
        for libType in LibType:
            for relType in ReleaseType:
                valid_folder_names.append(f'{str(libType)}_{str(relType)}')
    else:
        for relType in ReleaseType:
            valid_folder_names.append(f'Exec_{str(relType)}')

    return [name for name in os.listdir(buildFolder) if os.path.isdir(os.path.join(buildFolder, name)) and name in valid_folder_names]

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
    configurator = Configurator()
    buildFolder =  GetScriptBuildPath("build")     
    folders = GetBuildFolders(projType, buildFolder) 
    if (len(folders) == 0):
        print("Configured projects not found")
        return
    folder = folders[SelectVariant("Projects", folders)]
    relType = ReleaseType.FromStr(folder.split("_")[1])
    configurator.AddBuildProjectCommand(buildFolder, folder)
    configurator.AddBuildReleaseType(relType)
    os.system(configurator.GetCmd())    

def Test(projType : ProjectType):
    buildFolder =  GetScriptBuildPath("build")     
    folders = GetBuildFolders(projType, buildFolder) 
    if (len(folders) == 0):
        print("Configured projects not found")
        return

    folder = folders[SelectVariant("Projects", folders)]
    fullPath = os.path.join(buildFolder, folder)
    os.system(f'sh -c "cd {fullPath} && ctest"')

def SelectAction(projType : ProjectType):
    [Configure, Build, Test][SelectVariant("Commands",  AppOperations)](projType)
           
def main() -> int:
    SelectAction(ProjectType.Library)
    return 0

if __name__ == '__main__':
    main()
