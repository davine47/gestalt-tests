# Test Description
## Requirements
- 本测试是使用C/C++/asm编写的裸机测试，用来测试例如knowledge下Xiangshan的CPU架构功能与性能
- 裸机测试不应依赖linux或其它操作系统的系统调用库例如printf、malloc、system call等。
- 如果要使用系统调用库，根据knowledge下riscv-isa-sim的仿真环境，在src/libs中实现标准的调用库源代码并调用。
- 编译环境使用knowledge下的nexus-am，编译目标为nexus-am/am/arch/riscv64-cb-v1.mk，在scripts中生成使用nexus-am对应测试的编译脚本
- 生成后的测试应使用knowledge/riscv-isa-sim/build/spike进行仿真测试，运行仿真测试的脚本同样生成在scripts中，以便于后续手动运行

## Function Description
- 参考nexus-am/apps/hello，在src下创建hello文件夹，生成一个测试打印字符串的c测试
- 参考knowledge/Xiangshan的微架构，生成针对于此架构的复杂场景下的打印测试
- 使用nexus-am环境编译，并使用riscv-isa-sim进行仿真，生成脚本在scripts下以方便手动运行
- 在docs下生成测试文档，详细描述测试意图和测试实现