# Test Description
## Requirements
- 本测试是使用C/C++/asm编写的裸机测试，用来测试例如knowledge下Xiangshan的CPU架构功能与性能
- 裸机测试不应依赖linux或其它操作系统的系统调用库例如printf、malloc、system call等。
- 如果要使用系统调用库，根据knowledge下riscv-isa-sim的仿真环境，在src/libs中实现标准的调用库源代码并调用。
- 编译环境使用knowledge下的nexus-am，编译目标为nexus-am/am/arch/riscv64-cb-v1.mk，在scripts中生成使用nexus-am对应测试的编译脚本
- 生成后的测试应使用knowledge/riscv-isa-sim/build/spike进行仿真测试，运行仿真测试的脚本同样生成在scripts中，以便于后续手动运行

## Function Description

hints: 
1. 应该严格定义测试规模，否则大模型生成的测试规模很小