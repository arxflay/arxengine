import sys
import platform
import curses
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

class Configurator:
    def __init__(self): 
        self._cmd = ["cmake"]
    def AddLibType(self, libType : LibType) -> None:
        self._cmd.append(["-DPROJECT_TYPE=Shared", "-DPROJECT_TYPE=Static"][libType.value - 1])
    def AddReleaseType(self, relType : ReleaseType) -> None:
        self._cmd.append(["-DCMAKE_BUILD_TYPE=Debug", "-DCMAKE_BUILD_TYPE=Release"][relType.value - 1])
    def AddBuildTest(self, buildTest : bool) -> None:
        self._cmd.append(["-DBUILD_TEST=False", "-DBUILD_TEST=True"][int(buildTest)])
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

def MakeConsoleSane():
    if (os.name != ["Unix", "Linux"]):
        os.system("stty sane")

def AskForInput(win : 'curses._CursesWindow') -> str:
    win.addstr(": ")
    return win.getstr()

def GetLibTypeFromUser(win : 'curses._CursesWindow') -> LibType:
    win.clear()
    cntr = 0
    win.addstr("Library type: \n")
    for libType in LibType:
        win.addstr(f'[{cntr + 1}] {libType}\n')
        cntr += 1
    return LibType(int(AskForInput(win)))

def GetReleaseTypeFromUser(win : 'curses._CursesWindow') -> ReleaseType:
    win.clear()
    cntr = 0
    win.addstr("Release type: \n")
    for relType in ReleaseType:
        win.addstr(f'[{cntr + 1}] {relType}\n')
        cntr += 1
    return ReleaseType(int(AskForInput(win)))

def GetDoBuildTestFromUser(win : 'curses._CursesWindow') -> bool:
    win.clear()
    win.addstr("Build test?\n")
    win.addstr("[1] Yes\n")
    win.addstr("[2] No \n")
    return bool([True, False][int(AskForInput(win)) -1])


def Configure(win : 'curses._CursesWindow', projType : ProjectType) -> None:
    configurator = Configurator()
    libType = None
    if (projType == ProjectType.Library):
        libType = GetLibTypeFromUser(win)
        configurator.AddLibType(libType)
    
    relType = GetReleaseTypeFromUser(win)
    configurator.AddReleaseType(relType)
    if (projType == ProjectType.Library):
        configurator.AddLibBuildFolder(GetScriptRootFolder(), "build", libType, relType)
    else:
        configurator.AddExecBuildFolder(GetScriptRootFolder(), "build", relType)
    configurator.AddBuildTest(GetDoBuildTestFromUser(win))
    configurator.AddSourceFolder(GetScriptRootFolder())
    MakeConsoleSane()
    os.system(configurator.GetCmd())

def Build(win : 'curses._CursesWindow', projType : ProjectType):
    win.clear()
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
        curses.endwin()
        MakeConsoleSane()
        print("Configured folder not found")
        return
    elif(len(folders) == 1):
        curses.endwin()
        MakeConsoleSane()
        os.system(f'cmake --build {buildFolder}/{folders[0]}')
        return
    
    win.addstr("Projects\n")
    for i in range(0, len(folders)):
        win.addstr(f'[{i + 1}] {folders[i]}\n')

    win.addstr(len(folders) + 1, 0, ": ")
    option = int(win.getstr()) - 1
    curses.endwin()
    MakeConsoleSane()
    os.system(f'cmake --build {buildFolder}/{folders[option]}')    

def SelectAction(win : 'curses._CursesWindow', projType : ProjectType):
    cntr = 0
    win.addstr("Commands: \n")
    for cmd in AppOperations:
        win.addstr(f'[{cntr + 1}] {cmd}\n')
        cntr += 1
    
    [Configure, Build][int(AskForInput(win)) - 1](win, projType)
           
def main() -> int:
    win = curses.initscr() 
    curses.echo()
    win.keypad(True)
    curses.cbreak()
    try:
        SelectAction(win, ProjectType.Library)
    except Exception as e:
        MakeConsoleSane()
        raise e
    return 0

if __name__ == '__main__':
    main()
