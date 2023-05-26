# 01. 基本使用

## 01.1 多个框单选

```q
import QtQuick
import QtQuick.Controls

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    ButtonGroup {
        id: childGroup
        exclusive: true
        buttons: col.children
    }

    Column {
        id: col
        CheckBox {
            checked: true
            text: qStr("First")
        }
        CheckBox {
            checked: true
            text: qStr("Second")
        }
        CheckBox {
            checked: true
            text: qStr("Third")
        }
    }
}

```

- 实现多个 CheckBox 但只能同时选择一个的方法