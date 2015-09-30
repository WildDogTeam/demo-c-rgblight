#Demo
##1.用例说明
该用例展示如何在wiced平台上编译，运行智能彩灯.
##1.文件结构和说明
	
		.
		├── akiss.c
		├── akiss.h
		├── changhong.c
		├── changhong.h
		├── common.h
		├── cooee.c
		├── cooee.h
		├── demo_api.h
		├── demo_bindServer.c
		├── demo_devices.c
		├── demo_factoryRest.c
		├── demo_flash.c
		├── demo_main.c
		├── demo_platform.h
		├── demo_platform_wiced.c
		├── demo_subscribe.c
		├── demp_api.h
		├── easy_setup.c
		├── easy_setup.h
		├── easy_setup_wiced.c
		├── neeze.c
		├── neeze.h
		├── README.md
		├── smartConfig.c
		└── wilddog_demo_config.h


    
*	`akiss.h,akiss.c,changhong.h,changhong.c，cooee.c，cooee.h，easy_setup.c，easy_setup.h ,smartConfig.c,neeze.c，neeze.h` ： smart config配置算法实现 .
*	`demo_api.h` : 用例接口 .
*	`demo_bindServer.c`： 绑定部分接口.
*	`demo_devices.c`： 智能彩灯控制部分接口.
*	`demo_factoryRest.c`： 恢复出厂设置相关部分接口.
*	`demo_flash.c`： Flash读写部分接口.
*	`demo_platform.h，demo_platform_wiced.c`： wiced平台相关部分接口.
*	`demo_subscribe.`： 订阅云端智能彩灯部分接口.
*	`demo_main.c` : wiced工程的main.
*	`wilddog_demo_config.h`: 用户配置文件.

##2.配置说明
### 2.1 网络配置
用例需要连接路由并在云端URL建立智能彩灯节点并订阅。用户可以修改`wilddog_demo_config.h`配置：

- `YOUR_APPID` ： 用例使用的APPID.
- `YOUR_DEVICE_NAME` : 设备序号.

###2.2 wiced板硬件配置
1. 恢复出厂的`GPIO`口在`demo_platform.h`声明为`WICED_GPIO_2`，用户可以根据自己的需要修改，注意恢复出厂的`GPIO`必须能响应外部中断.
2. 智能彩灯的GPIO口在`demo_devices.c`中定义，默认如下,用户可以根据自己的需要直接修改，注意控制智能彩灯的GPIO必须带硬件`PWM`

		#define LED_RED_PIN    WICED_PWM_3	/* red */ 
		#define LED_GREEN_PIN  WICED_PWM_8 	/* green */
		#define LED_BLUE_PIN   WICED_PWM_7 	/* blue */



##3.烧录
###3.1 烧录wiced

1. 把`demo-c-rgblight/src/device/software`拷贝到`WICED-SDK-x.x.x\WICED-SDK\apps`目录下
2. 配置`software/examples/wiced/wilddog_demo_config.h`。
3. 在WICED-IDE上建立Target-`software.project.wiced-<yourplatform> download run`（我们这是使用的是WSDB750，所以Target为 `software.project.wiced-WSDB750 JTAG=ftdi_swd download_apps download  run`）。


	**注意**：在`WSDB750`烧录前需要安装`WICED-SDK\tools\drivers\BCM9WCD1EVAL1_Drivers_patch_SWD\InstallDriver.exe`。

4. 双击Target，编译并下载运行，同时打开串口工具，波特率设置为115200-8-n-1，查看运行的log输出。

##4. 简单使用说明

1. 打开iOS的APP并注册登陆，如果是第一次使用，会直接进入`寻找设备`界面；
2. 如果设备已连接WIFI，输入WIFI的密码，并点击`寻找设备`；
3. 给设备上电（或者恢复出厂），设备进入等待配置状态，红灯快闪；
4. 设备与APP连接上后，APP端收到寻找成功提示，点击`绑定`，将账号和设备绑定；
5. 绑定成功后，自动进入设备列表，点击所绑定的设备名，进入设备操作界面，即可以对设备进行调色和开关操作。

##5. 其他
###5.1 恢复出厂设置
恢复出厂的`GPIO`持续接地超过`5s`，wiced清除连接的`ssid`相关信息和清除绑定信息，并重启等待配置。烧录完成后，开发板第一次启动，默认进入等待配置状态。

###5.2配置智能彩灯指示状态信息

		红灯快闪————————正在连接路由
		蓝灯快闪————————绑定中
		蓝灯慢闪————————绑定超时

