This file gives information how to work with this tool chain.


The tool chain can be operated in following ways:
1. With command line
2. With Eclipse
3. With Visual Studio Code


Attention:
After first checkout or unpacking of a Zip file, the command line must be used to setup the tool chain.
See the instructions below. "make all" will prepare the tool chain.


Directories:
- smack_sl    :  sample project
- smack_rom   :  ROM library
- smack_lib   :  NVM library
- tools       :  build tools
- tool_config :  configuration files for build tools
- scripts     :  scripts for build tools
- .vscode     :  sample configuration files for Visual Studio Code


#Command line:
- double click on build_shell.bat to open a command shell
- issue 'make all' to execute
- other targets: clean, tools (unpack tool chain)

#Eclipse
- open any Eclipse (recommended is Mars2)
- import the existing project files of this repository (consider nested projects, too)
Note: Unpacking the tool chain is not supported from Eclipse


# Lauterbach
A batch file is provided to connect Lauterbach to the target:
- make sure the Lauterbach SW is installed (in C:\T32) and HW is connected properly with the device/FPGA
- run batch file "t32_start_real.bat" (currently the batch file only supports 64-bit version. For 32-bit modify the file accordingly)
Note: Additional batch files are provided to connect to specific silicon revision. 
      Please make sure to use the appropriate file (usually "t32_start_real.bat").

# Segger J-Link
- Configuration files for SmAcK are provided in directory "tool_config/jlink". These can be used in Eclipse and Visual Studio Code.
- Ozone and Segger's J-Link tools can also be used to connect to SmAcK. Please read the J-Link manuals how to import the files from
  the "tool_config/jlink" directory into the respective device database.
