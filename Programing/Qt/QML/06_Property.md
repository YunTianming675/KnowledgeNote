# 01. 基本使用

## 01.1 声明控件的属性

```q
...
    property int myTopMargin: 0
    property int myBottomMargin: 0
    property int myLeftMargin: 0
    property int myRightMargin: 0
...
```

- property 不仅可以用于普通的数据类型，还能用于组件等 QMLType

```q
...
property Component myComponent
...
```

- 在外部调用该控件时再传递 Component 的信息，实现动态加载未定组件的功能

## 01.2 var 类型

未指定类型的类型，当不确定 property 后跟的类型时可以用声明为 `var` 

```
property var myVar: [0, 1, "123"]
```

## 01.3 访问控制

当希望外部可以读取而不能修改某个属性时，可以使用 `readonly` 修饰

```
readonly property int myLeftMargin: 10
```

- 这样外部在调用该控件时就只能读取而不能访问

当希望外部必须对某个属性赋值才能调用时，可以使用 `required` 修饰

```q
required property Component myComponent
```

当希望外部可以直接控制某个属性，可以使用**别名** `alias` 

```
property alias newInnerRectColor: innerRect.color
```

- 此时外部可以通过 `newInnerRectColor` 来直接控制 `innerRect.color` 属性