Joystick CAN commander is an example application designed to show how to communicate with Imagry Car Controller.

Using an SDL2 supported game controller, inputs are normalized and converted to relative torque, throttle, and brake commands. This application also demonstrates registering callback functions to receive and parse controller reports thru CAN network.

# Pre-requisites:
- Imagry Car Controller: [Imagry Store](https://imagry.co/product/car_controller)
- An SDL2 supported game controller is also required, and the SDL2 library must be pre-installed
- A CAN interface adapter, such as the [Kvaser Leaf Light](https://www.kvaser.com), is also necessary in order to connect the API to the controller CAN network via USB

This application has been tested with a Logitech F310 gamepad and a wired Xbox 360 controller, but should work with any SDL2 supported game controller. Controllers with rumble capabilities will provide feedback when car control is enabled or disabled.

[Xbox 360 Wired Controller](https://www.amazon.com/dp/B004QRKWLA)

[Logitech Gamepad F310](http://a.co/3GoUlkN)

Install the SDL2 library with the command below.

```
sudo apt install libsdl2-dev
```

Install Kvaser Canlib:
```
sudo apt-add-repository -y ppa:jwhitleyastuff/kvaser-linux
sudo apt-get update
sudo apt-get install -y kvaser-canlib-dev kvaser-drivers-dkms
```

# Joystick Commander

clone and enter the joystick commander repo:

```
git clone https://github.com/imagry/joystick-can-commander.git
cd joystick-can-commander
```

# Building Joystick Commander

```
mkdir build
cd build
cmake ..
make
```

# Running Joystick Commander

```
./joystick
```
