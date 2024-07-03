# Terminal Raycasting

![torus in terminal](https://i.imgur.com/dGqf6j0.png)

## Description

Raycasted torus in terminal using C.

## Getting Started

### Dependencies

* C compiler
* Makefile

### Executing program

For Windows 10 command line:
* execute ```make``` in the project directory, program will be compiled to the ```build``` directory
* execute ```build\raycasting```
* if doesn't work, execute ```cc src/main.c && a.exe```

For other platforms:
* fuck around to find out :)

## Help

* Your terminal size need to be exactly 120*30 characters. You can change the size at lines 5 and 6 of the ```main.c``` file
* Executable is generated after compilation in the ```build``` directory as a ```raycasting``` file, or as a ```a``` file in the project directory

## License

This project is licensed under the MIT License - see the LICENSE file for details

## Acknowledgments

```iTorus``` function and formula for ```nTorus``` function:
* https://www.shadertoy.com/view/4sBGDy