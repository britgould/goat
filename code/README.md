**Prerequisites:**

Set up TensorFlow Lite by folling the instructions here:
* https://gist.github.com/khanhlvg/bbeb5e4ccfca6cbcf18508a44f5964be

*After that, remember to activate the virtual environment each time, associate the IDE with the virtual environment, or to run Python specifically from the virtual environment.*

For serial communication:
* pip install pyserial

For IKPY (inverse kinematics library) to install properly on the Raspberry Pi, we had to:
* sudo apt-get install gfortran libopenblas-dev cmake 
* pip install scipy==1.8.1 -v *(recommend -v to see progress because this will take a long time to run)*
* pip install matplotlib

Then install IKPY:
* pip install ikpy
* pip install 'ikpy[plot]'
* pip install ipywidgets

