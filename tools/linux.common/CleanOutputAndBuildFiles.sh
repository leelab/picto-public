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
  rm -f Makefile.source*
  echo "Cleaning Translation Object Files"
  rm -f `find . -name '*.qm'`
  echo "Cleaning Intermediate Files"
  rm -rf intermediates
  echo "Cleaning Output"
  rm -rf output
  popd
fi
