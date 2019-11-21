set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-as)
set(CMAKE_RANLIB arm-none-eabi-ranlib)

set(CMAKE_EXE_LINKER_FLAGS 
    "-mthumb -mcpu=cortex-m3 -mfloat-abi=soft -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs" 
    CACHE INTERNAL "")

set(CMAKE_C_FLAGS 
    "-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums"
    CACHE INTERNAL ""
)

set(CMAKE_CXX_FLAGS 
    "-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums"
    CACHE INTERNAL ""
)
