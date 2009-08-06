if [ "$PICTO_ENV_SET" != "TRUE" ]
then
  echo "Picto environment has not been configured"
  echo "Run the appropriate configure script for your platform from the Picto root directory and try again"
else
  echo "Adding Qt frameworks to app bundles and modifying Mach-O binaries to reference the frameworks from within the app bundles"
  find $PICTO_TREE -name '*.app' -exec $PICTO_TREE/tools/mac/deployment/deployqt/deployqt {} \; 
fi
