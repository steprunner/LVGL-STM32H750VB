# lvgl-stm32h750vb
后续会更新原理图以及PCB（现在这个板子设计了开槽但是不知道为何厂家没开，
应该是我设置问题，不过将将能用，打算等我有空完善PCB之后再传上来）
并未外挂flash，代码运行在片内flash上
采用LTDC+DMA2D驱动4.3英寸触摸屏（480x272，ips），LTDC像素时钟可以拉到30Mhz，触摸驱动为GT911
成功移植LVGL V8.2，相对流畅跑了music和widgets例程
本项目外设驱动代码是利用CUBEMX配置生成