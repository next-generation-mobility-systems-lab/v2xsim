

## User Manual:  V2X Simulator for Connected and Automated Vehicle Environment Simulation

 Ethan Zhang, Neda Masoud
 
 Next Generation Mobility System Lab, University of Michigan, Ann Arbor, MI, USA

## System setup

<img src="https://github.com/shuruiz/ros/blob/master/veh/carfollowing/pics/ubuntu.jpg" height="48" width="48"> <img src="https://github.com/shuruiz/ros/blob/master/veh/carfollowing/pics/gazebo.jpg" height="48" width="48"> <img src="https://github.com/shuruiz/ros/blob/master/veh/carfollowing/pics/cmake.jpg" height="48" width="110"> <img src="https://github.com/shuruiz/ros/blob/master/veh/carfollowing/pics/ros.svg" height="48" width="110"> 


 ><font size="3">Ubuntu 18.04 (Bionic Beaver) <br>

 >Gazebo 9.8.0 
 
 >CMake 3.10.2
 
 >Robot Operating System (optional)</font>

## Installation 

The simulator runs on top of Gazebo engine. The proposed simulator is developed under Ubuntu OS, for future integration with Robot Operating System (ROS) when necessary, which always runs on top of linux kernal. The simulator adopt Ubuntu 18.04 (Bionic Beaver). Instructions to install and use Ubuntu 18.04 can be found at the [Ubuntu releases--18.04.4 LTS](http://releases.ubuntu.com/18.04/). 


Check out the [Gazebo](http://gazebosim.org/tutorials?tut=install_ubuntu&cat=install) to install Gazebo with version 9 and above. After installing, install and setup Gazebo developer library <em>libgazebo9-dev</em>. 

CMake is used to compile the C++ script and build pulgins. Visit [CMake installation](https://cmake.org/install/) to build, install and setup CMake for the Ubuntu System. 

(optional) Visit [ROS installation](http://wiki.ros.org/ROS/Installation) to install and setup your ROS if necessary.

Clone this repo into your working folder. 
     

## Run the simulator

### Run with default setting

Properly set the system environment. In the terminal, before calling Gazebo, make sure to set the path to your current Gazebo <code>build</code> directory:

<code>export GAZEBO_PLUGIN_PATH=${GAZEBO_PLUGIN_PATH}:~/your_working_directory/v2x/build</code>

In your working directory, compile the plugins, then call and run the V2X simulator from terminal:

<code>cd build</code>

<code>cmake ..</code>

<code>make</code>

<code>gazebo ../v2x.world --verbose </code>

Then the simulator window should prompt out and starts simulation. It uses Gazebo as backend simulation engine. Terminate the simulation with <code> control + c</code>. Once terminated, under the <code>build</code> directory you can find Basic Safety Messages (BSMs) collected by virtual road side unit (virtual RSU) in <code>.txt</code> format. 



### Customization

Users can add, delete, or update models provided in the <code>v2x.world</code> file. And control models by adding, deleting, or updating plugins or APIs correspondingly. 

#### Customization with Plugins

In the source file, plugin scripts (e.g., <code>veh_plugin.cc</code>) can be found at the working directory. Users can directly modify plugin code to customize the plugin given in this repo, so as to control vehicles in a different way. To test an algorithm, customizing plugins is more recommended, because algorithm details can be implemented under Gazebo namespace. 
When customizing the plugin, the <code>CMakeLists.txt</code> should be updated accordingly with following CMake rules. To make the plugin be executable, users should use <code>CMake</code> to compile and link the Gazebo plugin to the model built the Gazebo engine. If you are not using <code>CMake 3.10.2</code>, you should make sure your existing <code>CMake</code> support C++11 standard.  Once built by <code>CMake</code>, you can find the executable plugin under <code>build</code> directory. The compiled plugin file will end with <code>lib*.so</code>, for example, <code>libveh_plugin.so</code>. In order to link compiled plugins to models when the simulator is running, add them to your <code>.world</code> file and link it to the corresponding model in <code>XML</code> format. 

Plugins provided in this repo are developed to give a intelligent driving model demonstration. 

#### Customization with APIs

In the source file, customized APIs are provided. For example  <code>vel.cc</code> is an API that can directly send out command to vehicle models which are running in simulator. To execute the API, go to the <code>build</code> directory and call the API from terminal like calling normal executable file <code>./vel</code>
When customize API, the <code>CMakeLists.txt</code> should be updated accordingly by following CMake rules. 

## Demo
Please check out the Demo video for detail regarding the performance of the simulator. 
