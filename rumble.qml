import QtQuick 2.0
import Ubuntu.Components 0.1
import QtFeedback 5.0

MainView {
    applicationName: "effects"
    automaticOrientation: true
    width: units.gu(100)
    height: units.gu(75)

    Page {
        title: "vibration"
        Column {
            anchors.fill:parent
            Button {
                text: "Haptics: Rumble"
                width: parent.width / 2
                MouseArea {
                    anchors.fill: parent
                    onClicked: {console.log("rumble");rumbleEffect.start()}
                }
            }
            Button {
                text: "Haptics: Theme Button"
                width: parent.width / 2
                MouseArea {
                    anchors.fill: parent
                    onClicked: ThemeEffect { effect: ThemeEffect.Press }
                }
            }
        }
    }

    HapticsEffect {
        id: rumbleEffect
        attackIntensity: 0.0
        attackTime: 250
        intensity: 1.0
        duration: 100
        fadeTime: 250
        fadeIntensity: 0.0
    }
}
