## Getting Started

### Prerequisites

- ARM Cortex-M microcontroller
- STM32Cube IDE (You can also use others like Keil-MDK, IAR,... but the following will focus on instructions with STM32Cube IDE)

### Installation

1. Clone the repository to your local machine:

```
git clone https://github.com/your-username/event-driven-library.git
```

2. Create your project with STM32Cube IDE, in STM32 Cube MX (.ioc file)
   -  Config RCC and SYS to suit with your application
   -  Config Clock Configuration to suit with your hardware
   -  If you want to use Console module, let enable USART 2 (default in library) and enable USART 2 Interrupt (optional)
   -  At System Core/NVIC, go to Code Generation tab and disable "Generate IRQ Handler" for Systick timer and USART2 (if enabled)
   -  Go to Project Manager Tab
     
       > Project: enable "Do not generate the main"
       > 
       > Code Generator: enable "Generate peripheral omotoa;ozatopm as a pair ...."
       > 
       > Advanced Settings: Choose LL Library for USART2 (if enabled)
       
    -  Generate Project

3. Copy the Components folder from repo to your project
4. Include this folder to Build progress
   
   - Right click to the Components folder > Resource Configuration > Exclude from Build => unSelect all
     
5. Add path to the linker
   
   - Right click to the Project Name > Properties > C/C++ General > Paths and Symbols
   - At Tab Include/GNU C click "Add.." > Directory: "${ProjDirPath}/Components"
   - At Tab Library Paths click "Add.." > Directory: "${ProjDirPath}/Components/core"
     
6. Add Object file to the Linker

   - Right click to the Project Name > Properties > C/C++ Build > Tool Settings > MCU GCC Linker > Miscellaneous
   - At "Additional objects files" bar, click add
     
     ![image](https://github.com/user-attachments/assets/cb8195a6-10f3-4834-8023-6b663d8381af)
     
   - **Locate to the .a file in Components/core/**
   - Click Apply and Close

7. Now we successfully import library to project.
8. In the Compoments folder, there is a test folder. This is project base for testing library, you can build and run it to verify if the library work fine.
9. Connect your board to PC and Open Console App on Desktop
10. Open the correspond COM and check the result
11. You can see specific operations here [TODO: Youtube Link]

### NOTE
 **In this repo the library files (.a files) are not included. So you cannot do step 6**
 
 **Let contact me to get it!!!**

   - **ed-core-m0.a  - 12$**
   - **ed-core-m3.a  - 12$**
   - **ed-core-m4-fpuHard.a  - 12$**
   - **ed-core-m7-fpuHard.a  - 12$**

### CONTACT
  - Facebook: https://www.facebook.com/nguyentien.thanh.1042032/
  - Gmail: thanhnguyendktd@gmail.com
  - Whatapps: (+84) 965107688 
