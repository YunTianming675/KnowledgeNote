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

## 02.1 运作机制

​	创建消息队列时 FreeRTOS 会先给消息队列分配一块内存空间，这块内存得大小等于队列控制块大小加上单个消息空间大小与消息队列长度的乘积，接着再初始化消息队列。此时消息队列为空。同时每个消息队列和消息空间在同一段连续的内存空间中，在创建成功的时候，这些内存就被占用，只有删除了消息队列的时候，这段内存才会被释放。创建成功时就已经分配好了每个消息空间与消息队列的容量，无法更改，每个消息空间可以存放不大于消息大小的任意类型的数据。

​	任务或者中断程序都可以给消息队列发送消息，当发送消息时，如果队列未满或者允许覆盖入队，FreeRTOS 会将消息**拷贝**到消息队列队尾，否则，会根据用户指定的阻塞超时时间进行阻塞，在这段时间中，如果队列一直不允许入队，该任务将保持阻塞状态以等待队列允许入队。在超时时间内任务将消息发送成功后，该任务会由阻塞态转为就绪态，或者在超时时间到消息还未能发送出去，任务也会转入就绪态，但此时将收到错误码 `errQUEUE_FULL`。

- 紧急消息：机制同普通的消息，只是该消息将发送到队列头而非队列尾。
- 在中断中发送消息不允许带有阻塞机制，需要调用专门在中断中发送消息的 API

## 02.2 常用函数

- 创建消息队列

    `xQueueCreate()`: 创建一个新的队列并返回可用于访问这个队列的句柄。

    - 参数：
        - `UBaseType_t uxQueueLength`: 队列长度
        - `UBaseType_t uxItemSize`: 每个数据项的大小，以字节为单位

    - 想要使用该函数，宏 `configSUPPORT_DYNAMIC_ALLOCATION` 必须置 1

    `xQueueCreateStatic()`: 用静态内存创建一个队列并返回可用于访问这个队列的句柄

    - 参数：
        - `UBaseType_t uxQueueLength`: 队列可同时容纳的最大项目数
        - `UBaseType_t uxItemSize`: 每个数据项的大小，以字节为单位
        - `uint8_t *pucQueueStorageBuffer`: 如果 `uxItemSize` 非 0，其大小必须大于 `uxQueueLength * uxItemSize`
        - `StaticQueue_t *pxQueueBuffer`: 队列控制块

- 删除消息队列

    `vQueueDelete()`

- 发送消息

    `xQueueSend()`

    `xQueueSendToBack()`

    - 等同于 `xQueueSend()` 

    `xQueueSendFromISR()`

    - 发送消息的中断保护版本

    `xQueueSendToBackFromISR()`

    - 同 `xQueueSendFromISR()`

- 接收队列

完整的 API 文档 [FreeRTOS - FreeRTOS queue API functions, including source code functions to create queues, send messages on queues, receive messages on queues, peek queues, use queues in interrupts.](https://www.freertos.org/zh-cn-cmn-s/a00018.html)

# 03. 信号量

​	信号量是一种实现任务间通信的机制，可以实现任务之间同步或临界资源的互斥访问，常用于协助一组相互竞争的任务来访问临界资源。信号量是一个非负整数，所有获取它的任务都会将该整数减一，当该整数为 0 时，所有试图获取它的任务都将处于阻塞状态。通常一个信号量的计数值用于对应有效的资源数，表示剩下的可被占用的互斥资源数。

## 03.1 二值信号量

​	二值信号量和互斥信号量非常相似，但仍有细微差别：互斥量有优先级继承机制，二值信号量没有该机制。这使得二值信号量更偏向应用于同步功能，而互斥量更偏向于临界资源的访问。

​	用作同步时，信号量在被创建后应该置空，任务 1 获取信号量进入阻塞状态，任务 2 在某种条件触发后释放信号量，于是任务 1 获得信号量进入就绪态，如果任务 1 的优先级是最高的，那么就会立即切换任务，从而达到两个任务间的同步。

## 03.2 计数信号量

​	在实际应用中，常将计数信号量用于事件计数与资源管理。每当某个事件发生时，任务或者中断将释放一个信号量，信号量计数值加一，当事件被处理时，处理任务会取走信号量，信号量计数值减一，信号量的计数值则表示还有多少事件没有被处理。此外，系统还有很多资源，也可以用计数信号量进行资源管理，信号量的计数值表示系统中可用的资源数目，任务必须先获取到信号量才能获取到资源访问权，当信号量为 0 时表示系统没有可用的资源。

## 03.3 互斥信号量

​	互斥信号量实际是特殊的二值信号量，特殊在于其有优先级继承机制，从而使它更适用于简单互锁，保护临界资源。任务在使用临界资源时，必须先获取互斥信号量，使其变空，这样其它任务需要使用临界资源时就会因无法获取信号量而进入阻塞。

> 临界资源：任何时刻只能被一个任务访问的资源

## 03.4 递归信号量

​	可重复获取调用的信号量。已经获取递归信号量的任务可以重复获取递归互斥量，该任务拥有递归信号量的所有权。任务成功获取几次递归互斥量就要返还几次，在此之前递归互斥量都处于无效状态，其他任务无法获取。

# 04. 互斥量





# 05. 事件





# 06. 软件定时器





# 07. 任务通知

FreeRTOS 提供一下几种方式发送通知给任务：

- 发送通知给任务，如果有通知未读，不覆盖通知值
- 发送通知给任务，直接覆盖通知值
- 发送通知给任务，设置通知值得一个或者多个位，可以当作事件组来使用
- 发送通知给任务，递增通知值，可以当作计数信号量使用

消息通知虽然**处理更快、RAM 开销更小**，但有以下限制：

- 只能有一个任务接收通知消息，因为必须指定接收通知得任务
- 只有等待通知的任务可以被阻塞，发送通知的任务，在任何情况下都不会因为发送失败而进入阻塞状态

## 07.1 任务通知机制

任务通知属于任务中附带的资源，所以在任务被创建的时候，任务通知也被初始化。任务通知的数据结构包含在任务控制块中，只要任务存在，任务通知数据结构就已经创建完毕，可以直接使用。

FreeRTOS 的每个任务都有一个 32 位的通知值，任务控制块中的成员变量 `ulNotifiedValue` 就是这个通知值。只有在任务中可以等待通知，而**不允许在中断中等待通知**。

## 07.2 任务通知接口函数

### 发送任务通知

xTaskNotifyGive

向一个任务发送通知，并将对方的任务通知值加 1。该函数可以作为二值信号量和计数信号量的一种轻量级实现，速度更快，在这种情况下对象任务在等待任务通知的时候应该使用函数 `ulTaskNotifyTake` 而非 `xTaskNotifyWait`。



# 08. 内存管理





# 09. 中断管理





# 10. CPU 使用率统计

