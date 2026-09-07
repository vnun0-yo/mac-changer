# mac-changer
Note: This tool only works on Linux systems. It changes the device's MAC address. The language is developed in C.
this tool by - Yen's
##Instalation

```bash
git clone https://github.com/vnun0-yo/mac-changer.git
```

```bash
sudo apt update
```
```bash
sudo apt install gcc -y
```
```bash
cd mac-changer
```
```bash
sudo gcc YenMacChanger.c -o YenMacChanger
```
## example
```bash
./YenMacChanger -i eth0 -m 00:11:22:33:44:55
```
## if you want run the tool From anywhere in the system use this commands

```bash
cd mac-changer
```
```bash
sudo gcc YenMacChanger.c -o YenMacChanger
```
```bash
sudo cp YenMacChanger /usr/bin/YenMacChanger
```
```bash
sudo chmod +x /usr/bin/YenMacChanger
```
## example 
```bash
YenMacChanger -i eth0 -m 00:11:22:33:44:55
```

# the tool by - Yen's
