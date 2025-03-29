# nvidia-json

Parses the NVIDIA supported-gpus.json file and generates the header file for [nvidia-detect](https://github.com/elrepo/packages/tree/master/nvidia-detect)

## Requirements
```
sudo dnf install cmake gcc-c++ jsoncpp-devel
```

## Build
Compile `nvidia-json`
```
cmake .
make
```

## Usage
1. Download and extract the [NVIDIA *run* file](https://www.nvidia.com/en-us/drivers/unix/)
   Note: The examples below use the production branch version 570.xxx.xxx
```
sh NVIDIA-Linux-x86_64-570.xxx.xxx.run --extract-only
```

2. Copy the extracted supported-gpus.json file to the nvidia-json directory
```
cp NVIDIA-Linux-x86_64-570.xxx.xxx/supported-gpus/supported-gpus.json <path-to-nvidia-json>
```

3. Run nvidia-json and generate the new nvidia-detect.h
```
./nvidia-json supported-gpus.json > nvidia-detect.h
```

4. If any legacy branches are unsupported, a *FIXME* warning will be printed to stderr

5. Otherwise copy the nvidia-detect.h file to the nvidia-detect source folder and enjoy

