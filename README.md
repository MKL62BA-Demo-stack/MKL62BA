
## Purpose
Created this project for easier development on MKL62BA.   
The project base on  nrf5_SDK 17.0 


## lorawan SDK
This project base on LoRaMac-Node lorawan stack  and ver is 4.4.4
github url:https://github.com/Lora-net/LoRaMac-node     



## MAIN DIR
M_Prj   									: MDK project dir   
src												: main source code dir  
src/lorawan_sdk	: include LoRaMac-Node stack and example app  


## note
in MDK compile option ,need add this define    
define:REGION_AS923 REGION_AU915  REGION_CN470 REGION_CN779 REGION_EU433  REGION_EU868 REGION_KR920  REGION_IN865 REGION_US915 REGION_RU864   to enable radio