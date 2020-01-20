import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2

Rectangle {
    id: scene_setting_system_timeset
    width: 800
    height: 480
    objectName: "SceneSettingSystemTimeset"

    property int year
    property int month
    property int day
    property int hours
    property int minutes
    property int seconds

    property int month_max1: 3
    property int month_max2: 1
    property bool isDayMax: false

    property  bool repeatTimer: true

    function calcLeapYear()
    {
        if ( (image_month1.number == 0 && image_month2.number == 4) ||
             (image_month1.number == 0 && image_month2.number == 6) ||
             (image_month1.number == 0 && image_month2.number == 9) ||
             (image_month1.number == 1 && image_month2.number == 1) )
        {
            month_max1 = 3; month_max2 = 0
        }
        else if (image_month1.number == 0 && image_month2.number == 2)
        {
            year = image_year1.number.toString() + image_year2.number.toString() + image_year3.number.toString() + image_year4.number.toString()
            if (year % 4 != 0 || (year % 4 == 0 && year % 100 == 0 && year % 400 == 0)) {
                month_max1 = 2; month_max2 = 8
            } else {
                month_max1 = 2; month_max2 = 9
            }
        }
        else
        {
            month_max1 = 3; month_max2 = 1
        }
        if (image_day1.number.toString() + image_day2.number.toString() >= month_max1.toString() + month_max2.toString()) {
            isDayMax = true
        } else {
            isDayMax = false
        }
        if (isDayMax == true && image_day1.number.toString() + image_day2.number.toString() != month_max1.toString() + month_max2.toString()) {
            image_day1.number = month_max1; image_day2.number = month_max2
        }
    }

    function resetSelect()
    {
        repeatTimer = false
        image_select_year.visible = false; image_up_year.visible = false; image_down_year.visible = false
        image_select_month.visible = false; image_up_month.visible = false; image_down_month.visible = false
        image_select_day.visible = false; image_up_day.visible = false; image_down_day.visible = false
        image_select_hours.visible = false; image_up_hours.visible = false; image_down_hours.visible = false
        image_select_minutes.visible = false; image_up_minutes.visible = false; image_down_minutes.visible = false
        image_select_seconds.visible = false; image_up_seconds.visible = false; image_down_seconds.visible = false
    }
    function selectYear()
    {
        image_select_year.visible = true; image_up_year.visible = true; image_down_year.visible = true
    }
    function selectMonth()
    {
        image_select_month.visible = true; image_up_month.visible = true; image_down_month.visible = true
    }
    function selectDay()
    {
        image_select_day.visible = true; image_up_day.visible = true; image_down_day.visible = true
    }
    function selectHours()
    {
        image_select_hours.visible = true; image_up_hours.visible = true; image_down_hours.visible = true
    }
    function selectMinutes()
    {
        image_select_minutes.visible = true; image_up_minutes.visible = true; image_down_minutes.visible = true
    }
    function selectSeconds()
    {
        image_select_seconds.visible = true; image_up_seconds.visible = true; image_down_seconds.visible = true
    }

    function getDateTime()
    {
        var date = new Date;
        year = date.getFullYear()
        month = date.getMonth()
        day = date.getDay()
        hours = date.getHours()
        minutes = date.getMinutes()
        seconds = date.getUTCSeconds();
        //console.log(year + "." + month + "." + day + "." + hours + "." + minutes + "." + seconds)
    }

    Timer {
        interval: 100; running: true; repeat: repeatTimer;
        onTriggered: scene_setting_system_timeset.getDateTime()
    }

    //背景
    Image {
        id: image_background
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: "images/PXOSD_R0_Background1.png"
    }

    //タイトル
    Image {
        id: image_title
        x: 0
        y: 0
        width: 800
        height: 60
        source: "qrc:///Set3_S2_Timeset_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingSystem }

    //設定ボタン
    Button_Setting3 { id: button_setting3_timeset_yes; x: 50; y:386; filePath: "image://image_provider/:/Set3_S2_Timeset_1"; strCmd: "Timeset"; strParam: "OK" }
    Button_Setting3 { id: button_setting3_timeset_no; x: 420; y:386; filePath: "image://image_provider/:/Set3_S2_Timeset_2"; strCmd: "Timeset"; strParam: "Cancel" }

    Image {
        x: 30
        y: 210
        source: "qrc:///Set3_S2_Timeset_bg1.png"
    }
    Image {
        x: 449
        y: 210
        source: "qrc:///Set3_S2_Timeset_bg2.png"
    }

    //年
    Image { id: image_select_year; x: 76; y: 210; source: "qrc:///Set3_S2_Timeset_bg_select4.png"; visible: false }
    Text_Number { id: image_year1; x: 80; y: 220; typeNumber: "year"; digit: 1; number: year.toString().charAt(0) }
    Text_Number { id: image_year2; x: 80; y: 220; typeNumber: "year"; digit: 2; number: year.toString().charAt(1) }
    Text_Number { id: image_year3; x: 80; y: 220; typeNumber: "year"; digit: 3; number: year.toString().charAt(2) }
    Text_Number { id: image_year4; x: 80; y: 220; typeNumber: "year"; digit: 4; number: year.toString().charAt(3) }
    Rectangle { x: 76; y: 149; width: 124; height: 61; color: "white"; visible: true }
    Rectangle { x: 76; y: 291; width: 124; height: 61; color: "white"; visible: true }
    Image {
        id: image_up_year
        x: 108
        y: 170
        source: "image://image_provider/:/Set3_S2_Timeset_UpN.png"
        visible: false
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_up_year.source = "image://image_provider/:/Set3_S2_Timeset_UpP.png"
            }
            onContainsMouseChanged: {
                image_up_year.source = "image://image_provider/:/Set3_S2_Timeset_UpN.png"
            }
            onReleased: {
                if (image_up_year.source == "image://image_provider/:/Set3_S2_Timeset_UpP.png") {
                    image_up_year.source = "image://image_provider/:/Set3_S2_Timeset_UpN.png"
                    image_year3.forceUp = true; image_year4.forceUp = true
                    if (year >= 2099) {
                        year = 2000
                    } else {
                        year++
                    }
                }
            }
        }
    }
    Image {
        id: image_down_year
        x: 108
        y: 290
        source: "image://image_provider/:/Set3_S2_Timeset_DownN.png"
        visible: false
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_down_year.source = "image://image_provider/:/Set3_S2_Timeset_DownP.png"
            }
            onContainsMouseChanged: {
                image_down_year.source = "image://image_provider/:/Set3_S2_Timeset_DownN.png"
            }
            onReleased: {
                if (image_down_year.source == "image://image_provider/:/Set3_S2_Timeset_DownP.png") {
                    image_down_year.source = "image://image_provider/:/Set3_S2_Timeset_DownN.png"
                    image_year3.forceDown = true; image_year4.forceDown = true
                    if (year <= 2000) {
                        year = 2099
                    } else {
                        year--
                    }
                }
            }
        }
    }

    //月
    Image { id: image_select_month; x: 223; y: 210; source: "qrc:///Set3_S2_Timeset_bg_select2.png"; visible: false }
    Text_Number { id: image_month1; x: 227; y: 220; typeNumber: "month"; digit: 1; number: month < 10 ? 0 : month.toString().charAt(0) }
    Text_Number { id: image_month2; x: 227; y: 220; typeNumber: "month"; digit: 2; number: month < 10 ? month.toString().charAt(0) : month.toString().charAt(1) }
    Rectangle { x: 223; y: 149; width: 64; height: 61; color: "white"; visible: true }
    Rectangle { x: 223; y: 291; width: 64; height: 61; color: "white"; visible: true }
    Image {
        id: image_up_month
        x: 225
        y: 170
        source: "image://image_provider/:/Set3_S2_Timeset_UpN.png"
        visible: false
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_up_month.source = "image://image_provider/:/Set3_S2_Timeset_UpP.png"
            }
            onContainsMouseChanged: {
                image_up_month.source = "image://image_provider/:/Set3_S2_Timeset_UpN.png"
            }
            onReleased: {
                if (image_up_month.source == "image://image_provider/:/Set3_S2_Timeset_UpP.png") {
                    image_up_month.source = "image://image_provider/:/Set3_S2_Timeset_UpN.png"
                    image_month1.forceUp = true; image_month2.forceUp = true
                    if (month >= 12) {
                        month = 1
                    } else {
                        month++
                    }
                    calcLeapYear()
                }
            }
        }
    }
    Image {
        id: image_down_month
        x: 225
        y: 290
        source: "image://image_provider/:/Set3_S2_Timeset_DownN.png"
        visible: false
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_down_month.source = "image://image_provider/:/Set3_S2_Timeset_DownP.png"
            }
            onContainsMouseChanged: {
                image_down_month.source = "image://image_provider/:/Set3_S2_Timeset_DownN.png"
            }
            onReleased: {
                if (image_down_month.source == "image://image_provider/:/Set3_S2_Timeset_DownP.png") {
                    image_down_month.source = "image://image_provider/:/Set3_S2_Timeset_DownN.png"
                    image_month1.forceDown = true; image_month2.forceDown = true
                    if (month <= 1) {
                        month = 12
                    } else {
                        month--
                    }
                    calcLeapYear()
                }
            }
        }
    }

    //日
    Image { id: image_select_day; x: 310; y: 210; source: "qrc:///Set3_S2_Timeset_bg_select2.png"; visible: false }
    Text_Number { id: image_day1; x: 314; y: 220; typeNumber: "day"; digit: 1; number: day < 10 ? 0 : day.toString().charAt(0) }
    Text_Number { id: image_day2; x: 314; y: 220; typeNumber: "day"; digit: 2; number: day < 10 ? day.toString().charAt(0) : day.toString().charAt(1) }
    Rectangle { x: 314; y: 149; width: 64; height: 61; color: "white"; visible: true }
    Rectangle { x: 314; y: 291; width: 64; height: 61; color: "white"; visible: true }
    Image {
        id: image_up_day
        x: 312
        y: 170
        source: "image://image_provider/:/Set3_S2_Timeset_UpN.png"
        visible: false
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_up_day.source = "image://image_provider/:/Set3_S2_Timeset_UpP.png"
            }
            onContainsMouseChanged: {
                image_up_day.source = "image://image_provider/:/Set3_S2_Timeset_UpN.png"
            }
            onReleased: {
                if (image_up_day.source == "image://image_provider/:/Set3_S2_Timeset_UpP.png") {
                    image_up_day.source = "image://image_provider/:/Set3_S2_Timeset_UpN.png"
                    image_day1.forceUp = true; image_day2.forceUp = true
                    if (day >= month_max1.toString() + month_max2.toString()) {
                        day = 1
                    } else {
                        day++
                    }
                }
            }
        }
    }
    Image {
        id: image_down_day
        x: 312
        y: 290
        source: "image://image_provider/:/Set3_S2_Timeset_DownN.png"
        visible: false
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_down_day.source = "image://image_provider/:/Set3_S2_Timeset_DownP.png"
            }
            onContainsMouseChanged: {
                image_down_day.source = "image://image_provider/:/Set3_S2_Timeset_DownN.png"
            }
            onReleased: {
                if (image_down_day.source == "image://image_provider/:/Set3_S2_Timeset_DownP.png") {
                    image_down_day.source = "image://image_provider/:/Set3_S2_Timeset_DownN.png"
                    image_day1.forceDown = true; image_day2.forceDown = true
                    if (day <= 1) {
                        day = month_max1.toString() + month_max2.toString()
                    } else {
                        day--
                    }
                }
            }
        }
    }
    //時
    Image { id: image_select_hours; x: 495; y: 210; source: "qrc:///Set3_S2_Timeset_bg_select2.png"; visible: false }
    Text_Number { id: image_hours1; x: 499; y: 220; typeNumber: "hours"; digit: 1; number: hours < 10 ? 0 : hours.toString().charAt(0) }
    Text_Number { id: image_hours2; x: 499; y: 220; typeNumber: "hours"; digit: 2; number: hours < 10 ? hours.toString().charAt(0) : hours.toString().charAt(1) }
    Rectangle { x: 499; y: 149; width: 64; height: 61; color: "white"; visible: true }
    Rectangle { x: 499; y: 291; width: 64; height: 61; color: "white"; visible: true }
    Image {
        id: image_up_hours
        x: 497
        y: 170
        source: "image://image_provider/:/Set3_S2_Timeset_UpN.png"
        visible: false
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_up_hours.source = "image://image_provider/:/Set3_S2_Timeset_UpP.png"
            }
            onContainsMouseChanged: {
                image_up_hours.source = "image://image_provider/:/Set3_S2_Timeset_UpN.png"
            }
            onReleased: {
                if (image_up_hours.source == "image://image_provider/:/Set3_S2_Timeset_UpP.png") {
                    image_up_hours.source = "image://image_provider/:/Set3_S2_Timeset_UpN.png"
                    image_hours1.forceUp = true; image_hours2.forceUp = true
                    if (hours >= 23) {
                        hours = 0
                    } else {
                        hours++
                    }
                }
            }
        }
    }
    Image {
        id: image_down_hours
        x: 497
        y: 290
        source: "image://image_provider/:/Set3_S2_Timeset_DownN.png"
        visible: false
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_down_hours.source = "image://image_provider/:/Set3_S2_Timeset_DownP.png"
            }
            onContainsMouseChanged: {
                image_down_hours.source = "image://image_provider/:/Set3_S2_Timeset_DownN.png"
            }
            onReleased: {
                if (image_down_hours.source == "image://image_provider/:/Set3_S2_Timeset_DownP.png") {
                    image_down_hours.source = "image://image_provider/:/Set3_S2_Timeset_DownN.png"
                    image_hours1.forceDown = true; image_hours2.forceDown = true
                    if (hours <= 0) {
                        hours = 23
                    } else {
                        hours--
                    }
                }
            }
        }
    }
    //分
    Image { id: image_select_minutes; x: 578; y: 210; source: "qrc:///Set3_S2_Timeset_bg_select2.png"; visible: false }
    Text_Number { id: image_minutes1; x: 582; y: 220; typeNumber: "minutes"; digit: 1; number: minutes < 10 ? 0 : minutes.toString().charAt(0) }
    Text_Number { id: image_minutes2; x: 582; y: 220; typeNumber: "minutes"; digit: 2; number: minutes < 10 ? minutes.toString().charAt(0) : minutes.toString().charAt(1) }
    Rectangle { x: 582; y: 149; width: 64; height: 61; color: "white"; visible: true }
    Rectangle { x: 582; y: 291; width: 64; height: 61; color: "white"; visible: true }
    Image {
        id: image_up_minutes
        x: 580
        y: 170
        source: "image://image_provider/:/Set3_S2_Timeset_UpN.png"
        visible: false
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_up_minutes.source = "image://image_provider/:/Set3_S2_Timeset_UpP.png"
            }
            onContainsMouseChanged: {
                image_up_minutes.source = "image://image_provider/:/Set3_S2_Timeset_UpN.png"
            }
            onReleased: {
                if (image_up_minutes.source == "image://image_provider/:/Set3_S2_Timeset_UpP.png") {
                    image_up_minutes.source = "image://image_provider/:/Set3_S2_Timeset_UpN.png"
                    image_minutes1.forceUp = true; image_minutes2.forceUp = true
                    if (minutes >= 59) {
                        minutes = 0
                    } else {
                        minutes++
                    }
                }
            }
        }
    }
    Image {
        id: image_down_minutes
        x: 580
        y: 290
        source: "image://image_provider/:/Set3_S2_Timeset_DownN.png"
        visible: false
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_down_minutes.source = "image://image_provider/:/Set3_S2_Timeset_DownP.png"
            }
            onContainsMouseChanged: {
                image_down_minutes.source = "image://image_provider/:/Set3_S2_Timeset_DownN.png"
            }
            onReleased: {
                if (image_down_minutes.source == "image://image_provider/:/Set3_S2_Timeset_DownP.png") {
                    image_down_minutes.source = "image://image_provider/:/Set3_S2_Timeset_DownN.png"
                    image_minutes1.forceDown = true; image_minutes2.forceDown = true
                    if (minutes <= 0) {
                        minutes = 59
                    } else {
                        minutes--
                    }
                }
            }
        }
    }
    //秒
    Image { id: image_select_seconds; x: 661; y: 210; source: "qrc:///Set3_S2_Timeset_bg_select2.png"; visible: false }
    Text_Number { id: image_seconds1; x: 665; y: 220; typeNumber: "seconds"; digit: 1; number: seconds < 10 ? 0 : seconds.toString().charAt(0) }
    Text_Number { id: image_seconds2; x: 665; y: 220; typeNumber: "seconds"; digit: 2; number: seconds < 10 ? seconds.toString().charAt(0) : seconds.toString().charAt(1) }
    Rectangle { x: 665; y: 149; width: 64; height: 61; color: "white"; visible: true }
    Rectangle { x: 665; y: 291; width: 64; height: 61; color: "white"; visible: true }
    Image {
        id: image_up_seconds
        x: 663
        y: 170
        source: "image://image_provider/:/Set3_S2_Timeset_UpN.png"
        visible: false
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_up_seconds.source = "image://image_provider/:/Set3_S2_Timeset_UpP.png"
            }
            onContainsMouseChanged: {
                image_up_seconds.source = "image://image_provider/:/Set3_S2_Timeset_UpN.png"
            }
            onReleased: {
                if (image_up_seconds.source == "image://image_provider/:/Set3_S2_Timeset_UpP.png") {
                    image_up_seconds.source = "image://image_provider/:/Set3_S2_Timeset_UpN.png"
                    image_seconds1.forceUp = true; image_seconds2.forceUp = true
                    if (seconds >= 59) {
                        seconds = 0
                    } else {
                        seconds++
                    }
                }
            }
        }
    }
    Image {
        id: image_down_seconds
        x: 663
        y: 290
        source: "image://image_provider/:/Set3_S2_Timeset_DownN.png"
        visible: false
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_down_seconds.source = "image://image_provider/:/Set3_S2_Timeset_DownP.png"
            }
            onContainsMouseChanged: {
                image_down_seconds.source = "image://image_provider/:/Set3_S2_Timeset_DownN.png"
            }
            onReleased: {
                if (image_down_seconds.source == "image://image_provider/:/Set3_S2_Timeset_DownP.png") {
                    image_down_seconds.source = "image://image_provider/:/Set3_S2_Timeset_DownN.png"
                    image_seconds1.forceDown = true; image_seconds2.forceDown = true
                    if (seconds <= 0) {
                        seconds = 59
                    } else {
                        seconds--
                    }
                }
            }
        }
    }
}
