# 01. Component

- Component 不会在创建时自动加载，一般用于配合 Loader 进行动态加载
- 主要使用的槽函数：
    - `completed()`: Component 创建完成时
    - `destruction()`: Component 销毁时



# 02. Loader

## 02.1 使用 Loader 加载一个 Component

- 示例：

```q
import QtQuick

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Rectangle {
        id: rect1
        width: 100
        height: 60
        color: "blue"
    }

    // @disable-check M300
    Component {
        id: com
        Rectangle {
            width: 100
            height: 60
            color: "red"
            // @disable-check M16
            Component.onCompleted: {
                console.log("onCompleted", width, height, color)
            }
            // @disable-check M16
            Component.onDestruction: {
                console.log("onDestruction")
            }
        }
    }

    Loader {
        id: loader
        sourceComponent: com
        x: 120
    }
}

```

- 执行结果

![](.\images\03\Snipaste_2023-05-14_20-40-32.png)

- 这样在启动时会直接加载 Component，下面这个示例体现动态加载：

```q
import QtQuick
import QtQuick.Controls

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Rectangle {
        id: rect1
        width: 100
        height: 60
        color: "blue"
    }

    Button {
        id: btn1
        width: 50
        height: 50
        x: 220
        text: "Loader"
        onClicked: {
            loader.sourceComponent = com
        }
    }

    Button {
        id: btn2
        text: "Clear"
        width: 50
        height: 50
        anchors.left: btn1.right
        anchors.leftMargin: 10
        onClicked: {
            loader.sourceComponent = null
        }
    }

    Component {
        id: com
        Rectangle {
            width: 100
            height: 60
            color: "red"
            Component.onCompleted: {
                console.log("onCompleted", width, height, color)
            }
            Component.onDestruction: {
                console.log("onDestruction")
            }
        }
    }

    Loader {
        id: loader
        sourceComponent: null
        x: 120
    }
}

```

- 运行结果：

    - 刚启动时：

    ![](.\images\03\Snipaste_2023-05-14_21-04-28.png)

    - 点击 Loader 按钮后加载 Rectangle，并在命令行输出日志：

    ![](.\images\03\Snipaste_2023-05-14_21-05-19.png)

![](.\images\03\Snipaste_2023-05-14_21-05-41.png)

- 点击 Clear 按钮后删除 Rectangle，画面回到启动时的样子，并在命令行输出日志：

![](.\images\03\Snipaste_2023-05-14_21-08-47.png)

## 02.2 通过 Loader 修改 Component 的属性

- 想要修改通过 Loader 修改 Component 的属性，需要通过 Loader 的 item 属性，示例：

```q
...

    Button {
        id: btn3
        text: "Change"
        width: 50
        height: 50
        anchors.left: btn2.right
        anchors.leftMargin: 10
        onClicked: {
            loader.item.color = "black"
        }
    }

...

```

- 运行结果，在点击 Loader 加载后再点击 Chan... 即可修改被 Loader 加载的 Component 的颜色属性

![](.\images\03\Snipaste_2023-05-14_21-17-10.png)

## 02.3 Loader 的 asynchronous 属性

- asynchronous 默认为 false，告知 Loader 是否使用异步加载
- 加载中的 status 为 2，加载完成为 1。对于大型的资源来说，可以通过此判断资源是否加载完成
- 示例：

```q
...
   Loader {
        id: loader
        x: 120
        sourceComponent: null
        asynchronous: true
        onStatusChanged: {
            console.log("status: ", status)
        }
    }
...
```

- 执行结果，仅命令行：

![](.\images\03\Snipaste_2023-05-14_21-23-37.png)

## 02.4 加载图片

- 可以通过 Image 加载图片

```q
...
    Component {
        id: com
        Image {
            id: img1
            source: "/img/image.jpg"
            height: 100
            width: 100
        }
    }
...
```

- 运行结果，注意需要将 qml 文件与 img 文件一同加入 qrc 中

![](.\images\03\Snipaste_2023-05-15_16-53-31.png)

- 工程结构

![](.\images\03\Snipaste_2023-05-15_16-55-02.png)
