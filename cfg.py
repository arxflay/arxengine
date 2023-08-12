import sys
import platform
import os
import curses

project_types = {
    "Shared" : "-DPROJECT_TYPE=Static",
    "Static" : "-DPROJECT_TYPE=Shared"
}

release_types = {
    "Debug" : "-DCMAKE_BUILD_TYPE=Debug",
    "Release" : "-DCMAKE_BUILD_TYPE=Release"
}

def makeConsoleSane():
    if (os.name != "Windows"):
        os.system("stty sane")


def configure(win : 'curses._CursesWindow') -> None:
    cfgString = ["cmake","-S", ".", "-B"]
 
    keys_type = list(project_types)
    win.addstr("Project type: ")
    for i in range(0, len(keys_type)):
        win.addstr(i + 1, 0, f'[{i + 1}] {keys_type[i]}')
    win.addstr(len(keys_type) + 1, 0, ": ")
    option = int(win.getstr()) -1
    win.clear()
    projectTypeName = keys_type[option]
    projectType =  project_types[keys_type[option]] 
    
    keys_release = list(release_types)
    win.addstr("Release type: ")
    for i in range(0, len(keys_release)):
        win.addstr(i + 1, 0, f'[{i + 1}] {keys_release[i]}')
    win.addstr(len(keys_release) + 1, 0, ": ")
    option = int(win.getstr()) - 1
    releaseTypeName = keys_release[option]
    releaseType =  release_types[keys_release[option]]

    projectFolder = f'build/{projectTypeName}_{releaseTypeName}'
    cfgString.append(projectFolder)
    cfgString.append(projectType)
    cfgString.append(releaseType)
    curses.endwin()
    makeConsoleSane()
    os.system(" ".join(cfgString))


valid_folder_names = [
    "Shared_Debug",
    "Static_Debug",
    "Shared_Release",
    "Static_Release"
]

def build(win : 'curses._CursesWindow'):
    thisFolder = os.path.join(os.path.abspath(os.path.dirname(os.path.abspath(__file__))), "build")
    folders = [name for name in os.listdir(thisFolder) if os.path.isdir(os.path.join(thisFolder, name)) and name in valid_folder_names]
    if (len(folders) == 0):
        curses.endwin()
        makeConsoleSane()
        print("Configured folder not found")
        return
    elif(len(folders) == 1):
        curses.endwin()
        makeConsoleSane()
        os.system("cmake --build build/" + folders[0])
        return
    
    win.addstr("Projects")
    for i in range(0, len(folders)):
        win.addstr(i + 1, 0, f'[{i + 1}] {folders[i]}')

    win.addstr(len(folders) + 1, 0, ": ")
    option = int(win.getstr()) - 1
    curses.endwin()
    makeConsoleSane()
    os.system("cmake --build " + folders[option])    

main_menu_options = {
    "Configure" : configure,
    "Build" : build 
}

def selectAction(win : 'curses._CursesWindow'):
    keys = list(main_menu_options.keys())
    if len(sys.argv) > 1:
        if (sys.argv[1] in keys):
            win.addstr(f'cfg {keys}')
            return
        main_menu_options[sys.argv[1]]()
        return

    win.addstr(0, 0, "Command: ")
    for i in range(0, len(main_menu_options)):
        win.addstr(i + 1, 0, f'[{i + 1}] {keys[i]}')
    win.addstr(len(keys) + 1, 0, ": ")
    win.refresh()
    option = int(win.getstr()) - 1
    
    win.clear()
    main_menu_options[keys[option]](win)

def main() -> int:
    win = curses.initscr()
    selectAction(win)
    return 0

if __name__ == '__main__':
    main()
