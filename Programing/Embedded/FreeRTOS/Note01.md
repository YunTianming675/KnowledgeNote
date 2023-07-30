# 01. 创建任务

## 01.1 使用静态内存创建

FreeRTOS 可以使用静态和动态内存来创建任务。静态指有关任务的信息和变量都已定义，其在内存中的位置由编译器决定。一个任务有四个要点：任务句柄、任务堆栈、任务控制块和任务函数主体。下面以创建一个 LED 闪烁任务为例梳理流程：

- 声明一个任务句柄

```c
/* LED任务句柄 */
static TaskHandle_t LEDTaskHandle;
```

- 声明任务堆栈

```c
/* LED任务堆栈 */
static StackType_t LEDTaskStack[128];
```

- 声明任务控制块

```c
/* LED任务控制块 */
static StaticTask_t LEDTaskTCB;
```

- 声明任务函数主体

```c
/* LED任务函数 */
static void LED_Task(void *parameter)
{
	while (1) {
		LED_HAL_Toggle(LED0);
		vTaskDelay(500);
	}
}
```

> `vTaskDelay()` 是 FreeRTOS 的非阻塞延时函数，使用该函数延时可以让 FreeRTOS 在延时时间里调度其它任务

- 创建任务

```c
/* 创建LED任务 */
LEDTaskHandle = xTaskCreateStatic((TaskFunction_t)LED_Task,		/* 任务函数 */
                                  (const char 	*)"LED_Task",	/* 任务名称 */
                                  (uint32_t		 )128,			/* 任务堆栈大小 */
                                  (void 		*)NULL,			/* 传递给任务函数的参数 */
                                  (UBaseType_t	 )4,			/* 任务优先级 */
                                  (StackType_t	*)LEDTaskStack, /* 任务堆栈 */
                                  (StaticTask_t *)&LEDTaskTCB); /* 任务控制块 */
```

> 在创建任务失败时 `xTaskCreateStatic()` 会返回空指针，可以通过返回值来判断任务是否创建成功

- 开始任务调度

```c
vTaskStartScheduler();
```



但在 FreeRTOS 中，一般不会一个任务一个任务的去创建然后逐一调度。一般会创建一个“创建任务”任务，该任务会创建所有任务，然后删除自身。同时，在 FreeRTOS 中如果开启了静态内存分配（`#define configSUPPORT_STATIC_ALLOCATION 1`）的支持，则需要自己实现空闲任务的获取任务堆栈和任务控制块方法，因为此时 FreeRTOS 将使用用户提供的静态内存来创建这两个任务。如果在打开了上面那个宏的前提下，再打开了支持软件定时器（`configUSE_TIMERS`）则还需要为软件定时器任务提供获取任务堆栈和任务控制块方法，这两个方法的一般实现如下：

- 定义任务堆栈和任务控制块

```c
/* 空闲任务堆栈 */
static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
/* 空闲任务控制块 */
static StaticTask_t IdleTaskTCB;

/* 定时器任务堆栈 */
static StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];
/* 定时器任务控制块 */
static StaticTask_t TimerTackTCB;
```

- 两个方法的实现，就是将上面创建的任务堆栈和任务控制块赋给函数形参

```c
/**
 * @brief	获取空闲任务的任务堆栈和任务控制块内存
 * @param[out]	任务控制块内存
 * @param[out]	任务堆栈内存
 * @param[out]	任务堆栈大小
 * @note	实现FreeRTOS接口
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &IdleTaskTCB;
	*ppxIdleTaskStackBuffer = IdleTaskStack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer = &TimerTackTCB;
	*ppxTimerTaskStackBuffer = TimerTaskStack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
```

关于“创建任务”任务，其一般的写法如下：

```c
/* 创建任务函数句柄 */
static TaskHandle_t AppTaskCreateHandle;
/* 创建任务函数堆栈 */
static StackType_t APPTaskCreateStack[128];
/* 创建任务任务控制块 */
static StaticTask_t AppTaskCreateTCB;
/* 创建任务函数 */
static void AppTaskCreate(void)
{
	taskENTER_CRITICAL();	/* 进入临界区 */
	/* 创建LED任务 */
	LEDTaskHandle = xTaskCreateStatic((TaskFunction_t)LED_Task,		/* 任务函数 */
									  (const char 	*)"LED_Task",	/* 任务名称 */
									  (uint32_t		 )128,			/* 任务堆栈大小 */
									  (void 		*)NULL,			/* 传递给任务函数的参数 */
									  (UBaseType_t	 )4,			/* 任务优先级 */
									  (StackType_t	*)LEDTaskStack, /* 任务堆栈 */
									  (StaticTask_t *)&LEDTaskTCB); /* 任务控制块 */
	if (LEDTaskHandle != NULL) {
		UART_BSP_Send(&UART1_Handler, "LED Task create success\r\n");
	}
	else {
		UART_BSP_Send(&UART1_Handler, "LED Task create fail\r\n");
	}
	vTaskDelete(AppTaskCreateHandle);	/* 删除创建任务任务 */
	taskEXIT_CRITICAL();				/* 退出临界区 */
}
```

- 开始任务必须位于临界区内，这样才能保证创建任务的代码能被执行完成。如果不使用临界区，则有三种情况：
    1. 创建的某个应用任务比开始任务优先级高。此时应用任务创建后会马上执行，而开始任务被阻塞，直到比开始任务优先级高的任务被阻塞，则系统回到开始任务被打断的地方继续往下去执行
    2. 创建的某个应用任务跟开始任务优先级一样。此时在应用任务创建后，则应用任务和开始任务根据时间片来执行，直到开始任务把自己删除
    3. 创建的应用任务优先级比开始任务低。被创建的应用任务不会马上执行，开始任务继续往下，继续创建任务，再参考 1 和 2 的情况，直到开始任务执行结束，把自己删除
- 任务如果没有启动成功，则通过 LR 寄存器指定的地址退出

## 01.2 使用动态内存创建

FreeRTOS 支持动态内存分配，在第一次使用的时候，系统会将定义的堆内存初始化，FreeRTOS 提供不同的动态内存分配算法，分别在 heap_1.c、heap_2.c、heap_3.c、heap_4.c 和 heap_5.c 中，不同的算法分配效率不同，使用时只需选择一个进行添加即可。

同时，想要使用动态内存分配，需要在 `FreeRTOSConfig.h` 中配置相应的宏

- `configSUPPORT_DYNAMIC_ALLOCATION`: 为 1 时，允许使用动态内存分配，如果未定义，则默认为 1
- `configTOTAL_HEAP_SIZE`: RAM 堆中的可用总量

在使用动态内存创建时，FreeRTOS 会在 `xTaskCreate()` 函数中判断堆内存是否被初始化并自动初始化堆，无需干预。使用动态内存创建任务的流程如下，以 LED 任务为例：

- 创建任务句柄

```c
/* LED任务句柄 */
static TaskHandle_t LEDTaskHandle = NULL;
```

- 任务函数主体

```c
/* LED任务函数 */
static void LED_Task(void *parameter)
{
	while (1) {
		LED_HAL_Toggle(LED1);
		vTaskDelay(500);
	}
}
```

- 由 `xTaskCreate()` 函数创建任务

```c
/* 创建 LED 任务 */
xReturn = xTaskCreate((TaskFunction_t	 )LED_Task,			/* 任务入口函数 */
                      (const char		*)"LED_Task",		/* 任务名字 */
                      (uint16_t			 )128,				/* 任务栈大小 */
                      (void				*)NULL,				/* 任务入口函数参数 */
                      (UBaseType_t		 )2,				/* 任务的优先级 */
                      (TaskHandle_t		*)&LEDTaskHandle);	/* 任务控制块指针 */
```

创建成功时返回 `pdPASS`，否则返回 `errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY` 

## 函数说明

FreeRTOS 中有关任务创建类函数的文档位于: [FreeRTOS - A FREE Open Source RTOS. The Free RTOS API functions for creating RTOS tasks and deleting RTOS tasks - xTaskCreate() and vTaskDelete.](https://www.freertos.org/zh-cn-cmn-s/a00019.html)

# 02. 消息队列



