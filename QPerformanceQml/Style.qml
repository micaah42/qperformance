pragma Singleton

import QtQuick

QtObject {
    property real fontPxLarge: 22
    property real fontPxSmall: 18

    property real chartHeight: 120
    property string fontFamily: Qt.font({}).family

    function roundWithPrecision(value, precision) {
        const factor = Math.pow(10, precision || 1)
        return (Math.round(value * factor) / factor).toLocaleString()
    }

    function formatKB(kb) {
        if (kb === 0)
            return '0 KB'

        const units = ['KB', 'MB', 'GB', 'TB', 'PB']
        const i = Math.floor(Math.log(kb) / Math.log(1000))
        const value = roundWithPrecision(kb / Math.pow(1000, i))
        return value.toLocaleString() + ' ' + units[i]
    }

    function formatKBFraction(a, b) {
        const units = ['KB', 'MB', 'GB', 'TB', 'PB']

        const ia = a ? Math.floor(Math.log(a) / Math.log(1000)) : 0
        const ib = b ? Math.floor(Math.log(b) / Math.log(1000)) : 0
        const i = Math.max(ia, ib)

        const valueA = roundWithPrecision(a / Math.pow(1000, i))
        const valueB = roundWithPrecision(b / Math.pow(1000, i))
        const stringA = valueA.toLocaleString()
        const stringB = valueB.toLocaleString()

        return `${stringA} / ${stringB} ${units[i]}`
    }

    function alpha(color, alpha) {
        return Qt.rgba(color.r, color.g, color.b, alpha)
    }
}
