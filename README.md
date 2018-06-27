# DISAPPEAR

This program is an experiment that plays back live video from a camera but removes all people from the scene.\

It does this by:

- HAAR face detection
- get blob 
- perform background subtraction on blob

It looks like this:\

<p><a href="https://vimeo.com/277366091">Disappear</a></p>

# PREREQUISITES

- OpenCV. See opencv.org for details.
- C++ compiler and/or IDE

# BUILDING

Make sure the executable ends up one directory above the data folder.
ie.:
`./bin/disappear # executable`
`./bin/data/* # data files`

# RUNNING

The present working directory when running the executable should be the directory that the executable resides in (ie. the one with a data folder in it)

When it runs it first needs to gather background information. This will take around 10 seconds. Please make sure there are no people, or moving elements in the scene at this time. 

After this, it will make any people's faces it recognises disappear from the scene.

# ISSUES

Currently, because of how the background substitution works, any moving elements will slowly be subsumed into the background.

The file loading is very basic and uses relative paths. These are relative to the directory the executable is run from and so that is why you must run it from a particular directory.

