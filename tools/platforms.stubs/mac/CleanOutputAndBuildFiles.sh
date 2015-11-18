if [ "$PICTO_ENV_SET" != "TRUE" ]
then
  echo "Picto environment has not been configured"
  echo "Run the appropriate configure script for your platform from the Picto root directory and try again"
else
  pushd $PICTO_TREE
  echo "Cleaning Makefiles"
  rm -f `find . -name 'Makefile'`
  rm -f `find . -name 'Makefile.Release'`
  rm -f `find . -name 'Makefile.Debug'`
  echo "Cleaning XCode Project Files"
  rm -rf `find . -name '*.xcodeproj'`
  rm -rf `find . -name 'build'`
  rm -rf `find ./source/ -name '*.app'`
  rm -rf `find ./source/ -name 'Info.plist'`
  echo "Cleaning Translation Object Files"
  rm -f `find . -name '*.qm'`
  echo "Cleaning Intermediate Files"
  rm -rf intermediates
  echo "Cleaning Output"
  rm -rf output
  popd
fi
