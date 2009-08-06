if [ "$PICTO_ENV_SET" != "TRUE" ]
then
  echo "Picto environment has not been configured"
  echo "Run the appropriate configure script for your platform from the Picto root directory and try again"
else
# Do not call lupdate on the root project file.
# While it will parse all the sub-project files, it will add translation strings found in
# earlier sub-projects into the later sub-projects.
# Instead, we parse all the sub-project files individually.
  echo "Updating Translation Tables"
  find $PICTO_TREE/source -name '*.pro' -exec lupdate {} \;
fi
