# NVmain Project

中央大學二年級計算機組織 Project2

## HW codes

- [BaseCPU.py](/BaseCPU.py)
- [CacheConfig.py](/CacheConfig.py)
- [Caches.py](/Caches.py)
- [Options.py](/Options.py)
- [XBar.py](/XBar.py)

## HW stats folders

by Questions in [PPT](/Documents/期末project_補充教學與demo流程.pptx)

> Below commands are in gem5/

### Compile

```.sh
scons EXTRAS=../NVmain build/X86/gem5.opt -j8
```

### Hello world

```.sh
 ./build/X86/gem5.opt configs/example/se.py -c tests/test-progs/hello/bin/x86/linux/hello --cpu-type=TimingSimpleCPU --caches --l2cache --l3cache --mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```

### Quicksort

> Config last level cache to 2-way and full-way associative cache and test performance

#### Command

```.sh
./build/X86/gem5.opt configs/example/se.py -c ../quicksort.out --cpu-type=TimingSimpleCPU --caches --l2cache --l3cache --mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```

#### Results

- [2-way Quicksort](/2-way_and_Full-way/2-way_Quicksort)
- [Full-way Quicksort](/2-way_and_Full-way/Full-way_Quicksort)

### RRIP

> Modify last level cache policy based on RRIP

DEMO

### Implement write through

> Test the performance of write back and write through policy based on 4-way associative cache with isscc_pcm

None

### Design last level cache policy

> Bonus: Design last level cache policy to reduce the energy consumption of pcm_based main memory Baseline:LRU

None

## Environments

Ubuntu 18.04 with win-docker

### Setups

see [REPO](https://github.com/cyjseagull/gem5-nvmain-hybrid-simulator)
or use [PPT](Documents/Nvmain_project_說明與教學.pptx)
