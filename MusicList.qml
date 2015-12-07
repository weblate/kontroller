import QtQuick 2.0
import QtQuick.Controls 1.1
import eu.tgcm 1.0

Rectangle
{
    anchors.fill: parent
    color:"#000";
    Text {
        id: upbtn
        text: qsTr("Sources")
        MouseArea {
            id:area
            onClicked: if(stackedview.depth > 1)
                       popCurrentView();
            anchors.fill: parent
        }
        color: "#00D"
        font.bold: true
        anchors.leftMargin: 10
        anchors.left: parent.left
        onTextChanged: trimText();
        anchors.verticalCenter: refreshbtn.verticalCenter
        font.pixelSize: 12 * scalingFactor
    }
    Image {
        id:refreshbtn
        height: 20 * scalingFactor
        width:height
        source: "icons/refresh.png"
        anchors.right: parent.right
        anchors.top:parent.top
        anchors.topMargin: 5
        anchors.rightMargin: 10
        MouseArea {
            anchors.fill: parent
            onClicked: stackedview.currentItem.refresh()
        }
    }
    StackView
    {
        anchors.top: refreshbtn.bottom
        anchors.topMargin: upbtn.height * 0.1
        anchors.bottom:parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        id: stackedview
        initialItem: MusicPage {
            browsingMode: ""
            browsingValue: ""
            label: ""
            state:"music"
        }
        Component.onCompleted: {
            initialItem.mediaClicked.connect(pushNewPage)
            initialItem.fileClicked.connect(playTheFile)
        }
    }


    Keys.onReleased: {
        if(event.key === Qt.Key_Back || event.key === Qt.Key_Backspace)
        {
            popCurrentView();
            event.accepted = true
        }
    }
    focus:true

    function trimText()
    {
        if(upbtn.paintedWidth > (width - upbtn.height - 20 * scalingFactor))
        {
            upbtn.text = upbtn.text.substring(0, 3) + "..." + upbtn.text.substring(13);
        }
    }




    function pushNewPage(filetype, file, label)
    {
        if(filetype === "")
        {
            return;
        }
        upbtn.text = label;
        if(musicPageComponent.status === Component.Ready)
        {
            var newView = musicPageComponent.createObject(stackedview, {
                                                     browsingMode: filetype,
                                                     browsingValue: file,
                                                     label : label
                                                 });
            newView.mediaClicked.connect(pushNewPage)
            newView.fileClicked.connect(playTheFile)
            stackedview.push(newView)
        }
    }

    function playTheFile(file)
    {

    }

    function popCurrentView()
    {
        stackedview.pop()
        if(stackedview.depth > 1)
            upbtn.text = stackedview.get(stackedview.depth - 1).label
        else
            upbtn.text = "Sources";
    }

    property var musicPageComponent

    Component.onCompleted: {
        musicPageComponent = Qt.createComponent("MusicPage.qml")
    }

}


