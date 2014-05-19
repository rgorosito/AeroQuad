#!/bin/sh

#subir
sudo dfu-util --reset --device 0483:df11 --alt 0 --dfuse-address 0x08010000 --download /home/rgorosito/git/AeroQuad/BuildAQ32/objSTM32/AeroQuad32/AeroQuadMain.bin



