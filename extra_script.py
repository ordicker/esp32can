import os
Import("env")

# include toolchain paths
env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)

# override compilation DB path
env.Replace(COMPILATIONDB_PATH=os.path.join("$BUILD_DIR", "compile_commands.json"))

# make link to main directory
os.system("rm ./compile_commands.json") #remove old file
os.system("ln -s .pio/build/esp32-main/compile_commands.json ./compile_commands.json") # make soft link
