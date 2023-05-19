# 01. Z 轴属性

- Z 轴控制控件所在的图层，在上方的图层会覆盖下方的图层，如下方的代码示例

```q
import QtQuick

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Item And Rectangle")

    Rectangle {
        x: 100
        y: 100
        /* 这个z表示该控件所在图层的位置，默认为0。默认时下方控件会覆盖上方控件 */
        /* Tips: 调整z顺序可以调整控件的显示顺序 */
        // z: 1
        width: 100
        height: 50
        color: "black"
    }

    Rectangle {
        x: 120
        y: 120
        width: 100
        height: 50
        color: "blue"
    }
}
```

- 运行结果

![](.\images\02\Snipaste_2023-05-14_15-30-40.png)

- 将 z 轴注释打开

![](.\images\02\Snipaste_2023-05-14_15-36-56.png)



# 02. 锚点 anchors

- anchors 可以用来对控件进行相对定位
- anchors 属性来自 Item
- 示例代码

```q
import QtQuick

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Item And Rectangle")

    Rectangle {
        id: rect1
        width: 100
        height: 50
        color: "black"
    }

    Rectangle {
        id: rect2
        width: 100
        height: 50
        /* 与rect1的右边对齐，且距离20个像素点 */
        anchors.left: rect1.right
        anchors.leftMargin: 20
        color: "blue"
    }

    Rectangle {
        id: rect3
        width: 100
        height: 50
        /* 与rect1的底部对齐，且距离20个像素点 */
        anchors.top: rect1.bottom
        anchors.topMargin: 20
        color: "black"
    }
}

```

- 运行结果

![](.\images\02\Snipaste_2023-05-14_15-44-59.png)

## 02.1 将控件居中

- anchors 可以将控件居中，示例：

```q
import QtQuick

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Item And Rectangle")

    Rectangle {
        id: rect1
        width: 100
        height: 50
        color: "black"
        anchors.centerIn: parent
    }
}

```

- 运行结果

![](.\images\02\Snipaste_2023-05-14_16-00-08.png)

- 将控件水平居中

```q
import QtQuick

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Item And Rectangle")

    Rectangle {
        id: rect1
        width: 100
        height: 50
        color: "black"
        anchors.horizontalCenter: parent.horizontalCenter
    }
}

```

- 运行结果

![](.\images\02\Snipaste_2023-05-14_16-03-19.png)

# 03. Rectangle 具有的属性

## 03.1 边框和圆角

- Rectangle 可以设置边框和圆角，如代码所示：

```q
import QtQuick

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Item And Rectangle")
    color: "grey"

    Rectangle {
        id: rect1
        x: 100
        y: 30
        width: 100
        height: 50
        color: "black"
        /* 设置边框的大小和颜色 */
        border.width: 2
        border.color: "red"
        /* 设置圆角 */
        radius: 10
    }
}

```

- 运行结果

![](.\images\02\Snipaste_2023-05-14_16-10-17.png)

## 03.2 自定义 MyRectangle 实现任意边框

- 自带的 border 属性不能仅给某一边添加边框，通过自定义一个 MyRectangle 实现，示例：
- MyRectangle.qml

```q
import QtQuick

Rectangle {
    id: borderRect
    property int myTopMargin: 0
    property int myBottomMargin: 0
    property int myLeftMargin: 0
    property int myRightMargin: 0
    color: "black"
    Rectangle {
        id: innerRect
        color: "blue"
        anchors.fill: parent
        anchors.topMargin: myTopMargin
        anchors.bottomMargin: myBottomMargin
        anchors.leftMargin: myLeftMargin
        anchors.rightMargin: myRightMargin
    }
}

```

- main.qml

```q
import QtQuick

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Item And Rectangle")
    color: "grey"

    MyRectangle {
        x: 100
        y: 30
        width: 100
        height: 80
        myTopMargin: 5
        myBottomMargin: 5
    }
}

```

- 运行结果

![](.\images\02\Snipaste_2023-05-14_16-28-26.png)

# 04. 状态机 States

- 控件可以设置状态，在不同的状态下发生不同的变化，示例：

```q
import QtQuick

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Rectangle {
        id: rect1
        x: 100
        y: 30
        width: 150
        height: 100
        state: "normal"

        states: [
            State {
                name: "normal"
                PropertyChanges {
                    // @disable-check M16
                    rect1.color: "black"
                }
            },
            State {
                name: "red_color"
                PropertyChanges {
                    // @disable-check M16
                    rect1.color: "red"
                }
            },
            State {
                name: "blue_color"
                PropertyChanges {
                    // @disable-check M16
                    rect1.color: "blue"
                }
            }
        ]

        MouseArea {
            anchors.fill: parent
            onPressed: {
                rect1.state = "red_color"
            }
            onReleased: {
                rect1.state = "blue_color"
            }
        }
    }
}

```

- 运行结果

    - 首先进入 normal 模式，控件为黑色

    ![](.\images\02\Snipaste_2023-05-14_19-10-55.png)

    - 在鼠标点击后，变为红色

        ![](.\images\02\Snipaste_2023-05-14_19-11-04.png)

    - 鼠标松开后，变为蓝色

        ![](.\images\02\Snipaste_2023-05-14_19-11-17.png)

# 05. 动画效果

- QT Assistant 的 Item 类 transitions