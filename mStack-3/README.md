## GETTING STARTED!!!

### Prerequisites

- ARM Cortex-Mx (0,3,4,7,...) MicroController. This tutorial uses STM32 MicroController Series.
- STM32Cube IDE (You can also use others like Keil-MDK, IAR,... but the following will focus on instructions with STM32Cube IDE)

### Installation

1. Clone the repository to your local machine:

```sh
git clone https://github.com/ThanhNguyen-Tien/ed-fsm-library.git
git checkout mStack-3
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
   - At Tab Include/GNU C click "Add.." > Workspace > Locate to `Components`: "/${ProjName}/Components"
   - At Tab Include/GNU C++ click "Add.." > Workspace > Locate to `Components`: "/${ProjName}/Components"

6. Now we successfully import library to project.

## DONATE
  - `102002286610` VIETTINBANK - NGUYEN TIEN THANH
  - `204341119001` HSBC - NGUYEN TIEN THANH

## CONTACT
  - Facebook: https://www.facebook.com/nguyentien.thanh.1042032/
  - Gmail: thanhnguyendktd@gmail.com
  - Whatapps: (+84) 965107688 