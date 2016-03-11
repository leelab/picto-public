if [ "$PICTO_ENV_SET" != "TRUE" ]
then
  echo "Picto environment has not been configured"
  echo "Run the appropriate configure script for your platform from the Picto root directory and try again"
else
  echo "Generating Translation Object Files"
  pushd $PICTO_TREE
  lrelease `find . -name '*.ts'`
  echo "Generating Makefiles"
  qmake -recursive -spec macx-g++
  echo "Generating XCode Project Files"
  qmake -recursive -spec macx-xcode
  popd
  echo "If using GNU make and you want to do a parallel build, be sure to specify the -j N parameter (where N is the number of simultaneous operations) to make"
  echo "After building, run the CreateDeployableAppBundles.sh script to prepare the program for distribution"
fi
