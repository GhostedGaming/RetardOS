@echo off
REM Clean up any existing files
del /F /Q *.bin *.vdi *.iso *.o

REM Build bootloader and kernel
nasm -f bin boot.asm -o boot.bin
gcc -m32 -fno-pie -ffreestanding -c kernel.c -o kernel.o
gcc -m32 -fno-pie -ffreestanding -c hdmi.c -o hdmi.o
ld -m i386pe -Ttext 0x1000 --oformat binary kernel.o hdmi.o -o kernel.bin

REM Create ISO image
mkdir iso
copy /b boot.bin + kernel.bin iso\
mkisofs -o CustomOS.iso -b boot.bin -no-emul-boot -boot-load-size 4 iso

REM Remove existing VM if it exists
"C:\Program Files\Oracle\VirtualBox\VBoxManage.exe" unregistervm "CustomOS" --delete

REM Create and configure new VM
"C:\Program Files\Oracle\VirtualBox\VBoxManage.exe" createvm --name "CustomOS" --ostype "Other" --register
"C:\Program Files\Oracle\VirtualBox\VBoxManage.exe" modifyvm "CustomOS" --memory 128 --vram 16 --acpi on --boot1 dvd
"C:\Program Files\Oracle\VirtualBox\VBoxManage.exe" storagectl "CustomOS" --name "IDE Controller" --add ide
"C:\Program Files\Oracle\VirtualBox\VBoxManage.exe" storageattach "CustomOS" --storagectl "IDE Controller" --port 0 --device 0 --type dvddrive --medium CustomOS.iso

REM Start the VM
"C:\Program Files\Oracle\VirtualBox\VBoxManage.exe" startvm "CustomOS"
