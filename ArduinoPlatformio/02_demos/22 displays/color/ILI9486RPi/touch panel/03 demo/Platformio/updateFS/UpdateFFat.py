import os
Import("env")

is_windows = os.name == 'nt'
# Since PlatformIO does not yet have a tool-mkfatfs, we have
# to last-minute correct the path to a locally downloaded version.
env.Replace(
    MKSPIFFSTOOL=env.get("PROJECT_DIR") + '/updateFS/' + 'UpdateFFat_windows' if is_windows else 'UpdateFFat_linux'
)