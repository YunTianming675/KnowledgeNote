# 01. 基本使用

## 01.1 选中状态

- Button 具备选中状态，开启后点击会保持选中的状态

```q
import QtQuick
import QtQuick.Controls

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("MY QML")

    Button {
        id: btn1
        width: 90
        height: 50
        checkable: true
    }
}

```

- 注意，当 `checked` 为 true 时，`checkable` 会被强制置为 true

```q
import QtQuick
import QtQuick.Controls

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("MY QML")

    Button {
        id: btn1
        width: 90
        height: 50
        checkable: false

        onCheckableChanged: {
            console.log("onCheckableChanged", checkable)
        }
        onClicked: {
            btn1.checked = !btn1.checked
        }
    }
}

```

> 注：QT6.2 后 `checked` 不再影响 `checkable` 

## 01.2 连按

```q
...
    Button {
        id: btn1
        width: 90
        height: 50
        checkable: false
        /* 连按 */
        autoRepeat: true
        /* 第一次点击后过多久触发下一次信号 */
        autoRepeatDelay: 3000
        /* 第二次开始过多久触发下一次信号 */
        autoRepeatInterval: 1000

        onCheckableChanged: {
            console.log("onCheckableChanged", checkable)
        }
        onClicked: {
            console.log("onClicked")
        }
        onPressed: {
            console.log("onPressed")
        }
        onReleased: {
            console.log("onReleased")
        }
    }
...
```

- 在点击鼠标后过 3 秒触发一次信号，之后每过一秒触发一次信号

## 01.3 修改颜色

Button 没有 `color` 这个属性，一般通过 `background` 属性去设置

```q
...
        background: Rectangle {
            anchors.fill: parent
            color: {
                if (btn1.pressed) {
                    return "green"
                }
                else {
                    return "blue"
                }
            }
        }
...
```

- 在没有按下时显示为蓝色，按下后显示为绿色