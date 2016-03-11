#note for Windows users: If you copied this file from a Windows package, you need to 
#                        mark it as executable so that the Linux shell will run it.
#                        You can do this by running "chmod +x script_name"
#                        You will also want to do this for the scripts in the tools
#                        directory
export PICTO_TREE=$PWD
export PICTO_THIRD_PARTY=$PICTO_TREE/3rdparty
export PICTO_MACHINE_TYPE=X86
#We don't export LD_LIBRARY_PATH as we set the RPATH for the executable instead as part of the build process
#export LD_LIBRARY_PATH=$PICTO_TREE/intermediates/lib/debug:$PICTO_TREE/intermediates/lib/release
export PICTO_ENV_SET=TRUE
echo "Picto environment set"
echo "Be sure you executed this script in the context of the current shell"
echo "  i.e. \". ./Configure_Linux_x86.sh\" so that the environment is"
echo "  propagated to the active terminal."
echo "Be sure the build environment for your compiler and Qt is also prepared"
echo "Use the GenerateBuildFiles.sh script in tools/linux.common to build"

