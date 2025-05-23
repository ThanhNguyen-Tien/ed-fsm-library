## GETTING STARTED!!!

### Prerequisites

- ARM Cortex-Mx (0,3,4,7,...) MicroController. This tutorial uses STM32 MicroController Series.
- STM32Cube IDE (You can also use others like Keil-MDK, IAR,... but the following will focus on instructions with STM32Cube IDE)

### Installation

1. Clone the repository to your local machine:

```sh
git clone https://github.com/ThanhNguyen-Tien/ed-fsm-library.git
```

2. Create your project with STM32Cube IDE, in STM32 Cube MX (.ioc file)
   -  Config RCC and SYS to suit with your application
   -  Config Clock Configuration to suit with your hardware
   -  At System Core/NVIC, go to Code Generation tab and disable "Generate IRQ Handler" for Systick timer
   -  If you use console App (optional)
      - Enable UASRT and its global interrupt
      - At NVIC/Code Generation tab disable "Generate IRQ Handler" for USART module
      - At ProjectManager/Advanced Settings, choose LL for USART module
        
   -  In Project Manager Tab
     
       > Project: enable "Do not generate the main"
       > 
       > Code Generator: enable "Generate peripheral initialization as a pair ...."
       
    -  Generate Project

3. Copy the `Components` folder from repo to your project
4. Include this folder to Build progress
   
   - Right click to the `Components` folder > Resource Configuration > Exclude from Build => unSelect all
     
5. Add path for `Components`
   
   - Right click to the Project Name > Properties > C/C++ General > Paths and Symbols
   - At Tab Include/GNU C click "Add.." > Directory: "${ProjDirPath}/Components"
     
6. Add Object file to the Linker

   - Right click to the Project Name > Properties > C/C++ Build > Tool Settings > MCU GCC Linker > Miscellaneous
   - At "Additional objects files" bar, click add
     
     ![image](https://github.com/user-attachments/assets/cb8195a6-10f3-4834-8023-6b663d8381af)
     
   - **Locate to the .a file in Components/core/**
   - Click Apply and Close

7. Now we successfully import library to project.
8. In this repo, there is a test folder. This folder contains basic examples to experiment with the mechanisms that the framework provides. You can choose one of them to test by copying that folder into Components folder in your project.
9. You can see specific operations here: https://youtu.be/eHAfCoUB478

## NOTE
 **In this repo the library files (.a file) for almost cortex Mx are not included. So you cannot do step 6. There is only ed-core-m3-debug.a for cortex M3 is free, you can use it for free trial**
 
 **Let's contact me to get .a file!**

>**You cannot import Library (.a) built for Cortex M3 into a project whose target device is another Corte (such as M0, M1, M23, M33, M4, M7, ...). Compiler will notice errors!!!**

<p><em><strong><span style="font-size: 1.5em;">Refer to the <span style="text-decoration: underline;">guide-line</span> directory for documentation on how to use the framework’s macro functions.</span></strong></em></p>

## DONATE
  - `102002286610` VIETTINBANK - NGUYEN TIEN THANH
  - `204341119001` HSBC - NGUYEN TIEN THANH

## CONTACT
  - Facebook: https://www.facebook.com/nguyentien.thanh.1042032/
  - Gmail: thanhnguyendktd@gmail.com
  - Whatapps: (+84) 965107688 
